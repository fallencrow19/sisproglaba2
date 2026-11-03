#include "Scanner.h"
#include "ElfParser.h"
#include <iostream>

std::vector<ScanResult> Scanner::scanDirectory(
    const std::filesystem::path& dir,
    const std::vector<std::string>& targetLibs) {
    
    std::vector<ScanResult> results;

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        std::cerr << "Ошибка: Каталог не существует: " << dir << "\n";
        return results;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        // Проверяем, является ли файл исполняемым
        auto perms = entry.status().permissions();
        bool isExec = (perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ||
                      (perms & std::filesystem::perms::group_exec) != std::filesystem::perms::none ||
                      (perms & std::filesystem::perms::others_exec) != std::filesystem::perms::none;
        
        if (!isExec) continue;

        std::string arch;
        if (!ElfParser::isSupportedArch(entry.path().string(), arch)) continue;

        auto libs = ElfParser::getNeededLibraries(entry.path().string());
        if (libs.empty()) continue;

        std::vector<std::string> matched;
        for (const auto& lib : libs) {
            for (const auto& target : targetLibs) {
                // Простое совпадение по подстроке (например, libssl.so.1.1 совпадет с libssl)
                if (lib.find(target) != std::string::npos) {
                    matched.push_back(lib);
                    break;
                }
            }
        }

        if (!matched.empty()) {
            results.push_back({entry.path().string(), arch, matched});
        }
    }

    return results;
}