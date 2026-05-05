#include "LevelStream/Level1_LevelStreamManager.h"
#include "LevelStream/Level1_DataAssetTransitionConfig.h"
#include "LevelStream/Effects/Level1_LevelTransitionEffect.h"
#include "Events/Level1_GameEventSubsystem.h"
#include "Level1_Tags.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

void ULevel1_LevelStreamManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Initialized."));

    if (auto* Bus = GetEventBus())
    {
        FOnGameEventDelegate Cb;
        Cb.BindDynamic(this, &ULevel1_LevelStreamManager::OnTransitionRequestReceived);
        Bus->RegisterListener(Level1_Tags::Event_Transition_Request, Cb);
    }
}

void ULevel1_LevelStreamManager::Deinitialize()
{
    if (auto* Bus = GetEventBus())
    {
        Bus->UnregisterAllListenersForObject(this);
    }
    Super::Deinitialize();
}

ULevel1_GameEventSubsystem* ULevel1_LevelStreamManager::GetEventBus() const
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            return GI->GetSubsystem<ULevel1_GameEventSubsystem>();
        }
    }
    return nullptr;
}

void ULevel1_LevelStreamManager::BroadcastTag(FGameplayTag Tag, UObject* Payload) const
{
    if (!Tag.IsValid()) return;
    if (auto* Bus = GetEventBus())
    {
        Bus->BroadcastEvent(Tag, nullptr, Payload);
    }
}

bool ULevel1_LevelStreamManager::BeginTransition(ULevel1_DataAssetTransitionConfig* Config)
{
    if (!Config)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] BeginTransition: null config."));
        return false;
    }
    if (IsTransitioning())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] BeginTransition rejected: already transitioning (%d)."), (int32)CurrentState);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] BeginTransition: %s -> %s"),
        *Config->SourceSublevel.ToString(), *Config->TargetSublevel.ToString());

    ActiveConfig        = Config;
    CurrentEffectIndex  = -1;
    bPreloadFinished    = false;
    bSwapDone           = false;
    bSwapEventReceived  = false;
    TimeInState         = 0.f;

    EnterState_Validating();
    return true;
}

void ULevel1_LevelStreamManager::CancelTransition()
{
    if (!IsTransitioning()) return;
    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] CancelTransition."));
    EnterState_Cancelled();
}

void ULevel1_LevelStreamManager::OnTransitionRequestReceived(FGameplayTag EventTag, AActor* Instigator, UObject* Payload)
{
    auto* Config = Cast<ULevel1_DataAssetTransitionConfig>(Payload);
    if (!Config)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] Event.Transition.Request had no ULevel1_DataAssetTransitionConfig payload."));
        return;
    }
    BeginTransition(Config);
}

void ULevel1_LevelStreamManager::EnterState_Validating()
{
    CurrentState    = ETransitionState::Validating;
    TimeInState     = 0.f;

    if (!ActiveConfig)
    {
        EnterState_Cancelled();
        return;
    }

    if (ActiveConfig->StartEventTag.IsValid())
    {
        BroadcastTag(ActiveConfig->StartEventTag, ActiveConfig);
    }
    BroadcastTag(Level1_Tags::Event_Transition_Started, ActiveConfig);
    EnterState_Preloading();
}

void ULevel1_LevelStreamManager::EnterState_Preloading()
{
    CurrentState = ETransitionState::Preloading;
    TimeInState  = 0.f;

    UWorld* World = GetWorld();
    if (!World) { EnterState_Cancelled(); return; }

    if (ActiveConfig->TargetSublevel.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] No TargetSublevel — skipping load."));
        bPreloadFinished = true;
    }
    else
    {
        PreloadLatentInfo.CallbackTarget    = this;
        PreloadLatentInfo.ExecutionFunction = FName("OnPreloadFinished");
        PreloadLatentInfo.UUID              = FMath::Rand();
        PreloadLatentInfo.Linkage           = 0;

        UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] LoadStreamLevel %s (hidden)"),
            *ActiveConfig->TargetSublevel.ToString());

        UGameplayStatics::LoadStreamLevel(
            World,
            ActiveConfig->TargetSublevel,
            false,
            false,
            PreloadLatentInfo);
    }

    EnterState_EffectsBeforeSwap();
}

void ULevel1_LevelStreamManager::OnPreloadFinished()
{
    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Preload finished."));
    bPreloadFinished = true;
    BroadcastTag(Level1_Tags::Event_Transition_Preloaded, ActiveConfig);
}

void ULevel1_LevelStreamManager::EnterState_EffectsBeforeSwap()
{
    CurrentState = ETransitionState::EffectsBeforeSwap;
    TimeInState = 0.f;
    CurrentEffectIndex = -1;

    if (ActiveConfig->SwapTrigger == ETransitionSwapTrigger::OnEvent && ActiveConfig->SwapEventTag.IsValid())
    {
        if (auto* Bus = GetEventBus())
        {
            FOnGameEventDelegate Cb;
            Cb.BindDynamic(this, &ULevel1_LevelStreamManager::OnSwapEventReceived);
            Bus->RegisterListener(ActiveConfig->SwapEventTag, Cb);
        }
    }
}

void ULevel1_LevelStreamManager::OnSwapEventReceived(FGameplayTag EventTag, AActor* Instigator, UObject* Payload)
{
    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Swap event received: %s"), *EventTag.ToString());
    bSwapEventReceived = true;
}

bool ULevel1_LevelStreamManager::ShouldSwapNow(int32 NextEffectIndex) const
{
    if (!ActiveConfig) return true;

    switch (ActiveConfig->SwapTrigger)
    {
    case ETransitionSwapTrigger::AfterAllEffects:
        return NextEffectIndex >= ActiveConfig->EffectsBeforeSwap.Num();

    case ETransitionSwapTrigger::OnEffectIndex:
        return NextEffectIndex >= ActiveConfig->SwapAtEffectIndex;

    case ETransitionSwapTrigger::OnEvent:
        return bSwapEventReceived;

    case ETransitionSwapTrigger::AfterDelay:
        return TimeInState >= ActiveConfig->SwapAfterSeconds;
    }
    return true;
}

void ULevel1_LevelStreamManager::TickEffects(float DeltaTime, TArray<TObjectPtr<ULevel1_LevelTransitionEffect>>& Effects, bool bIsBeforeSwap)
{
    UWorld* World = GetWorld();

    if (CurrentEffectIndex < 0)
    {
        CurrentEffectIndex = 0;
        if (Effects.IsValidIndex(CurrentEffectIndex) && Effects[CurrentEffectIndex])
        {
            UE_LOG(LogTemp, Verbose, TEXT("[LevelStreamManager] Effect[%d] Begin: %s"),
                CurrentEffectIndex, *Effects[CurrentEffectIndex]->DebugLabel);
            Effects[CurrentEffectIndex]->Begin(World);
            BroadcastTag(Level1_Tags::Event_Transition_EffectStarted, Effects[CurrentEffectIndex]);
        }
    }

    if (Effects.IsValidIndex(CurrentEffectIndex) && Effects[CurrentEffectIndex])
    {
        Effects[CurrentEffectIndex]->Tick(DeltaTime);

        if (Effects[CurrentEffectIndex]->IsFinished())
        {
            BroadcastTag(Level1_Tags::Event_Transition_EffectFinished, Effects[CurrentEffectIndex]);
            CurrentEffectIndex++;

            if (Effects.IsValidIndex(CurrentEffectIndex) && Effects[CurrentEffectIndex])
            {
                UE_LOG(LogTemp, Verbose, TEXT("[LevelStreamManager] Effect[%d] Begin: %s"),
                    CurrentEffectIndex, *Effects[CurrentEffectIndex]->DebugLabel);
                Effects[CurrentEffectIndex]->Begin(World);
                BroadcastTag(Level1_Tags::Event_Transition_EffectStarted, Effects[CurrentEffectIndex]);
            }
        }
    }
    else
    {
        CurrentEffectIndex++;
    }
}

void ULevel1_LevelStreamManager::Tick(float DeltaTime)
{
    TimeInState += DeltaTime;

    switch (CurrentState)
    {
    case ETransitionState::EffectsBeforeSwap:
    {
        if (ActiveConfig)
        {
            TickEffects(DeltaTime, ActiveConfig->EffectsBeforeSwap, true);

            if (ShouldSwapNow(CurrentEffectIndex))
            {
                if (bPreloadFinished)
                {
                    EnterState_Swap();
                }
                else if (TimeInState > 10.f)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] Preload timeout — forcing swap."));
                    EnterState_Swap();
                }
            }
        }
        break;
    }

    case ETransitionState::EffectsAfterSwap:
    {
        if (ActiveConfig)
        {
            TickEffects(DeltaTime, ActiveConfig->EffectsAfterSwap, false);

            if (CurrentEffectIndex >= ActiveConfig->EffectsAfterSwap.Num())
            {
                EnterState_Done();
            }
        }
        break;
    }

    default:
        break;
    }
}

void ULevel1_LevelStreamManager::EnterState_Swap()
{
    CurrentState = ETransitionState::Swap;
    TimeInState = 0.f;

    DoActualSwap();

    BroadcastTag(Level1_Tags::Event_Transition_SwapNow, ActiveConfig);

    CurrentEffectIndex = -1;
    EnterState_EffectsAfterSwap();
}

void ULevel1_LevelStreamManager::DoActualSwap()
{
    UWorld* World = GetWorld();
    if (!World || !ActiveConfig) return;

    // Make target visible
    if (!ActiveConfig->TargetSublevel.IsNone())
    {
        if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(World, ActiveConfig->TargetSublevel))
        {
            Streaming->SetShouldBeVisible(true);
            UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Made %s visible."), *ActiveConfig->TargetSublevel.ToString());
        }
    }

    // Reposition player if requested
    if (ActiveConfig->bRepositionPlayer)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                FVector OldVelocity = FVector::ZeroVector;
                if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
                {
                    if (Root->IsSimulatingPhysics())
                    {
                        OldVelocity = Root->GetPhysicsLinearVelocity();
                    }
                }

                Pawn->SetActorTransform(
                    ActiveConfig->PlayerSpawnInTarget,
                    false,
                    nullptr,
                    ETeleportType::TeleportPhysics);

                if (ActiveConfig->bPreserveVehicleVelocity)
                {
                    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
                    {
                        if (Root->IsSimulatingPhysics())
                        {
                            const FVector NewVel = ActiveConfig->PlayerSpawnInTarget.GetRotation().RotateVector(
                                FVector(OldVelocity.Size(), 0.f, 0.f));
                            Root->SetPhysicsLinearVelocity(NewVel);
                        }
                    }
                }
            }
        }
    }

    // Unload source
    if (!ActiveConfig->SourceSublevel.IsNone())
    {
        UnloadLatentInfo.CallbackTarget = this;
        UnloadLatentInfo.UUID           = FMath::Rand();
        UnloadLatentInfo.Linkage        = 0;
        UGameplayStatics::UnloadStreamLevel(World, ActiveConfig->SourceSublevel, UnloadLatentInfo, false);
        UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Unloading %s."), *ActiveConfig->SourceSublevel.ToString());
    }

    bSwapDone = true;
}

void ULevel1_LevelStreamManager::EnterState_EffectsAfterSwap()
{
    CurrentState = ETransitionState::EffectsAfterSwap;
    TimeInState = 0.f;

    if (!ActiveConfig || ActiveConfig->EffectsAfterSwap.Num() == 0)
    {
        EnterState_Done();
        return;
    }
}

void ULevel1_LevelStreamManager::EnterState_Done()
{
    CurrentState = ETransitionState::Done;
    UE_LOG(LogTemp, Log, TEXT("[LevelStreamManager] Transition complete."));

    if (ActiveConfig && ActiveConfig->CompleteEventTag.IsValid())
    {
        BroadcastTag(ActiveConfig->CompleteEventTag, ActiveConfig);
    }
    BroadcastTag(Level1_Tags::Event_Transition_Complete, ActiveConfig);

    OnTransitionComplete.Broadcast(ActiveConfig);

    if (ActiveConfig && ActiveConfig->SwapTrigger == ETransitionSwapTrigger::OnEvent)
    {
        if (auto* Bus = GetEventBus())
        {
            Bus->UnregisterListener(ActiveConfig->SwapEventTag, this);
        }
    }

    ActiveConfig = nullptr;
    CurrentState = ETransitionState::Idle;
}

void ULevel1_LevelStreamManager::EnterState_Cancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("[LevelStreamManager] Transition cancelled."));
    CurrentState = ETransitionState::Cancelled;

    if (ActiveConfig)
    {
        for (ULevel1_LevelTransitionEffect* Effect : ActiveConfig->EffectsBeforeSwap)
        {
            if (Effect) Effect->Cancel();
        }
        for (ULevel1_LevelTransitionEffect* Effect : ActiveConfig->EffectsAfterSwap)
        {
            if (Effect) Effect->Cancel();
        }
    }

    BroadcastTag(Level1_Tags::Event_Transition_Cancelled, ActiveConfig);

    ActiveConfig = nullptr;
    CurrentState = ETransitionState::Idle;
}
