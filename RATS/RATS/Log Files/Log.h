#pragma once
//--------------------------------------------------------------------------------
//__GOLD__BUILD__ takes out a lot of functionality
//If any of the next four defines get put in, some functionality returns
//THe fastest is to only have __GOLD__BUILD__ defined and the rest put a comment in front of the define
//#define __GOLD__BUILD__
//--------------------------------------------------------------------------------
//Below, these are the other 4 defines mentioned above
//--------------------------------------------------------------------------------
#define LOG_TO_FILE
#define LOG_TO_CONSOLE
#define LOG_TO_VISUAL_STUDIO_OUTPUT
#define VISUAL_STUDIO_VISUAL_DEBUG_INFO
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
#pragma region PRIVATE_FUNCTIONS_DO_NOT_USE THESE
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
	void DebugPrint(bool _in);
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
	const char * AppDataPath();
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
		DXEvent(const char *_name);
		DXEvent(const wchar_t *_name);
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
		DXEvent(const char *_name);
		DXEvent(const wchar_t *_name);
		~DXEvent();
	};
#endif //#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
};
//--------------------------------------------------------------------------------
#pragma endregion //Use the macros instead of the functions listed in this region.  You can remove and add functionality with a define as oppsed to going through the entire codebase and removing or adding them

//Make a sting out of a class or helpers like __FUNCTION__, __LINE__, __FILE__
#define STRINGIZE(A) #A
//--------------------------------------------------------------------------------
//Log file, and dump file got to the appdata/raoming folder
//To get there:
//Goto to windows run
//type %appdata%
//Look for folder name you supplied to the macro LogSetUp
//--------------------------------------------------------------------------------
//Only sets the object to nil if there are no references
#define SAFE_SHUT(obj) if(obj) { obj->ShutDown(); delete obj; obj = nullptr; }
//--------------------------------------------------------------------------------
//This define if to take out almost everything so the game can run faster
//This will still make a dump file and create the appdata folder
//n - Name of the game wide char * or std::wstring
// Creates a file in %appdata% where the log file and any dump files will be placed
//Should be one of the first things called
//This sets up dump files even when nothing is given always call first even with no defines
#define LogSetUp(gameName)	COMBINE(Log, __ID__)::InitDebugSystem(gameName)
//--------------------------------------------------------------------------------
//Returns the file path of the app data folder created after LogSetUp has been called
//This will return nullptr until the appdata folder has been created
#define GetAppDataLocation() COMBINE(Log, __ID__)::AppDataPath()
//--------------------------------------------------------------------------------
//This define if to take out almost everything so the game can run faster
//This will still make a dump file and create the appdata folder
#ifndef __GOLD__BUILD__
	#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		//Use instead of get last error for window calls
		//If there is any error it calls log
		#define LogError()		COMBINE(Log, __ID__)::LogLastError()
		//Send in any number of arguments to get logged to a file, the console window and the Output window
		//Use Log("something", vector.size(), argument)
		//If there is a linker error or an error where the compiler cannot find the function add the overload to region above
		#define Log(...)		COMBINE(Log, __ID__)::print(##__VA_ARGS__)
		//Clean-up the debugger
		//Call at the end of the program
		#define LogShutDown()		COMBINE(Log, __ID__)::Close()
		//m - msg from wndproc
		//Used to log what order of messages are sent in wndproc
		#define LogWndProc(msg)	COMBINE(Log, __ID__)::CheckMessage(msg)
		//m - Wparam from wndproc
		//Used to log the system messages (in wndproc), another case off WM_SYSKEYDOWN messages
		//Only valid in the secondary case, is used on the main switch or another message it will print unknown or incorrect infomration
		#define LogSystem(wParam)	COMBINE(Log, __ID__)::CheckSystemMessage(wParam)
	#endif //#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#if !defined(LOG_TO_CONSOLE) && !defined(LOG_TO_FILE) && !defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		#define LogError() __noop
		#define Log(...) __noop
		#define LogShutDown() __noop
		#define LogWndProc __noop
		#define LogSystem __noop
	#endif

#else //!__GOLD__BUILD__
	#define LogWndProc __noop
	#define LogError() __noop
#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#define Log(...) COMBINE(Log, __ID__)::print(##__VA_ARGS__)
#else
	#define Log(...) __noop
#endif
	#define LogShutDown() __noop
	#define LogSystem __noop
#endif //__GOLD__BUILD__