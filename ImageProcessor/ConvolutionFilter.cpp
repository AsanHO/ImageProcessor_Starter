/**
 * @file ConvolutionFilter.cpp
 */

#include "ConvolutionFilter.h"
#include "Exceptions.h"

#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>

using namespace std;

namespace ip {

ConvolutionFilter::ConvolutionFilter(Kernel kernel, int threadCount)
    : m_kernel(kernel), m_threadCount(threadCount)
{
    if (threadCount < 0 || threadCount > 64) {
        throw FilterError("thread count must be in [1, 64], or omitted for automatic (got " +
                          to_string(threadCount) + ")");
    }
    if (threadCount == 0) {
        // 지정하지 않으면 CPU 가 지원하는 동시 실행 스레드 수를 쓴다.
        // 알 수 없으면 hardware_concurrency 가 0 을 돌려주므로 1 로 대신한다.
        m_threadCount = static_cast<int>(thread::hardware_concurrency());
        if (m_threadCount < 1) {
            m_threadCount = 1;
        }
    }

    if (kernel == BLUR) {
        // 가우시안 근사: 중심에 가까울수록 가중치가 크다. 합 = 16/16 = 1.0
        const float blur[KERNEL_SIZE * KERNEL_SIZE] = {
            1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
            2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
            1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
        };
        for (int i = 0; i < KERNEL_SIZE * KERNEL_SIZE; ++i) {
            m_weights[i] = blur[i];
        }
    }
    else {
        // 샤프닝: 중심 5, 상하좌우 -1. 합 = 1.0 이라 평평한 영역의 밝기는 변하지 않는다.
        const float sharpen[KERNEL_SIZE * KERNEL_SIZE] = {
             0.0f, -1.0f,  0.0f,
            -1.0f,  5.0f, -1.0f,
             0.0f, -1.0f,  0.0f
        };
        for (int i = 0; i < KERNEL_SIZE * KERNEL_SIZE; ++i) {
            m_weights[i] = sharpen[i];
        }
    }
}

ImageBuffer ConvolutionFilter::apply(const ImageBuffer& input) const {
    const int height = input.height();
    ImageBuffer output(input.width(), height);

    // 행 수보다 많은 스레드는 일이 없으므로 행 수로 제한한다. 최소 1개.
    int threadCount = m_threadCount;
    if (threadCount > height) {
        threadCount = height;
    }
    if (threadCount < 1) {
        threadCount = 1;
    }

    // 스레드 1개: 새 스레드를 만들 필요 없이 지금 스레드에서 전체를 처리한다.
    if (threadCount == 1) {
        processRows(&input, &output, 0, height);
        return output;
    }

    // ── 이미지를 threadCount 개의 띠로 나눈다 ────────────────────
    // i 번째 띠: [ height * i / n,  height * (i+1) / n ) 행.
    // 나머지 행이 생겨도 띠마다 최대 1행 차이로 고르게 나뉜다.
    // (height * i 가 int 범위를 넘지 않도록 long long 으로 계산한다.)
    //
    // 새 스레드는 threadCount - 1 개만 만들고, 마지막 띠는 지금 스레드가 직접 처리한다.
    vector<thread> threads;
    threads.reserve(threadCount - 1);  // push_back 중 메모리 재할당으로 예외가 나는 일을 미리 막는다.

    try {
        for (int i = 0; i < threadCount - 1; ++i) {
            const int startRow = static_cast<int>(static_cast<long long>(height) * i / threadCount);
            const int endRow   = static_cast<int>(static_cast<long long>(height) * (i + 1) / threadCount);
            threads.push_back(thread(&ConvolutionFilter::processRows, this, &input, &output, startRow, endRow));
        }
    }
    catch (...) {
        // 스레드를 만들다 실패하면, 이미 시작된 스레드를 모두 끝낼 때까지 기다린 뒤 예외를 다시 던진다.
        // join 하지 않은 스레드가 남은 채로 벡터가 사라지면 프로그램이 강제 종료되기 때문이다.
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
        throw;
    }

    // 마지막 띠는 지금 스레드가 직접 처리한다.
    const int lastStart = static_cast<int>(static_cast<long long>(height) * (threadCount - 1) / threadCount);
    processRows(&input, &output, lastStart, height);

    // 모든 스레드가 끝날 때까지 기다린다. (끝나기 전에 output 을 반환하면 일부 행이 비어 있게 된다)
    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
    return output;
}

void ConvolutionFilter::processRows(const ImageBuffer* input, ImageBuffer* output,
                                    int startRow, int endRow) const {
    const int width  = input->width();
    const int height = input->height();
    const uint8_t* src = input->data();
    uint8_t* dst = output->data();
    const int radius = KERNEL_SIZE / 2;  // 3x3 이면 1: 중심에서 상하좌우로 1칸

    // 이 함수는 [startRow, endRow) 행만 계산한다. 다른 스레드는 다른 행을 계산한다.
    // 예외를 던지지 않는 단순 계산만 하므로 스레드 안에서 실행해도 안전하다.
    for (int y = startRow; y < endRow; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < ImageBuffer::CHANNELS; ++c) {
                float sum = 0.0f;

                // 3x3 이웃을 돌면서 (픽셀 값 × 커널 가중치) 를 모두 더한다.
                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {
                        // 가장자리를 벗어난 이웃은 가장 가까운 가장자리 픽셀로 대신한다.
                        int sy = y + ky;
                        int sx = x + kx;
                        if (sy < 0)          { sy = 0; }
                        if (sy > height - 1) { sy = height - 1; }
                        if (sx < 0)          { sx = 0; }
                        if (sx > width - 1)  { sx = width - 1; }

                        const size_t srcIndex =
                            (static_cast<size_t>(sy) * width + sx) * ImageBuffer::CHANNELS + c;
                        const float weight = m_weights[(ky + radius) * KERNEL_SIZE + (kx + radius)];
                        sum += weight * src[srcIndex];
                    }
                }

                const size_t dstIndex =
                    (static_cast<size_t>(y) * width + x) * ImageBuffer::CHANNELS + c;
                dst[dstIndex] = clampToUint8(sum);
            }
        }
    }
}

string ConvolutionFilter::name() const {
    return (m_kernel == BLUR) ? "blur" : "sharpen";
}

string ConvolutionFilter::describe() const {
    // 자동으로 정해진 경우에도 실제 사용한 스레드 수가 로그에 남는다.
    return name() + ":" + to_string(m_threadCount);
}

} // namespace ip
