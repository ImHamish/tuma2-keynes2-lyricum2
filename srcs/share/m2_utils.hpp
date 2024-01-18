#ifndef SHARE_UTILS_HPP
#define SHARE_UTILS_HPP

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <stdint.h>
#include <string>
#include <algorithm>
#include <regex>
#include <set>

inline bool str_to_number(bool& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (strtol(in, nullptr, 10) != 0);
    return true;
}

inline bool str_to_bool(bool& out, const std::string &in)
{
    out = in.at(0) == '1';
    return true;
}

inline bool str_to_number(char& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (char)strtol(in, nullptr, 10);
    return true;
}

inline bool str_to_number(unsigned char& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (unsigned char)strtoul(in, nullptr, 10);
    return true;
}

inline bool str_to_number(int16_t& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (int16_t)strtol(in, nullptr, 10);
    return true;
}

inline bool str_to_number(uint16_t& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (uint16_t)strtoul(in, nullptr, 10);
    return true;
}

inline bool str_to_number(int32_t& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (int32_t)strtol(in, nullptr, 10);
    return true;
}

inline bool str_to_number(uint32_t& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (uint32_t)strtoul(in, nullptr, 10);
    return true;
}

inline bool str_to_number(long& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (long)strtol(in, nullptr, 10);
    return true;
}

inline bool str_to_number(unsigned long& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (unsigned long)strtoul(in, nullptr, 10);
    return true;
}

inline bool str_to_number(long long& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (long long)strtoull(in, nullptr, 10);
    return true;
}


inline bool str_to_number(unsigned long long& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (unsigned long long)strtoull(in, nullptr, 10);
    return true;
}

inline bool str_to_number(float& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (float)strtof(in, nullptr);
    return true;
}

inline bool str_to_number(double& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (double)strtod(in, nullptr);
    return true;
}

#ifdef __FreeBSD__
inline bool str_to_number(long double& out, const char *in)
{
    if (in == 0 || in[0] == 0)
    {
        return false;
    }

    out = (long double)strtold(in, nullptr);
    return true;
}
#endif

inline bool is_positive_number(const std::string & str)
{
    if (str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+')))
    {
        return false;
    }

    char* p;
    strtol(str.c_str(), &p, 10);

    return (*p == 0);
}

inline std::string uri_encode(const std::string & sSrc)
{
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char* pSrc = (const unsigned char *)sSrc.c_str();
    const int32_t SRC_LEN = sSrc.length();
    unsigned char* const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char* pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
    {
        char c = *pSrc;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            *pEnd++ = *pSrc;
        }
        else
        {
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *)pStart, (char *)pEnd);
    delete[] pStart;
    return sResult;
}

int32_t uniform_random(int32_t a, int32_t b);
double uniform_random(double a, double b);

namespace m2
{
    template<typename T>
    T minmax(T min, T value, T max)
    {
        T tv;
        tv = (min > value ? min : value);
        return (max < tv) ? max : tv;
    }
}

#ifdef USE_PICKUP_FILTER
inline void split_argument(const std::string& stArg, std::vector<std::string>& r_vecArgs, const char* szArg2 = " ")
#else
inline void split_argument(const std::string& stArg, std::vector<std::string>& r_vecArgs)
#endif
{
#ifdef USE_PICKUP_FILTER
    const std::regex re{ szArg2 };
#else
    const std::regex re{ " " };
#endif
    const std::sregex_token_iterator first{ stArg.begin(), stArg.end(), re, -1 };
    const std::sregex_token_iterator last;
    r_vecArgs = { first, last };
}

inline std::set<std::string> g_country_flags = {
	"af",
	"al",
	"dz",
	"ad",
	"ao",
	"ar",
	"am",
	"au",
	"at",
	"az",
	"bs",
	"bh",
	"bd",
	"bb",
	"by",
	"be",
	"bz",
	"bj",
	"bt",
	"bw",
	"br",
	"bg",
	"bi",
	"kh",
	"cm",
	"ca",
	"td",
	"cl",
	"cn",
	"co",
	"km",
	"cg",
	"cu",
	"cy",
	"dj",
	"ec",
	"eg",
	"er",
	"et",
	"fi",
	"fr",
	"ga",
	"gm",
	"ge",
	"de",
	"gh",
	"gi",
	"gr",
	"gl",
	"gu",
	"gt",
	"gw",
	"gn",
	"gy",
	"ht",
	"hn",
	"hu",
	"is",
	"in",
	"id",
	"iq",
	"ie",
	"il",
	"it",
	"jm",
	"jp",
	"je",
	"jo",
	"kz",
	"ke",
	"kw",
	"kg",
	"lv",
	"lb",
	"ls",
	"lr",
	"ly",
	"li",
	"lt",
	"lu",
	"mg",
	"mw",
	"mv",
	"ml",
	"mt",
	"mr",
	"mu",
	"yt",
	"mx",
	"mc",
	"mn",
	"ms",
	"ma",
	"mz",
	"na",
	"nr",
	"np",
	"nl",
	"ni",
	"ne",
	"ng",
	"nu",
	"no",
	"om",
	"pk",
	"pw",
	"pa",
	"py",
	"pe",
	"ph",
	"pl",
	"pt",
	"qa",
	"ro",
	"rw",
	"ws",
	"sn",
	"rs",
	"sc",
	"sg",
	"sk",
	"si",
	"so",
	"es",
	"sd",
	"sr",
	"se",
	"ch",
	"tj",
	"th",
	"tl",
	"tg",
	"to",
	"tn",
	"tr",
	"tm",
	"ug",
	"ua",
	"uy",
	"uz",
	"vu",
	"ye",
	"zm",
	"zw",
};

#endif // SHARE_UTILS_HPP
