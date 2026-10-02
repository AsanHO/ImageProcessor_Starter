/**
 * @file BrightnessContrastFilter.cpp
 */

#include "BrightnessContrastFilter.h"
#include "Exceptions.h"

#include <cstddef>
#include <cstdint>
#include <sstream>

namespace ip {

BrightnessContrastFilter::BrightnessContrastFilter(float brightness, float contrast)
    : m_brightness(brightness), m_contrast(contrast)
{
    // 부정(!)으로 검사하면 NaN(숫자가 아닌 값)도 함께 걸러진다.
    if (!(brightness >= -255.0f && brightness <= 255.0f)) {
        std::ostringstream message;
        message << "brightness must be in [-255, 255] (got " << brightness << ")";
        throw FilterError(message.str());
    }
    if (!(contrast >= 0.0f && contrast <= 5.0f)) {
        std::ostringstream message;
        message << "contrast must be in [0, 5] (got " << contrast << ")";
        throw FilterError(message.str());
    }
}

ImageBuffer BrightnessContrastFilter::apply(const ImageBuffer& input) const {
    ImageBuffer output(input.width(), input.height());

    const std::uint8_t* src = input.data();
    std::uint8_t* dst = output.data();
    const std::size_t count = input.dataSize();  // 채널 구분 없이 모든 값에 동일하게 적용

    for (std::size_t i = 0; i < count; ++i) {
        // 중간 밝기(128)를 기준으로 대비를 조절한 뒤 밝기를 더한다.
        const float result = (src[i] - 128.0f) * m_contrast + 128.0f + m_brightness;

        // 0~255 범위로 고정(clamp)하고 반올림하여 변환한다. (FilterBase::clampToUint8)
        dst[i] = clampToUint8(result);
    }
    return output;
}

std::string BrightnessContrastFilter::describe() const {
    std::ostringstream text;
    text << name() << ":" << m_brightness << ":" << m_contrast;
    return text.str();
}

} // namespace ip
