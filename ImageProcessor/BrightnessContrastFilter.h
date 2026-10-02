#pragma once

/**
 * @file BrightnessContrastFilter.h
 * @brief 밝기 / 대비 조절.
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief 대비를 먼저 조절한 뒤 밝기를 더하고, 결과를 0~255 로 잘라(clamp) 저장한다.
 *
 *   out = clamp((in - 128) * contrast + 128 + brightness)
 *
 *   brightness : 더할 값. 양수면 밝게, 음수면 어둡게. 0 이면 변화 없음.
 *   contrast   : 대비 배율. 1.0 이면 변화 없음, 1.0 초과면 대비 증가,
 *                1.0 미만이면 감소(0 이면 전체가 회색).
 *
 * 모든 채널(B, G, R)에 같은 계산을 적용하므로 색상 균형은 유지된다.
 */
class BrightnessContrastFilter : public FilterBase {
public:
    /**
     * @param brightness 밝기 변화량 (-255~255).
     * @param contrast   대비 배율 (0.0~5.0).
     * @throws FilterError 범위를 벗어났거나 숫자가 아닌 경우.
     */
    BrightnessContrastFilter(float brightness, float contrast);

    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override { return "brightness_contrast"; }
    std::string describe() const override;

private:
    float m_brightness;
    float m_contrast;
};

} // namespace ip
