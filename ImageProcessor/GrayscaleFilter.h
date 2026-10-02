#pragma once

/**
 * @file GrayscaleFilter.h
 * @brief RGB 가중치 공식을 이용한 흑백 변환.
 */

#include "FilterBase.h"

namespace ip {

/**
 * @brief ITU-R BT.601 휘도 공식으로 그레이스케일 변환을 수행한다.
 *
 *   Y = 0.299 R + 0.587 G + 0.114 B
 *
 * 사람의 눈은 녹색에 가장 민감하고 청색에 가장 둔감하므로 단순 평균 대신 가중치를 쓴다.
 * 결과는 여전히 24비트 BGR 이미지이며 세 채널이 같은 값을 갖는다.
 */
class GrayscaleFilter : public FilterBase {
public:
    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override { return "grayscale"; }

private:
    static constexpr float WEIGHT_R = 0.299f;
    static constexpr float WEIGHT_G = 0.587f;
    static constexpr float WEIGHT_B = 0.114f;
};

} // namespace ip
