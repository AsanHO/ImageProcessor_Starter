#pragma once

/**
 * @file FilterPipeline.h
 * @brief 필터를 순서대로 연결해 실행하는 파이프라인.
 *
 * 필터 한 개도 "필터 1개짜리 파이프라인"으로 취급하므로
 * main 은 --filter / --pipeline 구분 없이 같은 경로로 실행한다.
 * 파이프라인은 FilterBase 만 알고 구체적인 필터 종류는 모른다 (OCP).
 */

#include "FilterBase.h"
#include "ImageBuffer.h"
#include "Logger.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ip {

/**
 * @brief 필터 목록을 소유하고 순서대로 적용한다.
 *
 * 소유권: add() 로 넘겨받은 필터는 파이프라인이 소멸될 때 delete 한다.
 * 같은 포인터를 두 객체가 delete 하는 일(이중 해제)이 없도록 복사는 금지한다.
 */
class FilterPipeline {
public:
    FilterPipeline() {}
    ~FilterPipeline();

    FilterPipeline(const FilterPipeline&) = delete;
    FilterPipeline& operator=(const FilterPipeline&) = delete;

    /**
     * @brief new 로 만든 필터를 파이프라인 끝에 추가한다. 소유권이 파이프라인으로 넘어간다.
     * @throws FilterError filter 가 nullptr 인 경우.
     */
    void add(FilterBase* filter);

    /**
     * @brief 모든 필터를 순서대로 적용한 새 이미지를 반환한다. 입력은 변경하지 않는다.
     * @param logger nullptr 이 아니면 필터마다 걸린 시간과 실패한 필터를 기록한다.
     * @throws FilterError 필터가 하나도 없는 경우.
     */
    ImageBuffer run(const ImageBuffer& input, Logger* logger = nullptr) const;

    /// "grayscale -> threshold:128" 형태의 설명 (콘솔/로그 출력용).
    std::string describe() const;

    std::size_t size() const { return m_filters.size(); }

private:
    std::vector<FilterBase*> m_filters;
};

} // namespace ip
