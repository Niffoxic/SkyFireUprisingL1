// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Level1_TestGameEvent.generated.h"

UCLASS()
class SKYFIREUPRISINGL1_API ALevel1_TestGameEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevel1_TestGameEvent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Game Event Test")
	FGameplayTag TagToListen;

	UPROPERTY(EditAnywhere, Category = "Game Event Test")
	FGameplayTag TagToBroadcast;

	UPROPERTY(EditAnywhere, Category = "Game Event Test")
	float BroadcastDelay = 5.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void OnHeard(FGameplayTag EventTag, AActor* Ins, UObject* Payload);

    void DoBroadcast();

private:
	float m_nTimeLeft{ 0.0f };
};
