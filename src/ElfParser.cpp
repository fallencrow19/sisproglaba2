#include "ElfParser.h"
#include <fstream>
#include <cstring>
#include <elf.h>
#include <iostream>

bool ElfParser::checkMagic(const unsigned char* ident) {
    return ident[EI_MAG0] == ELFMAG0 && ident[EI_MAG1] == ELFMAG1 &&
           ident[EI_MAG2] == ELFMAG2 && ident[EI_MAG3] == ELFMAG3;
}

bool ElfParser::isSupportedArch(const std::string& path, std::string& archName) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    unsigned char e_ident[EI_NIDENT];
    file.read(reinterpret_cast<char*>(e_ident), EI_NIDENT);
    if (file.gcount() < EI_NIDENT || !checkMagic(e_ident)) return false;

    uint16_t machine = 0;
    // Читаем e_machine в зависимости от класса (32/64)
    if (e_ident[EI_CLASS] == ELFCLASS32) {
        Elf32_Ehdr hdr;
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
        machine = hdr.e_machine;
    } else if (e_ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr hdr;
        file.seekg(0);
        file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
        machine = hdr.e_machine;
    } else {
        return false;
    }

    switch (machine) {
        case EM_386:     archName = "x86";    return true;
        case EM_X86_64:  archName = "x86-64"; return true;
        case EM_ARM:     archName = "arm";    return true;
        case EM_AARCH64: archName = "aarch64";return true;
        default: return false;
    }
}

std::vector<std::string> ElfParser::getNeededLibraries(const std::string& path) {
    std::vector<std::string> libs;
    std::ifstream file(path, std::ios::binary);
    if (!file) return libs;

    unsigned char e_ident[EI_NIDENT];
    file.read(reinterpret_cast<char*>(e_ident), EI_NIDENT);
    if (!checkMagic(e_ident)) return libs;

    bool is64 = (e_ident[EI_CLASS] == ELFCLASS64);
    
    // Читаем заголовок целиком
    file.seekg(0);
    Elf64_Ehdr eh64;
    Elf32_Ehdr eh32;
    uint64_t shoff; uint16_t shentsize, shnum, shstrndx;
    
    if (is64) {
        file.read(reinterpret_cast<char*>(&eh64), sizeof(eh64));
        shoff = eh64.e_shoff; shentsize = eh64.e_shentsize;
        shnum = eh64.e_shnum; shstrndx = eh64.e_shstrndx;
    } else {
        file.read(reinterpret_cast<char*>(&eh32), sizeof(eh32));
        shoff = eh32.e_shoff; shentsize = eh32.e_shentsize;
        shnum = eh32.e_shnum; shstrndx = eh32.e_shstrndx;
    }

    if (shoff == 0 || shnum == 0) return libs;

    // Читаем таблицу секций
    std::vector<Elf64_Shdr> sections64;
    std::vector<Elf32_Shdr> sections32;
    file.seekg(shoff);

    if (is64) {
        sections64.resize(shnum);
        file.read(reinterpret_cast<char*>(sections64.data()), shnum * sizeof(Elf64_Shdr));
    } else {
        sections32.resize(shnum);
        file.read(reinterpret_cast<char*>(sections32.data()), shnum * sizeof(Elf32_Shdr));
    }

    // Находим секцию .dynamic и связанную с ней строковую таблицу
    uint64_t dyn_off = 0, dyn_size = 0, str_off = 0, str_size = 0;
    for (int i = 0; i < shnum; ++i) {
        uint32_t type = is64 ? sections64[i].sh_type : sections32[i].sh_type;
        if (type == SHT_DYNAMIC) {
            dyn_off = is64 ? sections64[i].sh_offset : sections32[i].sh_offset;
            dyn_size = is64 ? sections64[i].sh_size : sections32[i].sh_size;
            uint32_t link = is64 ? sections64[i].sh_link : sections32[i].sh_link;
            if (link < shnum) {
                str_off = is64 ? sections64[link].sh_offset : sections32[link].sh_offset;
                str_size = is64 ? sections64[link].sh_size : sections32[link].sh_size;
            }
            break;
        }
    }

    if (dyn_off == 0 || str_off == 0) return libs;

    // Читаем строки
    std::string strtab(str_size, '\0');
    file.seekg(str_off);
    file.read(&strtab[0], str_size);

    // Читаем записи dynamic
    file.seekg(dyn_off);
    uint64_t count = dyn_size / (is64 ? sizeof(Elf64_Dyn) : sizeof(Elf32_Dyn));
    
    for (uint64_t i = 0; i < count; ++i) {
        uint64_t tag, val;
        if (is64) {
            Elf64_Dyn dyn;
            file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
            tag = dyn.d_tag; val = dyn.d_un.d_val;
        } else {
            Elf32_Dyn dyn;
            file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
            tag = dyn.d_tag; val = dyn.d_un.d_val;
        }

        if (tag == DT_NEEDED && val < str_size) {
            std::string lib = strtab.c_str() + val;
            if (!lib.empty()) libs.push_back(lib);
        }
    }

    return libs;
}