#include "toolchain.h"

#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "x64_codegen.h"

#ifndef COMPILER_RUNTIME_LIB
#error "COMPILER_RUNTIME_LIB must point to the runtime static library"
#endif

namespace {

std::string Quote(const std::string& arg) { return "'" + arg + "'"; }

}  // namespace

void WriteAssembly(const IR& ir, const std::string& asm_path) {
    std::ofstream out(asm_path);
    if (!out) throw std::runtime_error("cannot open " + asm_path);
    X64Codegen codegen(out);
    codegen.Generate(ir);
}

int BuildExecutable(const IR& ir, const std::string& exe_path) {
    auto asm_path = std::filesystem::temp_directory_path() /
                    ("compiler-" + std::to_string(getpid()) + ".s");
    WriteAssembly(ir, asm_path);

    auto command = "cc -o " + Quote(exe_path) + " " + Quote(asm_path) + " " +
                   Quote(COMPILER_RUNTIME_LIB);
    int status = std::system(command.c_str());

    std::filesystem::remove(asm_path);
    return status;
}
