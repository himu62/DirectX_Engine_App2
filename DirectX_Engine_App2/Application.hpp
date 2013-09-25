/******************************************************************************
	Project	: Direct3D11 Sample
	Version	: 0.0.0
	File	: Application.hpp
	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <string>
#include <memory>

// above Windows 7
#define _WIN32_WINNT	0x0601
#define WINVER			0x0601

#define _WIN32_DCOM

#include <Windows.h>

#include <CommCtrl.h>
#include <wrl/client.h>

#include <boost/noncopyable.hpp>

#include "GraphicDevice.hpp"
#include "SoundDevice.hpp"

using std::runtime_error;

#pragma comment(lib, "ComCtl32.lib")

/******************************************************************************
	CoInitter
******************************************************************************/

class CoInitter
{
public:
	explicit CoInitter();
	~CoInitter();
};

/******************************************************************************
	Application
******************************************************************************/

class Application : private boost::noncopyable
{
public:
	explicit Application(
		const std::wstring &caption,
		const int width,
		const int height,
		const bool windowed
		);

	int Run();
	
private:
	friend HWND InitWindow(const std::wstring &caption, const int width, const int height);

	void Update();

	const HWND m_WindowHandle;

	static LRESULT CALLBACK SubclassProcedure(
		const HWND window_handle,
		const UINT message,
		const WPARAM wprm,
		const LPARAM lprm,
		const UINT_PTR this_ptr,
		DWORD_PTR
		);

	CoInitter m_Initter;

	std::unique_ptr<GraphicDevice> m_GraphicDevice;
	std::unique_ptr<SoundDevice> m_SoundDevice;
};