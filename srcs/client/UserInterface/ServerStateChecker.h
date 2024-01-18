#pragma once

#include "../eterLib/NetStream.h"

class CServerStateChecker : public CSingleton<CServerStateChecker>
{
	public:
		CServerStateChecker();
		virtual ~CServerStateChecker();

	private:
		typedef struct SChannel
		{
			uint32_t idx;
			const char* addr;
			uint16_t port;
		} TChannel;

		PyObject* m_poWnd{};

		std::list<TChannel> m_lstChannel;

		CNetworkStream m_kStream;

	public:
		void Create(PyObject* poWnd);

		void AddChannel(uint32_t, const char*, uint16_t);

		void Request();
		void Update();

		void Initialize();
};
