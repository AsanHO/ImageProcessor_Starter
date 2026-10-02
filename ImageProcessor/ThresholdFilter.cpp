/**
 * @file ThresholdFilter.cpp
 */

#include "ThresholdFilter.h"
#include "Exceptions.h"

#include <cstddef>
#include <cstdint>
#include <sstream>

namespace ip {

ThresholdFilter::ThresholdFilter(float threshold)
    : m_threshold(threshold)
{
    // 부정(!)으로 검사하면 NaN(숫자가 아닌 값)도 함께 걸러진다.
    if (!(threshold >= 0.0f && threshold <= 255.0f)) {
        std::ostringstream message;
        message << "threshold must be in [0, 255] (got " << threshold << ")";
        throw FilterError(message.str());
    }
}

ImageBuffer ThresholdFilter::apply(const ImageBuffer& input) const {
    ImageBuffer output(input.width(), input.height());

    const std::uint8_t* src = input.data();
    std::uint8_t* dst = output.data();
    const std::size_t pixelCount =
        static_cast<std::size_t>(input.width()) * static_cast<std::size_t>(input.height());

    for (std::size_t i = 0; i < pixelCount; ++i) {
        // 메모리 채널 순서는 B, G, R.
        const float b = src[0];
        const float g = src[1];
        const float r = src[2];
        const float gray = WEIGHT_R * r + WEIGHT_G * g + WEIGHT_B * b;

        const std::uint8_t value = (gray >= m_threshold) ? 255 : 0;
        dst[0] = dst[1] = dst[2] = value;

        src += ImageBuffer::CHANNELS;
        dst += ImageBuffer::CHANNELS;
    }
    return output;
}

std::string ThresholdFilter::describe() const {
    std::ostringstream text;
    text << name() << ":" << m_threshold;
    return text.str();
}

} // namespace ip
