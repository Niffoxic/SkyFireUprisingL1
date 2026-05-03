#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Level1_LevelTransitionEffect.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class SKYFIREUPRISINGL1_API ULevel1_LevelTransitionEffect : public UObject
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, Category = "Transition")
    void Begin(UWorld* World);
    virtual void Begin_Implementation(UWorld* World) { bFinished = false; ElapsedTime = 0.f; }

    UFUNCTION(BlueprintNativeEvent, Category = "Transition")
    void Tick(float DeltaTime);
    virtual void Tick_Implementation(float DeltaTime) { ElapsedTime += DeltaTime; }

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Transition")
    bool IsFinished() const;
    virtual bool IsFinished_Implementation() const { return bFinished; }

    UFUNCTION(BlueprintNativeEvent, Category = "Transition")
    void Cancel();
    virtual void Cancel_Implementation() { bFinished = true; }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
    FString DebugLabel;

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Transition")
    bool bFinished = false;

    UPROPERTY(BlueprintReadWrite, Category = "Transition")
    float ElapsedTime = 0.f;
};
