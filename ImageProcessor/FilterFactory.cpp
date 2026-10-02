/**
 * @file FilterFactory.cpp
 */

#include "FilterFactory.h"
#include "Exceptions.h"
#include "BrightnessContrastFilter.h"
#include "FlipFilter.h"
#include "GrayscaleFilter.h"
#include "ThresholdFilter.h"

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

		/// 필터가 요구하는 인자 개수와 실제 개수가 같은지 검사한다.
		void requireArgCount(const std::string& filterName,
			const std::vector<std::string>& args,
			std::size_t expected) {
			if (args.size() != expected) {
				throw FilterError("'" + filterName + "' takes " + std::to_string(expected) +
					" argument(s) (got " + std::to_string(args.size()) + ")");
			}
		}

		/// 문자열을 실수(float)로 변환한다. 예) "128" → 128.0f, "1.5" → 1.5f
		/// (값의 허용 범위 검사는 각 필터의 생성자가 담당한다.)
		float parseFloat(const std::string& text) {
			return std::stof(text);
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
			requireArgCount(name, args, 0);
			return new GrayscaleFilter();
		}
		else if (name == "brightness_contrast" || name == "bc") {
			// 형식: brightness_contrast:<밝기>:<대비배율>   예) bc:30:1.5  (밝기 0, 대비 1.0 = 변화 없음)
			requireArgCount(name, args, 2);
			return new BrightnessContrastFilter(parseFloat(args[0]), parseFloat(args[1]));
		}
		else if (name == "threshold") {
			requireArgCount(name, args, 1);
			return new ThresholdFilter(parseFloat(args[0]));
		}
		else if (name == "flip") {
			requireArgCount(name, args, 1);
			const std::string direction = toLower(args[0]);
			if (direction == "h") {
				return new FlipFilter(FlipFilter::HORIZONTAL);
			}
			if (direction == "v") {
				return new FlipFilter(FlipFilter::VERTICAL);
			}
			throw FilterError("'flip': direction must be 'h' or 'v' (got '" + args[0] + "')");
		}

		throw FilterError("Unknown filter: '" + name +
			"' (available: grayscale, threshold:N, brightness_contrast:B:C, flip:h|v)");
	}

} // namespace ip
