#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BackBufferCaptureSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BACKBUFFERCAPTURE_API UBackBufferCaptureSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void Render(FRDGBuilder& GraphBuilder);
	UFUNCTION(BlueprintCallable)
	void SetRenderTarget(UTextureRenderTarget2D* InRenderTarget);
private:
	UPROPERTY()
	UTextureRenderTarget2D* RenderTextureTarget;
};
