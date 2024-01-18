#include "stdafx.h"

int32_t is_hangul(const uint8_t* str)
{
	if (str[0] >= 0xb0 && str[0] <= 0xc8 && str[1] >= 0xa1 && str[1] <= 0xfe)
		return 1;

	return 0;
}

int32_t check_han(const char* str)
{
	int32_t i, code;

	if (!str || !*str)
		return 0;

	for (i = 0; str[i]; i += 2)
	{
		if (isnhspace(str[i]))
			return 0;

		if (isalpha(str[i]) || isdigit(str[i]))
			continue;

		code = str[i];
		code += 256;

		if (code < 176 || code > 200)
			return 0;
	}

	return 1;
}

const char* first_han(const uint8_t* str)
{
	uint8_t high, low;
	int32_t len, i;
	const char* p = "±×¿Ü";

	static const char* wansung[] =
	{
		"°¡", "°¡", "³ª", "´Ù", "´Ù",
		"¶ó", "¸¶", "¹Ù", "¹Ù", "»ç",
		"»ç", "¾Æ", "ÀÚ", "ÀÚ", "Â÷",
		"Ä«", "Å¸", "ÆÄ", "ÇÏ", ""
	};

	static const char* johab[] =
	{
		"ˆa", "Œa", "a", "”a", "˜a",
		"œa", " a", "¤a", "¨a", "¬a",
		"°a", "´a", "¸a", "¼a", "Àa",
		"Äa", "?", "?", "?", "" 
	};

	len = strlen((const char*)str);

	if (len < 2)
		return p;

	high = str[0];
	low = str[1];

	if (!is_hangul(str))
	{
		return p;
	}

	high = (KStbl[(high - 0xb0) * 94 + low - 0xa1] >> 8) & 0x7c;

	for (i = 0; johab[i][0]; i++)
	{
		low = (johab[i][0] & 0x7f);

		if (low == high)
			return (wansung[i]);
	}

	return (p);
}

int32_t under_han(const void* orig)
{
	const uint8_t* str = (const uint8_t*)orig;
	uint8_t high, low;
	int32_t len;

	len = strlen((const char*)str);

	if (len < 2)
		return 0;

	if (str[len - 1] == ')')
	{
		while (str[len] != '(')
			len--;
	}

	high = str[len - 2];
	low = str[len - 1];

	if (!is_hangul(&str[len - 2]))
		return 0;

	high = KStbl[(high - 0xb0) * 94 + low - 0xa1] & 0x1f;

	if (high < 2)
		return 0;

	if (high > 28)
		return 0;

	return 1;
}
