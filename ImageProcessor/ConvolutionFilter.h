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
 */
class ConvolutionFilter : public FilterBase {
public:
    enum Kernel {
        BLUR,
        SHARPEN
    };

    explicit ConvolutionFilter(Kernel kernel);

    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override;

private:
    static constexpr int KERNEL_SIZE = 3;  ///< 3x3

    Kernel m_kernel;
    float m_weights[KERNEL_SIZE * KERNEL_SIZE];  ///< 행 우선(row-major) 순서의 커널 가중치
};

} // namespace ip
