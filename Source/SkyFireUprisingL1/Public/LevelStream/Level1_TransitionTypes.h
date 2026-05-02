#pragma once

#include "CoreMinimal.h"
#include "TransitionTypes.generated.h"

UENUM(BlueprintType)
enum class ETransitionState : uint8
{
    Idle               UMETA(DisplayName = "Idle"),
    Validating         UMETA(DisplayName = "Validating"),
    Preloading         UMETA(DisplayName = "Preloading"),
    EffectsBeforeSwap  UMETA(DisplayName = "Effects Before Swap"),
    Swap               UMETA(DisplayName = "Swap"),
    EffectsAfterSwap   UMETA(DisplayName = "Effects After Swap"),
    Done               UMETA(DisplayName = "Done"),
    Cancelled          UMETA(DisplayName = "Cancelled")
};

UENUM(BlueprintType)
enum class ETransitionSwapTrigger : uint8
{
    AfterAllEffects   UMETA(DisplayName = "After All Effects"),

    OnEffectIndex     UMETA(DisplayName = "On Effect Index"),

    OnEvent           UMETA(DisplayName = "On Event"),

    AfterDelay        UMETA(DisplayName = "After Delay")
};
