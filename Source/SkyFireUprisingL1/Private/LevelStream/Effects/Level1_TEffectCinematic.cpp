#include "LevelStream/Effects/Level1_TEffectCinematic.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"

ULevel1_TEffectCinematic::ULevel1_TEffectCinematic()
{
    DebugLabel = TEXT("Cinematic");
}

void ULevel1_TEffectCinematic::Begin_Implementation(UWorld* World)
{
    Super::Begin_Implementation(World);

    if (!World)
    {
        bFinished = true;
        return;
    }
    CachedWorld = World;

    ULevelSequence* LoadedSequence = Sequence.LoadSynchronous();
    if (!LoadedSequence)
    {
        UE_LOG(LogTemp, Warning, TEXT("[TEffectCinematic] No Sequence assigned - finishing immediately."));
        bFinished = true;
        return;
    }

    ALevelSequenceActor* OutActor = nullptr;
    ActivePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        World, LoadedSequence, PlaybackSettings, OutActor);
    ActiveActor = OutActor;

    if (!ActivePlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("[TEffectCinematic] Failed to create LevelSequencePlayer."));
        bFinished = true;
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
    {
        if (bHidePlayerHUD && PC->GetHUD())
        {
            PC->GetHUD()->bShowHUD = false;
        }
        if (bDisablePlayerInput && PC->GetPawn())
        {
            PC->GetPawn()->DisableInput(PC);
        }
    }

    ActivePlayer->OnFinished.AddDynamic(this, &ULevel1_TEffectCinematic::OnSequenceFinished);

    ActivePlayer->Play();
    UE_LOG(LogTemp, Log, TEXT("[TEffectCinematic] Playing sequence: %s"), *LoadedSequence->GetName());
}

void ULevel1_TEffectCinematic::Tick_Implementation(float DeltaTime)
{
    Super::Tick_Implementation(DeltaTime);

    if (MaxDuration > 0.f && ElapsedTime >= MaxDuration && !bFinished)
    {
        UE_LOG(LogTemp, Warning, TEXT("[TEffectCinematic] MaxDuration reached — forcing finish."));
        if (ActivePlayer && !bSequenceEnded)
        {
            ActivePlayer->Stop();
        }
        bFinished = true;
    }

    if (bSequenceEnded)
    {
        bFinished = true;
    }
}

void ULevel1_TEffectCinematic::OnSequenceFinished()
{
    UE_LOG(LogTemp, Log, TEXT("[TEffectCinematic] Sequence finished."));
    bSequenceEnded = true;

    if (UWorld* World = CachedWorld.Get())
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        if (PC)
        {
            if (bHidePlayerHUD && PC->GetHUD())
            {
                PC->GetHUD()->bShowHUD = true;
            }
            if (bDisablePlayerInput && PC->GetPawn())
            {
                PC->GetPawn()->EnableInput(PC);
            }
        }
    }
}

void ULevel1_TEffectCinematic::Cancel_Implementation()
{
    if (ActivePlayer && !bSequenceEnded)
    {
        ActivePlayer->Stop();
    }
    Super::Cancel_Implementation();
}
