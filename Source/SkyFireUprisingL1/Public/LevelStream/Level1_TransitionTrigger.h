// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Level1_TransitionTrigger.generated.h"

class ULevel1_DataAssetTransitionConfig;

UCLASS()
class SKYFIREUPRISINGL1_API ALevel1_TransitionTrigger : public ATriggerBox
{
	GENERATED_BODY()
public:
	ALevel1_TransitionTrigger();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    TObjectPtr<ULevel1_DataAssetTransitionConfig> TransitionConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    bool bFireOnce = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (MultiLine = true))
    FString DesignerNote;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleOverlap(AActor* OverlappedActor, AActor* OtherActor);

    bool m_bAlreadyFired = false;

private:
    bool m_bInitiated{ false };
};
