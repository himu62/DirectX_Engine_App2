/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: GraphicDevice.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "GraphicDevice.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Direct3D11 Device
ComPtr<ID3D11Device> InitD3DDevice()
{
	UINT f_device = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	f_device |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE drivers[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT n_drivers = sizeof(drivers) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT n_levels = sizeof(levels) / sizeof(D3D_FEATURE_LEVEL);

	ComPtr<ID3D11Device> d3d_device(nullptr);
	HRESULT hr = E_FAIL;
	for(UINT i=0; i<n_drivers; ++i)
	{
		hr = ::D3D11CreateDevice(
			nullptr,
			drivers[i],
			nullptr,
			f_device,
			levels,
			n_levels,
			D3D11_SDK_VERSION,
			&d3d_device,
			nullptr,
			nullptr
			);

		if(SUCCEEDED(hr))
		{
			break;
		}
	}
	if(FAILED(hr))
	{
		throw runtime_error("Failed to create a Direct3D device");
	}

	return d3d_device;
}

//*****************************************************************************
// Swap Chain
ComPtr<IDXGISwapChain> InitSwapChain(
	const ComPtr<ID3D11Device> d3d_device,
	const HWND window_handle
	)
{
	assert(d3d_device.Get());
	assert(window_handle);

	RECT rect;
	::GetClientRect(window_handle, &rect);

	DXGI_SWAP_CHAIN_DESC sc_desc;
	sc_desc.BufferDesc.Width					= rect.right - rect.left;
	sc_desc.BufferDesc.Height					= rect.bottom - rect.top;
	sc_desc.BufferDesc.RefreshRate.Numerator	= 0;
	sc_desc.BufferDesc.RefreshRate.Denominator	= 1;
	sc_desc.BufferDesc.Format					= DXGI_FORMAT_B8G8R8A8_UNORM;
	sc_desc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_STRETCHED;
	sc_desc.SampleDesc.Count	= 1;
	sc_desc.SampleDesc.Quality	= 0;
	sc_desc.BufferUsage		= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.BufferCount		= 1;
	sc_desc.OutputWindow	= window_handle;
	sc_desc.Windowed		= true;
	sc_desc.SwapEffect		= DXGI_SWAP_EFFECT_DISCARD;
	sc_desc.Flags			= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGIFactory> dxgi_factory(nullptr);
	if(FAILED(::CreateDXGIFactory(__uuidof(IDXGIFactory), &dxgi_factory)))
	{
		throw runtime_error("Failed to create a DXGI factory");
	}
	
	ComPtr<IDXGISwapChain> swap_chain(nullptr);
	if(FAILED(dxgi_factory->CreateSwapChain(d3d_device.Get(), &sc_desc, &swap_chain)))
	{
		throw runtime_error("Failed to create a swap chain");
	}

	return swap_chain;
}

//*****************************************************************************
// Direct2D Factory
ComPtr<ID2D1Factory> InitD2DFactory()
{
	ComPtr<ID2D1Factory> d2d_factory(nullptr);
	if(FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &d2d_factory)))
	{
		throw runtime_error("Failed to create a Direct2D factory");
	}

	return d2d_factory;
}

//*****************************************************************************
// Direct2D Render Target
ComPtr<ID2D1RenderTarget> InitRenderTarget(
	const ComPtr<IDXGISwapChain> swap_chain,
	const ComPtr<ID2D1Factory> d2d_factory
	)
{
	assert(swap_chain.Get());
	assert(d2d_factory.Get());

	ComPtr<IDXGISurface> surface(nullptr);
	swap_chain->GetBuffer(0, __uuidof(IDXGISurface), &surface);

	float dpiX, dpiY;
	d2d_factory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props;
	props.type			= D2D1_RENDER_TARGET_TYPE_DEFAULT;
	props.pixelFormat	= PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	props.dpiX			= dpiX;
	props.dpiY			= dpiY;
	props.usage			= D2D1_RENDER_TARGET_USAGE_NONE;
	props.minLevel		= D2D1_FEATURE_LEVEL_DEFAULT;

	ComPtr<ID2D1RenderTarget> render_target(nullptr);
	if(FAILED(d2d_factory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, &render_target)))
	{
		throw runtime_error("Failed to create a render target");
	}

	return render_target;
}

/******************************************************************************
	GraphicDevice
******************************************************************************/

GraphicDevice::GraphicDevice(const HWND window_handle) try :
	m_d2dFactory(InitD2DFactory()),
	m_d3dDevice(InitD3DDevice()),
	m_SwapChain(InitSwapChain(m_d3dDevice, window_handle)),
	m_RenderTarget(InitRenderTarget(m_SwapChain, m_d2dFactory))
{}
catch(std::runtime_error const &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Start of rendering
void GraphicDevice::BeginDraw() const
{
	assert(m_SwapChain.Get());
	assert(m_RenderTarget.Get());

	m_RenderTarget->BeginDraw();

	m_RenderTarget->SetTransform(Matrix3x2F::Identity());
	m_RenderTarget->Clear(ColorF(0xffffff));
}

//*****************************************************************************
// End of rendering
void GraphicDevice::EndDraw() const
{
	assert(m_SwapChain.Get());
	assert(m_RenderTarget.Get());

	m_RenderTarget->EndDraw();

	m_SwapChain->Present(1, 0);
}

//*****************************************************************************
// Create a texture
Texture GraphicDevice::CreateTexture(const std::wstring &file_name) const
{
	assert(m_RenderTarget.Get());
	return Texture(TextureKey(m_RenderTarget, file_name));
}

//*****************************************************************************
// Draw a texture
void GraphicDevice::DrawTexture(const Texture &tex) const
{
	assert(m_RenderTarget.Get());
	assert(tex.get().m_Bitmap.Get());

	const D2D1_RECT_F rect = tex.get().GetRect();
	const D2D1_POINT_2F center = Point2F(
		(rect.right - rect.left) / 2,
		(rect.bottom - rect.top) / 2
		);

	m_RenderTarget->SetTransform(
		Matrix3x2F::Rotation(tex.get().m_Rotate, center) *
		Matrix3x2F::Scale(tex.get().m_Scale, center)
		);

	m_RenderTarget->DrawBitmap(tex.get().m_Bitmap.Get(), rect);

	m_RenderTarget->SetTransform(Matrix3x2F::Identity());
}