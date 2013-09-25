/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: GraphicDevice.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <string>

#include <D3D11.h>
#include <DXGI.h>
#include <D2D1.h>
#include <D2D1Helper.h>

#include <wrl/client.h>

#include <boost/noncopyable.hpp>

#include "Texture.hpp"

using std::runtime_error;
using namespace D2D1;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")

/******************************************************************************
	GraphicDevice
******************************************************************************/

class GraphicDevice : private boost::noncopyable
{
public:
	explicit GraphicDevice(const HWND window_handle);

	void BeginDraw() const;
	void EndDraw() const;

	Texture CreateTexture(const std::wstring &file_name) const;
	void DrawTexture(const Texture &tex) const;

private:	
	const ComPtr<ID3D11Device> m_d3dDevice;
	const ComPtr<IDXGISwapChain> m_SwapChain;

	const ComPtr<ID2D1Factory> m_d2dFactory;
	const ComPtr<ID2D1RenderTarget> m_RenderTarget;
};