#include "Events/Level1_GameEventSubsystem.h"

void ULevel1_GameEventSubsystem::BroadcastEvent(FGameplayTag EventTag, AActor* Instigator, UObject* Payload)
{
    if (!EventTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventSubsystem] BroadcastEvent called with invalid tag"));
        return;
    }

    UE_LOG(LogTemp, Verbose, TEXT("[GameEventSubsystem] Broadcast: %s"), *EventTag.ToString());

    FOnGameEventMulticast Snapshot;
    if (FOnGameEventMulticast* Listeners = EventListeners.Find(EventTag))
    {
        Snapshot = *Listeners;
    }
    Snapshot.Broadcast(EventTag, Instigator, Payload);
}

bool ULevel1_GameEventSubsystem::RegisterListener(FGameplayTag EventTag, const FOnGameEventDelegate& Callback)
{
    if (!EventTag.IsValid() || !Callback.IsBound())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventSubsystem] RegisterListener: invalid tag or unbound callback"));
        return false;
    }

    FOnGameEventMulticast& Listeners = EventListeners.FindOrAdd(EventTag);

    if (Listeners.Contains(Callback.GetUObject(), Callback.GetFunctionName()))
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventSubsystem] Listener already registered for: %s"), *EventTag.ToString());
        return false;
	}
    Listeners.Add(Callback);

    UE_LOG(LogTemp, Verbose, TEXT("[GameEventSubsystem] Listener added for: %s"), *EventTag.ToString());
    return true;
}

void ULevel1_GameEventSubsystem::UnregisterListener(FGameplayTag EventTag, UObject* Listener)
{
    if (!EventTag.IsValid()) return;

    if (FOnGameEventMulticast* Listeners = EventListeners.Find(EventTag))
    {
        if (Listener)
        {
            Listeners->RemoveAll(Listener);
        }
        else
        {
            Listeners->Clear();
        }

        if (!Listeners->IsBound())
        {
            EventListeners.Remove(EventTag);
        }
    }
}

void ULevel1_GameEventSubsystem::UnregisterAllListenersForObject(UObject* Listener)
{
    if (!Listener) return;

    for (auto It = EventListeners.CreateIterator(); It; ++It)
    {
        It.Value().RemoveAll(Listener);
        if (!It.Value().IsBound())
        {
            It.RemoveCurrent();
        }
    }
}

void ULevel1_GameEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("[GameEventSubsystem] Initialized"));
}

void ULevel1_GameEventSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("[GameEventSubsystem] Deinitialized"));
    EventListeners.Empty();
    Super::Deinitialize();
}
