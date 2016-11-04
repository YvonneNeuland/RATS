#pragma once
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
//This was used to make sure these functions did not get called directly
//changing the number below will check to see if anyone has directly called the functions
//Macros were used to enable a way to quickly remove all the logging for release builds
#define __ID__	504
//--------------------------------------------------------------------------------

//////////////////////////////////////////////////////
//Don't use these directly
#define COMBINE_ACTION(A,B) A##B
#define COMBINE_STEP(A,B) COMBINE_ACTION(A,B)
//////////////////////////////////////////////////////

//makes one name into one that can be a class or namespace name
#define COMBINE(A,B) COMBINE_STEP(A,B)
//Make a sting out of a class or helpers like __FUNCTION__, __LINE__, __FILE__
#define STRINGIZE(A) #A

//--------------------------------------------------------------------------------
namespace COMBINE(Log, __ID__) {
	void InitDebugSystem(std::wstring _name);

#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	//--------------------------------------------------------------------------------
	//If you ever get a linker error when using Log, it means the overload for DebugPrint
	//	has not been made
	//To correct the issue, add another DebugPrint function with the proper type
	//--------------------------------------------------------------------------------
	void DebugPrint(std::string _str);
	void DebugPrint(std::wstring _str);
	void DebugPrint(float _in);
	void DebugPrint(int _in);
	void DebugPrint(unsigned long _in);
	void DebugPrint(unsigned int _in);
	void DebugPrint(void * _in);
	void DebugPrint(char * _in);
	void DebugPrint(wchar_t * _in);
	void DebugPrint(const void * _in);
	void DebugPrint(const char * _in);
	void DebugPrint(const wchar_t * _in);
	//--------------------------------------------------------------------------------
	void print();
	void SetDumpFile();
	void Close();
	void LogLastError();
	bool DXFunctionCall(long _hr);
	#ifdef VISUAL_STUDIO_VISUAL_DEBUG_INFO
		void CreateDXNaming(void *_context);
	#endif //VISUAL_STUDIO_VISUAL_DEBUG_INFO
	
		void DebugReport(int _ref, void *_context);
	void CheckMessage(unsigned int _msg);
	void CheckSystemMessage(unsigned int _msg);
	//--------------------------------------------------------------------------------
	template <typename work, typename ...type>
	void print(work _in, type...Pass){
		DebugPrint(_in);
		print(Pass...);
	}
	//--------------------------------------------------------------------------------
	//This class is used to group draw calls in the visual studio visual debugger
	//Just declare the class and when it runs out of scope it will group all those calls together
	//--------------------------------------------------------------------------------
	class DXEvent {
	public:
		DXEvent(char *_name);
		DXEvent(wchar_t *_name);
		~DXEvent();
	};
	//--------------------------------------------------------------------------------
#elif defined(VISUAL_STUDIO_VISUAL_DEBUG_INFO)
	void CreateDXNaming(void *_context);
	//--------------------------------------------------------------------------------
	//This class is used to group draw calls in the visual studio visual debugger
	//Just declare the class and when it runs out of scope it will group all those calls together
	//--------------------------------------------------------------------------------
	class DXEvent {
	public:
		DXEvent(char *_name);
		DXEvent(wchar_t *_name);
		~DXEvent();
	};
#endif //#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
};
//--------------------------------------------------------------------------------