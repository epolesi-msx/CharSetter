#ifndef __ALLCHARSETS_H__
#define __ALLCHARSETS_H__

// Arabian
#if defined(CS_ARABIAN) || defined(MULTI_CS)
#include "charset_arabian.h"
#define CS_NAME_ARABIAN "Arabian"
#define CS_FONT_ARABIAN charset_arabian
#ifdef CS_ARABIAN
#define CS_NAME CS_NAME_ARABIAN
#define CS_FONT CS_FONT_ARABIAN
#endif
#endif

// Brazilian BRASCII (Gradiente)
#if defined(CS_BRAZIL_G) || defined(MULTI_CS)
#include "charset_brazilian_g.h"
#define CS_NAME_BRAZIL_G "Brazilian BRASCII Gradiente"
#define CS_FONT_BRAZIL_G charset_brazilian_g
#ifdef CS_BRAZIL_G
#define CS_NAME CS_NAME_BRAZIL_G
#define CS_FONT CS_FONT_BRAZIL_G
#endif
#endif

// Brazilian BRASCII (Sharp)
#if defined(CS_BRAZIL_S) || defined(MULTI_CS)
#include "charset_brazilian_s.h"
#define CS_NAME_BRAZIL_S "Brazilian BRASCII Sharp"
#define CS_FONT_BRAZIL_S charset_brazilian_s
#ifdef CS_BRAZIL_S
#define CS_NAME CS_NAME_BRAZIL_S
#define CS_FONT CS_FONT_BRAZIL_S
#endif
#endif

// Brazilian Gradiente v1.0
#if defined(CS_BRAZIL_G10) || defined(MULTI_CS)
#include "charset_brazilian_g10.h"
#define CS_NAME_BRAZIL_G10 "Brazilian Gradiente 1.0"
#define CS_FONT_BRAZIL_G10 charset_brazilian_g10
#ifdef CS_BRAZIL_G10
#define CS_NAME CS_NAME_BRAZIL_G10
#define CS_FONT CS_FONT_BRAZIL_G10
#endif
#endif

// Brazilian Sharp v1.1
#if defined(CS_BRAZIL_S11) || defined(MULTI_CS)
#include "charset_brazilian_s11.h"
#define CS_NAME_BRAZIL_S11 "Brazilian Sharp 1.1"
#define CS_FONT_BRAZIL_S11 charset_brazilian_s11
#ifdef CS_BRAZIL_S11
#define CS_NAME CS_NAME_BRAZIL_S11
#define CS_FONT CS_FONT_BRAZIL_S11
#endif
#endif

// German
#if defined(CS_GERMAN) || defined(MULTI_CS)
#include "charset_german.h"
#define CS_NAME_GERMAN "German"
#define CS_FONT_GERMAN charset_german
#ifdef CS_GERMAN
#define CS_NAME CS_NAME_GERMAN
#define CS_FONT CS_FONT_GERMAN
#endif
#endif

// International
#if defined(CS_INTERNAT) || defined(MULTI_CS)
#include "charset_international.h"
#define CS_NAME_INTERNAT "International"
#define CS_FONT_INTERNAT charset_international
#ifdef CS_INTERNAT
#define CS_NAME CS_NAME_INTERNAT
#define CS_FONT CS_FONT_INTERNAT
#endif
#endif

// Japanese
#if defined(CS_JAPANESE) || defined(MULTI_CS)
#include "charset_japanese.h"
#define CS_NAME_JAPANESE "Japanese"
#define CS_FONT_JAPANESE charset_japanese
#ifdef CS_JAPANESE
#define CS_NAME CS_NAME_JAPANESE
#define CS_FONT CS_FONT_JAPANESE
#endif
#endif

// Korean
#if defined(CS_KOREAN) || defined(MULTI_CS)
#include "charset_korean.h"
#define CS_NAME_KOREAN "Korean"
#define CS_FONT_KOREAN charset_korean
#ifdef CS_KOREAN
#define CS_NAME CS_NAME_KOREAN
#define CS_FONT CS_FONT_KOREAN
#endif
#endif

// Russian
#if defined(CS_RUSSIAN) || defined(MULTI_CS)
#include "charset_russian.h"
#define CS_NAME_RUSSIAN "Russian"
#define CS_FONT_RUSSIAN charset_russian
#ifdef CS_RUSSIAN
#define CS_NAME CS_NAME_RUSSIAN
#define CS_FONT CS_FONT_RUSSIAN
#endif
#endif

#endif