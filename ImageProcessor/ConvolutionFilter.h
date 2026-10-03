#pragma once

/**
 * @file ConvolutionFilter.h
 * @brief 3x3 컨볼루션 필터 (Blur, Sharpen).
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief 3x3 커널로 각 픽셀과 주변 8개 픽셀을 가중합하여 새 이미지를 만든다.
 *
 * 한 픽셀(x, y)의 결과 = 주변 3x3 픽셀 값에 커널의 같은 위치 가중치를 곱해 모두 더한 값.
 *
 *   Blur    (가우시안 근사)      Sharpen
 *   1 2 1                         0 -1  0
 *   2 4 2   / 16                 -1  5 -1
 *   1 2 1                         0 -1  0
 *
 * - Blur   : 가중치 합이 1 이므로 전체 밝기는 유지되고 이웃 값과 섞여 부드러워진다.
 * - Sharpen: 중심을 키우고 상하좌우를 빼서 이웃과의 차이를 강조한다.
 *            결과가 0~255 를 벗어나기 쉬워 clampToUint8 로 고정한다.
 *
 * 이웃 픽셀은 항상 원본(input)에서 읽고 결과는 새 버퍼에 쓴다.
 * 결과를 원본에 바로 덮어쓰면 이미 바뀐 값을 이웃 계산에 다시 읽게 되기 때문이다.
 * 이미지 가장자리에서 범위를 벗어나는 이웃은 가장 가까운 가장자리 픽셀 값으로 대신한다.
 *
 * [병렬 처리]
 * 이미지를 위아래 띠(행 묶음)로 나누고, 스레드마다 한 띠를 맡아 계산한다.
 *
 *   스레드 1: 0 ~ 127 행
 *   스레드 2: 128 ~ 255 행   (4개로 나눈 512행 이미지의 예)
 *   ...
 *
 * 스레드끼리 부딪히지 않는 이유:
 *   - 원본(input)은 모든 스레드가 읽기만 한다. (이웃 행이 다른 띠에 있어도 원본에서 읽으므로 안전)
 *   - 결과(output)는 스레드마다 서로 다른 행에만 쓴다. (같은 칸을 두 스레드가 쓰는 일이 없다)
 * 그래서 락(mutex) 없이도 스레드 수와 상관없이 항상 같은 결과가 나온다.
 */
class ConvolutionFilter : public FilterBase {
public:
    enum Kernel {
        BLUR,
        SHARPEN
    };

    /**
     * @param kernel      BLUR 또는 SHARPEN.
     * @param threadCount 병렬 처리에 사용할 스레드 수 (1~64).
     *                    1 이면 새 스레드 없이 싱글스레드로 처리하고,
     *                    0 이면 CPU 가 지원하는 스레드 수만큼 자동으로 정한다.
     * @throws FilterError 범위를 벗어난 경우.
     */
    explicit ConvolutionFilter(Kernel kernel, int threadCount = 0);

    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override;

    /// 실제 사용하는 스레드 수를 포함한다. 예) "blur:4"
    std::string describe() const override;

private:
    static constexpr int KERNEL_SIZE = 3;  ///< 3x3

    /**
     * @brief startRow 이상 endRow 미만의 행만 계산하여 output 에 쓴다.
     *
     * std::thread 는 인자를 복사해서 전달하므로, 이미지(ImageBuffer)를 복사하지 않도록
     * 참조 대신 포인터로 받는다.
     */
    void processRows(const ImageBuffer* input, ImageBuffer* output, int startRow, int endRow) const;

    Kernel m_kernel;
    float m_weights[KERNEL_SIZE * KERNEL_SIZE];  ///< 행 우선(row-major) 순서의 커널 가중치
    int m_threadCount;
};

} // namespace ip
