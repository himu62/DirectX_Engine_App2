/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Font.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "Font.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Text Format
ComPtr<IDWriteTextFormat> InitTextFormat(
	const ComPtr<IDWriteFactory> dw_factory,
	const std::wstring &font_name,
	const float font_size,
	const char style
	)
{
	assert(dw_factory.Get());

	/*
	Enumerate installed fonts
	if the font is not found, add the font data to the system from the archive

	vector<uint8_t> font_data = ReadFile((font_name + ".ttf").c_str());
	DWORD font_num = 0;
	AddFontMemResourceEx(&font_data.front(), font_data.size(), NULL, &font_num);
	*/

	ComPtr<IDWriteTextFormat> format(nullptr);
	if(FAILED(dw_factory->CreateTextFormat(
		font_name.c_str(),
		nullptr,
		(style & FONT_STYLE_BOLD) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR,
		(style & FONT_STYLE_ITALIC) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		font_size,
		L"ja-jp",
		&format
		)))
	{
		throw runtime_error("Failed to create a text format");
	}

	return format;
}

/******************************************************************************
	Font
******************************************************************************/

Font::Font(
	const ComPtr<ID2D1Factory> d2d_factory,
	const ComPtr<IDWriteFactory> dw_factory,
	const ComPtr<ID2D1RenderTarget> render_target,
	const std::wstring &font_name,
	const float font_size,
	const char style,
	const uint32_t color
	) try :
	m_RefCount(1),
	m_d2dFactory(d2d_factory),
	m_dwFactory(dw_factory),
	m_RenderTarget(render_target),
	m_Format(InitTextFormat(dw_factory, font_name, font_size, style)),
	m_TextBrush(CreateSolidColorBrush(color))
{}
catch(const runtime_error& e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

Font::Font(
	const ComPtr<ID2D1Factory> d2d_factory,
	const ComPtr<IDWriteFactory> dw_factory,
	const ComPtr<ID2D1RenderTarget> render_target,
	const std::wstring &font_name,
	const float font_size,
	const char style,
	const vector<uint32_t> &colors,
	const vector<float> &positions,
	const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES &props
	) try :
	m_RefCount(1),
	m_d2dFactory(d2d_factory),
	m_dwFactory(dw_factory),
	m_RenderTarget(render_target),
	m_Format(InitTextFormat(dw_factory, font_name, font_size, style)),
	m_TextBrush(CreateLinearGradientBrush(colors, positions, props))
{}
catch(const runtime_error& e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Color Settings

void Font::SetTextColor(const uint32_t color)
{
	m_TextBrush = CreateSolidColorBrush(color);
}

void Font::SetEdgeColor(const uint32_t color)
{
	m_EdgetBrush = CreateSolidColorBrush(color);
}

void Font::SetShadowColor(const uint32_t color)
{
	m_ShadowBrush = CreateSolidColorBrush(color);
}

void Font::SetTextColor(
	const vector<uint32_t> &colors,
	const vector<float> &positions,
	const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES &props
	)
{
	m_TextBrush = CreateLinearGradientBrush(colors, positions, props);
}

void Font::SetEdgeColor(
	const vector<uint32_t> &colors,
	const vector<float> &positions,
	const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES &props
	)
{
	m_EdgetBrush = CreateLinearGradientBrush(colors, positions, props);
}

//*****************************************************************************
// Decoration Settings

void Font::Edge(const bool enable)
{
	if(enable)
	{
		m_State |= FONT_RENDERER_STATE_EDGE;
	}
	else
	{
		m_State &= ~FONT_RENDERER_STATE_EDGE;
	}
}

void Font::Shadow(const bool enable)
{
	if(enable)
	{
		m_State |= FONT_RENDERER_STATE_SHADOW;
	}
	else
	{
		m_State &= ~FONT_RENDERER_STATE_SHADOW;
	}
}

void Font::SetEdge(const float weight)
{
	m_EdgeWeight = weight;
}

void Font::SetShadow(const D2D1_POINT_2F offset, const float blur_radius)
{
	m_ShadowOffset = offset;
	m_BlurRadius = blur_radius;
}

//*****************************************************************************
// Virtual functions of IDWriteTextRenderer

IFACEMETHODIMP Font::DrawGlyphRun(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	__in DWRITE_GLYPH_RUN const* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
	IUnknown* clientDrawingEffect
	)
{
	assert(m_d2dFactory.Get());
	assert(m_dwFactory.Get());
	assert(m_RenderTarget.Get());

	ComPtr<ID2D1BitmapRenderTarget> bitmap_target(nullptr);
	m_RenderTarget->CreateCompatibleRenderTarget(&bitmap_target);

	ComPtr<ID2D1PathGeometry> path(nullptr);
	if(FAILED(m_d2dFactory->CreatePathGeometry(&path)))
	{
		return E_FAIL;
	}

	ComPtr<ID2D1GeometrySink> sink(nullptr);
	if(FAILED(path->Open(&sink)))
	{
		return E_FAIL;
	}

	if(FAILED(glyphRun->fontFace->GetGlyphRunOutline(
		glyphRun->fontEmSize,
		glyphRun->glyphIndices,
		glyphRun->glyphAdvances,
		glyphRun->glyphOffsets,
		glyphRun->glyphCount,
		glyphRun->isSideways,
		glyphRun->bidiLevel % 2,
		sink.Get()
		)))
	{
		return E_FAIL;
	}

	const Matrix3x2F matrix = Matrix3x2F(
		1.f, 0.f,
		0.f, 1.f,
		baselineOriginX, baselineOriginY
		);

	ComPtr<ID2D1TransformedGeometry> geometry(nullptr);
	if(FAILED(m_d2dFactory->CreateTransformedGeometry(
		path.Get(),
		&matrix,
		&geometry
		)))
	{
		return E_FAIL;
	}

	if(m_State & FONT_RENDERER_STATE_SHADOW)
	{
		// render drop shadow

		ComPtr<ID2D1GeometrySink> shadow_sink(nullptr);
		if(FAILED(path->Open(&shadow_sink)))
		{
			return E_FAIL;
		}

		vector<DWRITE_GLYPH_OFFSET> offsets(
			glyphRun->glyphOffsets,
			glyphRun->glyphOffsets + sizeof(DWRITE_GLYPH_OFFSET) * glyphRun->glyphCount
			);
		for(unsigned int i=0; i<offsets.size(); ++i)
		{
			offsets[i].advanceOffset	+= m_ShadowOffset.x;
			offsets[i].ascenderOffset	-= m_ShadowOffset.y;
		}

		if(FAILED(glyphRun->fontFace->GetGlyphRunOutline(
			glyphRun->fontEmSize,
			glyphRun->glyphIndices,
			glyphRun->glyphAdvances,
			&offsets.front(),
			glyphRun->glyphCount,
			glyphRun->isSideways,
			glyphRun->bidiLevel % 2,
			shadow_sink.Get()
			)))
		{
			return E_FAIL;
		}

		shadow_sink->Close();

		const Matrix3x2F shadow_matrix = Matrix3x2F(
			1.f, 0.f,
			0.f, 1.f,
			baselineOriginX + m_ShadowOffset.x, baselineOriginY + m_ShadowOffset.y
			);

		ComPtr<ID2D1TransformedGeometry> shadow_geometry(nullptr);
		if(FAILED(m_d2dFactory->CreateTransformedGeometry(
			path.Get(),
			&shadow_matrix,
			&shadow_geometry
			)))
		{
			return E_FAIL;
		}

		m_RenderTarget->DrawGeometry(
			shadow_geometry.Get(),
			m_ShadowBrush.Get(),
			10.f + ((m_State & FONT_RENDERER_STATE_EDGE) ? m_EdgeWeight : 0.f),
			nullptr
			);
		m_RenderTarget->FillGeometry(shadow_geometry.Get(), m_ShadowBrush.Get());
	}

	if(m_State & FONT_RENDERER_STATE_EDGE)
	{
		m_RenderTarget->DrawGeometry(
			geometry.Get(),
			m_EdgetBrush.Get(),
			m_EdgeWeight,
			nullptr
			);
	}

	m_RenderTarget->FillGeometry(geometry.Get(), m_TextBrush.Get());

	ComPtr<ID2D1Bitmap> bitmap(nullptr);
	bitmap_target->GetBitmap(&bitmap);

	m_RenderTarget->DrawBitmap(bitmap.Get());

	return S_OK;
}

IFACEMETHODIMP Font::IsPixelSnappingDisabled(
	__maybenull void* clientDrawingContext,
	__out BOOL* isDisabled
	)
{
	*isDisabled = false;

	return S_OK;
}

IFACEMETHODIMP Font::GetCurrentTransform(
	__maybenull void* clientDrawingContext,
	__out DWRITE_MATRIX* transform
	)
{
	assert(m_RenderTarget.Get());

	m_RenderTarget->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));

	return S_OK;
}

IFACEMETHODIMP Font::GetPixelsPerDip(
	__maybenull void* clientDrawingContext,
	__out FLOAT* pixelsPerDip
	)
{
	assert(m_RenderTarget.Get());

	float x, y_unused;

	m_RenderTarget->GetDpi(&x, &y_unused);
	*pixelsPerDip = x / 96.f;

	return S_OK;
}

IFACEMETHODIMP Font::DrawUnderline(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_UNDERLINE const* underline,
	IUnknown* clientDrawingEffect
	)
{
	return E_NOTIMPL;
}

IFACEMETHODIMP Font::DrawStrikethrough(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	__in DWRITE_STRIKETHROUGH const* strikethrough,
	IUnknown* clientDrawingEffect
	)
{
	return E_NOTIMPL;
}

IFACEMETHODIMP Font::DrawInlineObject(
	__maybenull void* clientDrawingContext,
	FLOAT originX,
	FLOAT originY,
	IDWriteInlineObject* inlineObject,
	BOOL isSideways,
	BOOL isRightToLeft,
	IUnknown* clientDrawingEffect
	)
{
	return E_NOTIMPL;
}

IFACEMETHODIMP_(unsigned long) Font::AddRef()
{
	return InterlockedIncrement(&m_RefCount);
}

IFACEMETHODIMP_(unsigned long) Font::Release()
{
	unsigned long newCount = InterlockedDecrement(&m_RefCount);

	if(!newCount)
	{
		delete this;
		return 0;
	}

	return newCount;
}

IFACEMETHODIMP Font::QueryInterface(IID const& riid, void** ppvObject)
{
	if(riid == __uuidof(IDWriteTextRenderer))
	{
		*ppvObject = this;
	}
	else if(riid == __uuidof(IDWritePixelSnapping))
	{
		*ppvObject = this;
	}
	else if(riid == __uuidof(IDWritePixelSnapping))
	{
		*ppvObject = this;
	}
	else
	{
		*ppvObject = NULL;
		return E_FAIL;
	}

	this->AddRef();

	return S_OK;
}


//*****************************************************************************
// Create Brush

ComPtr<ID2D1SolidColorBrush> Font::CreateSolidColorBrush(const uint32_t color)
{
	assert(m_RenderTarget.Get());

	ComPtr<ID2D1SolidColorBrush> brush(nullptr);
	if(FAILED(m_RenderTarget->CreateSolidColorBrush(ColorF(color), &brush)))
	{
		throw runtime_error("Failed to create a brush");
	}

	return brush;
}

ComPtr<ID2D1LinearGradientBrush> Font::CreateLinearGradientBrush(
	const vector<uint32_t> &colors,
	const vector<float> &positions,
	const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES &props
	)
{
	assert(m_RenderTarget.Get());

	vector<D2D1_GRADIENT_STOP> stops(colors.size());
	for(unsigned int i=0; i<stops.size(); ++i)
	{
		stops[i].color		= ColorF(colors[i]);
		stops[i].position	= positions[i];
	}

	ComPtr<ID2D1GradientStopCollection> collection(nullptr);
	if(FAILED(m_RenderTarget->CreateGradientStopCollection(
		&stops.front(),
		stops.size(),
		&collection
		)))
	{
		throw runtime_error("Failed to create a gradient stop collection");
	}

	ComPtr<ID2D1LinearGradientBrush> brush(nullptr);
	if(FAILED(m_RenderTarget->CreateLinearGradientBrush(
		props,
		collection.Get(),
		&brush
		)))
	{
		throw runtime_error("Failed to create a brush");
	}

	return brush;
}