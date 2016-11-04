#pragma once
//--------------------------------------------------------------------------------
#include <d3d11.h>			//For setting resource names

#include "./Log.h"			//For Log calls when only including DXLog.h
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
namespace COMBINE(Log, __ID__) {
#ifdef VISUAL_STUDIO_VISUAL_DEBUG_INFO
	void DXRelease();
#endif
};
//--------------------------------------------------------------------------------
//Will not set pointer to null if there are still refrences on the object
#define SAFE_RELEASE(com) { int keeper = 0; if(com) { if (0 == (keeper = com->Release() ) ) com = nullptr; } }

#ifndef __GOLD__BUILD__
	#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		//Wrap everything that returns an HRESULT
		//Logs out the com error if there is one
		//returns true if it is S_OK else false
		#define DXCall(hr)			COMBINE(Log, __ID__)::DXFunctionCall(hr)
	#endif //#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#if !defined(LOG_TO_CONSOLE) && !defined(LOG_TO_FILE) && !defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		#define DXCall __noop;
	#endif
	
#ifdef VISUAL_STUDIO_VISUAL_DEBUG_INFO
	//Set-up function: Call right after error checking, and there is a valid context
	//Nothing will do DX logs without calling this function
	#define DXNameInit(context) COMBINE(Log, __ID__)::CreateDXNaming(context)
	//Name objects for Graphics Object Table while visual debugging
	#define DXName(obj, str)	{ std::string s = str; D3D_SET_OBJECT_NAME_A(obj, s.c_str() ); }
	//n - wide char *
	//Call to group DX calls for Graphic Event List while visual debugging
	//Suggestions group HUD, particles, animated objects static objects as groups
	#define DXDrawName(n)		COMBINE(Log, __ID__)::DXEvent COMBINE(Log,__LINE__)(n)
	//Use: DXMemoryCheck(Device->Release(), context (sent into DXNameInit )) ;
	#define DXMemoryCheck(ref, context)  COMBINE(Log, __ID__)::DebugReport(ref, context)
	//Call last before end of renderer shutdown
	#define DXDebugRelease()	COMBINE(Log, __ID__)::DXRelease()
#else //VISUAL_STUDIO_VISUAL_DEBUG_INFO
	#define DXMemoryCheck __noop
	#define DXName(obj, str) __noop
	#define DXNameInit(context) __noop
	#define DXDrawName(n) __noop
	#define DXDebugRelease() __noop
#endif
#elif defined(VISUAL_STUDIO_VISUAL_DEBUG_INFO)
	//Set-up function: Call right after error checking, and there is a valid context
	//Nothing will do DX logs without calling this function
	#define DXNameInit(context) COMBINE(Log, __ID__)::CreateDXNaming(context)
	//Name objects for Graphics Object Table while visual debugging
	#define DXName(obj, str)	{ std::string s = str; D3D_SET_OBJECT_NAME_A(obj, s.c_str() ); }
	//n - wide char *
	//Call to group DX calls for Graphic Event List while visual debugging
	//Suggestions group HUD, particles, animated objects static objects as groups
	#define DXDrawName(n)		COMBINE(Log, __ID__)::DXEvent COMBINE(Log,__LINE__)(n)
	//Call last before end of renderer shutdown
	#define DXDebugRelease()	COMBINE(Log, __ID__)::DXRelease()
	#define DXCall __noop;
	#define DXMemoryCheck __noop
#else //!defined(VISUAL_STUDIO_VISUAL_DEBUG_INFO) && __GOLD__BUILD__
	//Needs to be an empty function so the enclosed function will still work
	#define DXCall __noop;
	#define DXMemoryCheck __noop
	#define DXName(obj, str) __noop
	#define DXNameInit(context) __noop
	#define DXDrawName(n) __noop
	#define DXDebugRelease() __noop
#endif //__GOLD__BUILD__
//--------------------------------------------------------------------------------