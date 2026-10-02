/**
 * @file ConvolutionFilter.cpp
 */

#include "ConvolutionFilter.h"

#include <cstddef>
#include <cstdint>

using namespace std;

namespace ip {

ConvolutionFilter::ConvolutionFilter(Kernel kernel)
    : m_kernel(kernel)
{
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
    const int width  = input.width();
    const int height = input.height();
    ImageBuffer output(width, height);

    const uint8_t* src = input.data();
    uint8_t* dst = output.data();
    const int radius = KERNEL_SIZE / 2;  // 3x3 이면 1: 중심에서 상하좌우로 1칸

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < ImageBuffer::CHANNELS; ++c) {
                float sum = 0.0f;

                // 3x3 이웃을 돌면서 (픽셀 값 × 커널 가중치) 를 모두 더한다.
                for (int ky = -radius; ky <= radius; ++ky) {
                    for (int kx = -radius; kx <= radius; ++kx) {
                        // 가장자리를 벗어난 이웃은 가장 가까운 가장자리 픽셀로 대신한다.
                        int sy = y + ky;
                        int sx = x + kx;
                        if (sy < 0)        { sy = 0; }
                        if (sy > height - 1) { sy = height - 1; }
                        if (sx < 0)        { sx = 0; }
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
    return output;
}

string ConvolutionFilter::name() const {
    return (m_kernel == BLUR) ? "blur" : "sharpen";
}

} // namespace ip
