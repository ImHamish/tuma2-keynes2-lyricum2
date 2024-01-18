#include "StdAfx.h"
#if defined(ENABLE_TEXTS_RENEWAL)
#include <map>
#include <string>

//using namespace std;

#include "TextTrans.h"
#include "../eterPack/EterPackManager.h"

int CTextTrans::Load(const char* localePath) {
	m_String.clear();

	char szStringList[256];
	snprintf(szStringList, sizeof(szStringList), "%s/locale_string.txt", localePath);

	CMappedFile file;
	const VOID* pvData;
	if (!CFileSystem::Instance().Get(file, szStringList, &pvData, __FUNCTION__)) {
		return -2;
	}

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(file.Size(), pvData);

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kMemTextFileLoader.GetLineCount(); ++i) {
		if (!kMemTextFileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		while (kTokenVector.size() < 2) {
			kTokenVector.push_back("");
		}

		DWORD idx = atoi(kTokenVector[0].c_str());
		const std::string& text = kTokenVector[1].c_str();
		AddStringText(idx, text);
	}
	
	return 1;
}

void CTextTrans::AddStringText(DWORD idx, const std::string& text) {
	if (text.empty()) {
		return;
	}

	m_String.insert(std::pair<DWORD, std::string> (idx, text));
#ifdef _DEBUG
	Tracef("CTextTrans<m_String>: %d : %s, new size(%d).\n", idx, text.c_str(), m_String.size());
#endif
}

std::string CTextTrans::GetStringText(DWORD idx) {
	auto it = m_String.find(idx);
	if (it != m_String.end()) {
		return it->second;
	}

	return "";
}

CTextTrans::CTextTrans(void) {
}

CTextTrans::~CTextTrans(void)
{
	m_String.clear();
}
#endif
