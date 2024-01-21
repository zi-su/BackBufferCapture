#include "BackBufferCaptureSubsystem.h"

#include "CommonRenderResources.h"
#include "RenderGraphEvent.h"
#include "RenderGraphUtils.h"
#include "ScreenRendering.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/SceneViewport.h"

void UBackBufferCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GEngine->GetPostRenderDelegateEx().AddUObject(this, &UBackBufferCaptureSubsystem::Render);
}

void UBackBufferCaptureSubsystem::Render(FRDGBuilder& GraphBuilder)
{
	if(!IsValid(RenderTextureTarget))
	{
		return;
	}
	
	AddPass(GraphBuilder, RDG_EVENT_NAME("UBackBufferCaptureSubsystem::Render"), [this](FRHICommandListImmediate& RHICmdList)
	{
		IRendererModule* RendererModule = &FModuleManager::GetModuleChecked<IRendererModule>(TEXT("Renderer"));
		
			const FIntPoint TargetSize(RenderTextureTarget->SizeX, RenderTextureTarget->SizeY);
			FRHITexture* DestRenderTarget = RenderTextureTarget->GetRenderTargetResource()->GetTextureRHI();
		
			FRHIRenderPassInfo RPInfo(DestRenderTarget, ERenderTargetActions::Load_Store);
				RHICmdList.BeginRenderPass(RPInfo, TEXT("FrameCaptureResolveRenderTarget"));
				{
					RHICmdList.SetViewport(0, 0, 0.0f, TargetSize.X, TargetSize.Y, 1.0f);

					FGraphicsPipelineStateInitializer GraphicsPSOInit;
					RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
					GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
					GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
					GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

					const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;

					FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(FeatureLevel);
					TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
					TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

					GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
					GraphicsPSOInit.BoundShaderState.VertexShaderRHI = static_cast<FRHIVertexShader*>(VertexShader.GetRHIShaderBase(SF_Vertex));
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = static_cast<FRHIPixelShader*>(PixelShader.GetRHIShaderBase(SF_Pixel));
					GraphicsPSOInit.PrimitiveType = PT_TriangleList;

					SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

	#if 0
					const bool bIsSourceBackBufferSameAsWindowSize = SceneTexture->GetSizeX() == WindowSize.X && SceneTexture->GetSizeY() == WindowSize.Y;
					const bool bIsSourceBackBufferSameAsTargetSize = TargetSize.X == SceneTexture->GetSizeX() && TargetSize.Y == SceneTexture->GetSizeY();

					if (bIsSourceBackBufferSameAsWindowSize || bIsSourceBackBufferSameAsTargetSize)
					{
						PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Point>::GetRHI(), SceneTexture);
					}
					else
					{
						PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Bilinear>::GetRHI(), SceneTexture);
					}
	#else
					// Widnow Size取ってないのでBilinear前提で
					FRHITexture* SceneTexture = GEngine->GameViewport->GetGameViewport()->GetRenderTargetTexture();
					FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
					PixelShader->SetParameters(BatchedParameters, TStaticSamplerState<SF_Bilinear>::GetRHI(), SceneTexture);
					RHICmdList.SetBatchedShaderParameters(RHICmdList.GetBoundPixelShader(), BatchedParameters);
	#endif
					FVector2D ViewRectSize;
					GEngine->GameViewport->GetViewportSize(ViewRectSize);
					RendererModule->DrawRectangle(
						RHICmdList,
						0, 0,										// Dest X, Y
						TargetSize.X, TargetSize.Y,					// Dest W, H
						0, 0,				// Source X, Y
						ViewRectSize.X, ViewRectSize.Y,				// Source W, H
						TargetSize,									// Dest buffer size
						FIntPoint(SceneTexture->GetSizeX(), SceneTexture->GetSizeY()),						// Source texture size
						VertexShader,
						EDRF_Default);
				}
				RHICmdList.EndRenderPass();
	});
}

void UBackBufferCaptureSubsystem::SetRenderTarget(UTextureRenderTarget2D* InRenderTarget)
{
	RenderTextureTarget = InRenderTarget;
}
