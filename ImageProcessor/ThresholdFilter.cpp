/**
 * @file ThresholdFilter.cpp
 */

#include "ThresholdFilter.h"
#include "Exceptions.h"

#include <cstddef>
#include <cstdint>
#include <sstream>

using namespace std;

namespace ip {

ThresholdFilter::ThresholdFilter(float threshold)
    : m_threshold(threshold)
{
    // 부정(!)으로 검사하면 NaN(숫자가 아닌 값)도 함께 걸러진다.
    if (!(threshold >= 0.0f && threshold <= 255.0f)) {
        ostringstream message;
        message << "threshold must be in [0, 255] (got " << threshold << ")";
        throw FilterError(message.str());
    }
}

ImageBuffer ThresholdFilter::apply(const ImageBuffer& input) const {
    ImageBuffer output(input.width(), input.height());

    const uint8_t* src = input.data();
    uint8_t* dst = output.data();
    const size_t pixelCount =
        static_cast<size_t>(input.width()) * static_cast<size_t>(input.height());

    for (size_t i = 0; i < pixelCount; ++i) {
        // 메모리 채널 순서는 B, G, R.
        const float b = src[0];
        const float g = src[1];
        const float r = src[2];
        const float gray = WEIGHT_R * r + WEIGHT_G * g + WEIGHT_B * b;

        const uint8_t value = (gray >= m_threshold) ? 255 : 0;
        dst[0] = dst[1] = dst[2] = value;

        src += ImageBuffer::CHANNELS;
        dst += ImageBuffer::CHANNELS;
    }
    return output;
}

string ThresholdFilter::describe() const {
    ostringstream text;
    text << name() << ":" << m_threshold;
    return text.str();
}

} // namespace ip
