//{future header message}
#ifndef __FvRenderState_H__
#define __FvRenderState_H__

#include <FvBaseTypes.h>

struct IDirect3DSurface9;
struct IDirect3DBaseTexture9;
struct IDirect3DVertexDeclaration9;
struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
struct _D3DVIEWPORT9;

class FvRenderState
{
public:
	enum Type
	{
		BLEND_STATE,
		COLOR_WRITE_STATE,
		DEPTH_STENCIL_STATE,
		ALPHA_TEST_STATE,
		RASTERIZER_STATE,
		RENDER_TARGET,
		DEPTH_STENCIL_SURFACE,
		VIEW_PORT,
		TEXTURE,
		SAMPLER_STATE,
		VERTEX_DECLARATION,
		STREAM_SOURCE,
		INDEX_BUFFER,
		VERTEX_SHADER,
		PIXEL_SHADER,
		VERTEX_SHADER_CONSTF,
		PIXEL_SHADER_CONSTF,
		VERTEX_SHADER_CONSTI,
		PIXEL_SHADER_CONSTI,
		VERTEX_SHADER_CONSTB,
		PIXEL_SHADER_CONSTB,
		UNKNOWN
	};

	Type GetType();

protected:
	FvRenderState(Type eType);

	Type m_eType;
};

typedef FvRenderState::Type FvRenderStateType;

class FvBlendSate : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32Enable;				//D3DRS_ALPHABLENDENABLE
		FvUInt32 m_u32Src;					//D3DRS_SRCBLEND
		FvUInt32 m_u32Dst;					//D3DRS_DESTBLEND
		FvUInt32 m_u32BlendOp;				//D3DRS_BLENDOP
		FvUInt32 m_u32SeparateEnable;		//D3DRS_SEPARATEALPHABLENDENABLE
		FvUInt32 m_u32SrcAlpha;				//D3DRS_SRCBLENDALPHA
		FvUInt32 m_u32DstAlpha;				//D3DRS_DESTBLENDALPHA
		FvUInt32 m_u32BlendOpAlpha;			//D3DRS_BLENDOPALPHA
	};

	FvBlendSate();

	const Object* m_pkBlendSate;

	static const Object ms_kDefault;
};

class FvColorWriteState : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32ColorWrite0;			//D3DRS_COLORWRITEENABLE
		FvUInt32 m_u32ColorWrite1;			//D3DRS_COLORWRITEENABLE1
		FvUInt32 m_u32ColorWrite2;			//D3DRS_COLORWRITEENABLE2
		FvUInt32 m_u32ColorWrite3;			//D3DRS_COLORWRITEENABLE3
	};

	FvColorWriteState();

	const Object* m_pkColorWriteState;

	static const Object ms_kDefault;
};

class FvDepthStencilState : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32ZEnable;				//D3DRS_ZENABLE
		FvUInt32 m_u32ZWriteEnable;			//D3DRS_ZWRITEENABLE
		FvUInt32 m_u32ZFunc;				//D3DRS_ZFUNC

		FvUInt32 m_u32StencilEnable;		//D3DRS_STENCILENABLE
		FvUInt32 m_u32StencilFail;			//D3DRS_STENCILFAIL
		FvUInt32 m_u32StencilZFail;			//D3DRS_STENCILZFAIL
		FvUInt32 m_u32StencilPass;			//D3DRS_STENCILPASS
		FvUInt32 m_u32StencilFunc;			//D3DRS_STENCILFUNC
		FvUInt32 m_u32StencilRef;			//D3DRS_STENCILREF
		FvUInt32 m_u32StencilMask;			//D3DRS_STENCILMASK
		FvUInt32 m_u32StencilWriteMask;		//D3DRS_STENCILWRITEMASK

		FvUInt32 m_u32TwoSideStencilMode;	//D3DRS_TWOSIDEDSTENCILMODE
		FvUInt32 m_u32StencilFailCCW;		//D3DRS_CCW_STENCILFAIL
		FvUInt32 m_u32StencilZFailCCW;		//D3DRS_CCW_STENCILZFAIL
		FvUInt32 m_u32StencilPassCCW;		//D3DRS_CCW_STENCILPASS
		FvUInt32 m_u32StencilFuncCCW;		//D3DRS_CCW_STENCILFUNC
	};

	FvDepthStencilState();

	const Object* m_pkDepthStencilState;

	static const Object ms_kDefault;
};

class FvAlphaTestState : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32AlphaTestEnable;		//D3DRS_ALPHATESTENABLE
		FvUInt32 m_u32AlphaTestRef;			//D3DRS_ALPHAREF
		FvUInt32 m_u32AlphaTestFunc;		//D3DRS_ALPHAFUNC
	};

	FvAlphaTestState();

	const Object* m_pkAlphaTestState;

	static const Object ms_kDefault;
};

class FvRasterizerState : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32FillMode;				//D3DRS_FILLMODE
		FvUInt32 m_u32ShadeMode;			//D3DRS_SHADEMODE
		FvUInt32 m_u32CullMode;				//D3DRS_CULLMODE
	};

	FvRasterizerState();

	const Object* m_pkRasterizerState;

	static const Object ms_kDefault;
};

class FvRenderTargetState : public FvRenderState
{
public:
	FvRenderTargetState();

	FvUInt32 m_u32Index;
	IDirect3DSurface9* m_pkRenderTarget;
};

class FvDepthStencilSurfaceState : public FvRenderState
{
public:
	FvDepthStencilSurfaceState();

	IDirect3DSurface9* m_pkDepthStencil;
};

class FvViewportState : public FvRenderState
{
public:
	FvViewportState();

	_D3DVIEWPORT9* m_pkViewport;
};

class FvTextureState : public FvRenderState
{
public:
	FvTextureState();

	FvUInt32 m_u32Index;
	IDirect3DBaseTexture9* m_pkTexture;
};

class FvSamplerState : public FvRenderState
{
public:
	struct Object
	{
		FvUInt32 m_u32AddressU;				//D3DSAMP_ADDRESSU
		FvUInt32 m_u32AddressV;				//D3DSAMP_ADDRESSV
		FvUInt32 m_u32AddressW;				//D3DSAMP_ADDRESSW
		FvUInt32 m_u32BorderColor;			//D3DSAMP_BORDERCOLOR
		FvUInt32 m_u32MagFilter;			//D3DSAMP_MAGFILTER
		FvUInt32 m_u32MinFilter;			//D3DSAMP_MINFILTER
		FvUInt32 m_u32MipFilter;			//D3DSAMP_MIPFILTER
		FvUInt32 m_u32MipMapLodBias;		//D3DSAMP_MIPMAPLODBIAS
		FvUInt32 m_u32MaxMipLevel;			//D3DSAMP_MAXMIPLEVEL
		FvUInt32 m_u32MaxAnisotropy;		//D3DSAMP_MAXANISOTROPY
		FvUInt32 m_u32SRGB;					//D3DSAMP_SRGBTEXTURE
	};

	FvSamplerState();

	FvUInt32 m_u32Index;
	const Object* m_pkSamplerState;

	static const Object ms_kDefault;
};

class FvVertexDeclarationState : public FvRenderState
{
public:
	FvVertexDeclarationState();

	IDirect3DVertexDeclaration9* m_pkVertexDeclaration;
};

class FvStreamSourceState : public FvRenderState
{
public:
	FvStreamSourceState();

	FvUInt32 m_u32Index;
	IDirect3DVertexBuffer9* m_pkStreamData;
	FvUInt32 m_u32OffsetInBytes;
	FvUInt32 m_u32Stride;
	FvUInt32 m_u32FrequencyParameter;
};

class FvIndexBufferState : public FvRenderState
{
public:
	FvIndexBufferState();

	IDirect3DIndexBuffer9* m_pkIndexData;
};

class FvVertexShaderState : public FvRenderState
{
public:
	FvVertexShaderState();

	IDirect3DVertexShader9* m_pkShader;
};

class FvPixelShaderState : public FvRenderState
{
public:
	FvPixelShaderState();

	IDirect3DPixelShader9* m_pkShader;
};

class FvShaderConstantState : public FvRenderState
{
public:
	FvShaderConstantState(FvRenderStateType eType);

	FvUInt32 m_u32StartRegister;
	union
	{
		float* m_pfFloatBuffer;
		FvUInt32* m_pu32IntBuffer;
		FvUInt32* m_pu32BoolBuffer;
	};
	FvUInt32 m_u32Count;
};

#endif // __FvRenderState_H__
