/**
 * @file CommandLineParser.cpp
 */

#include "CommandLineParser.h"
#include "Exceptions.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace ip {

namespace {
    /// argv 의 다음 인자를 안전하게 가져온다.
    std::string nextArg(int argc, char* argv[], int& i, const std::string& flag) {
        if (i + 1 >= argc) {
            throw ArgumentError(flag + ": missing value");
        }
        return argv[++i];
    }
} // anonymous namespace

ProgramOptions CommandLineParser::parse(int argc, char* argv[]) {
    ProgramOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--input" || arg == "-i") {
            options.inputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--output" || arg == "-o") {
            options.outputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--filter" || arg == "-f") {
            options.filterName = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--pipeline" || arg == "-p") {
            options.pipelineSpec = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(0);
        }
        else {
            throw ArgumentError("Unknown option: " + arg);
        }
    }

    // 필수 인자 검증
    if (options.inputPath.empty()) {
        throw ArgumentError("--input is required");
    }
    if (options.outputPath.empty()) {
        throw ArgumentError("--output is required");
    }
    // --filter 와 --pipeline 은 둘 중 정확히 하나만 지정해야 한다.
    const bool hasFilter   = !options.filterName.empty();
    const bool hasPipeline = !options.pipelineSpec.empty();
    if (!hasFilter && !hasPipeline) {
        throw ArgumentError("--filter or --pipeline is required");
    }
    if (hasFilter && hasPipeline) {
        throw ArgumentError("--filter and --pipeline cannot be used together");
    }

    return options;
}

void CommandLineParser::printUsage(const std::string& exeName) {
    std::cout
        << "Usage:\n"
        << "  " << exeName << " --input <path> --output <path> --filter <name>\n"
        << "  " << exeName << " --input <path> --output <path> --pipeline <list>\n\n"
        << "Options:\n"
        << "  -i, --input    <path>   Input BMP file (24-bit, uncompressed)\n"
        << "  -o, --output   <path>   Output BMP file\n"
        << "  -f, --filter   <name>   Single filter (e.g. grayscale, threshold:128)\n"
        << "  -p, --pipeline <list>   Comma-separated filters applied in order\n"
        << "                          (use either --filter or --pipeline, not both)\n"
        << "  -h, --help              Show this message\n\n"
        << "Examples:\n"
        << "  " << exeName << " -i input.bmp -o result.bmp -f grayscale\n"
        << "  " << exeName << " -i input.bmp -o result.bmp -p \"grayscale, blur, threshold:128\"\n";
}

} // namespace ip
/*
todo:
이 파서의 한계 (알아 두면 좋은 점)
1. 값 검사가 없다.
-i -o x 입력시
-o가 inputPath의 값으로 들어가 버립니다. 
*/