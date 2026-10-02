/**
 * @file GrayscaleFilter.cpp
 */

#include "GrayscaleFilter.h"

#include <cstddef>
#include <cstdint>

using namespace std;

namespace ip {

ImageBuffer GrayscaleFilter::apply(const ImageBuffer& input) const {
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

        // 가중치의 합이 1.0 이므로 결과는 항상 0~255 범위 안이다.
        // 반올림 변환은 FilterBase::clampToUint8 이 처리한다.
        const float gray = WEIGHT_R * r + WEIGHT_G * g + WEIGHT_B * b;
        dst[0] = dst[1] = dst[2] = clampToUint8(gray);

        src += ImageBuffer::CHANNELS;
        dst += ImageBuffer::CHANNELS;
    }
    return output;
}

} // namespace ip
