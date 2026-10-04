#pragma once

#include <string>

#include "ir.h"

// Writes x86-64 assembly for the program to `asm_path`.
void WriteAssembly(const IR& ir, const std::string& asm_path);

// Generates assembly, then assembles and links it with the runtime library
// using the system C compiler driver. Returns 0 on success.
int BuildExecutable(const IR& ir, const std::string& exe_path);
