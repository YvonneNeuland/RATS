#include "stdafx.h"
#include <algorithm>
#include <assert.h>

#include "EventManager.h"

namespace Events {
	//--------------------------------------------------------------------------------
	CEventManager* CEventManager::s_pInstance = NULL;
	using namespace std;
	//--------------------------------------------------------------------------------
	CEventManager * CEventManager::GetInstance() {
#if _DEBUG
		if (!s_pInstance)
			assert(0); // "call to CEventManager::GetInstance() before being created"
#endif
		return s_pInstance;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::CreateInstance() {
		if (!s_pInstance)
			s_pInstance = new CEventManager();
	}
	//--------------------------------------------------------------------------------
	void CEventManager::DestroyInstance() {
		if (s_pInstance)
			delete s_pInstance;
		s_pInstance = 0;
	}
	//--------------------------------------------------------------------------------
	CEventManager::CEventManager() : m_nLowEventNum(0), m_nAboveNormalEventNum(0), m_nBelowNormalEventNum(0),
		m_nHighEventNum(0), m_nNormalEventNum(0) {
		if (s_pInstance)
			//Log("CEventManager is trying to be created more than once.");
		assert(!s_pInstance);
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEvent(const EVENTID& id, const CBaseEventArgs* pEventArgs, const EVENT_PRIORITIES ePriority) {
		switch (ePriority) {
		case CEventManager::eEventInstant: {
			CEvent immediateEvent(id, pEventArgs, ePriority);
			DispatchEvent(immediateEvent);
			if (pEventArgs != NULL)	{ delete pEventArgs; }
		}
										   break;
		case CEventManager::eEventAboveNormal: {
			if (m_nAboveNormalEventNum < m_vAboveNormalEvents.size()) {
				m_vAboveNormalEvents[m_nAboveNormalEventNum].m_ID = id;
				m_vAboveNormalEvents[m_nAboveNormalEventNum].m_pArgs = pEventArgs;
				m_vAboveNormalEvents[m_nAboveNormalEventNum].m_nPriority = ePriority;
			}
			else
				m_vAboveNormalEvents.push_back(CEvent(id, pEventArgs, ePriority));
			++m_nAboveNormalEventNum;
		}
											   break;
		case CEventManager::eEventBelowNormal: {
			if (m_nBelowNormalEventNum < m_vBelowNormalEvents.size()) {
				m_vBelowNormalEvents[m_nAboveNormalEventNum].m_ID = id;
				m_vBelowNormalEvents[m_nAboveNormalEventNum].m_pArgs = pEventArgs;
				m_vBelowNormalEvents[m_nAboveNormalEventNum].m_nPriority = ePriority;
			}
			else
				m_vBelowNormalEvents.push_back(CEvent(id, pEventArgs, ePriority));
			++m_nBelowNormalEventNum;
		}
											   break;
		case CEventManager::eEventHigh: {
			if (m_nHighEventNum < m_vHighEvents.size())
			{
				m_vHighEvents[m_nHighEventNum].m_ID = id;
				m_vHighEvents[m_nHighEventNum].m_pArgs = pEventArgs;
				m_vHighEvents[m_nHighEventNum].m_nPriority = ePriority;
			}
			else
				m_vHighEvents.push_back(CEvent(id, pEventArgs, ePriority));
			++m_nHighEventNum;
		}
										break;
		case CEventManager::eEventLow: {
			if (m_nLowEventNum < m_vLowEvents.size()) {
				m_vLowEvents[m_nLowEventNum].m_ID = id;
				m_vLowEvents[m_nLowEventNum].m_pArgs = pEventArgs;
				m_vLowEvents[m_nLowEventNum].m_nPriority = ePriority;
			}
			else
				m_vLowEvents.push_back(CEvent(id, pEventArgs, ePriority));
			++m_nLowEventNum;
		}
									   break;
		case CEventManager::eEventNormal: {
			if (m_nNormalEventNum < m_vNormalEvents.size()) {
				m_vNormalEvents[m_nNormalEventNum].m_ID = id;
				m_vNormalEvents[m_nNormalEventNum].m_pArgs = pEventArgs;
				m_vNormalEvents[m_nNormalEventNum].m_nPriority = ePriority;
			}
			else
				m_vNormalEvents.push_back(CEvent(id, pEventArgs, ePriority));
			++m_nNormalEventNum;
		}
										  break;
		}
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventInstant(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		CEvent immediateEvent(id, pEventArgs, eEventInstant);
		DispatchEvent(immediateEvent);
		if (nullptr != pEventArgs)	{ delete pEventArgs; }
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventLow(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		if (m_nLowEventNum < m_vLowEvents.size()) {
			m_vLowEvents[m_nLowEventNum].m_ID = id;
			m_vLowEvents[m_nLowEventNum].m_pArgs = pEventArgs;
			m_vLowEvents[m_nLowEventNum].m_nPriority = eEventLow;
		}
		else
			m_vLowEvents.push_back(CEvent(id, pEventArgs, eEventLow));
		++m_nLowEventNum;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventBelowNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		if (m_nBelowNormalEventNum < m_vBelowNormalEvents.size()) {
			m_vBelowNormalEvents[m_nAboveNormalEventNum].m_ID = id;
			m_vBelowNormalEvents[m_nAboveNormalEventNum].m_pArgs = pEventArgs;
			m_vBelowNormalEvents[m_nAboveNormalEventNum].m_nPriority = eEventBelowNormal;
		}
		else
			m_vBelowNormalEvents.push_back(CEvent(id, pEventArgs, eEventBelowNormal));
		++m_nBelowNormalEventNum;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		if (m_nNormalEventNum < m_vNormalEvents.size()) {
			m_vNormalEvents[m_nNormalEventNum].m_ID = id;
			m_vNormalEvents[m_nNormalEventNum].m_pArgs = pEventArgs;
			m_vNormalEvents[m_nNormalEventNum].m_nPriority = eEventNormal;
		}
		else
			m_vNormalEvents.push_back(CEvent(id, pEventArgs, eEventNormal));
		++m_nNormalEventNum;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventAboveNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		if (m_nAboveNormalEventNum < m_vAboveNormalEvents.size()) {
			m_vAboveNormalEvents[m_nAboveNormalEventNum].m_ID = id;
			m_vAboveNormalEvents[m_nAboveNormalEventNum].m_pArgs = pEventArgs;
			m_vAboveNormalEvents[m_nAboveNormalEventNum].m_nPriority = eEventAboveNormal;
		}
		else
			m_vAboveNormalEvents.push_back(CEvent(id, pEventArgs, eEventAboveNormal));
		++m_nAboveNormalEventNum;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::SendEventHigh(const EVENTID& id, const CBaseEventArgs* pEventArgs) {
		if (m_nHighEventNum < m_vHighEvents.size()) {
			m_vHighEvents[m_nHighEventNum].m_ID = id;
			m_vHighEvents[m_nHighEventNum].m_pArgs = pEventArgs;
			m_vHighEvents[m_nHighEventNum].m_nPriority = eEventHigh;
		}
		else
			m_vHighEvents.push_back(CEvent(id, pEventArgs, eEventHigh));
		++m_nHighEventNum;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::DispatchEvent(const CEvent& pEvent) {
		vector<MemberFunctor> *clients = &m_ClientDatabase[pEvent.GetID()];
		for (unsigned int x = 0; x < clients->size(); ++x)
			(*(*clients)[x])(pEvent.GetArg());
	}
	//--------------------------------------------------------------------------------
	bool CEventManager::AlreadyRegistered(const EVENTID& id, const MemberFunctor& pClient) {
		map <EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);

		if (iter == m_ClientDatabase.end())
			return false;

		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x < clients->size(); ++x)
			if ((*clients)[x] == pClient)
				return true;
		return false;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::ProcessEvents() {
		for (unsigned int x = 0; x < m_nHighEventNum; ++x) {
			DispatchEvent(m_vHighEvents[x]);
			if (nullptr != m_vHighEvents[x].m_pArgs) {
				delete m_vHighEvents[x].m_pArgs;
				m_vHighEvents[x].m_pArgs = 0;
			}
		}

		for (unsigned int x = 0; x < m_nAboveNormalEventNum; ++x) {
			DispatchEvent(m_vAboveNormalEvents[x]);
			if (nullptr != m_vAboveNormalEvents[x].m_pArgs) {
				delete m_vAboveNormalEvents[x].m_pArgs;
				m_vAboveNormalEvents[x].m_pArgs = 0;
			}
		}

		for (unsigned int x = 0; x < m_nNormalEventNum; ++x) {
			DispatchEvent(m_vNormalEvents[x]);
			if (nullptr != m_vNormalEvents[x].m_pArgs) {
				delete m_vNormalEvents[x].m_pArgs;
				m_vNormalEvents[x].m_pArgs = 0;
			}
		}

		for (unsigned int x = 0; x < m_nBelowNormalEventNum; ++x) {
			DispatchEvent(m_vBelowNormalEvents[x]);
			if (nullptr != m_vBelowNormalEvents[x].m_pArgs) {
				delete m_vBelowNormalEvents[x].m_pArgs;
				m_vBelowNormalEvents[x].m_pArgs = 0;
			}
		}

		for (unsigned int x = 0; x < m_nLowEventNum; ++x) {
			DispatchEvent(m_vLowEvents[x]);
			if (nullptr != m_vLowEvents[x].m_pArgs) {
				delete m_vLowEvents[x].m_pArgs;
				m_vLowEvents[x].m_pArgs = 0;
			}
		}

		m_nLowEventNum = 0;
		m_nBelowNormalEventNum = 0;
		m_nNormalEventNum = 0;
		m_nAboveNormalEventNum = 0;
		m_nHighEventNum = 0;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::ClearAllEvents() {
		for (unsigned int x = 0; x < m_nHighEventNum; x++)
			if (nullptr != m_vHighEvents[x].m_pArgs) {
				delete m_vHighEvents[x].m_pArgs;
				m_vHighEvents[x].m_pArgs = 0;
			}

		for (unsigned int x = 0; x < m_nAboveNormalEventNum; ++x)
			if (nullptr != m_vAboveNormalEvents[x].m_pArgs) {
				delete m_vAboveNormalEvents[x].m_pArgs;
				m_vAboveNormalEvents[x].m_pArgs = 0;
			}

		for (unsigned int x = 0; x < m_nNormalEventNum; ++x)
			if (nullptr != m_vNormalEvents[x].m_pArgs) {
				delete m_vNormalEvents[x].m_pArgs;
				m_vNormalEvents[x].m_pArgs = 0;
			}

		for (unsigned int x = 0; x < m_nBelowNormalEventNum; ++x)
			if (nullptr != m_vBelowNormalEvents[x].m_pArgs) {
				delete m_vBelowNormalEvents[x].m_pArgs;
				m_vBelowNormalEvents[x].m_pArgs = 0;
			}

		for (unsigned int x = 0; x < m_nLowEventNum; ++x)
			if (nullptr != m_vLowEvents[x].m_pArgs) {
				delete m_vLowEvents[x].m_pArgs;
				m_vLowEvents[x].m_pArgs = 0;
			}

		m_nLowEventNum = 0;
		m_nBelowNormalEventNum = 0;
		m_nNormalEventNum = 0;
		m_nAboveNormalEventNum = 0;
		m_nHighEventNum = 0;
	}
	//--------------------------------------------------------------------------------
	void CEventManager::Shutdown() {
		ClearAllEvents();
		m_ClientDatabase.clear();
	}
	//--------------------------------------------------------------------------------
};