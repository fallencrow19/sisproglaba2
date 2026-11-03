#!/usr/bin/env bash
# Вспомогательный скрипт для проверки окружения и запуска
set -euo pipefail

BUILD_DIR="build"

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "Создание каталога сборки..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"
cmake .. > /dev/null
make
echo "Сборка завершена. Запустите ./lab2_elf --help"