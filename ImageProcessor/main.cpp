/**
 * @file main.cpp
 * @brief ImageProcessor 진입점 — 지원자가 작성해야 할 파일입니다.
 *
 * BMP 입출력과 커맨드라인 파싱은 제공된 코드가 처리합니다.
 * 본 과제에서 작성해야 할 것은 단 하나입니다:
 *
 *     ▶ 이미지 처리 필터 2개 이상 구현 + main 의 TODO 위치에 연결
 *
 * 또한 일관된 컨벤션과 예외 처리, 메모리 안정성도 함께 평가됩니다.
 */

#include "BmpParser.h"
#include "CommandLineParser.h"
#include "ImageBuffer.h"
#include "Exceptions.h"
#include "FilterFactory.h"
#include "FilterPipeline.h"
#include "Logger.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // 로거는 --log 가 지정되기 전까지 비활성 상태이므로, 어디서든 안전하게 호출할 수 있다.
    // try 바깥에 두어야 catch 블록에서도 실패를 기록할 수 있다.
    ip::Logger logger;
    ip::Stopwatch totalTime;  // 프로그램 전체 처리 시간

    try {
        // ── CLI 인자 파싱 (제공된 코드) ─────────────────────────
        const ip::ProgramOptions options = ip::CommandLineParser::parse(argc, argv);

        // ── 로그 파일 열기 ──────────────────────────────────────
        // 로그를 못 남기는 것이 이미지 처리를 막아서는 안 되므로 경고만 출력하고 계속한다.
        if (!options.logPath.empty() && !logger.open(options.logPath)) {
            std::cerr << "Warning: cannot open log file: " << options.logPath << "\n";
        }
        const std::string request = options.pipelineSpec.empty()
            ? "filter=\"" + options.filterName + "\""
            : "pipeline=\"" + options.pipelineSpec + "\"";
        logger.info("START input=" + options.inputPath + " output=" + options.outputPath + " " + request);

        // ── BMP 로드 (제공된 코드) ──────────────────────────────
        ip::ImageBuffer image = ip::BmpParser::loadFromFile(options.inputPath);
        std::cout << "Loaded: " << image.width() << " x " << image.height() << "\n";
        logger.info("LOADED " + std::to_string(image.width()) + "x" + std::to_string(image.height()));

        // ───────────────────────────────────────────────────────
        // TODO: options.filterName 에 따라 적절한 필터를 생성하고
        //       image 에 적용하세요.
        //
        //   예시 코드 (참고용):
        //
        //     if (options.filterName == "grayscale") {
        //         GrayscaleFilter filter;
        //         filter.apply(image);
        //     }
        //     else if (options.filterName == "threshold:128") {
        //         ThresholdFilter filter(128);
        //         filter.apply(image);
        //     }
        //     else {
        //         throw ip::FilterError("Unknown filter: " + options.filterName);
        //     }
        //
        //   ※ 가산점 항목:
        //     - 추상 클래스(FilterBase) 기반 다형성 설계
        //     - 필터 파이프라인 체인 (CLI 옵션 확장 필요)
        //     - 멀티쓰레드 처리
        //     - 로그 파일 출력 (CLI 옵션 확장 필요)
        // ───────────────────────────────────────────────────────

        // --filter 든 --pipeline 이든 "필터 목록"으로 통일해서 같은 경로로 실행한다.
        // 파이프라인이 필터의 소유권을 가지므로, 중간에 예외가 나도 소멸자가 필터를 해제한다.
        ip::FilterPipeline pipeline;
        if (!options.pipelineSpec.empty()) {
            ip::FilterFactory::createPipeline(options.pipelineSpec, pipeline);
        }
        else {
            pipeline.add(ip::FilterFactory::create(options.filterName));
        }
        logger.info("PIPELINE " + pipeline.describe());
        image = pipeline.run(image, &logger);  // 필터마다 걸린 시간이 로그에 기록된다
        std::cout << "Applied: " << pipeline.describe() << "\n";


        // ── BMP 저장 (제공된 코드) ──────────────────────────────
        ip::BmpParser::saveToFile(options.outputPath, image);
        std::cout << "Saved:  " << options.outputPath << "\n";
        logger.info("SAVED " + options.outputPath);
        logger.info("SUCCESS total=" + totalTime.elapsedText());
        return 0;
    }
    catch (const ip::ArgumentError& e) {
        std::cerr << e.what() << "\n\n";
        ip::CommandLineParser::printUsage(argc > 0 ? argv[0] : "ImageProcessor");
        logger.error("FAILED exit=4 total=" + totalTime.elapsedText() + " reason=" + e.what());
        return 4;
    }
    catch (const ip::BmpParseError& e) {
        std::cerr << e.what() << std::endl;
        logger.error("FAILED exit=2 total=" + totalTime.elapsedText() + " reason=" + e.what());
        return 2;
    }
    catch (const ip::FilterError& e) {
        std::cerr << e.what() << std::endl;
        logger.error("FAILED exit=3 total=" + totalTime.elapsedText() + " reason=" + e.what());
        return 3;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        logger.error("FAILED exit=1 total=" + totalTime.elapsedText() + " reason=" + e.what());
        return 1;
    }
}
