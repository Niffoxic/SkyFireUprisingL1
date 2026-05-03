// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "LevelStream/Level1_TransitionTypes.h"
#include "Level1_DataAssetTransitionConfig.generated.h"

class ULevel1_LevelTransitionEffect;

UCLASS(BlueprintType)
class SKYFIREUPRISINGL1_API ULevel1_DataAssetTransitionConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
    FName SourceSublevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
    FName TargetSublevel;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Effects")
    TArray<TObjectPtr<ULevel1_LevelTransitionEffect>> EffectsBeforeSwap;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Effects")
    TArray<TObjectPtr<ULevel1_LevelTransitionEffect>> EffectsAfterSwap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swap")
    ETransitionSwapTrigger SwapTrigger = ETransitionSwapTrigger::AfterAllEffects;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swap",
        meta = (EditCondition = "SwapTrigger == ETransitionSwapTrigger::OnEffectIndex"))
    int32 SwapAtEffectIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swap",
        meta = (EditCondition = "SwapTrigger == ETransitionSwapTrigger::OnEvent"))
    FGameplayTag SwapEventTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swap",
        meta = (EditCondition = "SwapTrigger == ETransitionSwapTrigger::AfterDelay"))
    float SwapAfterSeconds = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
    bool bRepositionPlayer = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player",
        meta = (EditCondition = "bRepositionPlayer"))
    FTransform PlayerSpawnInTarget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player",
        meta = (EditCondition = "bRepositionPlayer"))
    bool bPreserveVehicleVelocity = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Events")
    FGameplayTag StartEventTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Events")
    FGameplayTag CompleteEventTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Meta", meta = (MultiLine = true))
    FString DesignerNote;
};
