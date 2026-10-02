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
 * 부동소수점 대신 합이 256 인 정수 가중치(77, 150, 29)와 시프트 연산을 사용한다.
 * 결과는 여전히 24비트 BGR 이미지이며 세 채널이 같은 값을 갖는다.
 */
class GrayscaleFilter : public FilterBase {
public:
    ImageBuffer apply(const ImageBuffer& input) const override;
    std::string name() const override { return "grayscale"; }

private:
    static constexpr int WEIGHT_R = 77;   // 0.299 * 256 ≈ 76.5
    static constexpr int WEIGHT_G = 150;  // 0.587 * 256 ≈ 150.3
    static constexpr int WEIGHT_B = 29;   // 0.114 * 256 ≈ 29.2
    static constexpr int WEIGHT_SHIFT = 8;
};

} // namespace ip
