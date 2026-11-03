#include "Scanner.h"
#include "Reporter.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

void printHelp(const std::string& error = "") {
    if (!error.empty()) {
        std::cerr << "ОШИБКА: " << error << "\n\n";
    }
    std::cout << "Использование: lab2_elf [ОПЦИИ]\n\n";
    std::cout << "Опции:\n";
    std::cout << "  -d, --dir <путь>     Каталог для сканирования (по умолчанию: /usr/bin)\n";
    std::cout << "  -l, --lib <имя>      Библиотека для поиска (можно указать несколько раз)\n";
    std::cout << "  -h, --help           Показать эту справку\n\n";
    std::cout << "Примеры:\n";
    std::cout << "  ./lab2_elf -d /usr/bin -l libc -l libm\n";
    std::cout << "  ./lab2_elf --lib libssl\n";
}

int main(int argc, char* argv[]) {
    std::string scanDir = "/usr/bin";
    std::vector<std::string> libs;
    bool defaultDirUsed = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-d" || arg == "--dir") {
            if (i + 1 >= argc) { printHelp("Не указан путь после " + arg); return 1; }
            scanDir = argv[++i];
            defaultDirUsed = false;
        } else if (arg == "-l" || arg == "--lib") {
            if (i + 1 >= argc) { printHelp("Не указано имя библиотеки после " + arg); return 1; }
            libs.push_back(argv[++i]);
        } else {
            printHelp("Неизвестный аргумент: " + arg);
            return 1;
        }
    }

    if (libs.empty()) {
        printHelp("Не указана ни одна библиотека для поиска (используйте -l)");
        return 1;
    }

    if (defaultDirUsed) {
        std::cout << "[i] Каталог не указан. Используется по умолчанию: " << scanDir << "\n";
    }
    std::cout << "[i] Поиск библиотек: ";
    for (const auto& l : libs) std::cout << l << " ";
    std::cout << "\n";

    auto results = Scanner::scanDirectory(scanDir, libs);
    if (results.empty()) {
        std::cout << "Совпадений не найдено.\n";
        return 0;
    }

    std::string reportFile = Reporter::generateReportFilename(".");
    Reporter::writeReport(reportFile, results);
    std::cout << "[i] Отчёт сохранён: " << reportFile << "\n";

    return 0;
}