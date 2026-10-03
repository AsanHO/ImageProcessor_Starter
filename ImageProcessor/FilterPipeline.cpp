/**
 * @file FilterPipeline.cpp
 */

#include "FilterPipeline.h"
#include "Exceptions.h"

using namespace std;

namespace ip {

FilterPipeline::~FilterPipeline() {
    for (size_t i = 0; i < m_filters.size(); ++i) {
        delete m_filters[i];
    }
}

void FilterPipeline::add(FilterBase* filter) {
    if (filter == nullptr) {
        throw FilterError("Cannot add a null filter to the pipeline");
    }
    try {
        m_filters.push_back(filter);
    }
    catch (...) {
        delete filter;  // push_back 이 실패해도 새 필터가 누수되지 않도록 해제한다.
        throw;
    }
}

namespace {
    /// 필터 하나를 적용하면서 걸린 시간을 로그에 남긴다.
    /// 필터가 실패하면 어느 필터에서 실패했는지 남기고 예외를 다시 던진다.
    ImageBuffer applyWithLog(const FilterBase* filter, const ImageBuffer& input, Logger* logger) {
        Stopwatch stopwatch;
        try {
            ImageBuffer result = filter->apply(input);
            if (logger != nullptr) {
                logger->info("FILTER " + filter->describe() + " time=" + stopwatch.elapsedText());
            }
            return result;
        }
        catch (...) {
            if (logger != nullptr) {
                logger->error("FILTER " + filter->describe() + " failed after " + stopwatch.elapsedText());
            }
            throw;
        }
    }
} // anonymous namespace

ImageBuffer FilterPipeline::run(const ImageBuffer& input, Logger* logger) const {
    if (m_filters.empty()) {
        throw FilterError("Pipeline is empty");
    }

    // 첫 필터는 원본을 읽어 새 버퍼를 만들고, 이후 필터는 직전 결과를 입력으로 받는다.
    // 각 필터는 입력을 바꾸지 않고 새 버퍼를 반환하므로 중간 결과가 서로 섞이지 않는다.
    ImageBuffer current = applyWithLog(m_filters[0], input, logger);
    for (size_t i = 1; i < m_filters.size(); ++i) {
        current = applyWithLog(m_filters[i], current, logger);
    }
    return current;
}

string FilterPipeline::describe() const {
    string text;
    for (size_t i = 0; i < m_filters.size(); ++i) {
        if (i > 0) {
            text += " -> ";
        }
        text += m_filters[i]->describe();
    }
    return text;
}

} // namespace ip
