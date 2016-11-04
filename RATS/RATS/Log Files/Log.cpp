#include "stdafx.h"
#include <Windows.h>
#include <Shlobj.h>		//Get appdata folder location
#include <Dbghelp.h>	// For Dump Files
#include <ctime>		//for getting the current time
#include <direct.h>		//for mkdir
#include <shlwapi.h>	//AppendPath
#include <string>
#include "./Log.h"

#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
//#include <strsafe.h>	//For get last error text formatting
#include <sstream>		//for string stream used to print pointer addresses
#endif
#if defined(LOG_TO_CONSOLE)
#include <iostream>		// for cout
#include <io.h>			//for _open_osfhandle (console)
#include <fcntl.h>		//for _O_TEXT
#endif //LOG_TO_CONSOLE


//--------------------------------------------------------------------------------
#pragma comment(lib,"Dbghelp.lib")		//For MiniDumpWriteDump 
#pragma comment(lib,"shlwapi.lib")		//for PathAppendA

//--------------------------------------------------------------------------------
namespace COMBINE(Log, __ID__) {
	std::string Company = "\\My Company";
	std::string AppData = "";
	#ifdef LOG_TO_FILE
		bool fileOutput = false;
		bool LogToFile = false;
		std::string LogFile = "";
		bool InitFile();
	#endif //LOG_TO_FILE
	#ifdef LOG_TO_CONSOLE
		#define MAX_CONSOLE_BUFFER_LINES    300     // console line count
		#define MAX_CONSOLE_BUFFER_COLUMNS  80      // console line count
		#define MAX_CONSOLE_WINDOW_LINES    24      // console line count
		#define MAX_CONSOLE_WINDOW_COLUMNS  80      // console line coun
		void CreateConsole(short bufferWidth = MAX_CONSOLE_BUFFER_COLUMNS,
			short bufferHeight = MAX_CONSOLE_BUFFER_LINES,
			short windowWidth = MAX_CONSOLE_WINDOW_COLUMNS,
			short windowHeight = MAX_CONSOLE_WINDOW_LINES);
		void ReleaseConsole();
	#endif //LOG_TO_CONSOLE
	//--------------------------------------------------------------------------------
	LONG WINAPI WriteDumpFIle(_EXCEPTION_POINTERS *_exceptionInfo) {
		std::string dumpFile = AppData;
		time_t t = time(0);   // get time now
		struct tm now;
		localtime_s(&now, &t);

		dumpFile += std::to_string(now.tm_mday);
		dumpFile += " ";
		dumpFile += std::to_string(now.tm_mon + 1);
		dumpFile += " ";
		dumpFile += std::to_string(1900 + now.tm_year);
		dumpFile += " at ";
		dumpFile += std::to_string(now.tm_hour);
		dumpFile += ".";
		dumpFile += std::to_string(now.tm_min);
		dumpFile += ".";
		dumpFile += std::to_string(now.tm_sec);
		dumpFile += ".mdmp";

		Log("Dump File Made: ", dumpFile);
		HANDLE hFile = ::CreateFileA(dumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE != hFile) {
			_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = _exceptionInfo;
			ExInfo.ClientPointers = FALSE;
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, nullptr);
			::CloseHandle(hFile);
		}

		return 0;
	}
	//--------------------------------------------------------------------------------
	bool SetAppDataInfo() {
		char szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH);
		HRESULT hr = SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szPath);
		if (FAILED(hr) ) {
			Log("Could Not Find AppData");
			return false;
		}
		PathAppendA(szPath, Company.c_str() );

		int errorCode = _mkdir(szPath);
		_get_errno(&errorCode);

		switch (errorCode) {
		case 0:
#ifdef LOG_TO_FILE
			fileOutput = true;
#endif //LOG_TO_FILE
			break;
		case ENOENT:
			Log("Folder creation error, no file output");
			return false;
			break;
		case EEXIST:
#ifdef LOG_TO_FILE
			fileOutput = true;
#endif //LOG_TO_FILE
			break;
		default:
			Log("unknown error code for _mkdir");
			return false;
		}
		AppData = szPath;
		AppData += "\\";

		return true;
	}
	//--------------------------------------------------------------------------------
	void SetDumpFile() {
		SetAppDataInfo();
		SetUnhandledExceptionFilter(WriteDumpFIle);
	}
	//--------------------------------------------------------------------------------
	void InitDebugSystem(std::wstring _name) {
		std::string convert(_name.begin(), _name.end() );
		Company = "\\";
		Company += convert;
		SetDumpFile();
	#ifdef LOG_TO_CONSOLE
			CreateConsole();
	#endif //LOG_TO_CONSOLE
	#ifdef LOG_TO_FILE
		InitFile();
	#endif //LOG_TO_FILE
	}
	//--------------------------------------------------------------------------------
	void Close() {
#ifdef LOG_TO_CONSOLE
		ReleaseConsole();
		//--------------------------------------------------------------------------------	
#endif //LOG_TO_CONSOLE
	}
	//--------------------------------------------------------------------------------
#ifdef LOG_TO_CONSOLE
	
	bool g_InUse = false;
	
	void ReleaseConsole();
	//--------------------------------------------------------------------------------
	void ReleaseConsole() {
		g_InUse = 0;
		FreeConsole();
	}
	//--------------------------------------------------------------------------------
	void CreateConsole(short bufferWidth, short bufferHeight, short windowWidth, short windowHeight) {
		if (g_InUse)
			return;
		// locals
		CONSOLE_SCREEN_BUFFER_INFO  coninfo;
		FILE                       *pFile;
		int                         conHandle;
		HANDLE                      stdHandle;
		SMALL_RECT                  window = { 0, };

		// allocate console
		AllocConsole();

		// reset console properties
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = bufferHeight;
		coninfo.dwSize.X = bufferWidth;
		window.Left = 0;
		window.Top = 0;
		window.Right = windowWidth - 1;
		window.Bottom = windowHeight - 1;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
		SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &window);

		// redirect STDOUT to console
		stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		pFile = _fdopen(conHandle, "w");
		*stdout = *pFile;
		setvbuf(stdout, NULL, _IONBF, 0); // unbuffered

		// redirect STDIN to console
		stdHandle = GetStdHandle(STD_INPUT_HANDLE);
		conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		pFile = _fdopen(conHandle, "r");
		*stdin = *pFile;
		setvbuf(stdin, NULL, _IONBF, 0); // unbuffered

		// redirect STDERR to console
		stdHandle = GetStdHandle(STD_ERROR_HANDLE);
		conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		pFile = _fdopen(conHandle, "w");
		*stderr = *pFile;
		setvbuf(stderr, NULL, _IONBF, 0); // unbuffered

		// route cout, wcout, cin, wcin, wcerr, cerr, wclog & clog as well
		std::ios::sync_with_stdio();

		g_InUse = 1;
	}
#endif //LOG_TO_CONSOLE
#ifdef LOG_TO_FILE
	bool InitFile() {
		if (true == LogToFile) {
			return true;
		}

		LogFile = AppData;
		LogFile += "log.txt";

		FILE *file = nullptr;
		fopen_s(&file, LogFile.c_str(), "wb");
		if (nullptr == file) {
			Log("could not open the debug file ", LogFile);
			fileOutput = false;
			return false;
		}
		fclose(file);
		LogToFile = true;
		return true;
	}
	//--------------------------------------------------------------------------------
	void WriteToFile(std::string _in) {
		FILE *file = nullptr;
		fopen_s(&file, LogFile.c_str(), "at");
		if (nullptr == file) {
			Log("could not open the debug file ", LogFile);
			fileOutput = false;
			return;
		}

		fwrite(_in.c_str(), sizeof(char), _in.size(), file);
		fclose(file);
	}
#endif //LOG_TO_FILE

#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	//#include <strsafe.h>	//For get last error text formatting
	#include <sstream>		//for string stream used to print pointer addresses

	//For WndProc debugging functions
	#define TRACE_ID(iD) case iD: return #iD;
		//Metro Messages
	#define WM_UAHDESTROYWINDOW 0x0090
	#define WM_UAHDRAWMENU 0x0091
	#define WM_UAHDRAWMENUITEM 0x0092
	#define WM_UAHINITMENU 0x0093
	#define WM_UAHMEASUREMENUITEM 0x0094
	#define WM_UAHNCPAINTMENUPOPUP 0x0095
	#define WM_NCUAHDRAWCAPTION 0x00AE
	#define ODD_WINDOWS_8_MSG_NOT_DOCUMENTED 0xc0AE

	char* WINAPI TraceWindowsMessage(UINT _uMsg);
	char* WINAPI TraceSysCommandMessage(UINT _uMsg);
#pragma region LogFunctionOverloads
	//--------------------------------------------------------------------------------
	void DebugPrint(std::wstring _str) {
		std::string temp(_str.begin(), _str.end() );
		DebugPrint(temp);
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(float _in) {
		DebugPrint(std::to_string(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(bool _in) {
		DebugPrint(_in ? " true" : " false");
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(int _in) {
		DebugPrint(std::to_string(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(DWORD _in) {
		DebugPrint(std::to_string(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(unsigned int _in) {
		DebugPrint(std::to_string(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(void * _in) {
		std::stringstream pointerValue;
		pointerValue << _in;
		DebugPrint(pointerValue.str() );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(char * _in) {
		DebugPrint(std::string(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(wchar_t * _in) {
		DebugPrint(std::wstring(_in) );
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(const void * _in) {
		std::stringstream pointerValue;
		pointerValue << _in;
		DebugPrint(pointerValue.str());
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(const char * _in) {
		DebugPrint(std::string(_in));
	}
	//--------------------------------------------------------------------------------
	void DebugPrint(const wchar_t * _in) {
		DebugPrint(std::wstring(_in));
	}
	//--------------------------------------------------------------------------------
	void print(){
		DebugPrint("\n");
	}
#pragma endregion LogFunctionOverloads
	void DebugPrint(std::string _str) {
#ifdef LOG_TO_CONSOLE
		std::cout << _str;
#endif //LOG_TO_CONSOLE

#ifdef LOG_TO_VISUAL_STUDIO_OUTPUT
		OutputDebugStringA(_str.c_str() );
#endif //LOG_TO_VISUAL_STUDIO_OUTPUT

#ifdef LOG_TO_FILE
		if (true == LogToFile) {
			WriteToFile(_str);
		}
#endif //LOG_TO_FILE
	}
	//--------------------------------------------------------------------------------
	void LogLastError() {

		LPVOID lpMsgBuf = nullptr;
		DWORD dw = GetLastError();

		if(0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL) )
		return;

		Log("GetLastError returned: ", dw, " ", static_cast<LPTSTR>(lpMsgBuf) );

		LocalFree(lpMsgBuf);
	}
	const char * AppDataPath() {
		if (AppData.size() > 0)
			return AppData.c_str();
		return nullptr;
	}
	//--------------------------------------------------------------------------------
	void CheckMessage(unsigned int _msg) {
		Log(TraceWindowsMessage(_msg) );
	}
	//--------------------------------------------------------------------------------
	void CheckSystemMessage(unsigned int _msg) {
		std::string msg("SYSCMMAND: ");
		msg += TraceSysCommandMessage(_msg);

		Log(msg.c_str() );
	}
	//--------------------------------------------------------------------------------
	char* WINAPI TraceWindowsMessage(UINT _uMsg) {
		switch (_uMsg) {
			TRACE_ID(WM_NULL);
			TRACE_ID(WM_CREATE);
			TRACE_ID(WM_DESTROY);
			TRACE_ID(WM_MOVE);
			TRACE_ID(WM_SIZE);
			TRACE_ID(WM_ACTIVATE);
			TRACE_ID(WM_SETFOCUS);
			TRACE_ID(WM_KILLFOCUS);
			TRACE_ID(WM_ENABLE);
			TRACE_ID(WM_SETREDRAW);
			TRACE_ID(WM_SETTEXT);
			TRACE_ID(WM_GETTEXT);
			TRACE_ID(WM_GETTEXTLENGTH);
			TRACE_ID(WM_PAINT);
			TRACE_ID(WM_CLOSE);
			TRACE_ID(WM_QUERYENDSESSION);
			TRACE_ID(WM_QUERYOPEN);
			TRACE_ID(WM_ENDSESSION);
			TRACE_ID(WM_QUIT);
			TRACE_ID(WM_ERASEBKGND);
			TRACE_ID(WM_SYSCOLORCHANGE);
			TRACE_ID(WM_SHOWWINDOW);
			TRACE_ID(WM_WININICHANGE);
			TRACE_ID(WM_DEVMODECHANGE);
			TRACE_ID(WM_ACTIVATEAPP);
			TRACE_ID(WM_FONTCHANGE);
			TRACE_ID(WM_TIMECHANGE);
			TRACE_ID(WM_CANCELMODE);
			TRACE_ID(WM_SETCURSOR);
			TRACE_ID(WM_MOUSEACTIVATE);
			TRACE_ID(WM_CHILDACTIVATE);
			TRACE_ID(WM_QUEUESYNC);
			TRACE_ID(WM_GETMINMAXINFO);
			TRACE_ID(WM_PAINTICON);
			TRACE_ID(WM_ICONERASEBKGND);
			TRACE_ID(WM_NEXTDLGCTL);
			TRACE_ID(WM_SPOOLERSTATUS);
			TRACE_ID(WM_DRAWITEM);
			TRACE_ID(WM_MEASUREITEM);
			TRACE_ID(WM_DELETEITEM);
			TRACE_ID(WM_VKEYTOITEM);
			TRACE_ID(WM_CHARTOITEM);
			TRACE_ID(WM_SETFONT);
			TRACE_ID(WM_GETFONT);
			TRACE_ID(WM_SETHOTKEY);
			TRACE_ID(WM_GETHOTKEY);
			TRACE_ID(WM_QUERYDRAGICON);
			TRACE_ID(WM_COMPAREITEM);
			TRACE_ID(WM_GETOBJECT);
			TRACE_ID(WM_COMPACTING);
			TRACE_ID(WM_COMMNOTIFY);
			TRACE_ID(WM_WINDOWPOSCHANGING);
			TRACE_ID(WM_WINDOWPOSCHANGED);
			TRACE_ID(WM_POWER);
			TRACE_ID(WM_COPYDATA);
			TRACE_ID(WM_CANCELJOURNAL);
			TRACE_ID(WM_NOTIFY);
			TRACE_ID(WM_INPUTLANGCHANGEREQUEST);
			TRACE_ID(WM_INPUTLANGCHANGE);
			TRACE_ID(WM_TCARD);
			TRACE_ID(WM_HELP);
			TRACE_ID(WM_USERCHANGED);
			TRACE_ID(WM_NOTIFYFORMAT);
			TRACE_ID(WM_CONTEXTMENU);
			TRACE_ID(WM_STYLECHANGING);
			TRACE_ID(WM_STYLECHANGED);
			TRACE_ID(WM_DISPLAYCHANGE);
			TRACE_ID(WM_GETICON);
			TRACE_ID(WM_SETICON);
			TRACE_ID(WM_NCCREATE);
			TRACE_ID(WM_NCDESTROY);
			TRACE_ID(WM_NCCALCSIZE);
			TRACE_ID(WM_NCHITTEST);
			TRACE_ID(WM_NCPAINT);
			TRACE_ID(WM_NCACTIVATE);
			TRACE_ID(WM_GETDLGCODE);
			TRACE_ID(WM_SYNCPAINT);
			TRACE_ID(WM_NCMOUSEMOVE);
			TRACE_ID(WM_NCLBUTTONDOWN);
			TRACE_ID(WM_NCLBUTTONUP);
			TRACE_ID(WM_NCLBUTTONDBLCLK);
			TRACE_ID(WM_NCRBUTTONDOWN);
			TRACE_ID(WM_NCRBUTTONUP);
			TRACE_ID(WM_NCRBUTTONDBLCLK);
			TRACE_ID(WM_NCMBUTTONDOWN);
			TRACE_ID(WM_NCMBUTTONUP);
			TRACE_ID(WM_NCMBUTTONDBLCLK);
			TRACE_ID(WM_NCXBUTTONDOWN);
			TRACE_ID(WM_NCXBUTTONUP);
			TRACE_ID(WM_NCXBUTTONDBLCLK);
			TRACE_ID(WM_INPUT);
			TRACE_ID(WM_KEYDOWN);
			TRACE_ID(WM_KEYUP);
			TRACE_ID(WM_CHAR);
			TRACE_ID(WM_DEADCHAR);
			TRACE_ID(WM_SYSKEYDOWN);
			TRACE_ID(WM_SYSKEYUP);
			TRACE_ID(WM_SYSCHAR);
			TRACE_ID(WM_SYSDEADCHAR);
			TRACE_ID(WM_UNICHAR);
			TRACE_ID(WM_IME_STARTCOMPOSITION);
			TRACE_ID(WM_IME_ENDCOMPOSITION);
			TRACE_ID(WM_IME_COMPOSITION);
			TRACE_ID(WM_INITDIALOG);
			TRACE_ID(WM_COMMAND);
			TRACE_ID(WM_SYSCOMMAND);
			TRACE_ID(WM_TIMER);
			TRACE_ID(WM_HSCROLL);
			TRACE_ID(WM_VSCROLL);
			TRACE_ID(WM_INITMENU);
			TRACE_ID(WM_INITMENUPOPUP);
			TRACE_ID(WM_MENUSELECT);
			TRACE_ID(WM_MENUCHAR);
			TRACE_ID(WM_ENTERIDLE);
			TRACE_ID(WM_MENURBUTTONUP);
			TRACE_ID(WM_MENUDRAG);
			TRACE_ID(WM_MENUGETOBJECT);
			TRACE_ID(WM_UNINITMENUPOPUP);
			TRACE_ID(WM_MENUCOMMAND);
			TRACE_ID(WM_CHANGEUISTATE);
			TRACE_ID(WM_UPDATEUISTATE);
			TRACE_ID(WM_QUERYUISTATE);
			TRACE_ID(WM_CTLCOLORMSGBOX);
			TRACE_ID(WM_CTLCOLOREDIT);
			TRACE_ID(WM_CTLCOLORLISTBOX);
			TRACE_ID(WM_CTLCOLORBTN);
			TRACE_ID(WM_CTLCOLORDLG);
			TRACE_ID(WM_CTLCOLORSCROLLBAR);
			TRACE_ID(WM_CTLCOLORSTATIC);
			TRACE_ID(MN_GETHMENU);
			TRACE_ID(WM_MOUSEMOVE);
			TRACE_ID(WM_LBUTTONDOWN);
			TRACE_ID(WM_LBUTTONUP);
			TRACE_ID(WM_LBUTTONDBLCLK);
			TRACE_ID(WM_RBUTTONDOWN);
			TRACE_ID(WM_RBUTTONUP);
			TRACE_ID(WM_RBUTTONDBLCLK);
			TRACE_ID(WM_MBUTTONDOWN);
			TRACE_ID(WM_MBUTTONUP);
			TRACE_ID(WM_MBUTTONDBLCLK);
			TRACE_ID(WM_MOUSEWHEEL);
			TRACE_ID(WM_XBUTTONDOWN);
			TRACE_ID(WM_XBUTTONUP);
			TRACE_ID(WM_XBUTTONDBLCLK);
			TRACE_ID(WM_PARENTNOTIFY);
			TRACE_ID(WM_ENTERMENULOOP);
			TRACE_ID(WM_EXITMENULOOP);
			TRACE_ID(WM_NEXTMENU);
			TRACE_ID(WM_SIZING);
			TRACE_ID(WM_CAPTURECHANGED);
			TRACE_ID(WM_MOVING);
			TRACE_ID(WM_POWERBROADCAST);
			TRACE_ID(WM_DEVICECHANGE);
			TRACE_ID(WM_MDICREATE);
			TRACE_ID(WM_MDIDESTROY);
			TRACE_ID(WM_MDIACTIVATE);
			TRACE_ID(WM_MDIRESTORE);
			TRACE_ID(WM_MDINEXT);
			TRACE_ID(WM_MDIMAXIMIZE);
			TRACE_ID(WM_MDITILE);
			TRACE_ID(WM_MDICASCADE);
			TRACE_ID(WM_MDIICONARRANGE);
			TRACE_ID(WM_MDIGETACTIVE);
			TRACE_ID(WM_MDISETMENU);
			TRACE_ID(WM_ENTERSIZEMOVE);
			TRACE_ID(WM_EXITSIZEMOVE);
			TRACE_ID(WM_DROPFILES);
			TRACE_ID(WM_MDIREFRESHMENU);
			TRACE_ID(WM_IME_SETCONTEXT);
			TRACE_ID(WM_IME_NOTIFY);
			TRACE_ID(WM_IME_CONTROL);
			TRACE_ID(WM_IME_COMPOSITIONFULL);
			TRACE_ID(WM_IME_SELECT);
			TRACE_ID(WM_IME_CHAR);
			TRACE_ID(WM_IME_REQUEST);
			TRACE_ID(WM_IME_KEYDOWN);
			TRACE_ID(WM_IME_KEYUP);
			TRACE_ID(WM_MOUSEHOVER);
			TRACE_ID(WM_MOUSELEAVE);
			TRACE_ID(WM_NCMOUSEHOVER);
			TRACE_ID(WM_NCMOUSELEAVE);
			TRACE_ID(WM_WTSSESSION_CHANGE);
			TRACE_ID(WM_TABLET_FIRST);
			TRACE_ID(WM_TABLET_LAST);
			TRACE_ID(WM_CUT);
			TRACE_ID(WM_COPY);
			TRACE_ID(WM_PASTE);
			TRACE_ID(WM_CLEAR);
			TRACE_ID(WM_UNDO);
			TRACE_ID(WM_RENDERFORMAT);
			TRACE_ID(WM_RENDERALLFORMATS);
			TRACE_ID(WM_DESTROYCLIPBOARD);
			TRACE_ID(WM_DRAWCLIPBOARD);
			TRACE_ID(WM_PAINTCLIPBOARD);
			TRACE_ID(WM_VSCROLLCLIPBOARD);
			TRACE_ID(WM_SIZECLIPBOARD);
			TRACE_ID(WM_ASKCBFORMATNAME);
			TRACE_ID(WM_CHANGECBCHAIN);
			TRACE_ID(WM_HSCROLLCLIPBOARD);
			TRACE_ID(WM_QUERYNEWPALETTE);
			TRACE_ID(WM_PALETTEISCHANGING);
			TRACE_ID(WM_PALETTECHANGED);
			TRACE_ID(WM_HOTKEY);
			TRACE_ID(WM_PRINT);
			TRACE_ID(WM_PRINTCLIENT);
			TRACE_ID(WM_APPCOMMAND);
			TRACE_ID(WM_THEMECHANGED);
			TRACE_ID(WM_HANDHELDFIRST);
			TRACE_ID(WM_HANDHELDLAST);
			TRACE_ID(WM_AFXFIRST);
			TRACE_ID(WM_AFXLAST);
			TRACE_ID(WM_PENWINFIRST);
			TRACE_ID(WM_PENWINLAST);

			TRACE_ID(WM_UAHDESTROYWINDOW);
			TRACE_ID(WM_UAHDRAWMENU);
			TRACE_ID(WM_UAHDRAWMENUITEM);
			TRACE_ID(WM_UAHINITMENU);
			TRACE_ID(WM_UAHMEASUREMENUITEM);
			TRACE_ID(WM_UAHNCPAINTMENUPOPUP);
			TRACE_ID(WM_DWMCOMPOSITIONCHANGED);
			TRACE_ID(WM_DWMNCRENDERINGCHANGED);
			TRACE_ID(WM_DWMCOLORIZATIONCOLORCHANGED);
			TRACE_ID(WM_DWMWINDOWMAXIMIZEDCHANGE);
			TRACE_ID(WM_NCUAHDRAWCAPTION);
			TRACE_ID(WM_APP);
			TRACE_ID(ODD_WINDOWS_8_MSG_NOT_DOCUMENTED);

		default:
			return "Unknown";
		}
	}
	//--------------------------------------------------------------------------------
	char* WINAPI TraceSysCommandMessage(UINT _uMsg) {
		switch (_uMsg) {
			TRACE_ID(SC_SIZE);
			TRACE_ID(SC_MOVE);
			TRACE_ID(SC_MINIMIZE);
			TRACE_ID(SC_MAXIMIZE);
			TRACE_ID(SC_NEXTWINDOW);
			TRACE_ID(SC_PREVWINDOW);
			TRACE_ID(SC_CLOSE);
			TRACE_ID(SC_VSCROLL);
			TRACE_ID(SC_HSCROLL);
			TRACE_ID(SC_MOUSEMENU);
			TRACE_ID(SC_KEYMENU);
			TRACE_ID(SC_ARRANGE);
			TRACE_ID(SC_RESTORE);
			TRACE_ID(SC_TASKLIST);
			TRACE_ID(SC_SCREENSAVE);
			TRACE_ID(SC_HOTKEY);
			TRACE_ID(SC_DEFAULT);
			TRACE_ID(SC_SEPARATOR);
			TRACE_ID(SC_MONITORPOWER);
			TRACE_ID(SC_CONTEXTHELP);

		default:
			Log((int)_uMsg);
			return "Unknown";
		}
	};
	//--------------------------------------------------------------------------------
#endif  //defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
};
//--------------------------------------------------------------------------------