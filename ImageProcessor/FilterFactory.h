#pragma once

/**
 * @file FilterFactory.h
 * @brief "name:arg1:arg2" 형태의 문자열로부터 필터를 생성한다.
 *
 * 필터 스펙 문법:
 *   - 이름과 인자는 ':' 로 구분한다.   예) "threshold:128", "crop:10:20:100:50"
 *   - 이름은 대소문자를 구분하지 않으며 앞뒤 공백은 무시한다.
 *
 * 새 필터 추가 방법: FilterFactory.cpp 의 create() 에 if 분기를 하나 추가한다.
 */

#include "FilterBase.h"
#include "FilterPipeline.h"

#include <string>
#include <vector>

namespace ip {

class FilterFactory {
public:
    FilterFactory() = delete;  // 인스턴스화 금지 (정적 메서드만 제공)

    /**
     * @brief 필터 스펙 문자열로부터 필터를 생성한다.
     * @return new 로 생성된 필터. 소유권은 호출자에게 있으며, 사용 후 반드시 delete 해야 한다.
     * @throws FilterError 알 수 없는 필터 이름, 잘못된 인자 개수/형식/범위.
     */
    static FilterBase* create(const std::string& spec);

    /**
     * @brief "grayscale, blur, threshold:128" 처럼 ',' 로 연결된 문자열을 잘라
     *        필터를 순서대로 만들어 pipeline 에 추가한다.
     *
     * 생성된 필터의 소유권은 pipeline 으로 넘어가므로 호출자가 delete 할 필요가 없다.
     * 중간에 예외가 나도 이미 추가된 필터는 pipeline 의 소멸자가 해제한다.
     * @throws FilterError 빈 항목, 알 수 없는 필터 이름, 잘못된 인자.
     */
    static void createPipeline(const std::string& pipelineSpec, FilterPipeline& pipeline);
};

} // namespace ip
