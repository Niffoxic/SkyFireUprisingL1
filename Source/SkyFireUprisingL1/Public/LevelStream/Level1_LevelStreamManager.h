#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "LevelStream/Level1_TransitionTypes.h"
#include "Tickable.h"
#include "Engine/LatentActionManager.h"
#include "Level1_LevelStreamManager.generated.h"

class ULevel1_DataAssetTransitionConfig;
class ULevel1_LevelTransitionEffect;
class ULevel1_GameEventSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTransitionComplete,
    ULevel1_DataAssetTransitionConfig*, Config);

/**
*   Streams Level from One map to another with optional transition effects and events.
*   Designed to be used with a single active transition at a time,
*   but can be extended to support multiple simultaneous transitions if needed
*/

UCLASS()
class SKYFIREUPRISINGL1_API ULevel1_LevelStreamManager : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "SkyFire|Streaming")
    bool BeginTransition(ULevel1_DataAssetTransitionConfig* Config);

    UFUNCTION(BlueprintCallable, Category = "SkyFire|Streaming")
    void CancelTransition();

    UFUNCTION(BlueprintPure, Category = "SkyFire|Streaming")
    bool IsTransitioning() const { return CurrentState != ETransitionState::Idle && CurrentState != ETransitionState::Done; }

    UFUNCTION(BlueprintPure, Category = "SkyFire|Streaming")
    ETransitionState GetCurrentState() const { return CurrentState; }

    UPROPERTY(BlueprintAssignable, Category = "SkyFire|Streaming")
    FOnTransitionComplete OnTransitionComplete;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(ULevel1_LevelStreamManager, STATGROUP_Tickables); }
    virtual bool IsTickable() const override { return IsTransitioning(); }
    virtual bool IsTickableInEditor() const override { return false; }
    virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

private:
    void EnterState_Validating();
    void EnterState_Preloading();
    void EnterState_EffectsBeforeSwap();
    void EnterState_Swap();
    void EnterState_EffectsAfterSwap();
    void EnterState_Done();
    void EnterState_Cancelled();

    void TickEffects(float DeltaTime, TArray<TObjectPtr<ULevel1_LevelTransitionEffect>>& Effects, bool bIsBeforeSwap);
    bool ShouldSwapNow(int32 NextEffectIndex) const;
    void DoActualSwap();
    void BroadcastTag(FGameplayTag Tag, UObject* Payload = nullptr) const;
    ULevel1_GameEventSubsystem* GetEventBus() const;

    UFUNCTION()
    void OnPreloadFinished();

    UFUNCTION()
    void OnSwapEventReceived(FGameplayTag EventTag, AActor* Instigator, UObject* Payload);

    UFUNCTION()
    void OnTransitionRequestReceived(FGameplayTag EventTag, AActor* Instigator, UObject* Payload);

    UPROPERTY()
    ETransitionState CurrentState = ETransitionState::Idle;

    UPROPERTY()
    TObjectPtr<ULevel1_DataAssetTransitionConfig> ActiveConfig;

    int32 CurrentEffectIndex = -1;
    bool  bPreloadFinished = false;
    bool  bSwapDone = false;
    bool  bSwapEventReceived = false;
    float TimeInState = 0.f;

    FLatentActionInfo PreloadLatentInfo;
    FLatentActionInfo UnloadLatentInfo;
};
