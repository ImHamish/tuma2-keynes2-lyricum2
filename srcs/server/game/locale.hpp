#pragma once

extern "C"
{
	const char *locale_convert_language(BYTE bLanguage);
	extern int g_iUseLocale;
	#define LC_CONVERT(lang) locale_convert_language(lang)
};
