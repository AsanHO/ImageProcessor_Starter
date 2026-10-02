/**
 * @file GrayscaleFilter.cpp
 */

#include "GrayscaleFilter.h"

#include <cstddef>
#include <cstdint>

namespace ip {

ImageBuffer GrayscaleFilter::apply(const ImageBuffer& input) const {
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

        // 가중치의 합이 1.0 이므로 결과는 항상 0~255 범위 안이다.
        // 소수점은 버려지므로 0.5 를 더해 반올림한다.
        const float gray = WEIGHT_R * r + WEIGHT_G * g + WEIGHT_B * b;
        dst[0] = dst[1] = dst[2] = static_cast<std::uint8_t>(gray + 0.5f);

        src += ImageBuffer::CHANNELS;
        dst += ImageBuffer::CHANNELS;
    }
    return output;
}

} // namespace ip
