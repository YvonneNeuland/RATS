#include "stdafx.h"
#include "InputController.h"
#include "MessageSystem.h"
//#include "./Log.h"
#include <Windows.h>
#include "../TinyXml/tinystr.h"
#include "../TinyXml/tinyxml.h"
#include <cassert>
#include <fstream>

//--------------------------------------------------------------------------------
bool InputController::Init() {
	using namespace Events;
	MessageSystem *ms = MessageSystem::GetInstance();
	if (!ms) {
		std::cout << "cannot create input controller before the Message system has been created";
		return false;
	}

	
	
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyDown", this, &InputController::GetKeyDownEvent);
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyPress", this, &InputController::GetKeyPressEvent);
	ms->RegisterMessage<InputController, const EVENTID&, const EVENTID>("GetKeyUp", this, &InputController::GetKeyUpEvent);
	ms->RegisterMessage<InputController, bool, EVENTID>("RemoveKey", this, &InputController::UnRegisterEvent);
	ms->RegisterMessage<InputController, bool, const EVENTID, const unsigned char>("SwitchKeyDown", this, &InputController::SwithKeyDownEvent);
	
	m_loadPath = "../../Assets/Binds.xml";

	return true;
}
//--------------------------------------------------------------------------------
bool InputController::ShutDown() {
	m_keyDown.clear();
	m_keyPress.clear();
	m_keyUp.clear();
	m_codes.clear();

	return true;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyDownEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyDown.find(_key);
	if (itr == m_keyDown.end()) {
		std::cout << "No event at param _key (GETKEYDOWNEVENT())\n";
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyPressEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyPress.find(_key);
	if (itr == m_keyPress.end()) {
		std::cout << "No event at param _key (GETKEYPRESSEVENT())\n";
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
const Events::EVENTID & InputController::GetKeyUpEvent(const Events::EVENTID _key) {
	ConstMapItr itr = m_keyUp.find(_key);
	if (itr == m_keyUp.end() ) {
		std::cout << "No event at param _key (GETKEYUPEVENT())\n";
		static Events::EVENTID junk;
		return junk;
	}
	return itr->second;
}
//--------------------------------------------------------------------------------
void InputController::AddKeyEvent(Map &_store, const Events::EVENTID _key, KeyInfo _info) {
	MapItr itr = _store.find(_key);
	if (itr == _store.end() ) {
		using namespace Events;
		MessageSystem *ms = MessageSystem::GetInstance();
		if (!ms) {
			std::cout << "No Message system, cannot AddKeyEvent\n";
			return;
		}
		Data data;
		data.first = _key;
		std::string msgKey;
		switch (_info.type) {
		case MapType::DOWN:
			msgKey = "BindKeyDownEvent";
			break;
		case MapType::PRESS:
			msgKey = "BindKeyPressEvent";
			break;
		case MapType::UP:
			msgKey = "BindKeyReleaseEvent";
			break;
		default:
			std::cout << "Bad Type sent into AddKeyEvent()\n";
		};
		data.second = ms->Call<EVENTID, const unsigned char, EVENTID>(msgKey, _info.vkCode, _key);
		
		Code code(data.first, _info);

		m_codes.insert(code);
		_store.insert(data);
		return;
	}

	std::cout << "Already added a keydown event with this key " << _key << "\n";
}
//--------------------------------------------------------------------------------
bool InputController::UnRegisterEvent(const Events::EVENTID _key) {
	CodeItr itr = m_codes.find(_key);
	if (itr != m_codes.end()) {
		Events::MessageSystem *ms = Events::MessageSystem::GetInstance();
		if (!ms) {
			std::cout << "No Message system, cannot bind key\n";
			return false;
		}
		ms->Call<bool, const unsigned char>("DeActivateKey", itr->second.vkCode);
		
		switch (itr->second.type) {
		case MapType::DOWN:
			m_keyDown.erase(_key);
			break;
		case MapType::PRESS:
			m_keyPress.erase(_key);
			break;
		case MapType::UP:
			m_keyUp.erase(_key);
			break;
		default:
			std::cout << "Bad Type sent into UnRegisterEvent()\n";
		};
		m_codes.erase(itr);
		
		return true;
	}

	std::cout << "Message: "<< _key<< " Could not be found to erase\n";
	return false;
}
//--------------------------------------------------------------------------------
bool InputController::SwithKeyDownEvent(const Events::EVENTID _key, const unsigned char _vk) {
	CodeItr itr = m_codes.find(_key);
	if (itr != m_codes.end()) {
		using namespace Events;
		MessageSystem *ms = MessageSystem::GetInstance();
		if (!ms) {
			std::cout << "No Message system cannot bind key\n";
			return false;
		}
		ms->Call<bool, const unsigned char>("DeActivateKey", itr->second.vkCode);
		itr->second.vkCode = _vk;
		std::string msgKey;
		switch (itr->second.type) {
			case MapType::DOWN:
				msgKey = "BindKeyDownEvent";
				break;
			case MapType::PRESS:
				msgKey = "BindKeyPressEvent";
				break;
			case MapType::UP:
				msgKey = "BindKeyReleaseEvent";
				break;
			default:
				std::cout << "Bad Type sent into SwitchKeyDownEvent()\n";
		};
		ms->Call<EVENTID, const unsigned char, EVENTID>(msgKey, _vk, _key);
		return true;
	}
	std::cout << "Could not find the event Key " << _key <<  " in SwitchKeyDownEvent()";
	return false;
}

bool InputController::SaveKeys()
{
	return false;

	TiXmlDocument xmlDoc("Binds.xml");


	TiXmlElement xNode("");
	

	for (unsigned i = 0; i < m_Binds.size(); i++)
	{
		xNode.SetValue(m_Binds[i].toDo.c_str());
		//xAttrType.SetIntValue();
		//xAttrCode.SetIntValue((m_Binds[i].info.type));
		xNode.SetAttribute("VK", m_Binds[i].info.vkCode);
		xNode.SetAttribute("VType", (int)m_Binds[i].info.type);

		xmlDoc.InsertEndChild(xNode);
	}

	//xmlDoc.InsertEndChild()

	xmlDoc.SaveFile("Binds.xml");
}

bool InputController::LoadKeys()
{
	return false;

	TiXmlDocument xDoc("Binds.xml");;
	xDoc.LoadFile();


}

void InputController::SetDefaultKeys()
{
	//Down
	AddKeyEvent(m_keyPress, "DirForward", KeyInfo('W', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DirBack", KeyInfo('S', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DirLeft", KeyInfo('A', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DirRight", KeyInfo('D', MapType::PRESS));
	AddKeyEvent(m_keyDown, "MouseLButton", KeyInfo(VK_LBUTTON, MapType::DOWN));
	AddKeyEvent(m_keyUp, "MouseLButtonUp", KeyInfo(VK_LBUTTON, MapType::UP));

	// press
	//AddKeyEvent(m_keyPress, "TestMsg", KeyInfo(VK_SPACE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Dash", KeyInfo(VK_SPACE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Pause", KeyInfo(VK_ESCAPE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "HTP", KeyInfo(VK_F1, MapType::PRESS));
	AddKeyEvent(m_keyPress, "EMP", KeyInfo('E', MapType::PRESS));
	AddKeyEvent(m_keyPress, "GameOverReturn", KeyInfo(VK_RETURN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "ExitUnsafe", KeyInfo(VK_ESCAPE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "SwapSwap", KeyInfo(VK_TAB, MapType::PRESS));
	AddKeyEvent(m_keyPress, "PlayerDash", KeyInfo(VK_SPACE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Weapon1", KeyInfo(0x31, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Weapon2", KeyInfo(0x32, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Weapon3", KeyInfo(0x33, MapType::PRESS));

	// Up
	AddKeyEvent(m_keyUp, "UpForward", KeyInfo('W', MapType::UP));
	AddKeyEvent(m_keyUp, "UpBack", KeyInfo('S', MapType::UP));
	AddKeyEvent(m_keyUp, "UpLeft", KeyInfo('A', MapType::UP));
	AddKeyEvent(m_keyUp, "UpRight", KeyInfo('D', MapType::UP));

}

void InputController::ClearDefaultKeys()
{
	UnRegisterEvent("DirForward");
	UnRegisterEvent("DirBack");
	UnRegisterEvent("DirLeft");
	UnRegisterEvent("DirRight");
	UnRegisterEvent("MouseLButton");
	UnRegisterEvent("MouseLButtonUp");
	UnRegisterEvent("Dash");
	UnRegisterEvent("Pause");
	UnRegisterEvent("UpForward");
	UnRegisterEvent("UpBack");
	UnRegisterEvent("UpLeft");
	UnRegisterEvent("UpRight");
	UnRegisterEvent("HTP");
	UnRegisterEvent("EMP");
	UnRegisterEvent("GameOverReturn");
	UnRegisterEvent("ExitUnsafe");
	UnRegisterEvent("SwapSwap");
	UnRegisterEvent("PlayerDash");
	UnRegisterEvent("Weapon1");
	UnRegisterEvent("Weapon2");
	UnRegisterEvent("Weapon3");

}

void InputController::SetMenuKeys()
{
	AddKeyEvent(m_keyPress, "Return", KeyInfo(VK_ESCAPE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Up", KeyInfo('W', MapType::PRESS));
	AddKeyEvent(m_keyPress, "Down", KeyInfo('S', MapType::PRESS));
	AddKeyEvent(m_keyPress, "Confirm", KeyInfo(VK_RETURN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "UpArr", KeyInfo(VK_UP, MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownArr", KeyInfo(VK_DOWN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "MouseLClick", KeyInfo(VK_LBUTTON, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Right", KeyInfo('D', MapType::PRESS));
	AddKeyEvent(m_keyPress, "Left", KeyInfo('A', MapType::PRESS));
	AddKeyEvent(m_keyPress, "RightArr", KeyInfo(VK_RIGHT, MapType::PRESS));
	AddKeyEvent(m_keyPress, "LeftArr", KeyInfo(VK_LEFT, MapType::PRESS));

}

void InputController::ClearMenuKeys()
{
	UnRegisterEvent("Return");
	UnRegisterEvent("Up");
	UnRegisterEvent("UpArr");
	UnRegisterEvent("Down");
	UnRegisterEvent("DownArr");
	UnRegisterEvent("MouseLClick");
	UnRegisterEvent("Confirm");
	UnRegisterEvent("Right");
	UnRegisterEvent("RightArr");
	UnRegisterEvent("Left");
	UnRegisterEvent("LeftArr");
}

void InputController::SetHTPKeys()
{
	AddKeyEvent(m_keyPress, "Return", KeyInfo(VK_ESCAPE, MapType::PRESS));	
	AddKeyEvent(m_keyPress, "Confirm", KeyInfo(VK_RETURN, MapType::PRESS));	
	AddKeyEvent(m_keyPress, "MouseLClick", KeyInfo(VK_LBUTTON, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Right", KeyInfo('D', MapType::PRESS));
	AddKeyEvent(m_keyPress, "Left", KeyInfo('A', MapType::PRESS));
	AddKeyEvent(m_keyPress, "RightArr", KeyInfo(VK_RIGHT, MapType::PRESS));
	AddKeyEvent(m_keyPress, "LeftArr", KeyInfo(VK_LEFT, MapType::PRESS));

}

void InputController::ClearHTPKeys()
{
	UnRegisterEvent("Return");
	UnRegisterEvent("Confirm");
	UnRegisterEvent("MouseLClick");
	UnRegisterEvent("Right");
	UnRegisterEvent("Left");
	UnRegisterEvent("RightArr");
	UnRegisterEvent("LeftArr");

}

//--------------------------------------------------------------------------------
InputController::KeyInfo::KeyInfo(const unsigned char _vk, MapType _type) : vkCode(_vk), type(_type) {}
//--------------------------------------------------------------------------------

void InputController::SetSoundKeys()
{
	AddKeyEvent(m_keyDown, "Left", KeyInfo('A', MapType::DOWN));
	AddKeyEvent(m_keyDown, "LeftArr", KeyInfo(VK_LEFT, MapType::DOWN));
	AddKeyEvent(m_keyDown, "Right", KeyInfo('D', MapType::DOWN));
	AddKeyEvent(m_keyDown, "RightArr", KeyInfo(VK_RIGHT, MapType::DOWN));
	AddKeyEvent(m_keyPress, "Up", KeyInfo('W', MapType::PRESS));
	AddKeyEvent(m_keyPress, "UpArr", KeyInfo(VK_UP, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Down", KeyInfo('S', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownArr", KeyInfo(VK_DOWN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Return", KeyInfo(VK_ESCAPE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Confirm", KeyInfo(VK_RETURN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "MouseLClick", KeyInfo(VK_LBUTTON, MapType::PRESS));
	AddKeyEvent(m_keyDown, "MouseLDown", KeyInfo(VK_LBUTTON, MapType::DOWN));
}

void InputController::ClearSoundKeys()
{
	UnRegisterEvent("Left");
	UnRegisterEvent("LeftArr");
	UnRegisterEvent("Right");
	UnRegisterEvent("RightArr");
	UnRegisterEvent("Up");
	UnRegisterEvent("UpArr");
	UnRegisterEvent("Down");
	UnRegisterEvent("DownArr");
	UnRegisterEvent("Return");
	UnRegisterEvent("Confirm");
	UnRegisterEvent("MouseLClick");
	UnRegisterEvent("MouseLDown");
}

void InputController::SetGraphicsKeys()
{
	AddKeyEvent(m_keyPress, "Left", KeyInfo('A', MapType::PRESS));
	AddKeyEvent(m_keyPress, "LeftArr", KeyInfo(VK_LEFT, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Right", KeyInfo('D', MapType::PRESS));
	AddKeyEvent(m_keyPress, "RightArr", KeyInfo(VK_RIGHT, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Up", KeyInfo('W', MapType::PRESS));
	AddKeyEvent(m_keyPress, "UpArr", KeyInfo(VK_UP, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Down", KeyInfo('S', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownArr", KeyInfo(VK_DOWN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Return", KeyInfo(VK_ESCAPE, MapType::PRESS));
	AddKeyEvent(m_keyPress, "Confirm", KeyInfo(VK_RETURN, MapType::PRESS));
	AddKeyEvent(m_keyPress, "MouseLClick", KeyInfo(VK_LBUTTON, MapType::PRESS));
	AddKeyEvent(m_keyDown, "MouseLDown", KeyInfo(VK_LBUTTON, MapType::DOWN));

}

void InputController::ClearGraphicsKeys()
{
	UnRegisterEvent("Left");
	UnRegisterEvent("LeftArr");
	UnRegisterEvent("Right");
	UnRegisterEvent("RightArr");
	UnRegisterEvent("Up");
	UnRegisterEvent("UpArr");
	UnRegisterEvent("Down");
	UnRegisterEvent("DownArr");
	UnRegisterEvent("Return");
	UnRegisterEvent("Confirm");
	UnRegisterEvent("MouseLClick");
	UnRegisterEvent("MouseLDown");
}

void InputController::SetDebugKeys()
{
	// DEBUG DOWN
	AddKeyEvent(m_keyPress, "DownDebugCtrl", KeyInfo(VK_CONTROL, MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebugConfirm", KeyInfo('D', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_G", KeyInfo('G', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_F", KeyInfo('F', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_N", KeyInfo('N', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_L", KeyInfo('L', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_P", KeyInfo('P', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_E", KeyInfo('E', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_B", KeyInfo('B', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_C", KeyInfo('C', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_1", KeyInfo('1', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_2", KeyInfo('2', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_3", KeyInfo('3', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_4", KeyInfo('4', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_5", KeyInfo('5', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_6", KeyInfo('6', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_7", KeyInfo('7', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_8", KeyInfo('8', MapType::PRESS));
	AddKeyEvent(m_keyPress, "DownDebug_9", KeyInfo('9', MapType::PRESS));
	//AddKeyEvent(m_keyPress, "DownDebug_0", KeyInfo('0', MapType::PRESS));

	// DEBUG UP
	//AddKeyEvent(m_keyUp, "UpDebugCtrl", KeyInfo(VK_CONTROL, MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebugConfirm", KeyInfo('D', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_G", KeyInfo('G', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_F", KeyInfo('F', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_N", KeyInfo('N', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_P", KeyInfo('P', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_E", KeyInfo('E', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_B", KeyInfo('B', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_C", KeyInfo('C', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_1", KeyInfo('1', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_2", KeyInfo('2', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_3", KeyInfo('3', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_4", KeyInfo('4', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_5", KeyInfo('5', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_6", KeyInfo('6', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_7", KeyInfo('7', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_8", KeyInfo('8', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_9", KeyInfo('9', MapType::UP));
	//AddKeyEvent(m_keyUp, "UpDebug_0", KeyInfo('0', MapType::UP));

}

void InputController::ClearDebugKeys()
{
	UnRegisterEvent("DownDebugCtrl");
	UnRegisterEvent("DownDebugConfirm");
	UnRegisterEvent("DownDebug_G");
	UnRegisterEvent("DownDebug_F");
	UnRegisterEvent("DownDebug_N");
	UnRegisterEvent("DownDebug_L");
	UnRegisterEvent("DownDebug_P");
	UnRegisterEvent("DownDebug_E");
	UnRegisterEvent("DownDebug_B");
	UnRegisterEvent("DownDebug_C");
	UnRegisterEvent("DownDebug_1");
	UnRegisterEvent("DownDebug_2");
	UnRegisterEvent("DownDebug_3");
	UnRegisterEvent("DownDebug_4");
	UnRegisterEvent("DownDebug_5");
	UnRegisterEvent("DownDebug_6");
	UnRegisterEvent("DownDebug_7");
	UnRegisterEvent("DownDebug_8");
	UnRegisterEvent("DownDebug_9");
	//UnRegisterEvent("DownDebug_0");

	//UnRegisterEvent("UpDebugCtrl");
	//UnRegisterEvent("UpDebugConfirm");
	//UnRegisterEvent("UpDebug_G");
	//UnRegisterEvent("UpDebug_F");
	//UnRegisterEvent("UpDebug_N");
	//UnRegisterEvent("UpDebug_P");
	//UnRegisterEvent("UpDebug_E");
	//UnRegisterEvent("UpDebug_B");
	//UnRegisterEvent("UpDebug_C");
	//UnRegisterEvent("UpDebug_1");
	//UnRegisterEvent("UpDebug_2");
	//UnRegisterEvent("UpDebug_3");
	//UnRegisterEvent("UpDebug_4");
	//UnRegisterEvent("UpDebug_5");
	//UnRegisterEvent("UpDebug_6");
	//UnRegisterEvent("UpDebug_7");
	//UnRegisterEvent("UpDebug_8");
	//UnRegisterEvent("UpDebug_9");

}

void InputController::SetSplashScreenKeys()
{
	AddKeyEvent(m_keyPress, "Confirm", KeyInfo(VK_RETURN, MapType::PRESS));
}

void InputController::ClearSplashScreenKeys()
{
	UnRegisterEvent("Confirm");
}