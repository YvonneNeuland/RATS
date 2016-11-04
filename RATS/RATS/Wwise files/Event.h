#pragma once
//--------------------------------------------------------------------------------
//Lighter than including the entire event manager
#include <string>
//--------------------------------------------------------------------------------
namespace Events {
	// type def for easy changing between string and Hash ID versions
	typedef std::string EVENTID;
	//typedef unsigned int EVENTID;
	//enum EVENTID { EVENTS_SIZE = 0};
};
//--------------------------------------------------------------------------------