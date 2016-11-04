#pragma once
//--------------------------------------------------------------------------------
// Created by Justin Murphy
// Reference : Quynh Nguyen http://www.codeproject.com/KB/cpp/CppDelegateImplementation.aspx

#include <vector>
#include <map>

#include "Event.h"
#include "EventArgs.h"
//--------------------------------------------------------------------------------
namespace Events {
	class CEvent {
		friend class CEventManager;
		const CBaseEventArgs* m_pArgs;
		EVENTID m_ID;
		int m_nPriority;
	public:
		CEvent(const EVENTID& ID, const CBaseEventArgs* pArgs = 0, const int nPriority = 0);
		const CBaseEventArgs& GetArg() const;
		EVENTID GetID() const;
		int GetPriority() const;
	};
	//--------------------------------------------------------------------------------
	class CEventManager {
	public:
		CEventManager();
		~CEventManager() {};

		enum EVENT_PRIORITIES { eEventLow, eEventBelowNormal, eEventNormal, eEventAboveNormal, eEventHigh, eEventInstant };

		static CEventManager* GetInstance();
		static void CreateInstance();
		static void DestroyInstance();

		void SendEvent(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL, const EVENT_PRIORITIES ePriority = eEventNormal);
		void SendEventLow(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
		void SendEventBelowNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
		void SendEventNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
		void SendEventAboveNormal(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
		void SendEventHigh(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);
		void SendEventInstant(const EVENTID& id, const CBaseEventArgs* pEventArgs = NULL);

		template<class T, class Arg>
		bool RegisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&));

		template<class Arg>
		bool RegisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&));

		template<class T, class Arg>
		void UnregisterClientAll(T* p, void (T::*fn)(const Arg&));

		template<class Arg>
		void UnregisterClientAll(void(__cdecl *fn)(const Arg&));

		template<class T, class Arg>
		void UnregisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&));

		template<class Arg>
		void UnregisterClient(const EVENTID&, void(__cdecl *fn)(const Arg&));

		void ProcessEvents();
		void ClearAllEvents();
		void Shutdown();

	private:
		CEventManager(const CEventManager&);
		CEventManager& operator=(const CEventManager&);

		class MemberFunctorBase {
		protected:

			class NeverShouldExist;
			typedef void (NeverShouldExist::*thiscall_method)(const CBaseEventArgs&);
			typedef void(__cdecl NeverShouldExist::*cdecl_method)(const CBaseEventArgs&);
			typedef void(__stdcall NeverShouldExist::*stdcall_method)(const CBaseEventArgs&);
			typedef void(__fastcall NeverShouldExist::*fastcall_method)(const CBaseEventArgs&);
			typedef void(__cdecl *cdecl_function)(const CBaseEventArgs&);
			typedef void(__stdcall *stdcall_function)(const CBaseEventArgs&);
			typedef void(__fastcall *fastcall_function)(const CBaseEventArgs&);

			struct tLargestPointerType {
			private:
				char m_cNeverUse[sizeof(thiscall_method)];
			};

			NeverShouldExist* m_pObject;
			tLargestPointerType m_fpFunction;

		public:
			virtual void operator()(const CBaseEventArgs& pEventArgs) const = 0;
		};

		class MemberFunctorThisCall : public MemberFunctorBase {
		public:
			template<class ObjectType> MemberFunctorThisCall(ObjectType* p, void (ObjectType::*fn)(const CBaseEventArgs& pEventArgs));
			virtual void operator()(const CBaseEventArgs& pEventArgs) const;
		};

		class MemberFunctorCDeclCall : public MemberFunctorBase {
		public:
			MemberFunctorCDeclCall(void(__cdecl *fn)(const CBaseEventArgs&));
			virtual void operator()(const CBaseEventArgs& pEventArgs) const;
		};

		class MemberFunctor {
		private:
			char m_cData[sizeof(MemberFunctorBase)];
			const MemberFunctorBase& strategy() const;
		public:
			MemberFunctor(void(__cdecl *fn)(const CBaseEventArgs&));
			template<class T>
			MemberFunctor(T* p, void (T::*fn)(const CBaseEventArgs&));
			MemberFunctorBase const& operator*() const throw();
			void operator()(const CBaseEventArgs& pEventArgs) const;
			bool operator==(const MemberFunctor& rhs) const;
		};

		std::map<EVENTID, std::vector<MemberFunctor>> m_ClientDatabase;
		std::vector<CEvent> m_vLowEvents;
		std::vector<CEvent> m_vBelowNormalEvents;
		std::vector<CEvent> m_vNormalEvents;
		std::vector<CEvent> m_vAboveNormalEvents;
		std::vector<CEvent> m_vHighEvents;

		unsigned int m_nLowEventNum;
		unsigned int m_nBelowNormalEventNum;
		unsigned int m_nNormalEventNum;
		unsigned int m_nAboveNormalEventNum;
		unsigned int m_nHighEventNum;

		void DispatchEvent(const CEvent& pEvent);
		bool AlreadyRegistered(const EVENTID& id, const MemberFunctor& pClient);

	private:
		static CEventManager*	s_pInstance;
	};
	//--------------------------------------------------------------------------------
	inline CEvent::CEvent(const EVENTID& ID, const CBaseEventArgs* pArgs, const int nPriority) : m_ID(ID), m_pArgs(pArgs), m_nPriority(nPriority) {}
	//--------------------------------------------------------------------------------
	inline const CBaseEventArgs& CEvent::GetArg() const { return *m_pArgs; }
	//--------------------------------------------------------------------------------
	inline EVENTID CEvent::GetID() const { return m_ID; }
	//--------------------------------------------------------------------------------
	inline int CEvent::GetPriority() const { return m_nPriority; }
	//--------------------------------------------------------------------------------
	template<class ObjectType> inline CEventManager::MemberFunctorThisCall::MemberFunctorThisCall(ObjectType* p, void (ObjectType::*fn)(const CBaseEventArgs& pEventArgs)) {
		m_pObject = reinterpret_cast<NeverShouldExist*>(p);
		class AnotherNeverExistClass;
		typedef void (AnotherNeverExistClass::*LargePointerToMethod)(const CBaseEventArgs&);
		sizeof(LargePointerToMethod);
		class AnotherNeverExistClass : public ObjectType {};
		reinterpret_cast<LargePointerToMethod&>(m_fpFunction) = fn;
	}
	//--------------------------------------------------------------------------------
	inline void  CEventManager::MemberFunctorThisCall::operator()(const CBaseEventArgs& pEventArgs) const {
		(m_pObject->*(*(thiscall_method*)(&m_fpFunction)))(pEventArgs);
	}
	//--------------------------------------------------------------------------------
	inline  CEventManager::MemberFunctorCDeclCall::MemberFunctorCDeclCall(void(__cdecl *fn)(const CBaseEventArgs&)) {
		m_pObject = 0;
		reinterpret_cast<cdecl_function&>(m_fpFunction) = fn;
		// fill redundant bytes by ZERO for later sorting
		memset((char*)(&m_fpFunction) + sizeof(fn), 0, sizeof(m_fpFunction) - sizeof(fn));
	}
	//--------------------------------------------------------------------------------
	inline void  CEventManager::MemberFunctorCDeclCall::operator()(const CBaseEventArgs& pEventArgs) const {
		return (*(*(cdecl_function*)(&m_fpFunction)))(pEventArgs);
	}
	//--------------------------------------------------------------------------------
	inline const CEventManager::MemberFunctorBase& CEventManager::MemberFunctor::strategy() const {
		return *reinterpret_cast<MemberFunctorBase const*>(&m_cData);
	}
	//--------------------------------------------------------------------------------
	inline CEventManager::MemberFunctor::MemberFunctor(void(__cdecl *fn)(const CBaseEventArgs&)) {
		new (&m_cData) MemberFunctorCDeclCall(fn);
	}
	//--------------------------------------------------------------------------------
	inline bool CEventManager::MemberFunctor::operator==(const MemberFunctor& rhs) const {
		if (memcmp(m_cData, rhs.m_cData, sizeof(MemberFunctorBase)) == 0)
			return true;
		return false;
	}
	//--------------------------------------------------------------------------------
	inline void CEventManager::MemberFunctor::operator()(const CBaseEventArgs& pEventArgs) const {
		return strategy()(pEventArgs);
	}
	//--------------------------------------------------------------------------------
	template<class T> inline CEventManager::MemberFunctor::MemberFunctor(T* p, void (T::*fn)(const CBaseEventArgs&)) {
		new (&m_cData) MemberFunctorThisCall(p, fn);
	}
	//--------------------------------------------------------------------------------
	inline CEventManager::MemberFunctorBase const& CEventManager::MemberFunctor::operator*() const throw() {
		return strategy();
	}
	//--------------------------------------------------------------------------------
	inline CEventManager* EventManager() { return CEventManager::GetInstance(); }
	//--------------------------------------------------------------------------------
	template<class Arg> bool  CEventManager::RegisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&)) {
		if (fn == NULL)
			return false;
		MemberFunctor newClient(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn));
		map <EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);

		if (iter != m_ClientDatabase.end()) {
			vector<MemberFunctor> *clients = &(*iter).second;
			for (unsigned int x = 0; x<clients->size(); ++x)
				if ((*clients)[x] == newClient)
					return false;
			clients->push_back(newClient);
		}
		else {
			std::pair<EVENTID, vector<MemberFunctor>> newPair(id, vector<MemberFunctor>());
			newPair.second.push_back(newClient);
			m_ClientDatabase.insert(newPair);
		}
		return true;
	}
	//--------------------------------------------------------------------------------
	template<class T, class Arg> bool  CEventManager::RegisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&)) {
		using namespace std;

		if (p == NULL || fn == NULL)
			return false;
		MemberFunctor newClient(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn));
		map <EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);

		if (iter != m_ClientDatabase.end()) {
			vector<MemberFunctor> *clients = &(*iter).second;
			for (unsigned int x = 0; x<clients->size(); ++x)
				if ((*clients)[x] == newClient)
					return false;
			clients->push_back(newClient);
		}
		else {
			std::pair<EVENTID, vector<MemberFunctor>> newPair(id, vector<MemberFunctor>());
			newPair.second.push_back(newClient);
			m_ClientDatabase.insert(newPair);
		}
		return true;
	}
	//--------------------------------------------------------------------------------
	template<class T, class Arg> void CEventManager::UnregisterClientAll(T* p, void (T::*fn)(const Arg&)) {
		map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.begin();
		MemberFunctor clientToDelete(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn));
		while (iter != m_ClientDatabase.end()) {
			vector<MemberFunctor> *clients = &(*iter).second;
			for (unsigned int x = 0; x<clients->size(); ++x) {
				if ((*clients)[x] == clientToDelete) {
					clients->erase(clients->begin() + x);
					break;
				}
			}
			++iter;
		}
	}
	//--------------------------------------------------------------------------------
	template<class Arg> void CEventManager::UnregisterClientAll(void(__cdecl *fn)(const Arg&)) {
		using namespace std;
		map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.begin();
		MemberFunctor clientToDelete(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn));
		while (iter != m_ClientDatabase.end()) {
			vector<MemberFunctor> *clients = &(*iter).second;
			for (unsigned int x = 0; x<clients->size(); ++x) {
				if ((*clients)[x] == clientToDelete)
				{
					clients->erase(clients->begin() + x);
					break;
				}
			}
			++iter;
		}
	}
	//--------------------------------------------------------------------------------
	template<class T, class Arg> void  CEventManager::UnregisterClient(const EVENTID& id, T* p, void (T::*fn)(const Arg&)) {
		using namespace std;
		map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);
		if (m_ClientDatabase.end() == iter)
			return;
		MemberFunctor clientToDelete(p, reinterpret_cast<void (T::*)(const CBaseEventArgs&)>(fn));
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x) {
			if ((*clients)[x] == clientToDelete)
			{
				clients->erase(clients->begin() + x);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------
	template<class Arg> void  CEventManager::UnregisterClient(const EVENTID& id, void(__cdecl *fn)(const Arg&)) {
		map<EVENTID, vector<MemberFunctor>>::iterator iter = m_ClientDatabase.find(id);
		if (m_ClientDatabase.end() == iter)
			return;
		MemberFunctor clientToDelete(reinterpret_cast<void(__cdecl *)(const CBaseEventArgs&)>(fn));
		vector<MemberFunctor> *clients = &(*iter).second;
		for (unsigned int x = 0; x<clients->size(); ++x) {
			if ((*clients)[x] == clientToDelete) {
				clients->erase(clients->begin() + x);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------
};
//--------------------------------------------------------------------------------