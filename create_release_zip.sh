#!/bin/bash
# Skrypt tworzący archiwum ZIP z kodem v1.4.0
# Copyright (c) 2026 MT220126 Engineering Team

VERSION="1.4.0"
PROJECT_NAME="MT220126-RoadPainter"
ZIP_NAME="${PROJECT_NAME}-v${VERSION}.zip"
TEMP_DIR="${PROJECT_NAME}-v${VERSION}"

echo "========================================="
echo "  Tworzenie archiwum ZIP v${VERSION}"
echo "========================================="

# Usuń stare archiwa
rm -f ${ZIP_NAME}
rm -rf ${TEMP_DIR}

# Utwórz katalog tymczasowy
mkdir -p ${TEMP_DIR}

echo "Kopiowanie plików projektu..."

# Kopiuj strukturę projektu
cp -r src ${TEMP_DIR}/
cp -r docs ${TEMP_DIR}/
cp platformio.ini ${TEMP_DIR}/
cp README.md ${TEMP_DIR}/
cp CHANGELOG.md ${TEMP_DIR}/
cp CLAUDE.md ${TEMP_DIR}/

# Usuń pliki tymczasowe i backupy
find ${TEMP_DIR} -name "*.bak" -delete
find ${TEMP_DIR} -name "*_OLD.cpp" -delete
find ${TEMP_DIR} -name "*_OLD.h" -delete
find ${TEMP_DIR} -name "*_backup_*" -delete
find ${TEMP_DIR} -name ".DS_Store" -delete

# Utwórz ZIP
echo "Pakowanie do ${ZIP_NAME}..."
zip -r ${ZIP_NAME} ${TEMP_DIR}/ -x "*.git*" "*.vscode*" "*/.pio/*" "*/build/*"

# Usuń katalog tymczasowy
rm -rf ${TEMP_DIR}

# Statystyki
ZIP_SIZE=$(du -h ${ZIP_NAME} | cut -f1)
FILE_COUNT=$(unzip -l ${ZIP_NAME} | tail -1 | awk '{print $2}')

echo "========================================="
echo "  ✅ GOTOWE!"
echo "========================================="
echo "Plik: ${ZIP_NAME}"
echo "Rozmiar: ${ZIP_SIZE}"
echo "Plików: ${FILE_COUNT}"
echo "========================================="
echo ""
echo "📦 Archiwum gotowe do dystrybucji!"
echo "🚀 Wersja: ${VERSION}"
echo "📅 Data: $(date '+%Y-%m-%d %H:%M:%S')"
echo "========================================="
