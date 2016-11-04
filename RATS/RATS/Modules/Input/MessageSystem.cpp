#include "stdafx.h"
#include "MessageSystem.h"
//#include "./Log.h"
#include "Delegate.h"
namespace Events {
	//--------------------------------------------------------------------------------
	MessageSystem* MessageSystem::s_pInstance = nullptr;
	//--------------------------------------------------------------------------------
	MessageSystem::MessageSystem() {

	}
	//--------------------------------------------------------------------------------
	void MessageSystem::DestroyInstance() {
		if (s_pInstance)
		{
			s_pInstance->ShutDown();
			delete s_pInstance;
			s_pInstance = 0;
		}
	}
	//--------------------------------------------------------------------------------
	MessageSystem * MessageSystem::GetInstance() {
		if (!s_pInstance) {
			std::cout << "call to CEventManager::GetInstance() before being created\n";
		}

		return s_pInstance;
	}
	//--------------------------------------------------------------------------------
	void MessageSystem::CreateInstance() {
		if (!s_pInstance)
			s_pInstance = new MessageSystem();
	}
	//--------------------------------------------------------------------------------

	bool MessageSystem::AddDelegate(const EVENTID & _msg, delegate_root *_in) {
		if (_in == NULL)
			return false;
		MsgItr itr = m_MessageList.find(_msg);
		if (itr == m_MessageList.end()) {
			Data data(_msg, _in);
			m_MessageList.insert(data);
			m_MessageLog.insert(_msg);
			return true;
		}
		std::cout << "Message "<< _msg << " Was already registered.  Function will not be added\n";
		return false;
	}
	//--------------------------------------------------------------------------------
	delegate_root * MessageSystem::GetDelegate(const EVENTID & _msg) {
		MsgItr itr = m_MessageList.find(_msg);
		if (itr != m_MessageList.end()) {
			return itr->second;
		}
		if (m_MessageLog.end() != m_MessageLog.find(_msg)) {
			std::cout << "Message "<< _msg<< " Was unregistered\n";
			return nullptr;
		}

		std::cout << "Message "<< _msg << " Was not registered\n";
		return nullptr;
	}
	//--------------------------------------------------------------------------------
	void MessageSystem::ShutDown() {
		for (auto itr = m_MessageList.begin(); itr != m_MessageList.end(); ++itr) {
			delete itr->second;
		}
		m_MessageList.clear();
		m_MessageLog.clear();
	}
	//--------------------------------------------------------------------------------
	bool MessageSystem::UnregisterRegisterMessage(EVENTID _msg) {
		MsgItr itr = m_MessageList.find(_msg);
		if (itr == m_MessageList.end()) {
			delete itr->second;
			m_MessageList.erase(itr);
			return true;
		}

		std::cout << "Message "<< _msg<< " Is not registered\n";
		return false;
	}
	//--------------------------------------------------------------------------------
};