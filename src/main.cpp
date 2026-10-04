#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "driver.h"
#include "interpreter.h"
#include "pretty_print_visitor.h"
#include "print_visitor.h"
#include "toolchain.h"
#include "x64_codegen.h"

namespace {

const char* const kUsage =
    "usage: compiler [options] [file]\n"
    "Reads the program from file, or from stdin if no file is given.\n"
    "\n"
    "  (default)    interpret the program\n"
    "  -S           emit x86-64 assembly (to stdout, or to -o file)\n"
    "  -o <file>    write output to file; without -S builds an executable\n"
    "  --ast        print the AST and pretty-printed source first\n"
    "  -h, --help   show this help\n";

struct Options {
    bool emit_asm = false;
    bool dump_ast = false;
    std::string output;
    std::string input;
};

Options ParseArgs(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-S") {
            options.emit_asm = true;
        } else if (arg == "--ast") {
            options.dump_ast = true;
        } else if (arg == "-o") {
            if (++i == argc) throw std::runtime_error("-o requires a file");
            options.output = argv[i];
        } else if (arg == "-h" || arg == "--help") {
            std::cout << kUsage;
            std::exit(0);
        } else if (!arg.empty() && arg[0] == '-') {
            throw std::runtime_error("unknown option " + arg);
        } else if (options.input.empty()) {
            options.input = arg;
        } else {
            throw std::runtime_error("more than one input file");
        }
    }
    return options;
}

int Run(const Options& options) {
    Driver driver;
    int res = 0;
    if (options.input.empty()) {
        res = driver.Run(std::cin);
    } else {
        std::ifstream input(options.input);
        if (!input) throw std::runtime_error("cannot open " + options.input);
        res = driver.Run(input);
    }
    if (res) return res;

    auto ir = driver.GetIR();

    if (options.dump_ast) {
        PrintVisitor print_visitor(std::cout);
        ir.GetAST()->accept(&print_visitor);
        std::cout << "\n\n";

        PrettyPrintVisitor pretty_print_visitor(std::cout);
        ir.GetAST()->accept(&pretty_print_visitor);
        std::cout << "\n\n";
    }

    if (options.emit_asm) {
        if (options.output.empty()) {
            X64Codegen codegen(std::cout);
            codegen.Generate(ir);
        } else {
            WriteAssembly(ir, options.output);
        }
        return 0;
    }

    if (!options.output.empty()) {
        return BuildExecutable(ir, options.output) ? 1 : 0;
    }

    Interpreter interpreter(ir, std::cout);
    try {
        interpreter.Exec();
    } catch (const std::exception& ex) {
        std::cout << "\033[31mInterpreter error: " << ex.what() << "\033[0m\n";
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        return Run(ParseArgs(argc, argv));
    } catch (const std::exception& ex) {
        std::cerr << "\033[31mError: " << ex.what() << "\033[0m\n";
        return 1;
    }
}
