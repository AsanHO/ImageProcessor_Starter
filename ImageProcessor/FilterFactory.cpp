/**
 * @file FilterFactory.cpp
 */

#include "FilterFactory.h"
#include "Exceptions.h"
#include "GrayscaleFilter.h"

#include <cctype>

namespace ip {

namespace {

/// 앞뒤 공백을 제거한다.
std::string trim(const std::string& s) {
    const std::string::size_type first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::string::size_type last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

/// 소문자로 변환한다.
std::string toLower(std::string s) {
    for (std::size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    }
    return s;
}

/// "name:a:b" 를 ':' 기준으로 잘라 {"name", "a", "b"} 로 만든다.
std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    while (true) {
        const std::string::size_type pos = text.find(delimiter, start);
        if (pos == std::string::npos) {
            tokens.push_back(trim(text.substr(start)));
            break;
        }
        tokens.push_back(trim(text.substr(start, pos - start)));
        start = pos + 1;
    }
    return tokens;
}

/// 인자가 없어야 하는 필터에서 인자가 들어왔는지 검사한다.
void requireNoArgs(const std::string& filterName, const std::vector<std::string>& args) {
    if (!args.empty()) {
        throw FilterError("'" + filterName + "' takes no arguments (got " +
                          std::to_string(args.size()) + ")");
    }
}

} // anonymous namespace

FilterBase* FilterFactory::create(const std::string& spec) {
    // "name:arg1:arg2" → name, {arg1, arg2}
    const std::vector<std::string> tokens = split(spec, ':');
    const std::string name = toLower(tokens[0]);
    if (name.empty()) {
        throw FilterError("Empty filter name in spec: '" + spec + "'");
    }
    const std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    if (name == "grayscale") {
        requireNoArgs(name, args);
        return new GrayscaleFilter();
    }
    // 새 필터는 여기에 else if 로 추가한다.
    // else if (name == "threshold") { ... }

    throw FilterError("Unknown filter: '" + name + "' (available: grayscale)");
}

} // namespace ip
