#include "StdAfx.h"
#include "Packet.h"
#include "ServerStateChecker.h"

CServerStateChecker::CServerStateChecker()
{
	Initialize();
}

CServerStateChecker::~CServerStateChecker()
{
	Initialize();
	m_poWnd = nullptr;
}

void CServerStateChecker::Create(PyObject* poWnd)
{
	m_poWnd = poWnd;
}

void CServerStateChecker::AddChannel(uint32_t idx, const char* addr, uint16_t port)
{
	TChannel c;
	c.idx = idx;
	c.addr = addr;
	c.port = port;

	m_lstChannel.emplace_back(c);
}

void CServerStateChecker::Request()
{
	if (m_lstChannel.empty())
	{
		return;
	}

	auto it = m_lstChannel.begin();

	if (!m_kStream.Connect(it->addr, it->port))
	{
		for (const auto& it : m_lstChannel)
		{
			PyCallClassMemberFunc(m_poWnd, "NotifyChannelState", Py_BuildValue("(ii)", it.idx, 0));
		}

		return;
	}

	m_kStream.ClearRecvBuffer();
	m_kStream.SetSendBufferSize(1024);
	m_kStream.SetRecvBufferSize(1024);

	const uint8_t bHeader = HEADER_CG_STATE_CHECKER;
	if (!m_kStream.Send(sizeof(bHeader), &bHeader))
	{
		for (const auto& [idx, addr, port] : m_lstChannel)
		{
			PyCallClassMemberFunc(m_poWnd, "NotifyChannelState", Py_BuildValue("(ii)", idx, 0));
		}

		Initialize();
	}
}

void CServerStateChecker::Update()
{
	m_kStream.Process();

	uint8_t bHeader;
	if (!m_kStream.Recv(sizeof(uint8_t), &bHeader))
	{
		return;
	}

	if (bHeader != HEADER_GC_RESPOND_CHANNELSTATUS)
	{
		return;
	}

	int32_t size;
	if (!m_kStream.Recv(sizeof(int32_t), &size))
	{
		return;
	}

	for (int i = 0; i < size; i++)
	{
		TChannelStatus p;
		if (!m_kStream.Recv(sizeof(TChannelStatus), &p))
		{
			return;
		}

		for (const auto& [idx, addr, port] : m_lstChannel)
		{
			if (p.port == port)
			{
				PyCallClassMemberFunc(m_poWnd, "NotifyChannelState", Py_BuildValue("(ii)", idx, p.status));
				break;
			}
		}
	}

	Initialize();
}

void CServerStateChecker::Initialize()
{
	m_lstChannel.clear();
	m_kStream.Disconnect();
}
