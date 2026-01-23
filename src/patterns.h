/**
 * Definicje wzorców malowania pasów drogowych
 * Zgodne z normami drogowymi
 *
 * WERSJA 1.1.0 - POPRAWIONE MAPOWANIE PISTOLETÓW
 * Zgodnie ze specyfikacją:
 * - P1, P2, P3, P4 = Oś jezdni
 * - P5, P6 = Krawędź jezdni
 * - P1, P2, P3, P5 = 12cm szerokość (pojedyncze dysze)
 * - P4, P6 = 24cm szerokość (podwójne dysze)
 *
 * Rzeczywista szerokość:
 * - Pojedyncza dysza 12cm ≈ 4cm malowania (pistolet ma ~4cm szerokość)
 * - Podwójna dysza 24cm ≈ 8cm malowania (pistolet ma ~8cm szerokość)
 */

#ifndef PATTERNS_H
#define PATTERNS_H

#include "config.h"

// Rzeczywiste szerokości pistoletów (cm)
#define GUN_WIDTH_12CM 4.0   // Pojedyncza dysza
#define GUN_WIDTH_24CM 8.0   // Podwójna dysza

// Tablica wszystkich wzorców malowania
// MAPOWANIE ZGODNE ZE SPECYFIKACJĄ KLIENTA:
/*
Wzorzec → Pistolety
P-1a → P2
P-1b → P2
P-1c → P2
P-1d → P2
P-1e → P4
P-2a → P2
P-2b → P4
P-3a → P1 + P3
P-3b → P1 + P3
P-4  → P1 + P3
P-6  → P5
P-7a → P6
P-7b → P6
P-7c → P5
P-7d → P5
*/

const Pattern PATTERNS[PATTERN_COUNT] = {
    // P-1a: Przerywana długa → Tylko P2
    {
        PATTERN_P1A,
        "P-1a",
        4.0,     // 4m linia
        8.0,     // 8m przerwa
        12,      // 12cm nominalna szerokość
        "Przerywana dluga",
        false,   // Nie odwracalna
        // Pistolety: gun1, gun2, gun3, gun4, gun5, gun6
        false, true, false, false, false, false,  // Tylko P2
        GUN_WIDTH_12CM  // 4cm rzeczywista szerokość
    },

    // P-1b: Przerywana krótka → Tylko P2
    {
        PATTERN_P1B,
        "P-1b",
        2.0,     // 2m linia
        4.0,     // 4m przerwa
        12,      // 12cm nominalna
        "Przerywana krotka",
        false,
        false, true, false, false, false, false,  // Tylko P2
        GUN_WIDTH_12CM
    },

    // P-1c: Wydzielająca → Tylko P2
    {
        PATTERN_P1C,
        "P-1c",
        2.0,     // 2m linia
        2.0,     // 2m przerwa
        12,      // 12cm nominalna
        "Wydzielajaca",
        false,
        false, true, false, false, false, false,  // Tylko P2
        GUN_WIDTH_12CM
    },

    // P-1d: Prowadząca wąska → Tylko P2
    {
        PATTERN_P1D,
        "P-1d",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        12,      // 12cm nominalna
        "Prowadzaca waska",
        false,
        false, true, false, false, false, false,  // Tylko P2
        GUN_WIDTH_12CM
    },

    // P-1e: Prowadząca szeroka → Tylko P4
    {
        PATTERN_P1E,
        "P-1e",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        24,      // 24cm nominalna
        "Prowadzaca szeroka",
        false,
        false, false, false, true, false, false,  // Tylko P4
        GUN_WIDTH_24CM  // 8cm rzeczywista
    },

    // P-2a: Ciągła wąska → Tylko P2
    {
        PATTERN_P2A,
        "P-2a",
        0.0,     // Ciągła (0 = bez przerw)
        0.0,     // Brak przerwy
        12,      // 12cm nominalna
        "Ciagla waska",
        false,
        false, true, false, false, false, false,  // Tylko P2
        GUN_WIDTH_12CM
    },

    // P-2b: Ciągła szeroka → Tylko P4
    {
        PATTERN_P2B,
        "P-2b",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm nominalna
        "Ciagla szeroka",
        false,
        false, false, false, true, false, false,  // Tylko P4
        GUN_WIDTH_24CM
    },

    // P-3a: Przekraczalna długa → P1 + P3 (ODWRACALNA)
    {
        PATTERN_P3A,
        "P-3a",
        4.0,     // 4m linia przerywana
        2.0,     // 2m przerwa
        12,      // 12cm nominalna
        "Przekraczalna dluga",
        true,    // ODWRACALNA!
        true, false, true, false, false, false,  // P1 + P3
        GUN_WIDTH_12CM * 2  // 2 pistolety = 8cm łącznie
    },

    // P-3b: Przekraczalna krótka → P1 + P3 (ODWRACALNA)
    {
        PATTERN_P3B,
        "P-3b",
        1.0,     // 1m linia przerywana
        1.0,     // 1m przerwa
        12,      // 12cm nominalna
        "Przekraczalna krotka",
        true,    // ODWRACALNA!
        true, false, true, false, false, false,  // P1 + P3
        GUN_WIDTH_12CM * 2  // 8cm łącznie
    },

    // P-4: Podwójna ciągła → P1 + P3
    {
        PATTERN_P4,
        "P-4",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm nominalna (2x12cm)
        "Podwojna ciagla",
        false,
        true, false, true, false, false, false,  // P1 + P3
        GUN_WIDTH_12CM * 2  // 8cm łącznie
    },

    // P-6: Ostrzegawcza → Tylko P5 (krawędź)
    {
        PATTERN_P6,
        "P-6",
        4.0,     // 4m linia
        2.0,     // 2m przerwa
        12,      // 12cm nominalna
        "Ostrzegawcza",
        false,
        false, false, false, false, true, false,  // Tylko P5 (krawędź)
        GUN_WIDTH_12CM
    },

    // P-7a: Krawędziowa przerywana szeroka → Tylko P6 (krawędź 24cm)
    {
        PATTERN_P7A,
        "P-7a",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        24,      // 24cm nominalna
        "Krawedz. przeryw. szer.",
        false,
        false, false, false, false, false, true,  // Tylko P6 (krawędź)
        GUN_WIDTH_24CM
    },

    // P-7b: Krawędziowa ciągła szeroka → Tylko P6 (krawędź 24cm)
    {
        PATTERN_P7B,
        "P-7b",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm nominalna
        "Krawedz. ciagla szer.",
        false,
        false, false, false, false, false, true,  // Tylko P6 (krawędź)
        GUN_WIDTH_24CM
    },

    // P-7c: Krawędziowa przerywana wąska → Tylko P5 (krawędź 12cm)
    {
        PATTERN_P7C,
        "P-7c",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        12,      // 12cm nominalna
        "Krawedz. przeryw. waska",
        false,
        false, false, false, false, true, false,  // Tylko P5 (krawędź)
        GUN_WIDTH_12CM
    },

    // P-7d: Krawędziowa ciągła wąska → Tylko P5 (krawędź 12cm)
    {
        PATTERN_P7D,
        "P-7d",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        12,      // 12cm nominalna
        "Krawedz. ciagla waska",
        false,
        false, false, false, false, true, false,  // Tylko P5 (krawędź)
        GUN_WIDTH_12CM
    }
};

/**
 * Pobiera wzorzec na podstawie typu
 */
inline Pattern* getPattern(PatternType type) {
    if (type >= 0 && type < PATTERN_COUNT) {
        return (Pattern*)&PATTERNS[type];
    }
    return nullptr;
}

/**
 * Sprawdza czy wzorzec jest odwracalny
 */
inline bool isPatternReversible(PatternType type) {
    Pattern* pattern = getPattern(type);
    return pattern ? pattern->reversible : false;
}

/**
 * Pobiera nazwę wzorca
 */
inline const char* getPatternName(PatternType type) {
    Pattern* pattern = getPattern(type);
    return pattern ? pattern->name : "???";
}

/**
 * Pobiera opis wzorca
 */
inline const char* getPatternDescription(PatternType type) {
    Pattern* pattern = getPattern(type);
    return pattern ? pattern->description : "Nieznany";
}

/**
 * NOWA FUNKCJA: Oblicza rzeczywistą szerokość malowania
 * Uwzględnia tylko aktywne pistolety
 */
inline float getPatternRealWidth(PatternType type) {
    Pattern* pattern = getPattern(type);
    return pattern ? pattern->realWidth : 0.0;
}

/**
 * NOWA FUNKCJA: Sprawdza czy pistolet powinien być aktywny
 */
inline bool isGunActive(PatternType type, uint8_t gunNumber) {
    Pattern* pattern = getPattern(type);
    if (!pattern || gunNumber < 1 || gunNumber > 6) {
        return false;
    }

    switch (gunNumber) {
        case 1: return pattern->gun1;
        case 2: return pattern->gun2;
        case 3: return pattern->gun3;
        case 4: return pattern->gun4;
        case 5: return pattern->gun5;
        case 6: return pattern->gun6;
        default: return false;
    }
}

/**
 * NOWA FUNKCJA: Pobiera listę aktywnych pistoletów dla wzorca
 * Zwraca ilość aktywnych pistoletów
 */
inline uint8_t getActiveGuns(PatternType type, bool activeGuns[6]) {
    Pattern* pattern = getPattern(type);
    if (!pattern) {
        for (int i = 0; i < 6; i++) {
            activeGuns[i] = false;
        }
        return 0;
    }

    activeGuns[0] = pattern->gun1;
    activeGuns[1] = pattern->gun2;
    activeGuns[2] = pattern->gun3;
    activeGuns[3] = pattern->gun4;
    activeGuns[4] = pattern->gun5;
    activeGuns[5] = pattern->gun6;

    uint8_t count = 0;
    for (int i = 0; i < 6; i++) {
        if (activeGuns[i]) count++;
    }

    return count;
}

#endif // PATTERNS_H
