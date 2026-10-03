/**
 * @file FilterFactory.cpp
 */

#include "FilterFactory.h"
#include "Exceptions.h"
#include "BrightnessContrastFilter.h"
#include "ConvolutionFilter.h"
#include "FlipFilter.h"
#include "GrayscaleFilter.h"
#include "ThresholdFilter.h"

#include <cctype>

using namespace std;

namespace ip {

	namespace {

		/// 앞뒤 공백을 제거한다.
		string trim(const string& s) {
			const string::size_type first = s.find_first_not_of(" \t\r\n");
			if (first == string::npos) {
				return "";
			}
			const string::size_type last = s.find_last_not_of(" \t\r\n");
			return s.substr(first, last - first + 1);
		}

		/// 소문자로 변환한다.
		string toLower(string s) {
			for (size_t i = 0; i < s.size(); ++i) {
				s[i] = static_cast<char>(tolower(static_cast<unsigned char>(s[i])));
			}
			return s;
		}

		/// "name:a:b" 를 ':' 기준으로 잘라 {"name", "a", "b"} 로 만든다.
		vector<string> split(const string& text, char delimiter) {
			vector<string> tokens;
			string::size_type start = 0;
			while (true) {
				const string::size_type pos = text.find(delimiter, start);
				if (pos == string::npos) {
					tokens.push_back(trim(text.substr(start)));
					break;
				}
				tokens.push_back(trim(text.substr(start, pos - start)));
				start = pos + 1;
			}
			return tokens;
		}

		/// 필터가 요구하는 인자 개수와 실제 개수가 같은지 검사한다.
		void requireArgCount(const string& filterName,
			const vector<string>& args,
			size_t expected) {
			if (args.size() != expected) {
				throw FilterError("'" + filterName + "' takes " + to_string(expected) +
					" argument(s) (got " + to_string(args.size()) + ")");
			}
		}

		/// 문자열을 실수(float)로 변환한다. 예) "128" → 128.0f, "1.5" → 1.5f
		/// (값의 허용 범위 검사는 각 필터의 생성자가 담당한다.)
		float parseFloat(const string& text) {
			return stof(text);
		}

		/// 문자열을 정수(int)로 변환한다. 예) "4" → 4
		/// (값의 허용 범위 검사는 각 필터의 생성자가 담당한다.)
		int parseInt(const string& text) {
			return stoi(text);
		}

	} // anonymous namespace

	FilterBase* FilterFactory::create(const string& spec) {
		// "name:arg1:arg2" → name, {arg1, arg2}
		const vector<string> tokens = split(spec, ':');
		const string name = toLower(tokens[0]);
		if (name.empty()) {
			throw FilterError("Empty filter name in spec: '" + spec + "'");
		}
		const vector<string> args(tokens.begin() + 1, tokens.end());

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
		else if (name == "blur" || name == "sharpen") {
			// 형식: blur 또는 blur:<스레드 수>   예) blur (자동, 멀티스레드), blur:1 (싱글), blur:4
			if (args.size() > 1) {
				throw FilterError("'" + name + "' takes 0 or 1 argument (got " +
					to_string(args.size()) + ")");
			}
			ConvolutionFilter::Kernel kernel = (name == "blur") ? ConvolutionFilter::BLUR
				: ConvolutionFilter::SHARPEN;
			int threadCount = 0;  // 0 = 자동 (CPU 가 지원하는 스레드 수)
			if (args.size() == 1) {
				threadCount = parseInt(args[0]);
			}
			return new ConvolutionFilter(kernel, threadCount);
		}
		else if (name == "flip") {
			requireArgCount(name, args, 1);
			const string direction = toLower(args[0]);
			if (direction == "h") {
				return new FlipFilter(FlipFilter::HORIZONTAL);
			}
			if (direction == "v") {
				return new FlipFilter(FlipFilter::VERTICAL);
			}
			throw FilterError("'flip': direction must be 'h' or 'v' (got '" + args[0] + "')");
		}

		throw FilterError("Unknown filter: '" + name +
			"' (available: grayscale, threshold:N, brightness_contrast:B:C, blur[:threads], sharpen[:threads], flip:h|v)");
	}

	void FilterFactory::createPipeline(const string& pipelineSpec, FilterPipeline& pipeline) {
		// "grayscale, blur, threshold:128" → {"grayscale", "blur", "threshold:128"}
		const vector<string> specs = split(pipelineSpec, ',');
		for (size_t i = 0; i < specs.size(); ++i) {
			pipeline.add(create(specs[i]));
		}
	}

} // namespace ip
