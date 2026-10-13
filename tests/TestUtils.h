#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

inline void requireTrue(bool cond, const std::string& message) {
    if (!cond) throw std::runtime_error(message);
}

template<class A, class B>
inline void requireEqual(const A& actual, const B& expected, const std::string& message) {
    if (!(actual == expected)) {
        std::ostringstream ss;
        ss << message << " (expected: " << expected << ", actual: " << actual << ")";
        throw std::runtime_error(ss.str());
    }
}

inline void requireOutOfRange(const std::function<void()>& fn, const std::string& message) {
    try {
        fn();
    } catch (const std::out_of_range&) {
        return;
    } catch (...) {
        throw std::runtime_error(message + " (wrong exception type)");
    }
    throw std::runtime_error(message + " (no exception thrown)");
}

class PublicTestSuite {
private:
    struct TestCase {
        std::string name;
        std::function<void()> fn;
    };

    std::string question;
    std::vector<TestCase> cases;

public:
    explicit PublicTestSuite(std::string question) : question(question) {}

    void add(const std::string& name, const std::function<void()>& fn) {
        cases.push_back({name, fn});
    }

    int run(int argc, char* argv[]) {
        int selected = -1;
        if (argc >= 2) {
            try {
                selected = std::stoi(argv[1]);
            } catch (...) {
                std::cerr << "Invalid test number: " << argv[1] << std::endl;
                return 2;
            }
            if (selected < 1 || selected > static_cast<int>(cases.size())) {
                std::cerr << "Test number must be between 1 and " << cases.size() << std::endl;
                return 2;
            }
        }

        std::cout << "== " << question << " public tests ==" << std::endl;

        int passed = 0;
        int failed = 0;
        int executed = 0;

        for (int i = 0; i < static_cast<int>(cases.size()); ++i) {
            if (selected != -1 && selected != i + 1) continue;
            ++executed;

            try {
                cases[i].fn();
                ++passed;
                std::cout << "[PASS] " << i + 1 << ". " << cases[i].name << std::endl;
            } catch (const std::exception& e) {
                ++failed;
                std::cout << "[FAIL] " << i + 1 << ". " << cases[i].name
                          << " -- " << e.what() << std::endl;
            } catch (...) {
                ++failed;
                std::cout << "[FAIL] " << i + 1 << ". " << cases[i].name
                          << " -- unknown error" << std::endl;
            }
        }

        std::cout << "Result: " << passed << " / " << executed << " tests passed" << std::endl;
        return failed == 0 ? 0 : 1;
    }
};

#endif // TESTUTILS_H
