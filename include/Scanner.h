#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>

struct ScanResult {
    std::string exePath;
    std::string arch;
    std::vector<std::string> matchedLibs; // Библиотеки из списка пользователя, которые использует файл
};

class Scanner {
public:
    // Сканирует каталог, возвращает список исполняемых файлов, использующих хотя бы одну из библиотек
    static std::vector<ScanResult> scanDirectory(
        const std::filesystem::path& dir,
        const std::vector<std::string>& targetLibs
    );
};