// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelStream/Effects/Level1_LevelTransitionEffect.h"
#include "Level1_TEffectFade.generated.h"

UENUM(BlueprintType)
enum class EFadeDirection : uint8
{
	FadeOut UMETA(DisplayName = "Fade Out (to color)"),
	FadeIn  UMETA(DisplayName = "Fade In (from color)")
};

UCLASS(meta = (DisplayName = "Fade"))
class SKYFIREUPRISINGL1_API ULevel1_TEffectFade : public ULevel1_LevelTransitionEffect
{
	GENERATED_BODY()
public:
    ULevel1_TEffectFade();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
    EFadeDirection Direction = EFadeDirection::FadeOut;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
    float Duration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
    FLinearColor Color = FLinearColor::Black;

    /** If true, hold the fade after finishing (only meaningful for FadeOut). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
    bool bHoldWhenFinished = true;

    virtual void Begin_Implementation(UWorld* World) override;
    virtual void Tick_Implementation(float DeltaTime) override;

private:
    TWeakObjectPtr<class APlayerCameraManager> CachedCameraManager;
};
