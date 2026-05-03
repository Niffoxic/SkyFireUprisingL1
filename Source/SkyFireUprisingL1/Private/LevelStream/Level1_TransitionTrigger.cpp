#include "LevelStream/Level1_TransitionTrigger.h"
#include "LevelStream/Level1_DataAssetTransitionConfig.h"
#include "Events/Level1_GameEventSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Level1_Tags.h"

ALevel1_TransitionTrigger::ALevel1_TransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevel1_TransitionTrigger::BeginPlay()
{
	Super::BeginPlay();
    if (not m_bInitiated)
    {
        OnActorBeginOverlap.AddDynamic(this, &ALevel1_TransitionTrigger::HandleOverlap);
        m_bInitiated = true;
    }
}

void ALevel1_TransitionTrigger::HandleOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (m_bAlreadyFired && bFireOnce) return;
    if (!TransitionConfig)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ALevel1_TransitionTrigger] No TransitionConfig set on %s"), *GetName());
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || OtherActor != PC->GetPawn()) return;

    m_bAlreadyFired = true;

    UE_LOG(LogTemp, Log, TEXT("[ALevel1_TransitionTrigger] Player overlapped %s - broadcasting transition request."), *GetName());

    if (UGameInstance* GI = GetGameInstance())
    {
        if (auto* EBus = GI->GetSubsystem<ULevel1_GameEventSubsystem>())
        {
            EBus->BroadcastEvent(
                Level1_Tags::Event_Transition_Request,
                this,
                TransitionConfig);
        }
    }
}
