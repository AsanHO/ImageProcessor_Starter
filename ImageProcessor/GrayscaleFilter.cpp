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
        const int b = src[0];
        const int g = src[1];
        const int r = src[2];
		//성능을 위해 부동소수점 대신 정수 가중치와 시프트 연산을 사용한다.
        const auto gray = static_cast<std::uint8_t>(
            (WEIGHT_R * r + WEIGHT_G * g + WEIGHT_B * b) >> WEIGHT_SHIFT);
        dst[0] = dst[1] = dst[2] = gray;
        src += ImageBuffer::CHANNELS;
        dst += ImageBuffer::CHANNELS;
    }
    return output;
}

} // namespace ip
