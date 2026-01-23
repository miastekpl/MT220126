/**
 * Definicje wzorców malowania pasów drogowych
 * Zgodne z normami drogowymi
 */

#ifndef PATTERNS_H
#define PATTERNS_H

#include "config.h"

// Tablica wszystkich wzorców malowania
const Pattern PATTERNS[PATTERN_COUNT] = {
    // P-1a: Przerywana długa
    {
        PATTERN_P1A,
        "P-1a",
        4.0,     // 4m linia
        8.0,     // 8m przerwa
        12,      // 12cm szerokość
        "Przerywana dluga",
        false
    },

    // P-1b: Przerywana krótka
    {
        PATTERN_P1B,
        "P-1b",
        2.0,     // 2m linia
        4.0,     // 4m przerwa
        12,      // 12cm szerokość
        "Przerywana krotka",
        false
    },

    // P-1c: Wydzielająca
    {
        PATTERN_P1C,
        "P-1c",
        2.0,     // 2m linia
        2.0,     // 2m przerwa
        12,      // 12cm szerokość
        "Wydzielajaca",
        false
    },

    // P-1d: Prowadząca wąska
    {
        PATTERN_P1D,
        "P-1d",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        12,      // 12cm szerokość
        "Prowadzaca waska",
        false
    },

    // P-1e: Prowadząca szeroka
    {
        PATTERN_P1E,
        "P-1e",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        24,      // 24cm szerokość
        "Prowadzaca szeroka",
        false
    },

    // P-2a: Ciągła wąska
    {
        PATTERN_P2A,
        "P-2a",
        0.0,     // Ciągła (0 = bez przerw)
        0.0,     // Brak przerwy
        12,      // 12cm szerokość
        "Ciagla waska",
        false
    },

    // P-2b: Ciągła szeroka
    {
        PATTERN_P2B,
        "P-2b",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm szerokość
        "Ciagla szeroka",
        false
    },

    // P-3a: Przekraczalna długa (ODWRACALNA)
    {
        PATTERN_P3A,
        "P-3a",
        4.0,     // 4m linia przerywana
        2.0,     // 2m przerwa
        12,      // 12cm szerokość
        "Przekraczalna dluga",
        true     // Można odwrócić
    },

    // P-3b: Przekraczalna krótka (ODWRACALNA)
    {
        PATTERN_P3B,
        "P-3b",
        1.0,     // 1m linia przerywana
        1.0,     // 1m przerwa
        12,      // 12cm szerokość
        "Przekraczalna krotka",
        true     // Można odwrócić
    },

    // P-4: Podwójna ciągła
    {
        PATTERN_P4,
        "P-4",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm szerokość (2x12cm)
        "Podwojna ciagla",
        false
    },

    // P-6: Ostrzegawcza
    {
        PATTERN_P6,
        "P-6",
        4.0,     // 4m linia
        2.0,     // 2m przerwa
        12,      // 12cm szerokość
        "Ostrzegawcza",
        false
    },

    // P-7a: Krawędziowa przerywana szeroka
    {
        PATTERN_P7A,
        "P-7a",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        24,      // 24cm szerokość
        "Krawedz. przeryw. szer.",
        false
    },

    // P-7b: Krawędziowa ciągła szeroka
    {
        PATTERN_P7B,
        "P-7b",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        24,      // 24cm szerokość
        "Krawedz. ciagla szer.",
        false
    },

    // P-7c: Krawędziowa przerywana wąska
    {
        PATTERN_P7C,
        "P-7c",
        1.0,     // 1m linia
        1.0,     // 1m przerwa
        12,      // 12cm szerokość
        "Krawedz. przeryw. waska",
        false
    },

    // P-7d: Krawędziowa ciągła wąska
    {
        PATTERN_P7D,
        "P-7d",
        0.0,     // Ciągła
        0.0,     // Brak przerwy
        12,      // 12cm szerokość
        "Krawedz. ciagla waska",
        false
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

#endif // PATTERNS_H
