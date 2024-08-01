#include "pch.h"
#include <filesystem>
#include <iostream>
#include <cstdlib>

const auto TEST_PREFIX = "test";

int main(void) {
    const auto currentDirectory = std::filesystem::current_path();
    for (const auto & entry : std::filesystem::directory_iterator(currentDirectory)) {
        const auto filename = entry.path().filename().string();
        if (filename.substr(0, 4) == TEST_PREFIX && filename.substr(filename.size() - 4) == ".exe") {
            const auto command = entry.path().string();
            std::cout << "\n===> Executing: " << command << '\n';
            system(command.c_str());
        }
    }
    return 0;
}