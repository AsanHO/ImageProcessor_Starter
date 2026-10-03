#pragma once

/**
 * @file Logger.h
 * @brief 처리 과정을 파일에 기록하는 로거와 처리 시간 측정용 스톱워치.
 */

#include <chrono>
#include <fstream>
#include <string>

namespace ip {

/**
 * @brief 로그 파일에 한 줄씩 기록한다.
 *
 * 형식:  [2026-10-03 14:05:12] [INFO ] 메시지
 *
 * - open() 을 호출하기 전에는 아무것도 기록하지 않는다(비활성). 그래서
 *   --log 옵션이 없을 때도 호출하는 쪽에서 if 로 확인할 필요 없이 같은 코드를 쓸 수 있다.
 * - 파일은 이어쓰기(append) 모드로 열어 여러 번 실행한 기록이 한 파일에 쌓인다.
 * - 한 줄을 쓸 때마다 즉시 디스크로 내보내(flush) 프로그램이 비정상 종료돼도 기록이 남는다.
 */
class Logger {
public:
    Logger() {}

    /**
     * @brief 로그 파일을 열어 기록을 시작한다.
     * @return 성공하면 true. 열 수 없으면 false 를 반환하고 로거는 비활성 상태로 남는다.
     *         (로그를 못 남기는 것이 이미지 처리 자체를 막아서는 안 되므로 예외는 던지지 않는다.)
     */
    bool open(const std::string& path);

    /// 일반 정보를 기록한다.
    void info(const std::string& message);

    /// 오류를 기록한다.
    void error(const std::string& message);

private:
    void write(const std::string& level, const std::string& message);

    std::ofstream m_file;
};

/**
 * @brief 생성된 시점부터의 경과 시간을 잰다. (steady_clock: 시스템 시간이 바뀌어도 영향 없음)
 */
class Stopwatch {
public:
    Stopwatch() : m_start(std::chrono::steady_clock::now()) {}

    /// 경과 시간(밀리초).
    double elapsedMs() const;

    /// 경과 시간을 "12.34ms" 형태의 문자열로 반환한다.
    std::string elapsedText() const;

private:
    std::chrono::steady_clock::time_point m_start;
};

} // namespace ip
