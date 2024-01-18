#pragma once

#include <map>

class CNetworkPacketHeaderMap
{
	public:
		typedef struct SPacketType
		{
			SPacketType(int32_t iSize = 0, uint8_t bType = 0)
			{
				iPacketSize = iSize;
				iPacketType = bType;
			}

			int32_t iPacketSize;
			uint8_t iPacketType;
		} TPacketType;
	public:
		CNetworkPacketHeaderMap();
		virtual ~CNetworkPacketHeaderMap();

		void Set(int header, TPacketType & rPacketType);
		bool Get(int header, TPacketType * pPacketType);

	protected:
		std::map<int, TPacketType> m_headerMap;
};
