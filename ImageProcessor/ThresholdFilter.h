#pragma once

/**
 * @file ThresholdFilter.h
 * @brief 임계값 기준 이진화(흑백 변환).
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief 밝기가 임계값 이상이면 흰색(255), 미만이면 검은색(0) 으로 만든다.
 *
 * 밝기는 그레이스케일과 같은 휘도 공식(0.299 R + 0.587 G + 0.114 B)으로 계산한다.
 * 결과는 24비트 BGR 이미지이며 세 채널이 모두 0 또는 255 이다.
 */
class ThresholdFilter : public FilterBase {
public:
    /**
     * @param threshold 임계값 (0~255).
     * @throws FilterError 범위를 벗어났거나 숫자가 아닌 경우.
     */
    explicit ThresholdFilter(float threshold);

    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override { return "threshold"; }
    std::string describe() const override;

private:
    static constexpr float WEIGHT_R = 0.299f;
    static constexpr float WEIGHT_G = 0.587f;
    static constexpr float WEIGHT_B = 0.114f;

    float m_threshold;
};

} // namespace ip
