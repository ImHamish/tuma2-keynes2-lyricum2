#pragma once

#if defined(ENABLE_TEXTS_RENEWAL)
class CTextTrans
{
	public:
		CTextTrans(void);
		virtual ~CTextTrans(void);

		int Load(const char* localePath);

		void AddStringText(DWORD idx, const std::string& text);

		std::string GetStringText(DWORD idx);

	private:
		std::map<DWORD, std::string> m_Quest, m_String;
};
#endif
