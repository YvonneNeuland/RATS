#include "stdafx.h"
#include "./Log.h"
#include "./DXLog.h"

#include <string>
//--------------------------------------------------------------------------------
namespace COMBINE(Log, __ID__) {

#ifdef VISUAL_STUDIO_VISUAL_DEBUG_INFO
	#include <Initguid.h>
	#include <dxgidebug.h >
	#include <d3d11_1.h>
#endif

#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#include <comdef.h>
#endif

#ifdef VISUAL_STUDIO_VISUAL_DEBUG_INFO
	//--------------------------------------------------------------------------------
	#pragma comment( lib, "dxguid.lib")

	typedef HRESULT(__stdcall *GetDXGIFunction)(const IID&, void**);
	GetDXGIFunction test;
	HMODULE						m_hModule = nullptr;
	IDXGIDebug *				m_pDXGIDebug = nullptr;
	ID3DUserDefinedAnnotation * m_pAnnotation = nullptr;
	//--------------------------------------------------------------------------------
	void CreateDXNaming(void *_context) {
		DXCall(static_cast<ID3D11DeviceContext *>(_context)->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void **)&m_pAnnotation) );

		//Done this way so the current device does not get an extra reference 
		m_hModule = LoadLibraryEx(L"Dxgidebug.dll", 0, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);
		if (nullptr == m_hModule) {
			Log("Could not find Dxgidebug.dll");
			return;
		}

		GetDXGIFunction GetDXGI = reinterpret_cast<GetDXGIFunction>(GetProcAddress(m_hModule, "DXGIGetDebugInterface") );
		if (GetDXGI)
			GetDXGI(__uuidof(IDXGIDebug), (void**)&m_pDXGIDebug);
		else {
			Log("Warning: Live objects not collected, D3D11_CREATE_DEVICE_DEBUG flag not set");
}
	}
	//--------------------------------------------------------------------------------
	bool DXFunctionCall(long _hr) {
		if (_hr == S_OK)
			return true;

		_com_error err(_hr);
		Log(err.ErrorMessage());
		return false;
	}
	//--------------------------------------------------------------------------------
	void DebugReport(int _ref, void *_context) {
		//m_pAnnotation has a ref to the context that throws off the report
		if (nullptr != m_pAnnotation) {
			int num = static_cast<ID3D11DeviceContext *>(_context)->Release();
			m_pAnnotation = nullptr;
			//SAFE_RELEASE(m_pAnnotation);
			_ref -= 1;
		}

		if (_ref > 0) {
			Log("\n** DX LIVE OBJECT REPORT **\n");
			if (m_pDXGIDebug)
				m_pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
			else
				Log("DXGI Module was not found");
			Log("\n** END OF OBJECT REPORT **\n");
		}
	}
	//--------------------------------------------------------------------------------
	void DXRelease() {
		m_pAnnotation = nullptr;
		m_pDXGIDebug = nullptr;
		FreeLibrary(m_hModule);
		m_hModule = nullptr;
	}
	//--------------------------------------------------------------------------------
	DXEvent::DXEvent(const wchar_t * _name) {
		if (nullptr != m_pAnnotation)
			m_pAnnotation->BeginEvent(_name);
		else
			Log("No Pointer cannot Group Visual debugging calls");
	}
	//--------------------------------------------------------------------------------
	DXEvent::DXEvent(const char *_name) {
		::std::wstring var(_name, _name + strlen(_name));
		if (nullptr != m_pAnnotation)
			m_pAnnotation->BeginEvent(var.c_str() );
		else
			Log("No Pointer cannot Group Visual debugging calls");
	}
	//--------------------------------------------------------------------------------
	DXEvent::~DXEvent() {
		if (nullptr != m_pAnnotation)
			m_pAnnotation->EndEvent();
		else
			Log("No Pointer cannot end log");

	};
	//--------------------------------------------------------------------------------
#elif defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	bool DXFunctionCall(long _hr) {
		if (_hr == S_OK)
			return true;

		_com_error err(_hr);
		Log(err.ErrorMessage());
		return false;
	}
#endif //VISUAL_STUDIO_VISUAL_DEBUG_INFO
};
//--------------------------------------------------------------------------------