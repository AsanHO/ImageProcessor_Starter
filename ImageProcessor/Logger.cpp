/**
 * @file Logger.cpp
 */

#include "Logger.h"

#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

namespace ip {

namespace {
    /// 현재 시각을 "2026-10-03 14:05:12" 형태로 반환한다.
    string currentTimestamp() {
        const time_t now = time(nullptr);
        const tm* local = localtime(&now);
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
        return buffer;
    }
} // anonymous namespace

bool Logger::open(const string& path) {
    m_file.open(path, ios::out | ios::app);  // app: 기존 내용 뒤에 이어서 쓴다
    return m_file.is_open();
}

void Logger::info(const string& message) {
    write("INFO ", message);
}

void Logger::error(const string& message) {
    write("ERROR", message);
}

void Logger::write(const string& level, const string& message) {
    if (!m_file.is_open()) {
        return;  // 비활성 상태: 아무것도 하지 않는다
    }
    m_file << "[" << currentTimestamp() << "] [" << level << "] " << message << endl;  // endl: 즉시 flush
}

double Stopwatch::elapsedMs() const {
    const chrono::duration<double, milli> elapsed = chrono::steady_clock::now() - m_start;
    return elapsed.count();
}

string Stopwatch::elapsedText() const {
    ostringstream text;
    text << fixed << setprecision(2) << elapsedMs() << "ms";
    return text.str();
}

} // namespace ip
