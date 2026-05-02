#include "Events/Level1_TestGameEvent.h"
#include "Events/Level1_GameEventSubsystem.h"
#include "Level1_Tags.h"

// Sets default values
ALevel1_TestGameEvent::ALevel1_TestGameEvent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel1_TestGameEvent::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto* GInstance = GetGameInstance())
	{
		if (auto* EInstance = GInstance->GetSubsystem<ULevel1_GameEventSubsystem>())
		{
			FOnGameEventDelegate Callback;
			Callback.BindDynamic(this, &ALevel1_TestGameEvent::OnHeard);
			EInstance->RegisterListener(Level1_Tags::Event_Test_Hello, Callback);

			if (TagToListen.IsValid())
			{
				EInstance->RegisterListener(TagToListen, Callback);
			}
		}
	}

	m_nTimeLeft = BroadcastDelay;
}

void ALevel1_TestGameEvent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto* GI = GetGameInstance())
	{
		if (auto* Bus = GI->GetSubsystem<ULevel1_GameEventSubsystem>())
		{
			Bus->UnregisterAllListenersForObject(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ALevel1_TestGameEvent::OnHeard(FGameplayTag EventTag, AActor* Ins, UObject* Payload)
{
	const FString Msg = FString::Printf(TEXT("[SmokeTest] %s HEARD %s from %s"),
		*GetName(),
		*EventTag.ToString(),
		Ins ? *Ins->GetName() : TEXT("null"));
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
	}
}

void ALevel1_TestGameEvent::DoBroadcast()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto* Bus = GI->GetSubsystem<ULevel1_GameEventSubsystem>())
		{
			if (TagToBroadcast.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("[SmokeTest] %s broadcasting %s"), *GetName(), *TagToBroadcast.ToString());
				Bus->BroadcastEvent(TagToBroadcast, this, nullptr);
			}

			Bus->BroadcastEvent(Level1_Tags::Event_Test_Hello, this, nullptr);
		}
	}
}

// Called every frame
void ALevel1_TestGameEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_nTimeLeft -= DeltaTime;

	if (m_nTimeLeft <= 0.0f)
	{
		DoBroadcast();
		m_nTimeLeft = BroadcastDelay;
	}
}
