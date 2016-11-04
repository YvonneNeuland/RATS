#pragma once
//--------------------------------------------------------------------------------
#include <map>
#include <set>
#include "../../Wwise files/Event.h"
namespace Events {

#include "Delegate.h"
	//--------------------------------------------------------------------------------
	struct delegate_root;
	//--------------------------------------------------------------------------------
	class MessageSystem {
	public:
		static MessageSystem* GetInstance();
		static void CreateInstance();
		static void DestroyInstance();
		~MessageSystem() {};

		bool UnregisterRegisterMessage(EVENTID _msg);

		template<typename retVal, typename ...Parameters>
		bool RegisterMessage(EVENTID _msg, retVal(*_Function)(Parameters...)) {
			return AddDelegate(_msg, new Delegate<retVal, Parameters...>(_Function));
		}
		template<typename type, typename retVal, typename ...Parameters>
		bool RegisterMessage(EVENTID _msg, type *_obj, retVal(type::*_Function)(Parameters...)) {
			return AddDelegate(_msg, new Delegate<retVal, Parameters...>(_obj, _Function));
		}
		template<typename type, typename retVal, typename ...Parameters>
		bool RegisterMessage(EVENTID _msg, type *_obj, retVal(type::*_Function)(Parameters...) const) {
			//becuase the user of this system has no idea if the function is const protects the function
			//You are unable to bind a const protected functions
			//If you took this overload out, the system would crash on the Call function
			//COMPILE_TIME_ASSERT(0);
			return false;
		}

		template<typename retVal, typename ...Parameters>
		retVal Call(EVENTID _msg, Parameters..._input) {
			delegate_root *del = GetDelegate(_msg);

			if (nullptr == del) {
				std::runtime_error("call to invalid Message");
			}
			//(*func)() instead of func() to call a function
			return (*(*static_cast<Delegate<retVal, Parameters...> *>(del)))(std::forward<Parameters>(_input)...);
		}

		void ShutDown();
	private:
		static MessageSystem*	s_pInstance;

		std::map<EVENTID, delegate_root *>	m_MessageList;
		std::set<EVENTID>					m_MessageLog;

		typedef std::map<EVENTID, delegate_root *>::iterator	MsgItr;
		typedef std::pair<EVENTID, delegate_root *>				Data;

		MessageSystem();
		MessageSystem(const MessageSystem&);
		MessageSystem(MessageSystem&&);
		MessageSystem& operator=(const MessageSystem&);
		MessageSystem& operator=(MessageSystem&&);

		bool AddDelegate(const EVENTID & _msg, delegate_root *_in);
		delegate_root * GetDelegate(const EVENTID & _msg);
	};
};
//--------------------------------------------------------------------------------