#include "LevelStream/Effects/Level1_TEffectFade.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ULevel1_TEffectFade::ULevel1_TEffectFade()
{
	DebugLabel = TEXT("Fade");
}

void ULevel1_TEffectFade::Begin_Implementation(UWorld* World)
{
    Super::Begin_Implementation(World);

    if (!World) { bFinished = true; return; }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC || !PC->PlayerCameraManager) { bFinished = true; return; }

    CachedCameraManager = PC->PlayerCameraManager;

    const float FromAlpha = (Direction == EFadeDirection::FadeOut) ? 0.f : 1.f;
    const float ToAlpha   = (Direction == EFadeDirection::FadeOut) ? 1.f : 0.f;

    CachedCameraManager->StartCameraFade(
        FromAlpha,
        ToAlpha,
        FMath::Max(Duration, 0.01f),
        Color,
        false,
        (Direction == EFadeDirection::FadeOut && bHoldWhenFinished)
    );
}

void ULevel1_TEffectFade::Tick_Implementation(float DeltaTime)
{
    Super::Tick_Implementation(DeltaTime);
    if (ElapsedTime >= Duration)
    {
        bFinished = true;
    }
}
