#include "stdafx.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "PetSystem.h"
#if defined(__NEWPET_SYSTEM__)
#include "New_PetSystem.h"
#endif

CEntity::CEntity()
{
	Initialize();
}

CEntity::~CEntity()
{
	if (!m_bIsDestroyed)
		assert(!"You must call CEntity::destroy() method in your derived class destructor");
}

void CEntity::Initialize(int type)
{
	m_bIsDestroyed = false;

	m_iType = type;
	m_iViewAge = 0;
	m_pos.x = m_pos.y = m_pos.z = 0;
	m_map_view.clear();

	m_pSectree = NULL;
	m_lpDesc = NULL;
	m_lMapIndex = 0;
	m_bIsObserver = false;
	m_bObserverModeChange = false;
}

void CEntity::Destroy()
{
	if (m_bIsDestroyed) {
		return;
	}
	ViewCleanup();
	m_bIsDestroyed = true;
}

void CEntity::SetType(int type)
{
	m_iType = type;
}

int CEntity::GetType() const
{
	return m_iType;
}

bool CEntity::IsType(int type) const
{
	return (m_iType == type ? true : false);
}

struct FuncPacketAround
{
	const void *        m_data;
	int                 m_bytes;
	LPENTITY            m_except;

	FuncPacketAround(const void * data, int bytes, LPENTITY except = NULL) :m_data(data), m_bytes(bytes), m_except(except)
	{
	}

	void operator () (LPENTITY ent)
	{
		if (ent == m_except)
			return;

		const LPDESC d = ent->GetDesc();

		if (d) {
			d->Packet(m_data, m_bytes);
		}
	}
};

struct FuncPacketView : public FuncPacketAround
{
	FuncPacketView(const void * data, int bytes, LPENTITY except = NULL) : FuncPacketAround(data, bytes, except)
	{}

	void operator() (const CEntity::ENTITY_MAP::value_type& v)
	{
		FuncPacketAround::operator() (v.first);
	}
};

void CEntity::PacketAround(const void * data, int bytes, LPENTITY except)
{
	PacketView(data, bytes, except);
}

void CEntity::PacketView(const void * data, int bytes, LPENTITY except)
{
	if (!GetSectree())
		return;

	FuncPacketView f(data, bytes, except);

	if (!m_bIsObserver) {
		for_each(m_map_view.begin(), m_map_view.end(), f);
	}

	f(std::make_pair(this, 0));
}

void CEntity::SetObserverMode(bool bFlag
#if defined(ENABLE_RENEWAL_OX)
, bool bOXAttender
#endif
, bool bObserverModeChange
)
{
	if (m_bIsObserver == bFlag) {
		return;
	}

#if defined(ENABLE_RENEWAL_OX)
	if (bOXAttender != true && IsType(ENTITY_CHARACTER)) {
		const LPCHARACTER pChar = (LPCHARACTER) this;

		if (pChar->GetMapIndex() == OX_MAP_INDEX && pChar->GetGMLevel() == GM_PLAYER) {
			return;
		}
	}
#endif

	m_bIsObserver = bFlag;
	m_bObserverModeChange = bObserverModeChange;
	UpdateSectree();

#if defined(ENABLE_RENEWAL_OX)
	if (bOXAttender != true && IsType(ENTITY_CHARACTER)) {
#else
	if (IsType(ENTITY_CHARACTER)) {
#endif
		LPCHARACTER ch = (LPCHARACTER) this;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ObserverMode %d", m_bIsObserver ? 1 : 0);

		LPCHARACTER chHorse = ch->GetHorse();
		if (chHorse) {
			chHorse->SetObserverMode(m_bIsObserver);
		}

		auto petSystem = ch->GetPetSystem();
		if (petSystem) {
			petSystem->UpdateObserver(m_bIsObserver);
		}

#if defined(__NEWPET_SYSTEM__)
		auto newPetSystem = ch->GetNewPetSystem();
		if (newPetSystem) {
			newPetSystem->UpdateObserver(m_bIsObserver);
		}
#endif
	}
}
