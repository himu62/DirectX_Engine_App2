/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Font.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <D2D1.h>
#include <DWrite.h>

#include <wrl/client.h>

using std::runtime_error;
using namespace D2D1;
using namespace Microsoft::WRL;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define FONT_STYLE_NORMAL	(char)0
#define FONT_STYLE_BOLD		(char)1
#define FONT_STYLE_ITALIC	(char)2

#define FONT_RENDERER_STATE_NONE	(char)0
#define FONT_RENDERER_STATE_EDGE	(char)1
#define FONT_RENDERER_STATE_SHADOW	(char)2

/******************************************************************************
	Font
******************************************************************************/

class Font : public IDWriteTextRenderer
{
public:
	Font(
		const ComPtr<ID2D1Factory> d2d_factory,
		const ComPtr<IDWriteFactory> dw_factory,
		const ComPtr<ID2D1RenderTarget> render_target,
		const std::wstring &font_name,
		const float font_size,
		const char style,
		const uint32_t color
		);

	Font(
		const ComPtr<ID2D1Factory> d2d_factory,
		const ComPtr<IDWriteFactory> dw_factory,
		const ComPtr<ID2D1RenderTarget> render_target,
		const std::wstring &font_name,
		const float font_size,
		const char style,
		const std::vector<uint32_t> colors,
		const std::vector<float> points
		);

	void SetTextColor(const uint32_t color);
	void SetEdgeColor(const uint32_t color);
	void SetShadowColor(const uint32_t color);

	// Gradient version
	void SetTextColor(const std::vector<uint32_t> colors, const std::vector<float> points);
	void SetEdgeColor(const std::vector<uint32_t> colors, const std::vector<float> points);
	//void SetShadowColor(const std::vector<uint32_t> colors, const std::vector<float> points);

	void Edge(const bool enable);
	void Shadow(const bool enable);

	void SetEdge(const float weight);
	void SetShadow(const D2D1_POINT_2F offset, const float blur_radius);

	/*****************************************************************************
		Virtual functions of IDWriteTextRenderer
	*****************************************************************************/

    IFACEMETHOD(DrawGlyphRun)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD(IsPixelSnappingDisabled)(
		__maybenull void* clientDrawingContext,
		__out BOOL* isDisabled
		);

	IFACEMETHOD(GetCurrentTransform)(
		__maybenull void* clientDrawingContext,
		__out DWRITE_MATRIX* transform
		);

	IFACEMETHOD(GetPixelsPerDip)(
		__maybenull void* clientDrawingContext,
		__out FLOAT* pixelsPerDip
		);

	IFACEMETHOD(DrawUnderline)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_UNDERLINE const* underline,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD(DrawStrikethrough)(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_STRIKETHROUGH const* strikethrough,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD(DrawInlineObject)(
		__maybenull void* clientDrawingContext,
		FLOAT originX,
		FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways,
		BOOL isRightToLeft,
		IUnknown* clientDrawingEffect
		);

	IFACEMETHOD_(unsigned long, AddRef)();
	IFACEMETHOD_(unsigned long,  Release)();
	IFACEMETHOD(QueryInterface)(IID const& riid, void** ppvObject);

private:
	friend class GraphicDevice;

	unsigned long m_RefCount;

	const ComPtr<ID2D1Factory> m_d2dFactory;
	const ComPtr<IDWriteFactory> m_dwFactory;
	const ComPtr<ID2D1RenderTarget> m_RenderTarget;

	ComPtr<IDWriteTextFormat> m_Format;

	ComPtr<ID2D1Brush> m_TextBrush;
	ComPtr<ID2D1Brush> m_EdgetBrush;
	ComPtr<ID2D1Brush> m_ShadowBrush;

	char m_State;
	float m_EdgeWeight;
	D2D1_POINT_2F m_ShadowOffset;
	float m_BlurRadius;
};