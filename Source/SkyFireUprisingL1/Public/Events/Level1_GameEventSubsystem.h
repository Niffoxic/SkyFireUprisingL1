#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Level1_GameEventSubsystem.generated.h"

// Called by listeners: std::function
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
    FOnGameEventDelegate,
    FGameplayTag,   EventTag,
    AActor*,        Instigator,
    UObject*,       Payload
);

// Just a storage stuff: std::vector<std::function>>
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnGameEventMulticast,
    FGameplayTag,   EventTag,
    AActor*,        Instigator,
    UObject*,       Payload
);

/**
 *  Central Publish-Subscribe event bus
 *  Anyone broadcasts a tag, anyone can listen to it.
 *
 * Usage (broadcast):
 *     GetGameInstance()->GetSubsystem<ULevel1_GameEventSubsystem>()
 *         ->BroadcastEvent(FGameplayTag::RequestGameplayTag(TEXT("Event.Test.Hello")), this, nullptr);
 *
 * Usage (listen):
 *     FOnGameEventDelegate Cb;
 *     Cb.BindDynamic(this, &AMyActor::HandleEvent);
 *     EventBus->RegisterListener(Tag, Cb);
 */
UCLASS()
class SKYFIREUPRISINGL1_API ULevel1_GameEventSubsystem final : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "SkyFire|Events")
    void BroadcastEvent(
        FGameplayTag EventTag,
        AActor* Instigator = nullptr,
        UObject* Payload = nullptr
    );

    UFUNCTION(BlueprintCallable, Category = "SkyFire|Events", meta = (AutoCreateRefTerm = "Callback"))
    bool RegisterListener(FGameplayTag EventTag, const FOnGameEventDelegate& Callback);

    UFUNCTION(BlueprintCallable, Category = "SkyFire|Events")
    void UnregisterListener(FGameplayTag EventTag, UObject* Listener);

    UFUNCTION(BlueprintCallable, Category = "SkyFire|Events")
    void UnregisterAllListenersForObject(UObject* Listener);

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    TMap<FGameplayTag, FOnGameEventMulticast> EventListeners;
};
