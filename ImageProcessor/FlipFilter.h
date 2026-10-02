#pragma once

/**
 * @file FlipFilter.h
 * @brief 좌우 / 상하 반전.
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief 이미지를 좌우 또는 상하로 뒤집는다.
 *
 *   HORIZONTAL (좌우 반전): (x, y) 의 픽셀이 (width-1-x, y) 로 이동
 *   VERTICAL   (상하 반전): (x, y) 의 픽셀이 (x, height-1-y) 로 이동
 *
 * 상하 반전은 행 전체를 memcpy 한 번으로 옮기고,
 * 좌우 반전은 한 픽셀(3바이트 = B, G, R)을 묶어서 옮긴다.
 * 채널을 따로 옮기면 B, G, R 순서가 뒤섞여 색이 깨지므로 항상 픽셀 단위로 옮긴다.
 */
class FlipFilter : public FilterBase {
public:
    enum Direction {
        HORIZONTAL,  ///< 좌우 반전
        VERTICAL     ///< 상하 반전
    };

    explicit FlipFilter(Direction direction);

    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override { return "flip"; }
    std::string describe() const override;

private:
    Direction m_direction;
};

} // namespace ip
