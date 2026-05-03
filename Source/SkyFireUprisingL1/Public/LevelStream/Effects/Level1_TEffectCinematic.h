#pragma once

#include "CoreMinimal.h"
#include "LevelStream/Effects/Level1_LevelTransitionEffect.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Level1_TEffectCinematic.generated.h"

class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;

UCLASS(meta = (DisplayName = "Cinematic"))
class SKYFIREUPRISINGL1_API ULevel1_TEffectCinematic : public ULevel1_LevelTransitionEffect
{
	GENERATED_BODY()
public:
    ULevel1_TEffectCinematic();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    TSoftObjectPtr<ULevelSequence> Sequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    FMovieSceneSequencePlaybackSettings PlaybackSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    float MaxDuration = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    bool bHidePlayerHUD = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic")
    bool bDisablePlayerInput = true;

    virtual void Begin_Implementation   (UWorld* World) override;
    virtual void Tick_Implementation    (float DeltaTime) override;
    virtual void Cancel_Implementation  () override;

private:
    UFUNCTION()
    void OnSequenceFinished();

    UPROPERTY()
    TObjectPtr<ULevelSequencePlayer> ActivePlayer;

    UPROPERTY()
    TObjectPtr<ALevelSequenceActor> ActiveActor;

    TWeakObjectPtr<UWorld> CachedWorld;
    bool bSequenceEnded = false;
};
