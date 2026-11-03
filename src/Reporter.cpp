#include "Reporter.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

std::string Reporter::generateReportFilename(const std::string& baseDir) {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << baseDir << "/report_" 
        << std::put_time(tm, "%Y%m%d_%H%M%S") << ".txt";
    
    // Проверка на существование (на случай очень быстрых повторных запусков)
    std::string name = oss.str();
    int suffix = 1;
    while (std::filesystem::exists(name)) {
        oss.str(""); oss.clear();
        oss << baseDir << "/report_" << std::put_time(tm, "%Y%m%d_%H%M%S") << "_" << suffix++ << ".txt";
        name = oss.str();
    }
    return name;
}

void Reporter::writeReport(const std::string& filename, std::vector<ScanResult>& results) {
    // Сортировка: сначала по количеству библиотек (убывание), затем по пути (возрастание)
    std::sort(results.begin(), results.end(), [](const ScanResult& a, const ScanResult& b) {
        if (a.matchedLibs.size() != b.matchedLibs.size())
            return a.matchedLibs.size() > b.matchedLibs.size();
        return a.exePath < b.exePath;
    });

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Не удалось создать файл отчёта: " << filename << "\n";
        return;
    }

    out << "ОТЧЁТ ОБ ИСПОЛЬЗОВАНИИ БИБЛИОТЕК\n";
    out << "================================\n\n";

    for (const auto& res : results) {
        out << "Файл: " << res.exePath << " [" << res.arch << "]\n";
        out << "  Использует библиотеки (" << res.matchedLibs.size() << "):\n";
        for (const auto& lib : res.matchedLibs) {
            out << "    - " << lib << "\n";
        }
        out << "\n";
    }

    out << "Всего найдено исполняемых файлов: " << results.size() << "\n";
    out.close();
}