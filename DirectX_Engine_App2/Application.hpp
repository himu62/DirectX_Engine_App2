/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: Core_0.0.0
*	File	: Application.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

// Windows 7 以降
#define WINVER 0x0700
#define _WIN32_WINNT 0x0700

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define _WIN32_DCOM

#include <memory>
#include <string>

#include <CommCtrl.h>

#include <wrl/client.h>
#include <boost/noncopyable.hpp>
#include <boost/intrusive_ptr.hpp>

#include "GraphicDevice.hpp"

#pragma comment(lib, "Comctl32.lib")

/******************************************************************************
	CoInitter クラス
******************************************************************************/

class CoInitter{
public:
	explicit CoInitter();
	~CoInitter();
};

/******************************************************************************
	Application クラス
******************************************************************************/

class Application : private boost::noncopyable
{
public:
	explicit Application(
		const std::wstring &caption, const int width, const int height, const bool windowed
		);

	int AddRef();
	int Release();

	virtual int Run();
	void Exit();

private:
	void Update();

	static LRESULT CALLBACK SubclassProcedure(
		const HWND window_handle,
		const UINT message,
		const WPARAM wp,
		const LPARAM lp,
		const UINT_PTR this_ptr,
		DWORD_PTR
		);

	int ref_count;

	const HWND m_WindowHandle;

protected:
	CoInitter m_Initter;

	std::unique_ptr<GraphicDevice> m_GraphicDevice;
};