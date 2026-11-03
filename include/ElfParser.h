#pragma once
#include <string>
#include <vector>
#include <cstdint>

class ElfParser {
public:
    // Проверяет, является ли файл ELF и поддерживается ли архитектура
    static bool isSupportedArch(const std::string& path, std::string& archName);
    
    // Извлекает имена зависимостей (DT_NEEDED) из динамической секции
    static std::vector<std::string> getNeededLibraries(const std::string& path);

private:
    static bool checkMagic(const unsigned char* ident);
    static uint16_t getMachine(const unsigned char* ident, const void* header);
};