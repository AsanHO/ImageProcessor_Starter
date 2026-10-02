#pragma once

/**
 * @file FilterBase.h
 * @brief 모든 이미지 필터의 추상 기반 클래스.
 *
 * 새 필터를 추가하려면 FilterBase 를 상속하고 FilterFactory 에 분기를 추가하면 된다.
 * main / 파이프라인 코드는 수정할 필요가 없다 (OCP).
 */

#include "ImageBuffer.h"

#include <cstdint>
#include <string>

namespace ip {

/**
 * @brief 이미지 필터 인터페이스.
 *
 * apply() 는 입력을 변경하지 않고 결과를 새 ImageBuffer 로 반환한다.
 * 따라서 크기가 바뀌는 필터(크롭/리사이즈)와 이웃 픽셀을 읽는 필터(convolution)를
 * 동일한 시그니처로 다룰 수 있고, 같은 입력에 여러 번 적용해도 안전하다.
 */
class FilterBase {
public:
    // 기반 클래스 포인터로 delete 해도 파생 클래스 소멸자가 호출되도록 virtual.
    virtual ~FilterBase() {}

    /**
     * @brief 필터를 적용한 새 이미지를 반환한다.
     * @throws FilterError 입력이 필터의 전제 조건을 만족하지 않는 경우.
     */
    virtual ImageBuffer apply(const ImageBuffer& input) const = 0;

    /// 필터 이름 (예: "grayscale").
    virtual std::string name() const = 0;

    /// 파라미터를 포함한 설명 (예: "threshold:128"). 로그 출력에 사용한다.
    virtual std::string describe() const { return name(); }

protected:
    /**
     * @brief float 계산 결과를 0~255 로 고정(clamp)한 뒤 반올림하여 uint8_t 로 변환한다.
     *
     * 범위를 벗어난 값을 그대로 uint8_t 에 넣으면 값이 한 바퀴 돌아(300 → 44) 색이 뒤집힌다.
     * 순서가 중요하다: 반드시 clamp 를 먼저 하고, 그 다음 반올림(+0.5)하여 변환한다.
     * 이 함수가 그 순서를 보장하므로 필터에서는 계산 결과를 그대로 넘기면 된다.
     */
    static std::uint8_t clampToUint8(float value) {
        if (value < 0.0f) {
            value = 0.0f;
        }
        else if (value > 255.0f) {
            value = 255.0f;
        }
        return static_cast<std::uint8_t>(value + 0.5f);
    }
};

} // namespace ip
