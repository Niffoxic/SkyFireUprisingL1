#include "CityGreyBox.h"

#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

ACityGreyboxGenerator::ACityGreyboxGenerator()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    RoadInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RoadInstances"));
    RoadInstances->SetupAttachment(SceneRoot);

    BuildingInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BuildingInstances"));
    BuildingInstances->SetupAttachment(SceneRoot);

    SideRoadInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SideRoadInstances"));
    SideRoadInstances->SetupAttachment(SceneRoot);

    BarrierInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BarrierInstances"));
    BarrierInstances->SetupAttachment(SceneRoot);

    SidewalkInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SidewalkInstances"));
    SidewalkInstances->SetupAttachment(SceneRoot);

    RoadLineInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RoadLineInstances"));
    RoadLineInstances->SetupAttachment(SceneRoot);

    StreetLightInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StreetLightInstances"));
    StreetLightInstances->SetupAttachment(SceneRoot);

    RubbleInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RubbleInstances"));
    RubbleInstances->SetupAttachment(SceneRoot);

    ParkedCarInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ParkedCarInstances"));
    ParkedCarInstances->SetupAttachment(SceneRoot);

    CollapsingBuilding = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollapsingBuilding"));
    CollapsingBuilding->SetupAttachment(SceneRoot);
    CollapsingBuilding->SetMobility(EComponentMobility::Movable);

    CityStartTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CityStartTrigger"));
    CityStartTrigger->SetupAttachment(SceneRoot);

    ExplosionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ExplosionTrigger"));
    ExplosionTrigger->SetupAttachment(SceneRoot);

    CollapseTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CollapseTrigger"));
    CollapseTrigger->SetupAttachment(SceneRoot);

    ExitBlockTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitBlockTrigger"));
    ExitBlockTrigger->SetupAttachment(SceneRoot);

    CityEndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CityEndTrigger"));
    CityEndTrigger->SetupAttachment(SceneRoot);

    PathPoints =
    {
        FVector(0.0f, 0.0f, 0.0f),
        FVector(0.0f, 6000.0f, 0.0f),
        FVector(3500.0f, 12000.0f, 0.0f),
        FVector(3500.0f, 18000.0f, 0.0f),
        FVector(-2000.0f, 24000.0f, 0.0f),
        FVector(-2000.0f, 31000.0f, 0.0f),
        FVector(3000.0f, 37000.0f, 0.0f),
        FVector(3000.0f, 44000.0f, 0.0f),
        FVector(-3500.0f, 51000.0f, 0.0f),
        FVector(-3500.0f, 58000.0f, 0.0f),
        FVector(1500.0f, 65000.0f, 0.0f)
    };

    SetupCubeMesh();
}

void ACityGreyboxGenerator::BeginPlay()
{
    Super::BeginPlay();

    ExplosionTrigger->OnComponentBeginOverlap.AddDynamic(
        this,
        &ACityGreyboxGenerator::OnExplosionTriggerBeginOverlap
    );

    CollapseTrigger->OnComponentBeginOverlap.AddDynamic(
        this,
        &ACityGreyboxGenerator::OnCollapseTriggerBeginOverlap
    );

    CityEndTrigger->OnComponentBeginOverlap.AddDynamic(
        this,
        &ACityGreyboxGenerator::OnCityEndTriggerBeginOverlap
    );
}

void ACityGreyboxGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bBuildingFalling && CollapsingBuilding)
    {
        FallTimer += DeltaTime * FallSpeed;

        float Alpha = FMath::Clamp(FallTimer, 0.0f, 1.0f);

        FQuat NewRotation = FQuat::Slerp(
            BuildingStartRotation,
            BuildingTargetRotation,
            Alpha
        );

        CollapsingBuilding->SetRelativeRotation(NewRotation.Rotator());

        if (Alpha >= 1.0f)
        {
            bBuildingFalling = false;
        }
    }
}

void ACityGreyboxGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    GenerateCity();
}

void ACityGreyboxGenerator::SetupCubeMesh()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(
        TEXT("/Engine/BasicShapes/Cube.Cube")
    );

    if (CubeMeshFinder.Succeeded())
    {
        UStaticMesh* CubeMesh = CubeMeshFinder.Object;

        RoadInstances->SetStaticMesh(CubeMesh);
        BuildingInstances->SetStaticMesh(CubeMesh);
        SideRoadInstances->SetStaticMesh(CubeMesh);
        BarrierInstances->SetStaticMesh(CubeMesh);
        SidewalkInstances->SetStaticMesh(CubeMesh);
        RoadLineInstances->SetStaticMesh(CubeMesh);
        StreetLightInstances->SetStaticMesh(CubeMesh);
        RubbleInstances->SetStaticMesh(CubeMesh);
        ParkedCarInstances->SetStaticMesh(CubeMesh);
        CollapsingBuilding->SetStaticMesh(CubeMesh);
    }

    RoadInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BuildingInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SideRoadInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BarrierInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SidewalkInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    StreetLightInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    RubbleInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ParkedCarInstances->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollapsingBuilding->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    RoadLineInstances->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CityStartTrigger->SetCollisionProfileName(TEXT("Trigger"));
    ExplosionTrigger->SetCollisionProfileName(TEXT("Trigger"));
    CollapseTrigger->SetCollisionProfileName(TEXT("Trigger"));
    ExitBlockTrigger->SetCollisionProfileName(TEXT("Trigger"));
    CityEndTrigger->SetCollisionProfileName(TEXT("Trigger"));

    CityStartTrigger->SetGenerateOverlapEvents(true);
    ExplosionTrigger->SetGenerateOverlapEvents(true);
    CollapseTrigger->SetGenerateOverlapEvents(true);
    ExitBlockTrigger->SetGenerateOverlapEvents(true);
    CityEndTrigger->SetGenerateOverlapEvents(true);
}

void ACityGreyboxGenerator::GenerateCity()
{
    if (
        !RoadInstances ||
        !BuildingInstances ||
        !SideRoadInstances ||
        !BarrierInstances ||
        !SidewalkInstances ||
        !RoadLineInstances ||
        !StreetLightInstances ||
        !RubbleInstances ||
        !ParkedCarInstances
        )
    {
        return;
    }

    RoadInstances->ClearInstances();
    BuildingInstances->ClearInstances();
    SideRoadInstances->ClearInstances();
    BarrierInstances->ClearInstances();
    SidewalkInstances->ClearInstances();
    RoadLineInstances->ClearInstances();
    StreetLightInstances->ClearInstances();
    RubbleInstances->ClearInstances();
    ParkedCarInstances->ClearInstances();

    if (RoadMaterial)
    {
        RoadInstances->SetMaterial(0, RoadMaterial);
        SideRoadInstances->SetMaterial(0, RoadMaterial);
    }

    if (BuildingMaterial)
    {
        BuildingInstances->SetMaterial(0, BuildingMaterial);
        CollapsingBuilding->SetMaterial(0, BuildingMaterial);
    }

    if (BarrierMaterial)
    {
        BarrierInstances->SetMaterial(0, BarrierMaterial);
    }

    if (SidewalkMaterial)
    {
        SidewalkInstances->SetMaterial(0, SidewalkMaterial);
    }

    if (RoadLineMaterial)
    {
        RoadLineInstances->SetMaterial(0, RoadLineMaterial);
    }

    if (DetailMaterial)
    {
        StreetLightInstances->SetMaterial(0, DetailMaterial);
        RubbleInstances->SetMaterial(0, DetailMaterial);
        ParkedCarInstances->SetMaterial(0, DetailMaterial);
    }

    GenerateRoads();
    GenerateSidewalks();
    GenerateRoadLines();
    GenerateBuildings();

    if (bGenerateSideRoads)
    {
        GenerateSideRoads();
    }

    GenerateStreetLights();
    GenerateParkedCars();
    GenerateRubble();
    GenerateExitBlockers();

    SetupEventTriggers();
    SetupCollapsingBuilding();
}

void ACityGreyboxGenerator::GenerateRoads()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    for (int32 i = 0; i < PathPoints.Num() - 1; i++)
    {
        CreateRoadSegment(PathPoints[i], PathPoints[i + 1]);
    }
}

void ACityGreyboxGenerator::CreateRoadSegment(const FVector& Start, const FVector& End)
{
    FVector Direction = End - Start;
    float Length = Direction.Size();

    if (Length <= 1.0f)
    {
        return;
    }

    Direction.Normalize();

    FVector Center = (Start + End) * 0.5f;
    Center.Z += RoadThickness * 0.5f;

    FRotator Rotation = Direction.Rotation();

    FVector Scale;
    Scale.X = Length / 100.0f;
    Scale.Y = RoadWidth / 100.0f;
    Scale.Z = RoadThickness / 100.0f;

    FTransform RoadTransform(Rotation, Center, Scale);
    RoadInstances->AddInstance(RoadTransform);
}

void ACityGreyboxGenerator::GenerateBuildings()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    FRandomStream RandomStream(Seed);

    for (int32 i = 0; i < PathPoints.Num() - 1; i++)
    {
        FVector Start = PathPoints[i];
        FVector End = PathPoints[i + 1];

        FVector Direction = End - Start;
        float SegmentLength = Direction.Size();

        if (SegmentLength <= 1.0f)
        {
            continue;
        }

        Direction.Normalize();

        FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

        int32 Count = FMath::FloorToInt(SegmentLength / BuildingSpacing);

        for (int32 j = 1; j < Count; j++)
        {
            FVector BasePos = Start + Direction * (j * BuildingSpacing);

            CreateBuilding(BasePos, Direction, Right, 1, RandomStream);
            CreateBuilding(BasePos, Direction, Right, -1, RandomStream);
        }
    }
}

void ACityGreyboxGenerator::CreateBuilding(
    const FVector& BasePos,
    const FVector& Direction,
    const FVector& Right,
    int32 Side,
    FRandomStream& RandomStream
)
{
    float Height = RandomStream.FRandRange(MinBuildingHeight, MaxBuildingHeight);
    float Width = RandomStream.FRandRange(MinBuildingWidth, MaxBuildingWidth);
    float Depth = RandomStream.FRandRange(MinBuildingDepth, MaxBuildingDepth);

    FVector Position = BasePos;
    Position += Right * Side * ((RoadWidth * 0.5f) + SidewalkOffset + (Depth * 0.5f));
    Position.Z = Height * 0.5f;

    FRotator Rotation = Direction.Rotation();

    FVector Scale;
    Scale.X = Width / 100.0f;
    Scale.Y = Depth / 100.0f;
    Scale.Z = Height / 100.0f;

    FTransform BuildingTransform(Rotation, Position, Scale);
    BuildingInstances->AddInstance(BuildingTransform);
}

void ACityGreyboxGenerator::GenerateSidewalks()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    for (int32 i = 0; i < PathPoints.Num() - 1; i++)
    {
        CreateSidewalkSegment(PathPoints[i], PathPoints[i + 1], 1);
        CreateSidewalkSegment(PathPoints[i], PathPoints[i + 1], -1);
    }
}

void ACityGreyboxGenerator::CreateSidewalkSegment(
    const FVector& Start,
    const FVector& End,
    int32 Side
)
{
    FVector Direction = End - Start;
    float Length = Direction.Size();

    if (Length <= 1.0f)
    {
        return;
    }

    Direction.Normalize();

    FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

    FVector Center = (Start + End) * 0.5f;
    Center += Right * Side * ((RoadWidth * 0.5f) + (SidewalkWidth * 0.5f));
    Center.Z += SidewalkHeight * 0.5f;

    FRotator Rotation = Direction.Rotation();

    FVector Scale;
    Scale.X = Length / 100.0f;
    Scale.Y = SidewalkWidth / 100.0f;
    Scale.Z = SidewalkHeight / 100.0f;

    SidewalkInstances->AddInstance(FTransform(Rotation, Center, Scale));
}

void ACityGreyboxGenerator::GenerateRoadLines()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    for (int32 i = 0; i < PathPoints.Num() - 1; i++)
    {
        CreateRoadLineSegment(PathPoints[i], PathPoints[i + 1]);
    }
}

void ACityGreyboxGenerator::CreateRoadLineSegment(
    const FVector& Start,
    const FVector& End
)
{
    FVector Direction = End - Start;
    float Length = Direction.Size();

    if (Length <= 1.0f)
    {
        return;
    }

    Direction.Normalize();

    int32 Count = FMath::FloorToInt(Length / RoadLineSpacing);

    for (int32 i = 1; i < Count; i++)
    {
        FVector Position = Start + Direction * (i * RoadLineSpacing);
        Position.Z += RoadThickness + 5.0f;

        FRotator Rotation = Direction.Rotation();

        FVector Scale;
        Scale.X = 320.0f / 100.0f;
        Scale.Y = 25.0f / 100.0f;
        Scale.Z = 5.0f / 100.0f;

        RoadLineInstances->AddInstance(FTransform(Rotation, Position, Scale));
    }
}

void ACityGreyboxGenerator::GenerateSideRoads()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    for (int32 i = 0; i < PathPoints.Num() - 1; i++)
    {
        if (SideRoadInterval > 0 && i % SideRoadInterval != 0)
        {
            continue;
        }

        FVector Start = PathPoints[i];
        FVector End = PathPoints[i + 1];

        FVector Direction = End - Start;

        if (Direction.Size() <= 1.0f)
        {
            continue;
        }

        Direction.Normalize();

        FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

        FVector BasePos = FMath::Lerp(Start, End, 0.5f);

        CreateSideRoad(BasePos, Right);
        CreateSideRoad(BasePos, -Right);
    }
}

void ACityGreyboxGenerator::CreateSideRoad(
    const FVector& BasePos,
    const FVector& Direction
)
{
    FVector SideDirection = Direction.GetSafeNormal();

    FVector RoadCenter = BasePos;
    RoadCenter += SideDirection * ((RoadWidth * 0.5f) + (SideRoadLength * 0.5f));
    RoadCenter.Z += RoadThickness * 0.5f;

    FRotator Rotation = SideDirection.Rotation();

    FVector SideRoadScale;
    SideRoadScale.X = SideRoadLength / 100.0f;
    SideRoadScale.Y = (RoadWidth * 0.7f) / 100.0f;
    SideRoadScale.Z = RoadThickness / 100.0f;

    SideRoadInstances->AddInstance(FTransform(Rotation, RoadCenter, SideRoadScale));

    FVector BarrierPosition = BasePos;
    BarrierPosition += SideDirection * ((RoadWidth * 0.5f) + SideRoadLength - 300.0f);
    BarrierPosition.Z += 120.0f;

    FVector BarrierScale;
    BarrierScale.X = 150.0f / 100.0f;
    BarrierScale.Y = (RoadWidth * 0.65f) / 100.0f;
    BarrierScale.Z = 240.0f / 100.0f;

    BarrierInstances->AddInstance(FTransform(Rotation, BarrierPosition, BarrierScale));
}

void ACityGreyboxGenerator::GenerateStreetLights()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    for (int32 SegmentIndex = 0; SegmentIndex < PathPoints.Num() - 1; SegmentIndex++)
    {
        FVector Start = PathPoints[SegmentIndex];
        FVector End = PathPoints[SegmentIndex + 1];

        FVector Direction = End - Start;
        float Length = Direction.Size();

        if (Length <= 1.0f)
        {
            continue;
        }

        Direction.Normalize();

        FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

        int32 Count = FMath::FloorToInt(Length / StreetLightSpacing);

        for (int32 i = 1; i < Count; i++)
        {
            FVector BasePos = Start + Direction * (i * StreetLightSpacing);

            for (int32 Side = -1; Side <= 1; Side += 2)
            {
                FVector PolePosition = BasePos;
                PolePosition += Right * Side * ((RoadWidth * 0.5f) + SidewalkWidth + 80.0f);
                PolePosition.Z = 300.0f;

                FRotator PoleRotation = Direction.Rotation();

                FVector PoleScale;
                PoleScale.X = 35.0f / 100.0f;
                PoleScale.Y = 35.0f / 100.0f;
                PoleScale.Z = 600.0f / 100.0f;

                StreetLightInstances->AddInstance(FTransform(PoleRotation, PolePosition, PoleScale));

                FVector LampHeadPosition = PolePosition;
                LampHeadPosition.Z += 320.0f;
                LampHeadPosition += Right * -Side * 120.0f;

                FVector HeadScale;
                HeadScale.X = 160.0f / 100.0f;
                HeadScale.Y = 50.0f / 100.0f;
                HeadScale.Z = 45.0f / 100.0f;

                StreetLightInstances->AddInstance(FTransform(PoleRotation, LampHeadPosition, HeadScale));
            }
        }
    }
}

void ACityGreyboxGenerator::GenerateParkedCars()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    FRandomStream RandomStream(Seed + 88);

    for (int32 SegmentIndex = 0; SegmentIndex < PathPoints.Num() - 1; SegmentIndex++)
    {
        FVector Start = PathPoints[SegmentIndex];
        FVector End = PathPoints[SegmentIndex + 1];

        FVector Direction = End - Start;
        float Length = Direction.Size();

        if (Length <= 1.0f)
        {
            continue;
        }

        Direction.Normalize();

        FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

        int32 Count = FMath::FloorToInt(Length / ParkedCarSpacing);

        for (int32 i = 1; i < Count; i++)
        {
            if (RandomStream.FRand() > 0.6f)
            {
                continue;
            }

            int32 Side = RandomStream.RandRange(0, 1) == 0 ? -1 : 1;

            FVector BodyPosition = Start + Direction * (i * ParkedCarSpacing);
            BodyPosition += Right * Side * ((RoadWidth * 0.5f) - 180.0f);
            BodyPosition.Z = 70.0f;

            FRotator CarRotation = Direction.Rotation();

            if (Side < 0)
            {
                CarRotation.Yaw += 180.0f;
            }

            CarRotation.Yaw += RandomStream.FRandRange(-8.0f, 8.0f);

            FVector BodyScale;
            BodyScale.X = 360.0f / 100.0f;
            BodyScale.Y = 170.0f / 100.0f;
            BodyScale.Z = 120.0f / 100.0f;

            ParkedCarInstances->AddInstance(FTransform(CarRotation, BodyPosition, BodyScale));

            FVector CabinPosition = BodyPosition;
            CabinPosition.Z += 95.0f;
            CabinPosition += Direction * 20.0f;

            FVector CabinScale;
            CabinScale.X = 170.0f / 100.0f;
            CabinScale.Y = 135.0f / 100.0f;
            CabinScale.Z = 80.0f / 100.0f;

            ParkedCarInstances->AddInstance(FTransform(CarRotation, CabinPosition, CabinScale));
        }
    }
}

void ACityGreyboxGenerator::GenerateRubble()
{
    if (PathPoints.Num() < 2)
    {
        return;
    }

    FRandomStream RandomStream(Seed + 155);

    for (int32 SegmentIndex = 0; SegmentIndex < PathPoints.Num() - 1; SegmentIndex++)
    {
        FVector Start = PathPoints[SegmentIndex];
        FVector End = PathPoints[SegmentIndex + 1];

        FVector Direction = End - Start;
        float Length = Direction.Size();

        if (Length <= 1.0f)
        {
            continue;
        }

        Direction.Normalize();

        FVector Right = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();

        int32 Count = FMath::FloorToInt(Length / RubbleSpacing);

        for (int32 i = 1; i < Count; i++)
        {
            if (RandomStream.FRand() > 0.5f)
            {
                continue;
            }

            FVector BasePos = Start + Direction * (i * RubbleSpacing);

            int32 RubblePieces = RandomStream.RandRange(3, 8);

            for (int32 j = 0; j < RubblePieces; j++)
            {
                FVector Position = BasePos;
                Position += Direction * RandomStream.FRandRange(-250.0f, 250.0f);
                Position += Right * RandomStream.FRandRange(-RoadWidth * 0.3f, RoadWidth * 0.3f);
                Position.Z = RandomStream.FRandRange(25.0f, 80.0f);

                FRotator Rotation;
                Rotation.Roll = RandomStream.FRandRange(-30.0f, 30.0f);
                Rotation.Pitch = RandomStream.FRandRange(-20.0f, 20.0f);
                Rotation.Yaw = RandomStream.FRandRange(0.0f, 360.0f);

                float Size = RandomStream.FRandRange(60.0f, 180.0f);

                FVector Scale;
                Scale.X = RandomStream.FRandRange(0.6f, 1.6f) * Size / 100.0f;
                Scale.Y = RandomStream.FRandRange(0.6f, 1.4f) * Size / 100.0f;
                Scale.Z = RandomStream.FRandRange(0.3f, 0.8f) * Size / 100.0f;

                RubbleInstances->AddInstance(FTransform(Rotation, Position, Scale));
            }
        }
    }
}

void ACityGreyboxGenerator::GenerateExitBlockers()
{
    if (PathPoints.Num() < 6)
    {
        return;
    }

    FVector ExitBlockPos = GetPointOnPathSegment(4, 0.45f);

    FVector SegmentDirection = PathPoints[5] - PathPoints[4];

    if (SegmentDirection.Size() <= 1.0f)
    {
        return;
    }

    SegmentDirection.Normalize();

    FVector Right = FVector::CrossProduct(SegmentDirection, FVector::UpVector).GetSafeNormal();

    for (int32 i = -3; i <= 3; i++)
    {
        FVector Position = ExitBlockPos;
        Position += Right * i * 220.0f;
        Position.Z = 120.0f;

        FRotator Rotation = SegmentDirection.Rotation();
        Rotation.Yaw += 90.0f;

        FVector Scale;
        Scale.X = 180.0f / 100.0f;
        Scale.Y = 160.0f / 100.0f;
        Scale.Z = 240.0f / 100.0f;

        BarrierInstances->AddInstance(FTransform(Rotation, Position, Scale));
    }
}

void ACityGreyboxGenerator::SetupEventTriggers()
{
    if (PathPoints.Num() < 6)
    {
        return;
    }

    FVector StartPos = PathPoints[0];
    FVector ExplosionPos = PathPoints[1];
    FVector CollapsePos = GetPointOnPathSegment(2, 0.45f);
    FVector ExitBlockPos = GetPointOnPathSegment(4, 0.45f);
    FVector EndPos = PathPoints.Last();

    CityStartTrigger->SetRelativeLocation(StartPos + FVector(0.0f, 0.0f, 250.0f));
    CityStartTrigger->SetBoxExtent(FVector(1000.0f, 500.0f, 250.0f));

    ExplosionTrigger->SetRelativeLocation(ExplosionPos + FVector(0.0f, 0.0f, 250.0f));
    ExplosionTrigger->SetBoxExtent(FVector(1000.0f, 600.0f, 250.0f));

    CollapseTrigger->SetRelativeLocation(CollapsePos + FVector(0.0f, 0.0f, 250.0f));
    CollapseTrigger->SetBoxExtent(FVector(1000.0f, 600.0f, 250.0f));

    ExitBlockTrigger->SetRelativeLocation(ExitBlockPos + FVector(0.0f, 0.0f, 250.0f));
    ExitBlockTrigger->SetBoxExtent(FVector(1000.0f, 600.0f, 250.0f));

    CityEndTrigger->SetRelativeLocation(EndPos + FVector(0.0f, 0.0f, 250.0f));
    CityEndTrigger->SetBoxExtent(FVector(1000.0f, 600.0f, 250.0f));
}

void ACityGreyboxGenerator::SetupCollapsingBuilding()
{
    if (PathPoints.Num() < 4)
    {
        return;
    }

    FVector CollapsePos = GetPointOnPathSegment(2, 0.45f);

    FVector SegmentDirection = PathPoints[3] - PathPoints[2];

    if (SegmentDirection.Size() <= 1.0f)
    {
        return;
    }

    SegmentDirection.Normalize();

    FVector Right = FVector::CrossProduct(SegmentDirection, FVector::UpVector).GetSafeNormal();

    int32 CollapseSide = 1;

    FVector BuildingPosition = CollapsePos;
    BuildingPosition += Right * CollapseSide * ((RoadWidth * 0.5f) + 600.0f);
    BuildingPosition.Z = 1800.0f;

    FRotator BuildingRotation = SegmentDirection.Rotation();

    CollapsingBuilding->SetRelativeLocation(BuildingPosition);
    CollapsingBuilding->SetRelativeRotation(BuildingRotation);
    CollapsingBuilding->SetRelativeScale3D(FVector(12.0f, 12.0f, 36.0f));

    BuildingStartRotation = BuildingRotation.Quaternion();

    FQuat FallQuat = FQuat(
        SegmentDirection,
        FMath::DegreesToRadians(-FallAngle * CollapseSide)
    );

    BuildingTargetRotation = FallQuat * BuildingStartRotation;

    FallTimer = 0.0f;
    bBuildingFalling = false;
}

FVector ACityGreyboxGenerator::GetPointOnPathSegment(int32 SegmentIndex, float Alpha) const
{
    if (!PathPoints.IsValidIndex(SegmentIndex) || !PathPoints.IsValidIndex(SegmentIndex + 1))
    {
        return FVector::ZeroVector;
    }

    return FMath::Lerp(PathPoints[SegmentIndex], PathPoints[SegmentIndex + 1], Alpha);
}

void ACityGreyboxGenerator::OnExplosionTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (bExplosionTriggered)
    {
        return;
    }

    bExplosionTriggered = true;

    FVector Location = ExplosionTrigger->GetComponentLocation();

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }

    if (CameraShakeClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (PC && PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
        }
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Yellow,
            TEXT("City attack event triggered: explosion zone")
        );
    }
}

void ACityGreyboxGenerator::OnCollapseTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (bCollapseTriggered)
    {
        return;
    }

    bCollapseTriggered = true;
    bBuildingFalling = true;
    FallTimer = 0.0f;

    FVector Location = CollapseTrigger->GetComponentLocation();

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }

    if (CameraShakeClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (PC && PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
        }
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.0f,
            FColor::Red,
            TEXT("Collapse event triggered: building falling")
        );
    }
}

void ACityGreyboxGenerator::OnCityEndTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (bCityEndTriggered)
    {
        return;
    }

    bCityEndTriggered = true;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.0f,
            FColor::Cyan,
            TEXT("City section complete: connect this point to docks")
        );
    }
}