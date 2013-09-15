/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: WinMain.cpp
*	Author	: himu62
******************************************************************************/

#include "Application.hpp"

/******************************************************************************
	Entry Point
******************************************************************************/

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPWSTR lpCmdLine, int nCmdShow)
{
	Application app(L"Engine Test", 1280, 720, true);
	return app.Run();
}