#include <gtest/gtest.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <sstream>
#include <string>

#include "driver.h"
#include "interpreter.h"
#include "toolchain.h"

namespace {

struct RunResult {
    std::string output;
    int exit_code = -1;
};

IR Compile(const std::string& source) {
    std::istringstream iss(source);
    Driver driver;
    if (driver.Run(iss)) throw std::runtime_error("Driver run error");
    return driver.GetIR();
}

std::string Interpret(const std::string& source) {
    std::ostringstream oss;
    Interpreter interpreter(Compile(source), oss);
    interpreter.Exec();
    return oss.str();
}

// Builds a native executable and runs it, capturing stdout and stderr.
RunResult RunNative(const std::string& source) {
    auto exe = std::filesystem::temp_directory_path() /
               ("codegen-test-" + std::to_string(getpid()));
    if (BuildExecutable(Compile(source), exe) != 0) {
        throw std::runtime_error("BuildExecutable failed");
    }

    RunResult result;
    FILE* pipe = popen((exe.string() + " 2>&1").c_str(), "r");
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        result.output.append(buffer, n);
    }
    int status = pclose(pipe);
    result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    std::filesystem::remove(exe);
    return result;
}

}  // namespace

// Programs whose native output must match the interpreter's.
class CodegenMatchesInterpreter : public ::testing::TestWithParam<const char*> {
};

TEST_P(CodegenMatchesInterpreter, SameOutput) {
    std::string source = GetParam();
    EXPECT_EQ(RunNative(source).output, Interpret(source));
}

INSTANTIATE_TEST_SUITE_P(
    Programs, CodegenMatchesInterpreter,
    ::testing::Values(
        R"(void main() {})",
        R"(void main() { print(42); print(" "); print(true); print(false); })",
        R"(void main() { print("tab\tquote\"backslash\\esc\033[0m\n"); })",
        R"(void main() { print(1 + 2 * 3 - 8 / 2 % 3); })",
        R"(void main() { print((0 - 7) / 2); print(" "); print((0 - 7) % 2); })",
        R"(void main() { print(2147483647 + 1); })",
        R"(void main() { print(1 < 2); print(2 < 1); print(3 > 2); print(3 == 3); print(3 == 4); })",
        R"(void main() {
            int x;
            x = 5;
            if (x > 3) print("big"); else print("small");
            if (x < 3) { print("small"); } else { print("big"); }
        })",
        R"(void main() {
            int i;
            int j;
            i = 0;
            while (i < 4) {
                j = 0;
                while (j < i) { print(j); j = j + 1; }
                print("\n");
                i = i + 1;
            }
        })",
        R"(int fib(int n) {
            if (n < 2) { return n; }
            return fib(n - 1) + fib(n - 2);
        }
        void main() { print(fib(20)); })",
        R"(int sum8(int a, int b, int c, int d, int e, int f, int g, int h) {
            return a * 1 + b * 2 + c * 3 + d * 4 + e * 5 + f * 6 + g * 7 + h * 8;
        }
        int id(int x) { return x; }
        void main() {
            print(sum8(1, 2, 3, 4, 5, 6, 7, 8));
            print(" ");
            print(1 + sum8(id(1), 2, id(3), 4, 5, id(6), id(7), sum8(1, 1, 1, 1, 1, 1, 1, id(1))));
        })",
        R"(int counter;
        int arr[5];
        void bump() { counter = counter + 1; }
        void main() {
            counter = 0;
            int i;
            i = 0;
            while (i < 5) { arr[i] = i * i; bump(); i = i + 1; }
            print(counter); print(" "); print(arr[4] + arr[3]);
        })",
        R"(int total(int[4] a, int n) {
            int s;
            s = 0;
            int i;
            i = 0;
            while (i < n) { s = s + a[i]; i = i + 1; }
            return s;
        }
        void fill(int[4] a) { a[0] = 10; a[1] = 20; a[2] = 30; a[3] = 40; }
        void main() {
            int local[4];
            fill(local);
            print(total(local, 4));
        })",
        R"(int abs(int x) {
            if (x < 0) return (0) - (x);
            return x;
        }
        int sum(int x, int y) {
            int z;
            z = 3;
            while (z < 20) z = (z) + ((x) + (y));
            return z;
        }
        int main() {
            print(abs((0 - 10) + (sum(1, 2))));
            return 0;
        })"));

TEST(CodegenTests, MainReturnValueIsExitCode) {
    EXPECT_EQ(RunNative("int main() { return 7; }").exit_code, 7);
    EXPECT_EQ(RunNative("void main() { print(1); }").exit_code, 0);
}

TEST(CodegenTests, RandIsInRange) {
    auto result = RunNative(R"(void main() {
        int i;
        int ok;
        i = 0;
        ok = 1;
        while (i < 1000) {
            int r;
            r = rand() % 10;
            if (r < 0) ok = 0;
            if (r > 9) ok = 0;
            i = i + 1;
        }
        print(ok);
    })");
    EXPECT_EQ(result.output, "1");
}

TEST(CodegenTests, DivisionByZeroIsRuntimeError) {
    auto result = RunNative(R"(void main() {
        int z;
        z = 0;
        print("before");
        print(1 / z);
    })");
    EXPECT_EQ(result.exit_code, 1);
    EXPECT_EQ(result.output, "before5:17: division by zero\n");
}

TEST(CodegenTests, ArrayOutOfBoundsIsRuntimeError) {
    auto result = RunNative(R"(int a[3];
void main() {
    int i;
    i = 0 - 1;
    a[i] = 1;
})");
    EXPECT_EQ(result.exit_code, 1);
    EXPECT_EQ(result.output, "5:10: array index -1 out of bounds 3\n");
}

// The cases below crash or misbehave in the interpreter, so they are
// checked against expected output instead.
TEST(CodegenTests, BoolEquality) {
    EXPECT_EQ(RunNative(R"(void main() {
        bool a;
        a = true;
        print(a == true);
        print(a == false);
    })").output,
              "10");
}

TEST(CodegenTests, ReturnInsideWhileStopsFunction) {
    EXPECT_EQ(RunNative(R"(int f() {
        int i;
        i = 0;
        while (i < 5) { return 7; print("LEAK"); }
        return 0;
    }
    void main() { print(f()); })")
                  .output,
              "7");
}

TEST(CodegenTests, LexicalScoping) {
    EXPECT_EQ(RunNative(R"(int g;
    int show() { return g; }
    void main() {
        g = 1;
        int g;
        g = 99;
        print(show());
    })")
                  .output,
              "1");
}
