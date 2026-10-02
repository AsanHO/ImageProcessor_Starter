/**
 * @file FlipFilter.cpp
 */

#include "FlipFilter.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

using namespace std;

namespace ip {

FlipFilter::FlipFilter(Direction direction)
    : m_direction(direction)
{
}

ImageBuffer FlipFilter::apply(const ImageBuffer& input) const {
    const int width  = input.width();
    const int height = input.height();
    ImageBuffer output(width, height);

    for (int y = 0; y < height; ++y) {
        if (m_direction == VERTICAL) {
            // 행 전체를 통째로 복사: y 번째 행 → (height-1-y) 번째 행
            memcpy(output.rowPtr(height - 1 - y), input.rowPtr(y), input.rowStride());
        }
        else {
            // 한 행 안에서 픽셀(3바이트) 단위로 좌우 위치를 바꿔 복사
            const uint8_t* srcRow = input.rowPtr(y);
            uint8_t* dstRow = output.rowPtr(y);
            for (int x = 0; x < width; ++x) {
                const uint8_t* srcPixel = srcRow + static_cast<size_t>(x) * ImageBuffer::CHANNELS;
                uint8_t* dstPixel =
                    dstRow + static_cast<size_t>(width - 1 - x) * ImageBuffer::CHANNELS;
                memcpy(dstPixel, srcPixel, ImageBuffer::CHANNELS);
            }
        }
    }
    return output;
}

string FlipFilter::describe() const {
    return name() + ((m_direction == HORIZONTAL) ? ":h" : ":v");
}

} // namespace ip
