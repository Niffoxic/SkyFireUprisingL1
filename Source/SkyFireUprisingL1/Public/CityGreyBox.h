#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGreyBox.generated.h"

class UBoxComponent;
class UInstancedStaticMeshComponent;
class UStaticMeshComponent;
class USceneComponent;
class UMaterialInterface;
class USoundBase;
class UCameraShakeBase;
class UPrimitiveComponent;

UCLASS()
class SKYFIREUPRISINGL1_API ACityGreyboxGenerator : public AActor
{
    GENERATED_BODY()

public:
    ACityGreyboxGenerator();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* RoadInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* BuildingInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* SideRoadInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* BarrierInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* SidewalkInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* RoadLineInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* StreetLightInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* RubbleInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* ParkedCarInstances;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CollapsingBuilding;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
    UBoxComponent* CityStartTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
    UBoxComponent* ExplosionTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
    UBoxComponent* CollapseTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
    UBoxComponent* ExitBlockTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggers")
    UBoxComponent* CityEndTrigger;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Path")
    TArray<FVector> PathPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
    float RoadWidth = 1400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
    float RoadThickness = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Detail")
    float SidewalkWidth = 260.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Detail")
    float SidewalkHeight = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Detail")
    float RoadLineSpacing = 650.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    int32 Seed = 27;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float BuildingSpacing = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MinBuildingHeight = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MaxBuildingHeight = 4500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MinBuildingWidth = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MaxBuildingWidth = 1600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MinBuildingDepth = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float MaxBuildingDepth = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buildings")
    float SidewalkOffset = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side Roads")
    bool bGenerateSideRoads = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side Roads")
    int32 SideRoadInterval = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Side Roads")
    float SideRoadLength = 3500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Detail")
    float StreetLightSpacing = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Detail")
    float ParkedCarSpacing = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "City Detail")
    float RubbleSpacing = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
    float FallAngle = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
    float FallSpeed = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
    USoundBase* ExplosionSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
    TSubclassOf<UCameraShakeBase> CameraShakeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* RoadMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* BuildingMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* BarrierMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* SidewalkMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* RoadLineMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* DetailMaterial;

private:
    void SetupCubeMesh();
    void GenerateCity();

    void GenerateRoads();
    void GenerateBuildings();
    void GenerateSideRoads();
    void GenerateSidewalks();
    void GenerateRoadLines();
    void GenerateStreetLights();
    void GenerateParkedCars();
    void GenerateRubble();
    void GenerateExitBlockers();

    void SetupEventTriggers();
    void SetupCollapsingBuilding();

    void CreateRoadSegment(const FVector& Start, const FVector& End);

    void CreateBuilding(
        const FVector& BasePos,
        const FVector& Direction,
        const FVector& Right,
        int32 Side,
        FRandomStream& RandomStream
    );

    void CreateSideRoad(
        const FVector& BasePos,
        const FVector& Direction
    );

    void CreateSidewalkSegment(
        const FVector& Start,
        const FVector& End,
        int32 Side
    );

    void CreateRoadLineSegment(
        const FVector& Start,
        const FVector& End
    );

    FVector GetPointOnPathSegment(int32 SegmentIndex, float Alpha) const;

    UFUNCTION()
    void OnExplosionTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnCollapseTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnCityEndTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

private:
    bool bBuildingFalling = false;
    bool bExplosionTriggered = false;
    bool bCollapseTriggered = false;
    bool bCityEndTriggered = false;

    float FallTimer = 0.0f;

    FQuat BuildingStartRotation;
    FQuat BuildingTargetRotation;
};