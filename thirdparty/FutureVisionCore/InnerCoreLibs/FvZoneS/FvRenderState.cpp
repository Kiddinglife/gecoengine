#include "FvRenderState.h"
#include <d3d9.h>
#include <FvDebug.h>

//----------------------------------------------------------------------------
FvRenderState::FvRenderState(Type eType) : m_eType(eType)
{

}
//----------------------------------------------------------------------------
FvRenderState::Type FvRenderState::GetType()
{
	return m_eType;
}
//----------------------------------------------------------------------------
const FvBlendSate::Object FvBlendSate::ms_kDefault =
{
	FALSE,
	D3DBLEND_ONE,
	D3DBLEND_ZERO,
	D3DBLENDOP_ADD,
	FALSE,
	D3DBLEND_ONE,
	D3DBLEND_ZERO,
	D3DBLENDOP_ADD
};
//----------------------------------------------------------------------------
FvBlendSate::FvBlendSate() : FvRenderState(BLEND_STATE),
	m_pkBlendSate(&ms_kDefault)
{

}
//----------------------------------------------------------------------------
const FvColorWriteState::Object FvColorWriteState::ms_kDefault =
{
	0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F
};
//----------------------------------------------------------------------------
FvColorWriteState::FvColorWriteState() : FvRenderState(COLOR_WRITE_STATE),
	m_pkColorWriteState(&ms_kDefault)
{

}
//----------------------------------------------------------------------------
const FvDepthStencilState::Object FvDepthStencilState::ms_kDefault =
{
	D3DZB_TRUE,
	TRUE,
	D3DCMP_LESSEQUAL,
	FALSE,
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_KEEP,
	D3DCMP_ALWAYS,
	0,
	0xFFFFFFFF,
	0xFFFFFFFF,
	FALSE,
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_KEEP,
	D3DSTENCILOP_KEEP,
	D3DCMP_ALWAYS
};
//----------------------------------------------------------------------------
FvDepthStencilState::FvDepthStencilState()
	: FvRenderState(DEPTH_STENCIL_STATE), m_pkDepthStencilState(&ms_kDefault)
{

}
//----------------------------------------------------------------------------
const FvAlphaTestState::Object FvAlphaTestState::ms_kDefault =
{
	FALSE, 0, D3DCMP_ALWAYS
};
//----------------------------------------------------------------------------
FvAlphaTestState::FvAlphaTestState() : FvRenderState(ALPHA_TEST_STATE),
	m_pkAlphaTestState(&ms_kDefault)
{

}
//----------------------------------------------------------------------------
const FvRasterizerState::Object FvRasterizerState::ms_kDefault =
{
	D3DFILL_SOLID, D3DSHADE_GOURAUD, D3DCULL_CW
};
//----------------------------------------------------------------------------
FvRasterizerState::FvRasterizerState() : FvRenderState(RASTERIZER_STATE),
	m_pkRasterizerState(&ms_kDefault)
{

}
//----------------------------------------------------------------------------
FvRenderTargetState::FvRenderTargetState() : FvRenderState(RENDER_TARGET),
	m_u32Index(-1), m_pkRenderTarget(NULL)
{
	
}
//----------------------------------------------------------------------------
FvDepthStencilSurfaceState::FvDepthStencilSurfaceState()
	: FvRenderState(DEPTH_STENCIL_SURFACE), m_pkDepthStencil(NULL)
{

}
//----------------------------------------------------------------------------
FvViewportState::FvViewportState() : FvRenderState(VIEW_PORT),
	m_pkViewport(NULL)
{

}
//----------------------------------------------------------------------------
FvTextureState::FvTextureState() : FvRenderState(TEXTURE), m_u32Index(-1),
	m_pkTexture(NULL)
{

}
//----------------------------------------------------------------------------
const FvSamplerState::Object FvSamplerState::ms_kDefault =
{
	D3DTADDRESS_WRAP,
	D3DTADDRESS_WRAP,
	D3DTADDRESS_WRAP,
	0,
	D3DTEXF_POINT,
	D3DTEXF_POINT,
	D3DTEXF_NONE,
	0,
	0,
	1,
	0
};
//----------------------------------------------------------------------------
FvSamplerState::FvSamplerState() : FvRenderState(SAMPLER_STATE),
	m_u32Index(-1), m_pkSamplerState(NULL)
{

}
//----------------------------------------------------------------------------
FvVertexDeclarationState::FvVertexDeclarationState()
	: FvRenderState(VERTEX_DECLARATION), m_pkVertexDeclaration(NULL)
{

}
//----------------------------------------------------------------------------
FvStreamSourceState::FvStreamSourceState() : FvRenderState(STREAM_SOURCE),
	m_u32Index(-1), m_pkStreamData(NULL), m_u32OffsetInBytes(0),
	m_u32Stride(0), m_u32FrequencyParameter(1)
{

}
//----------------------------------------------------------------------------
FvIndexBufferState::FvIndexBufferState() : FvRenderState(INDEX_BUFFER),
	m_pkIndexData(NULL)
{

}
//----------------------------------------------------------------------------
FvVertexShaderState::FvVertexShaderState() : FvRenderState(VERTEX_SHADER),
	m_pkShader(NULL)
{

}
//----------------------------------------------------------------------------
FvPixelShaderState::FvPixelShaderState() : FvRenderState(PIXEL_SHADER),
	m_pkShader(NULL)
{

}
//----------------------------------------------------------------------------
FvShaderConstantState::FvShaderConstantState(FvRenderStateType eType)
	: FvRenderState(eType), m_u32StartRegister(0), m_pfFloatBuffer(NULL),
	m_u32Count(0)
{
	FV_ASSERT(m_eType >= VERTEX_SHADER_CONSTF && m_eType <= PIXEL_SHADER_CONSTB);
}
//----------------------------------------------------------------------------
