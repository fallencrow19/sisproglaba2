#pragma once
#include "Scanner.h"
#include <string>
#include <vector>

class Reporter {
public:
    // Генерирует уникальное имя файла отчёта (чтобы не перезаписывать)
    static std::string generateReportFilename(const std::string& baseDir = ".");
    
    // Сортирует и записывает отчёт
    static void writeReport(const std::string& filename, std::vector<ScanResult>& results);
};