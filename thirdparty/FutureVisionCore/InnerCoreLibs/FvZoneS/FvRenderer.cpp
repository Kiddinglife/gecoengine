#include "FvRenderer.h"
#include <d3d9.h>
#include <FvVector4.h>

#ifndef FV_SHIPPING
#	define FV_DX_METHOD_1(device,method,param0) FV_ASSERT(SUCCEEDED(device->method((param0))))
#	define FV_DX_METHOD_2(device,method,param0,param1) FV_ASSERT(SUCCEEDED(device->method((param0),(param1))))
#	define FV_DX_METHOD_3(device,method,param0,param1,param2) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2))))
#	define FV_DX_METHOD_4(device,method,param0,param1,param2,param3) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2),(param3))))
#	define FV_DX_METHOD_5(device,method,param0,param1,param2,param3,param4) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2),(param3),(param4))))
#	define FV_DX_METHOD_6(device,method,param0,param1,param2,param3,param4,param5) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2),(param3),(param4),(param5))))
#else
#	define FV_DX_METHOD_1(device,method,param0) device->method((param0))
#	define FV_DX_METHOD_2(device,method,param0,param1) device->method((param0),(param1))
#	define FV_DX_METHOD_3(device,method,param0,param1,param2) device->method((param0),(param1),(param2))
#	define FV_DX_METHOD_4(device,method,param0,param1,param2,param3) device->method((param0),(param1),(param2),(param3))
#	define FV_DX_METHOD_5(device,method,param0,param1,param2,param3,param4) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2),(param3),(param4))))
#	define FV_DX_METHOD_6(device,method,param0,param1,param2,param3,param4,param5) FV_ASSERT(SUCCEEDED(device->method((param0),(param1),(param2),(param3),(param4),(param5))))
#endif

//----------------------------------------------------------------------------
FvRenderer::FvRenderer(IDirect3DDevice9* pkDevice) : m_pkDevice(pkDevice)
{
	m_kBlendStateStack.m_Content = FvRenderState::BLEND_STATE;
	m_kColorWriteStack.m_Content = FvRenderState::COLOR_WRITE_STATE;
	m_kDepthStencilStateStack.m_Content = FvRenderState::DEPTH_STENCIL_STATE;
	m_kAlphaTestStateStack.m_Content = FvRenderState::ALPHA_TEST_STATE;
	m_kRasterizerStateStack.m_Content = FvRenderState::RASTERIZER_STATE;
	for(FvUInt32 i(0); i < FV_MAX_RENDER_TARGET_NUMBER; ++i)
	{
		m_akRenderTargetStacks[i].m_Content = FvRenderState::RENDER_TARGET;
	}
	m_kDepthStencilStack.m_Content = FvRenderState::DEPTH_STENCIL_SURFACE;
	m_kViewportStack.m_Content = FvRenderState::VIEW_PORT;
	for(FvUInt32 i(0); i < FV_MAX_TEXTURE_NUMBER; ++i)
	{
		m_akTextureStacks[i].m_Content = FvRenderState::TEXTURE;
		m_akSamplerStacks[i].m_Content = FvRenderState::SAMPLER_STATE;
	}
	m_kVertexDeclarationStack.m_Content = FvRenderState::VERTEX_DECLARATION;
	for(FvUInt32 i(0); i < FV_MAX_STREAM_NUMBER; ++i)
	{
		m_akStreamStacks[i].m_Content = FvRenderState::STREAM_SOURCE;
	}
	m_kIndexBufferStack.m_Content = FvRenderState::INDEX_BUFFER;
	m_kVertexShaderStack.m_Content = FvRenderState::VERTEX_SHADER;
	m_kPixelShaderStack.m_Content = FvRenderState::PIXEL_SHADER;
	m_kVertexConstBufferStack.m_Content = FvRenderState::VERTEX_SHADER_CONSTF;
	m_kPixelConstBufferStack.m_Content = FvRenderState::PIXEL_SHADER_CONSTF;		
}
//----------------------------------------------------------------------------
FvRenderer::~FvRenderer()
{

}
//----------------------------------------------------------------------------
void FvRenderer::PushState(FvRenderState* pkState)
{
	switch(pkState->GetType())
	{
	case FvRenderState::BLEND_STATE:
		m_kBlendStateStack.m_kStack.push_back(((FvBlendSate*)pkState)->m_pkBlendSate);
		m_kStateNeedUpdate.PushBack(m_kBlendStateStack);
		break;
	case FvRenderState::COLOR_WRITE_STATE:
		m_kColorWriteStack.m_kStack.push_back(((FvColorWriteState*)pkState)->m_pkColorWriteState);
		m_kStateNeedUpdate.PushBack(m_kColorWriteStack);
		break;
	case FvRenderState::DEPTH_STENCIL_STATE:
		m_kDepthStencilStateStack.m_kStack.push_back(((FvDepthStencilState*)pkState)->m_pkDepthStencilState);
		m_kStateNeedUpdate.PushBack(m_kDepthStencilStateStack);
		break;
	case FvRenderState::ALPHA_TEST_STATE:
		m_kAlphaTestStateStack.m_kStack.push_back(((FvAlphaTestState*)pkState)->m_pkAlphaTestState);
		m_kStateNeedUpdate.PushBack(m_kAlphaTestStateStack);
		break;
	case FvRenderState::RASTERIZER_STATE:
		m_kRasterizerStateStack.m_kStack.push_back(((FvRasterizerState*)pkState)->m_pkRasterizerState);
		m_kStateNeedUpdate.PushBack(m_kRasterizerStateStack);
		break;
	case FvRenderState::RENDER_TARGET:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvRenderTargetState*)pkState)->m_u32Index < FV_MAX_RENDER_TARGET_NUMBER);
#			endif
			ObjectStack<IDirect3DSurface9*>& kRenderTarget = m_akRenderTargetStacks[((FvRenderTargetState*)pkState)->m_u32Index];
			kRenderTarget.m_kStack.push_back(((FvRenderTargetState*)pkState)->m_pkRenderTarget);
			m_kStateNeedUpdate.PushBack(kRenderTarget);
		}
		break;
	case FvRenderState::DEPTH_STENCIL_SURFACE:
		m_kDepthStencilStack.m_kStack.push_back(((FvDepthStencilSurfaceState*)pkState)->m_pkDepthStencil);
		m_kStateNeedUpdate.PushBack(m_kDepthStencilStack);
		break;
	case FvRenderState::VIEW_PORT:
		m_kViewportStack.m_kStack.push_back(((FvViewportState*)pkState)->m_pkViewport);
		m_kStateNeedUpdate.PushBack(m_kViewportStack);
		break;
	case FvRenderState::TEXTURE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvTextureState*)pkState)->m_u32Index < FV_MAX_TEXTURE_NUMBER);
#			endif
			ObjectStack<IDirect3DBaseTexture9*>& kTexture = m_akTextureStacks[((FvTextureState*)pkState)->m_u32Index];
			kTexture.m_kStack.push_back(((FvTextureState*)pkState)->m_pkTexture);
			m_kStateNeedUpdate.PushBack(kTexture);
		}
		break;
	case FvRenderState::SAMPLER_STATE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvSamplerState*)pkState)->m_u32Index < FV_MAX_TEXTURE_NUMBER);
#			endif
			ObjectStack<const FvSamplerState::Object*>& kSampler = m_akSamplerStacks[((FvSamplerState*)pkState)->m_u32Index];
			kSampler.m_kStack.push_back(((FvSamplerState*)pkState)->m_pkSamplerState);
			m_kStateNeedUpdate.PushBack(kSampler);
		}
		break;
	case FvRenderState::VERTEX_DECLARATION:
		m_kVertexDeclarationStack.m_kStack.push_back(((FvVertexDeclarationState*)pkState)->m_pkVertexDeclaration);
		m_kStateNeedUpdate.PushBack(m_kVertexDeclarationStack);
		break;
	case FvRenderState::STREAM_SOURCE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvStreamSourceState*)pkState)->m_u32Index < FV_MAX_STREAM_NUMBER);
#			endif
			ObjectStack<Stream>& kStream = m_akStreamStacks[((FvStreamSourceState*)pkState)->m_u32Index];
			Stream kStreamData(((FvStreamSourceState*)pkState)->m_pkStreamData);
			kStreamData.m_u32OffsetInBytes = ((FvStreamSourceState*)pkState)->m_u32OffsetInBytes;
			kStreamData.m_u32Stride = ((FvStreamSourceState*)pkState)->m_u32Stride;
			kStreamData.m_u32FrequencyParameter = ((FvStreamSourceState*)pkState)->m_u32FrequencyParameter;
			kStream.m_kStack.push_back(kStreamData);
			m_kStateNeedUpdate.PushBack(kStream);
		}
		break;
	case FvRenderState::INDEX_BUFFER:
		m_kIndexBufferStack.m_kStack.push_back(((FvIndexBufferState*)pkState)->m_pkIndexData);
		m_kStateNeedUpdate.PushBack(m_kIndexBufferStack);
		break;
	case FvRenderState::VERTEX_SHADER:
		m_kVertexShaderStack.m_kStack.push_back(((FvVertexShaderState*)pkState)->m_pkShader);
		m_kStateNeedUpdate.PushBack(m_kVertexShaderStack);
		break;
	case FvRenderState::PIXEL_SHADER:
		m_kPixelShaderStack.m_kStack.push_back(((FvPixelShaderState*)pkState)->m_pkShader);
		m_kStateNeedUpdate.PushBack(m_kPixelShaderStack);
		break;
	case FvRenderState::VERTEX_SHADER_CONSTF:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvShaderConstantState*)pkState)->m_pfFloatBuffer);
#			endif

			if(m_kVertexConstBufferStack.m_pkCurrent.m_pfBuffer)
			{
				m_kVertexConstBufferStack.m_kStack.push_back(m_kVertexConstBufferStack.m_pkCurrent);
			}
			
			m_kVertexConstBufferStack.m_pkCurrent.m_pfBuffer = ((FvShaderConstantState*)pkState)->m_pfFloatBuffer;
			m_kVertexConstBufferStack.m_pkCurrent.m_u32Start = ((FvShaderConstantState*)pkState)->m_u32StartRegister;
			m_kVertexConstBufferStack.m_pkCurrent.m_u32Count = ((FvShaderConstantState*)pkState)->m_u32Count;

			FvUInt32 u32Used = m_kVertexConstBufferStack.m_pkCurrent.m_u32Start + m_kVertexConstBufferStack.m_pkCurrent.m_u32Count;
			if(u32Used > m_kVertexConstBufferStack.m_pkCurrent.m_u32MaxBufferUsed)
			{
				m_kVertexConstBufferStack.m_pkCurrent.m_u32MaxBufferUsed = u32Used;
			}

			FV_DX_METHOD_3(m_pkDevice, SetVertexShaderConstantF, m_kVertexConstBufferStack.m_pkCurrent.m_u32Start,
				m_kVertexConstBufferStack.m_pkCurrent.m_pfBuffer, m_kVertexConstBufferStack.m_pkCurrent.m_u32Count);
		}
		break;
	case FvRenderState::PIXEL_SHADER_CONSTF:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvShaderConstantState*)pkState)->m_pfFloatBuffer);
#			endif

			if(m_kPixelConstBufferStack.m_pkCurrent.m_pfBuffer)
			{
				m_kPixelConstBufferStack.m_kStack.push_back(m_kPixelConstBufferStack.m_pkCurrent);
			}

			m_kPixelConstBufferStack.m_pkCurrent.m_pfBuffer = ((FvShaderConstantState*)pkState)->m_pfFloatBuffer;
			m_kPixelConstBufferStack.m_pkCurrent.m_u32Start = ((FvShaderConstantState*)pkState)->m_u32StartRegister;
			m_kPixelConstBufferStack.m_pkCurrent.m_u32Count = ((FvShaderConstantState*)pkState)->m_u32Count;

			FvUInt32 u32Used = m_kPixelConstBufferStack.m_pkCurrent.m_u32Start + m_kPixelConstBufferStack.m_pkCurrent.m_u32Count;
			if(u32Used > m_kPixelConstBufferStack.m_pkCurrent.m_u32MaxBufferUsed)
				m_kPixelConstBufferStack.m_pkCurrent.m_u32MaxBufferUsed = u32Used;

			FV_DX_METHOD_3(m_pkDevice,SetPixelShaderConstantF,m_kPixelConstBufferStack.m_pkCurrent.m_u32Start,
				m_kPixelConstBufferStack.m_pkCurrent.m_pfBuffer, m_kPixelConstBufferStack.m_pkCurrent.m_u32Count);
		}
		break;
	}
}
//----------------------------------------------------------------------------
static float s_afTempBuffer[256 * 4];
//----------------------------------------------------------------------------
void FvRenderer::DoPopShaderConstant(
	ConstantF& kCurrent, ConstantF& kLast, 
	std::vector<ConstantF> kStack,
	FvUInt32& u32OutStart, FvUInt32& u32OutCount)
{
	FvUInt32 u32Start = kCurrent.m_u32Start;
	FvUInt32 u32Count = kLast.m_u32MaxBufferUsed - kCurrent.m_u32Start;
	if(u32Count > kCurrent.m_u32Count) u32Count = kCurrent.m_u32Count;
	memset(s_afTempBuffer, 0xff, u32Count * sizeof(float) * 4);
	FvUInt32 u32NeedSet = u32Count;
	FvUInt32 u32End = u32Start + u32Count;

	u32OutStart = u32Start;
	u32OutCount = u32Count;

	for(FvUInt32 i(kStack.size()-1); i >=0; --i)
	{
		ConstantF& kIter = kStack[i];
		if(kIter.m_u32MaxBufferUsed <= u32Start) break;
		FvUInt32 u32IterEnd = kIter.m_u32Start + kIter.m_u32Count;
		if(u32Start >= u32IterEnd) continue;

		for(FvUInt32 j(kIter.m_u32Start); j < u32IterEnd && j < u32End; ++j)
		{
			FvInt32 i32Now = j - u32Start;
			if(i32Now >= 0)
			{
				float* pfCurrent = &s_afTempBuffer[i32Now * 4];

				if((*(FvUInt32*)pfCurrent) == 0xffffffff)
				{
					*((FvVector4*)pfCurrent) = *(FvVector4*)(kIter.m_pfBuffer + (j - kIter.m_u32Start) * 4);
					if((--u32NeedSet) == 0) return;
				}
			}
		}
	}

	if(u32NeedSet == u32Count) u32OutCount = 0;
}
//----------------------------------------------------------------------------
void FvRenderer::PopState(FvRenderState* pkState)
{
	switch(pkState->GetType())
	{
	case FvRenderState::BLEND_STATE:
		m_kBlendStateStack.m_kStack.pop_back();
		if(m_kBlendStateStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kBlendStateStack);
		}
		else
		{
			m_kBlendStateStack.Detach();
		}
		break;
	case FvRenderState::COLOR_WRITE_STATE:
		m_kColorWriteStack.m_kStack.pop_back();
		if(m_kColorWriteStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kColorWriteStack);
		}
		else
		{
			m_kColorWriteStack.Detach();
		}
		break;
	case FvRenderState::DEPTH_STENCIL_STATE:
		m_kDepthStencilStateStack.m_kStack.pop_back();
		if(m_kDepthStencilStateStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kDepthStencilStateStack);
		}
		else
		{
			m_kDepthStencilStateStack.Detach();
		}
		break;
	case FvRenderState::ALPHA_TEST_STATE:
		m_kAlphaTestStateStack.m_kStack.pop_back();
		if(m_kAlphaTestStateStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kAlphaTestStateStack);
		}
		else
		{
			m_kAlphaTestStateStack.Detach();
		}
		break;
	case FvRenderState::RASTERIZER_STATE:
		m_kRasterizerStateStack.m_kStack.pop_back();
		if(m_kRasterizerStateStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kRasterizerStateStack);
		}
		else
		{
			m_kRasterizerStateStack.Detach();
		}
		break;
	case FvRenderState::RENDER_TARGET:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvRenderTargetState*)pkState)->m_u32Index < FV_MAX_RENDER_TARGET_NUMBER);
#			endif
			ObjectStack<IDirect3DSurface9*>& kRenderTarget = m_akRenderTargetStacks[((FvRenderTargetState*)pkState)->m_u32Index];
			kRenderTarget.m_kStack.pop_back();
			if(kRenderTarget.m_kStack.size())
			{
				m_kStateNeedUpdate.PushBack(kRenderTarget);
			}
			else
			{
				kRenderTarget.Detach();
			}
		}
		break;
	case FvRenderState::DEPTH_STENCIL_SURFACE:
		m_kDepthStencilStack.m_kStack.pop_back();
		if(m_kDepthStencilStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kDepthStencilStack);
		}
		else
		{
			m_kDepthStencilStack.Detach();
		}
		break;
	case FvRenderState::VIEW_PORT:
		m_kViewportStack.m_kStack.pop_back();
		if(m_kViewportStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kViewportStack);
		}
		else
		{
			m_kViewportStack.Detach();
		}
		break;
	case FvRenderState::TEXTURE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvTextureState*)pkState)->m_u32Index < FV_MAX_TEXTURE_NUMBER);
#			endif
			ObjectStack<IDirect3DBaseTexture9*>& kTexture = m_akTextureStacks[((FvTextureState*)pkState)->m_u32Index];
			kTexture.m_kStack.pop_back();
			if(kTexture.m_kStack.size())
			{
				m_kStateNeedUpdate.PushBack(kTexture);
			}
			else
			{
				kTexture.Detach();
			}
		}
		break;
	case FvRenderState::SAMPLER_STATE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvSamplerState*)pkState)->m_u32Index < FV_MAX_TEXTURE_NUMBER);
#			endif
			ObjectStack<const FvSamplerState::Object*>& kSampler = m_akSamplerStacks[((FvSamplerState*)pkState)->m_u32Index];
			kSampler.m_kStack.pop_back();
			if(kSampler.m_kStack.size())
			{
				m_kStateNeedUpdate.PushBack(kSampler);
			}
			else
			{
				kSampler.Detach();
			}
		}
		break;
	case FvRenderState::VERTEX_DECLARATION:
		m_kVertexDeclarationStack.m_kStack.pop_back();
		if(m_kVertexDeclarationStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kVertexDeclarationStack);
		}
		else
		{
			m_kVertexDeclarationStack.Detach();
		}
		break;
	case FvRenderState::STREAM_SOURCE:
		{
#			ifndef FV_SHIPPING
			FV_ASSERT(((FvStreamSourceState*)pkState)->m_u32Index < FV_MAX_STREAM_NUMBER);
#			endif
			ObjectStack<Stream>& kStream = m_akStreamStacks[((FvStreamSourceState*)pkState)->m_u32Index];
			Stream kStreamData(((FvStreamSourceState*)pkState)->m_pkStreamData);
			kStreamData.m_u32OffsetInBytes = ((FvStreamSourceState*)pkState)->m_u32OffsetInBytes;
			kStreamData.m_u32Stride = ((FvStreamSourceState*)pkState)->m_u32Stride;
			kStreamData.m_u32FrequencyParameter = ((FvStreamSourceState*)pkState)->m_u32FrequencyParameter;
			kStream.m_kStack.pop_back();
			if(kStream.m_kStack.size())
			{
				m_kStateNeedUpdate.PushBack(kStream);
			}
			else
			{
				kStream.Detach();
			}
		}
		break;
	case FvRenderState::INDEX_BUFFER:
		m_kIndexBufferStack.m_kStack.pop_back();
		if(m_kIndexBufferStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kIndexBufferStack);
		}
		else
		{
			m_kIndexBufferStack.Detach();
		}
		break;
	case FvRenderState::VERTEX_SHADER:
		m_kVertexShaderStack.m_kStack.pop_back();
		if(m_kVertexShaderStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kVertexShaderStack);
		}
		else
		{
			m_kVertexShaderStack.Detach();
		}
		break;
	case FvRenderState::PIXEL_SHADER:
		m_kPixelShaderStack.m_kStack.pop_back();
		if(m_kPixelShaderStack.m_kStack.size())
		{
			m_kStateNeedUpdate.PushBack(m_kPixelShaderStack);
		}
		else
		{
			m_kPixelShaderStack.Detach();
		}
		break;
	case FvRenderState::VERTEX_SHADER_CONSTF:
		{
			if(m_kVertexConstBufferStack.m_pkCurrent.m_pfBuffer)
			{
				FvUInt32 u32StackSize = m_kVertexConstBufferStack.m_kStack.size();
				if(u32StackSize)
				{
					ConstantF& kLast = m_kVertexConstBufferStack.m_kStack[u32StackSize-1];
					if(m_kVertexConstBufferStack.m_pkCurrent.m_u32Start < kLast.m_u32MaxBufferUsed)
					{
						FvUInt32 u32Start, u32Count;
						DoPopShaderConstant(m_kVertexConstBufferStack.m_pkCurrent,
							kLast, m_kVertexConstBufferStack.m_kStack, u32Start, u32Count);

						if(u32Count)
						{
							FV_DX_METHOD_3(m_pkDevice,SetVertexShaderConstantF,u32Start, s_afTempBuffer, u32Count);
						}
					}

					m_kVertexConstBufferStack.m_pkCurrent = kLast;
					m_kVertexConstBufferStack.m_kStack.pop_back();
				}
				else
				{
					m_kVertexConstBufferStack.m_pkCurrent.m_pfBuffer = NULL;
				}
			}
		}
		break;
	case FvRenderState::PIXEL_SHADER_CONSTF:
		{
			if(m_kPixelConstBufferStack.m_pkCurrent.m_pfBuffer)
			{
				FvUInt32 u32StackSize = m_kPixelConstBufferStack.m_kStack.size();
				if(u32StackSize)
				{
					ConstantF& kLast = m_kPixelConstBufferStack.m_kStack[u32StackSize-1];
					if(m_kPixelConstBufferStack.m_pkCurrent.m_u32Start < kLast.m_u32MaxBufferUsed)
					{
						FvUInt32 u32Start, u32Count;
						DoPopShaderConstant(m_kPixelConstBufferStack.m_pkCurrent,
							kLast, m_kPixelConstBufferStack.m_kStack, u32Start, u32Count);

						if(u32Count)
						{
							FV_DX_METHOD_3(m_pkDevice, SetPixelShaderConstantF, u32Start, s_afTempBuffer, u32Count);
						}
					}

					m_kPixelConstBufferStack.m_pkCurrent = kLast;
					m_kPixelConstBufferStack.m_kStack.pop_back();
				}
				else
				{
					m_kPixelConstBufferStack.m_pkCurrent.m_pfBuffer = NULL;
				}
			}
		}
		break;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::Draw(FvRenderDrawNode::Data* pkData)
{
#	ifndef FV_SHIPPING
	FV_ASSERT(pkData);
#	endif

	UpdateStates();

	if(pkData->m_eType == FvRenderDrawNode::INDEXED_DRAW)
	{
		FV_DX_METHOD_6(m_pkDevice, DrawIndexedPrimitive
			,pkData->m_kIndexed.m_eType
			,pkData->m_kIndexed.m_i32BaseVertexIndex
			,pkData->m_kIndexed.m_u32MinIndex
			,pkData->m_kIndexed.m_u32NumVertices
			,pkData->m_kIndexed.m_u32StartIndex
			,pkData->m_kIndexed.m_u32PrimitiveCount);
	}
	else
	{
		FV_DX_METHOD_3(m_pkDevice, DrawPrimitive
			,pkData->m_kUnindexed.m_eType
			,pkData->m_kUnindexed.m_i32StartVertex
			,pkData->m_kUnindexed.m_u32PrimitiveCount);
	}
}
//----------------------------------------------------------------------------
void FvRenderer::UpdateStates()
{
	FvDoubleLink<FvRenderStateType>::Iterator iter = m_kStateNeedUpdate.GetHead();
	while(!m_kStateNeedUpdate.IsEnd(iter))
	{
		FvDoubleLinkNode2<FvRenderStateType>* pkNode = iter;
		FvDoubleLink<FvRenderStateType>::Next(iter);
		UpdateState(pkNode);
		pkNode->Detach();
	}
}
//----------------------------------------------------------------------------
void FvRenderer::UpdateState(FvDoubleLinkNode2<FvRenderStateType>* pkState)
{
	switch(pkState->m_Content)
	{
	case FvRenderState::BLEND_STATE:
		ChangeBlendState(((ObjectStack<const FvBlendSate::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvBlendSate::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::COLOR_WRITE_STATE:
		ChangeColorWriteState(((ObjectStack<const FvColorWriteState::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvColorWriteState::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::DEPTH_STENCIL_STATE:
		ChangeDepthStencilState(((ObjectStack<const FvDepthStencilState::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvDepthStencilState::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::ALPHA_TEST_STATE:
		ChangeAlphaTestState(((ObjectStack<const FvAlphaTestState::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvAlphaTestState::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::RASTERIZER_STATE:
		ChangeRasterizerState(((ObjectStack<const FvRasterizerState::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvRasterizerState::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::RENDER_TARGET:
		ChangeRenderTarget(((ObjectStack<IDirect3DSurface9*>*)pkState) - m_akRenderTargetStacks,
			((ObjectStack<IDirect3DSurface9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DSurface9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::DEPTH_STENCIL_SURFACE:
		ChangeDepthStencilSurface(((ObjectStack<IDirect3DSurface9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DSurface9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::VIEW_PORT:
		ChangeViewport(((ObjectStack<_D3DVIEWPORT9*>*)pkState)->m_pkCurrent,
			((ObjectStack<_D3DVIEWPORT9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::TEXTURE:
		ChangeTexture(((ObjectStack<IDirect3DBaseTexture9*>*)pkState) - m_akTextureStacks,
			((ObjectStack<IDirect3DBaseTexture9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DBaseTexture9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::SAMPLER_STATE:
		ChangeSamplerState(((ObjectStack<const FvSamplerState::Object*>*)pkState) - m_akSamplerStacks,
			((ObjectStack<const FvSamplerState::Object*>*)pkState)->m_pkCurrent,
			((ObjectStack<const FvSamplerState::Object*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::VERTEX_DECLARATION:
		ChangeVertexDeclaration(((ObjectStack<IDirect3DVertexDeclaration9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DVertexDeclaration9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::STREAM_SOURCE:
		ChangeStream(((ObjectStack<Stream>*)pkState) - m_akStreamStacks,
			((ObjectStack<Stream>*)pkState)->m_pkCurrent,
			((ObjectStack<Stream>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::INDEX_BUFFER:
		ChangeIndexBuffer(((ObjectStack<IDirect3DIndexBuffer9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DIndexBuffer9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::VERTEX_SHADER:
		ChangeVertexShader(((ObjectStack<IDirect3DVertexShader9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DVertexShader9*>*)pkState)->m_kStack.back());
		break;
	case FvRenderState::PIXEL_SHADER:
		ChangePixelShader(((ObjectStack<IDirect3DPixelShader9*>*)pkState)->m_pkCurrent,
			((ObjectStack<IDirect3DPixelShader9*>*)pkState)->m_kStack.back());
		break;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeBlendState(const FvBlendSate::Object*& pkCurrent,
	const FvBlendSate::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(pkLast);
#		endif

		if((!pkCurrent) || (pkCurrent->m_u32Enable != pkLast->m_u32Enable))
		{
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHABLENDENABLE, pkLast->m_u32Enable);
			if(pkLast->m_u32Enable)
			{
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SRCBLEND, pkLast->m_u32Src);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_DESTBLEND, pkLast->m_u32Dst);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_BLENDOP, pkLast->m_u32BlendOp);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SEPARATEALPHABLENDENABLE, pkLast->m_u32SeparateEnable);
				if(pkLast->m_u32SeparateEnable)
				{
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SRCBLENDALPHA, pkLast->m_u32SrcAlpha);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_DESTBLENDALPHA, pkLast->m_u32DstAlpha);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_BLENDOPALPHA, pkLast->m_u32BlendOpAlpha);
				}
			}
		}
		else if(pkLast->m_u32Enable)
		{
			if(pkCurrent->m_u32Src != pkLast->m_u32Src)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SRCBLEND, pkLast->m_u32Src);
			if(pkCurrent->m_u32Dst != pkLast->m_u32Dst)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_DESTBLEND, pkLast->m_u32Dst);
			if(pkCurrent->m_u32BlendOp != pkLast->m_u32BlendOp)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_BLENDOP, pkLast->m_u32BlendOp);
			if(pkCurrent->m_u32SeparateEnable != pkLast->m_u32SeparateEnable)
			{
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SEPARATEALPHABLENDENABLE, pkLast->m_u32SeparateEnable);
				if(pkLast->m_u32SeparateEnable)
				{
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SRCBLENDALPHA, pkLast->m_u32SrcAlpha);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_DESTBLENDALPHA, pkLast->m_u32DstAlpha);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_BLENDOPALPHA, pkLast->m_u32BlendOpAlpha);
				}
			}
			else if(pkLast->m_u32SeparateEnable)
			{
				if(pkCurrent->m_u32SrcAlpha != pkLast->m_u32SrcAlpha)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SRCBLENDALPHA, pkLast->m_u32SrcAlpha);
				if(pkCurrent->m_u32DstAlpha != pkLast->m_u32DstAlpha)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_DESTBLENDALPHA, pkLast->m_u32DstAlpha);
				if(pkCurrent->m_u32BlendOpAlpha != pkLast->m_u32BlendOpAlpha)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_BLENDOPALPHA, pkLast->m_u32BlendOpAlpha);
			}
		}

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeColorWriteState(const FvColorWriteState::Object*& pkCurrent,
	const FvColorWriteState::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(pkLast);
#		endif

		if(pkCurrent)
		{
			if(pkCurrent->m_u32ColorWrite0 != pkLast->m_u32ColorWrite0)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE, pkLast->m_u32ColorWrite0);
			if(pkCurrent->m_u32ColorWrite0 != pkLast->m_u32ColorWrite0)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE1, pkLast->m_u32ColorWrite1);
			if(pkCurrent->m_u32ColorWrite0 != pkLast->m_u32ColorWrite0)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE2, pkLast->m_u32ColorWrite2);
			if(pkCurrent->m_u32ColorWrite0 != pkLast->m_u32ColorWrite0)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE3, pkLast->m_u32ColorWrite3);
		}
		else
		{
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE, pkLast->m_u32ColorWrite0);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE1, pkLast->m_u32ColorWrite1);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE2, pkLast->m_u32ColorWrite2);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_COLORWRITEENABLE3, pkLast->m_u32ColorWrite3);
		}

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeDepthStencilState(const FvDepthStencilState::Object*& pkCurrent,
	const FvDepthStencilState::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(pkLast);
#		endif

		if(pkCurrent)
		{
			if(pkCurrent->m_u32ZEnable != pkLast->m_u32ZEnable)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZENABLE, pkLast->m_u32ZEnable);
			if(pkCurrent->m_u32ZWriteEnable != pkLast->m_u32ZWriteEnable)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZWRITEENABLE, pkLast->m_u32ZWriteEnable);
			if(pkCurrent->m_u32ZFunc != pkLast->m_u32ZFunc)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZFUNC, pkLast->m_u32ZFunc);

			if(pkCurrent->m_u32StencilEnable != pkLast->m_u32StencilEnable)
			{
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILENABLE, pkLast->m_u32StencilEnable);
				if(pkLast->m_u32StencilEnable)
				{
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFAIL, pkLast->m_u32StencilFail);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILZFAIL, pkLast->m_u32StencilZFail);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILPASS, pkLast->m_u32StencilPass);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFUNC, pkLast->m_u32StencilFunc);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILREF, pkLast->m_u32StencilRef);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILMASK, pkLast->m_u32StencilMask);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILWRITEMASK, pkLast->m_u32StencilWriteMask);

					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_TWOSIDEDSTENCILMODE, pkLast->m_u32TwoSideStencilMode);
					if(pkLast->m_u32TwoSideStencilMode)
					{
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFAIL, pkLast->m_u32StencilFailCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILZFAIL, pkLast->m_u32StencilZFailCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILPASS, pkLast->m_u32StencilPassCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFUNC, pkLast->m_u32StencilFuncCCW);
					}
				}
			}
			else if(pkLast->m_u32StencilEnable)
			{
				if(pkCurrent->m_u32StencilFail != pkLast->m_u32StencilFail)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFAIL, pkLast->m_u32StencilFail);
				if(pkCurrent->m_u32StencilZFail != pkLast->m_u32StencilZFail)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILZFAIL, pkLast->m_u32StencilZFail);
				if(pkCurrent->m_u32StencilPass != pkLast->m_u32StencilPass)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILPASS, pkLast->m_u32StencilPass);
				if(pkCurrent->m_u32StencilFunc != pkLast->m_u32StencilFunc)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFUNC, pkLast->m_u32StencilFunc);
				if(pkCurrent->m_u32StencilRef != pkLast->m_u32StencilRef)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILREF, pkLast->m_u32StencilRef);
				if(pkCurrent->m_u32StencilMask != pkLast->m_u32StencilMask)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILMASK, pkLast->m_u32StencilMask);
				if(pkCurrent->m_u32StencilWriteMask != pkLast->m_u32StencilWriteMask)
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILWRITEMASK, pkLast->m_u32StencilWriteMask);

				if(pkCurrent->m_u32TwoSideStencilMode != pkLast->m_u32TwoSideStencilMode)
				{
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_TWOSIDEDSTENCILMODE, pkLast->m_u32TwoSideStencilMode);
					if(pkLast->m_u32TwoSideStencilMode)
					{
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFAIL, pkLast->m_u32StencilFailCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILZFAIL, pkLast->m_u32StencilZFailCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILPASS, pkLast->m_u32StencilPassCCW);
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFUNC, pkLast->m_u32StencilFuncCCW);
					}
				}
				else if(pkLast->m_u32TwoSideStencilMode)
				{
					if(pkCurrent->m_u32StencilFailCCW != pkLast->m_u32StencilFailCCW)
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFAIL, pkLast->m_u32StencilFailCCW);
					if(pkCurrent->m_u32StencilZFailCCW != pkLast->m_u32StencilZFailCCW)
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILZFAIL, pkLast->m_u32StencilZFailCCW);
					if(pkCurrent->m_u32StencilPassCCW != pkLast->m_u32StencilPassCCW)
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILPASS, pkLast->m_u32StencilPassCCW);
					if(pkCurrent->m_u32StencilFuncCCW != pkLast->m_u32StencilFuncCCW)
						FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFUNC, pkLast->m_u32StencilFuncCCW);
				}
			}
		}
		else
		{
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZENABLE, pkLast->m_u32ZEnable);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZWRITEENABLE, pkLast->m_u32ZWriteEnable);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ZFUNC, pkLast->m_u32ZFunc);

			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILENABLE, pkLast->m_u32StencilEnable);
			if(pkLast->m_u32StencilEnable)
			{
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFAIL, pkLast->m_u32StencilFail);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILZFAIL, pkLast->m_u32StencilZFail);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILPASS, pkLast->m_u32StencilPass);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILFUNC, pkLast->m_u32StencilFunc);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILREF, pkLast->m_u32StencilRef);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILMASK, pkLast->m_u32StencilMask);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_STENCILWRITEMASK, pkLast->m_u32StencilWriteMask);

				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_TWOSIDEDSTENCILMODE, pkLast->m_u32TwoSideStencilMode);
				if(pkLast->m_u32TwoSideStencilMode)
				{
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFAIL, pkLast->m_u32StencilFailCCW);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILZFAIL, pkLast->m_u32StencilZFailCCW);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILPASS, pkLast->m_u32StencilPassCCW);
					FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CCW_STENCILFUNC, pkLast->m_u32StencilFuncCCW);
				}
			}
		}

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeAlphaTestState(const FvAlphaTestState::Object*& pkCurrent,
	const FvAlphaTestState::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(pkLast);
#		endif

		if((!pkCurrent) || (pkCurrent->m_u32AlphaTestEnable != pkLast->m_u32AlphaTestEnable))
		{
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHATESTENABLE, pkLast->m_u32AlphaTestEnable);
			if(pkLast->m_u32AlphaTestEnable)
			{
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHAREF, pkLast->m_u32AlphaTestRef);
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHAFUNC, pkLast->m_u32AlphaTestFunc);
			}
		}
		else if(pkLast->m_u32AlphaTestEnable)
		{
			if(pkCurrent->m_u32AlphaTestRef != pkLast->m_u32AlphaTestRef)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHAREF, pkLast->m_u32AlphaTestRef);
			if(pkCurrent->m_u32AlphaTestFunc != pkLast->m_u32AlphaTestFunc)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_ALPHAFUNC, pkLast->m_u32AlphaTestFunc);
		}

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeRasterizerState(const FvRasterizerState::Object*& pkCurrent,
	const FvRasterizerState::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(pkLast);
#		endif

		if(pkCurrent)
		{
			if(pkCurrent->m_u32FillMode != pkLast->m_u32FillMode)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_FILLMODE, pkLast->m_u32FillMode);
			if(pkCurrent->m_u32ShadeMode != pkLast->m_u32ShadeMode)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SHADEMODE, pkLast->m_u32ShadeMode);
			if(pkCurrent->m_u32CullMode != pkLast->m_u32CullMode)
				FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CULLMODE, pkLast->m_u32CullMode);
		}
		else
		{
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_FILLMODE, pkLast->m_u32FillMode);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_SHADEMODE, pkLast->m_u32ShadeMode);
			FV_DX_METHOD_2(m_pkDevice, SetRenderState, D3DRS_CULLMODE, pkLast->m_u32CullMode);
		}

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeRenderTarget(FvUInt32 u32Index,
	IDirect3DSurface9*& pkCurrent, IDirect3DSurface9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(u32Index < FV_MAX_RENDER_TARGET_NUMBER);
#		endif


		FV_DX_METHOD_2(m_pkDevice, SetRenderTarget, u32Index, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeDepthStencilSurface(
	IDirect3DSurface9*& pkCurrent, IDirect3DSurface9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetDepthStencilSurface, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeViewport(_D3DVIEWPORT9*& pkCurrent, _D3DVIEWPORT9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetViewport, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeTexture(FvUInt32 u32Index,
	IDirect3DBaseTexture9*& pkCurrent, IDirect3DBaseTexture9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(u32Index < FV_MAX_RENDER_TARGET_NUMBER);
#		endif

		if(u32Index >= 8)
		{
			u32Index = D3DVERTEXTEXTURESAMPLER0 + u32Index - 8;
		}


		FV_DX_METHOD_2(m_pkDevice, SetTexture, u32Index, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeSamplerState(FvUInt32 u32Index,
	const FvSamplerState::Object*& pkCurrent,
	const FvSamplerState::Object*& pkLast)
{
	if(pkCurrent != pkLast)
	{
#		ifndef FV_SHIPPING
		FV_ASSERT(u32Index < FV_MAX_RENDER_TARGET_NUMBER);
		FV_ASSERT(pkLast);
#		endif

		if(u32Index >= 8)
		{
			u32Index = D3DVERTEXTEXTURESAMPLER0 + u32Index - 8;
		}

		if(pkCurrent->m_u32AddressU != pkLast->m_u32AddressU)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_ADDRESSU, pkLast->m_u32AddressU);
		if(pkCurrent->m_u32AddressV != pkLast->m_u32AddressV)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_ADDRESSV, pkLast->m_u32AddressV);
		if(pkCurrent->m_u32AddressW != pkLast->m_u32AddressW)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_ADDRESSW, pkLast->m_u32AddressW);
		if(pkCurrent->m_u32BorderColor != pkLast->m_u32BorderColor)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_BORDERCOLOR, pkLast->m_u32BorderColor);
		if(pkCurrent->m_u32MagFilter != pkLast->m_u32MagFilter)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MAGFILTER, pkLast->m_u32MagFilter);
		if(pkCurrent->m_u32MinFilter != pkLast->m_u32MinFilter)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MINFILTER, pkLast->m_u32MinFilter);
		if(pkCurrent->m_u32MipFilter != pkLast->m_u32MipFilter)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MIPFILTER, pkLast->m_u32MipFilter);
		if(pkCurrent->m_u32MipMapLodBias != pkLast->m_u32MipMapLodBias)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MIPMAPLODBIAS, pkLast->m_u32MipMapLodBias);
		if(pkCurrent->m_u32MaxMipLevel != pkLast->m_u32MaxMipLevel)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MAXMIPLEVEL, pkLast->m_u32MaxMipLevel);
		if(pkCurrent->m_u32MaxAnisotropy != pkLast->m_u32MaxAnisotropy)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_MAXANISOTROPY, pkLast->m_u32MaxAnisotropy);
		if(pkCurrent->m_u32SRGB != pkLast->m_u32SRGB)
			FV_DX_METHOD_3(m_pkDevice, SetSamplerState, u32Index, D3DSAMP_SRGBTEXTURE, pkLast->m_u32SRGB);

		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeVertexDeclaration(IDirect3DVertexDeclaration9*& pkCurrent,
	IDirect3DVertexDeclaration9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetVertexDeclaration, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeStream(FvUInt32 u32Index, Stream& pkCurrent, Stream& pkLast)
{
#	ifndef FV_SHIPPING
	FV_ASSERT(u32Index < FV_MAX_STREAM_NUMBER);
#	endif

	if((pkCurrent.m_pkStreamData != pkLast.m_pkStreamData)
		|| (pkCurrent.m_u32OffsetInBytes != pkLast.m_u32OffsetInBytes)
		|| (pkCurrent.m_u32Stride != pkLast.m_u32Stride))
	{
		FV_DX_METHOD_4(m_pkDevice, SetStreamSource, u32Index,
			pkLast.m_pkStreamData, pkLast.m_u32OffsetInBytes, pkLast.m_u32Stride);

		pkCurrent.m_pkStreamData = pkLast.m_pkStreamData;
		pkCurrent.m_u32OffsetInBytes = pkLast.m_u32OffsetInBytes;
		pkCurrent.m_u32Stride = pkLast.m_u32Stride;
	}

	if(pkCurrent.m_u32FrequencyParameter != pkLast.m_u32FrequencyParameter)
	{
		FV_DX_METHOD_2(m_pkDevice, SetStreamSourceFreq, u32Index,
			pkLast.m_u32FrequencyParameter);

		pkCurrent.m_u32FrequencyParameter = pkLast.m_u32FrequencyParameter;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeIndexBuffer(IDirect3DIndexBuffer9*& pkCurrent,
	IDirect3DIndexBuffer9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetIndices, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangeVertexShader(IDirect3DVertexShader9*& pkCurrent,
	IDirect3DVertexShader9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetVertexShader, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
void FvRenderer::ChangePixelShader(IDirect3DPixelShader9*& pkCurrent,
	IDirect3DPixelShader9*& pkLast)
{
	if(pkCurrent != pkLast)
	{
		FV_DX_METHOD_1(m_pkDevice, SetPixelShader, pkLast);
		pkCurrent = pkLast;
	}
}
//----------------------------------------------------------------------------
