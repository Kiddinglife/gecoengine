//{future header message}
#ifndef __FvRenderer_H__
#define __FvRenderer_H__

#include <FvBaseTypes.h>
#include "FvRenderState.h"
#include "FvRenderStateNode.h"
#include <FvDoubleLink.h>
#include <vector>

#define FV_MAX_RENDER_TARGET_NUMBER (4)
#define FV_MAX_TEXTURE_NUMBER (12)
#define FV_MAX_STREAM_NUMBER (8)

struct IDirect3DDevice9;

class FvRenderer
{
public:
	FvRenderer(IDirect3DDevice9* pkDevice);

	~FvRenderer();

	void PushState(FvRenderState* pkState);

	void PopState(FvRenderState* pkState);

	void Draw(FvRenderDrawNode::Data* pkData);

protected:
	void UpdateStates();

	void UpdateState(FvDoubleLinkNode2<FvRenderStateType>* pkState);

	void ChangeBlendState(const FvBlendSate::Object*& pkCurrent, const FvBlendSate::Object*& pkLast);

	void ChangeColorWriteState(const FvColorWriteState::Object*& pkCurrent, const FvColorWriteState::Object*& pkLast);

	void ChangeDepthStencilState(const FvDepthStencilState::Object*& pkCurrent, const FvDepthStencilState::Object*& pkLast);

	void ChangeAlphaTestState(const FvAlphaTestState::Object*& pkCurrent, const FvAlphaTestState::Object*& pkLast);

	void ChangeRasterizerState(const FvRasterizerState::Object*& pkCurrent, const FvRasterizerState::Object*& pkLast);

	void ChangeRenderTarget(FvUInt32 u32Index, IDirect3DSurface9*& pkCurrent, IDirect3DSurface9*& pkLast);

	void ChangeDepthStencilSurface(IDirect3DSurface9*& pkCurrent, IDirect3DSurface9*& pkLast);

	void ChangeViewport(_D3DVIEWPORT9*& pkCurrent, _D3DVIEWPORT9*& pkLast);

	void ChangeTexture(FvUInt32 u32Index, IDirect3DBaseTexture9*& pkCurrent, IDirect3DBaseTexture9*& pkLast);

	void ChangeSamplerState(FvUInt32 u32Index, const FvSamplerState::Object*& pkCurrent, const FvSamplerState::Object*& pkLast);

	void ChangeVertexDeclaration(IDirect3DVertexDeclaration9*& pkCurrent, IDirect3DVertexDeclaration9*& pkLast);

	void ChangeIndexBuffer(IDirect3DIndexBuffer9*& pkCurrent, IDirect3DIndexBuffer9*& pkLast);

	void ChangeVertexShader(IDirect3DVertexShader9*& pkCurrent, IDirect3DVertexShader9*& pkLast);

	void ChangePixelShader(IDirect3DPixelShader9*& pkCurrent, IDirect3DPixelShader9*& pkLast);

	template <class T>
	struct ObjectStack : public FvDoubleLinkNode2<FvRenderStateType>
	{
		ObjectStack() : m_pkCurrent(NULL) {}

		std::vector<T> m_kStack;
		T m_pkCurrent;
	};

	IDirect3DDevice9* m_pkDevice;

	ObjectStack<const FvBlendSate::Object*> m_kBlendStateStack;
	ObjectStack<const FvColorWriteState::Object*> m_kColorWriteStack;
	ObjectStack<const FvDepthStencilState::Object*> m_kDepthStencilStateStack;
	ObjectStack<const FvAlphaTestState::Object*> m_kAlphaTestStateStack;
	ObjectStack<const FvRasterizerState::Object*> m_kRasterizerStateStack;

	ObjectStack<IDirect3DSurface9*> m_akRenderTargetStacks[FV_MAX_RENDER_TARGET_NUMBER];
	ObjectStack<IDirect3DSurface9*> m_kDepthStencilStack;

	ObjectStack<_D3DVIEWPORT9*> m_kViewportStack;

	ObjectStack<IDirect3DBaseTexture9*> m_akTextureStacks[FV_MAX_TEXTURE_NUMBER];
	ObjectStack<const FvSamplerState::Object*> m_akSamplerStacks[FV_MAX_TEXTURE_NUMBER];

	ObjectStack<IDirect3DVertexDeclaration9*> m_kVertexDeclarationStack;

	struct Stream
	{
		Stream(IDirect3DVertexBuffer9* pkVertexBuffer) : m_pkStreamData(NULL), m_u32OffsetInBytes(0), m_u32Stride(0), m_u32FrequencyParameter(1) {}

		bool operator != (const Stream& kStream) const
		{
			if(m_pkStreamData != kStream.m_pkStreamData) return true;
			if(m_u32OffsetInBytes != kStream.m_u32OffsetInBytes) return true;
			if(m_u32Stride != kStream.m_u32Stride) return true;
			if(m_u32FrequencyParameter != kStream.m_u32FrequencyParameter) return true;
			return false;
		}

		IDirect3DVertexBuffer9* m_pkStreamData;
		FvUInt32 m_u32OffsetInBytes;
		FvUInt32 m_u32Stride;
		FvUInt32 m_u32FrequencyParameter;
	};
	ObjectStack<Stream> m_akStreamStacks[FV_MAX_STREAM_NUMBER];
	ObjectStack<IDirect3DIndexBuffer9*> m_kIndexBufferStack;
	ObjectStack<IDirect3DVertexShader9*> m_kVertexShaderStack;
	ObjectStack<IDirect3DPixelShader9*> m_kPixelShaderStack;

	void ChangeStream(FvUInt32 u32Index, Stream& pkCurrent, Stream& pkLast);

	struct ConstantF
	{
		ConstantF(float* pfBuffer) : m_u32Start(-1), m_pfBuffer(pfBuffer), m_u32Count(-1), m_u32MaxBufferUsed(0) {}

		FvUInt32 m_u32Start;
		float* m_pfBuffer;
		FvUInt32 m_u32Count;
		FvUInt32 m_u32MaxBufferUsed;
	};

	ObjectStack<ConstantF> m_kVertexConstBufferStack;
	ObjectStack<ConstantF> m_kPixelConstBufferStack;

	static void DoPopShaderConstant(ConstantF& kCurrent,
		ConstantF& kLast, std::vector<ConstantF> kStack,
		FvUInt32& u32OutStart, FvUInt32& u32OutCount);

	FvDoubleLink<FvRenderStateType> m_kStateNeedUpdate;
};

#endif // __FvRenderer_H__
