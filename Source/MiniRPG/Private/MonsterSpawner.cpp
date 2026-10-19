#include "MonsterSpawner.h"
#include "MonsterBase.h"
#include "NavigationSystem.h"
#include "Components/BillboardComponent.h"
AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

#if WITH_EDITORONLY_DATA
    UBillboardComponent* Billboard =
        CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
    Billboard->SetupAttachment(RootComponent);
#endif
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    InitializePool();

    GetWorldTimerManager().SetTimer(
        SpawnTimer, this, &AMonsterSpawner::TrySpawn, SpawnInterval, true, 1.f);
}

void AMonsterSpawner::InitializePool()
{
    if (!MonsterClass) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < PoolSize; ++i)
    {
        AMonsterBase* Monster = World->SpawnActor<AMonsterBase>(
            MonsterClass, GetActorLocation(), FRotator::ZeroRotator, Params);

        if (Monster)
        {
            Monster->DeactivateMonster();
            MonsterPool.Add(Monster);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Pool initialized: %d"), MonsterPool.Num());
}

AMonsterBase* AMonsterSpawner::GetInactiveMonster()
{
    for (AMonsterBase* Monster : MonsterPool)
    {
        if (Monster && !Monster->IsActiveMonster())
        {
            return Monster;
        }
    }
    return nullptr;
}

FVector AMonsterSpawner::GetRandomSpawnLocation() const
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return GetActorLocation();

    FNavLocation Result;
    if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), SpawnRadius, Result))
    {
        UE_LOG(LogTemp, Warning, TEXT("NavOK: %s"), *Result.Location.ToString());
        return Result.Location + FVector(0.f, 0.f, 90.f);
    }
    UE_LOG(LogTemp, Warning, TEXT("Nav FAILED - Spawner at %s"), *GetActorLocation().ToString());
    return GetActorLocation();
}

void AMonsterSpawner::TrySpawn()
{
    int32 AliveCount = 0;
    for (AMonsterBase* Monster : MonsterPool)
    {
        if (Monster && Monster->IsActiveMonster()) ++AliveCount;
    }
    UE_LOG(LogTemp, Warning, TEXT("TrySpawn - Alive:%d / Max:%d"), AliveCount, MaxAliveCount);

    if (AliveCount >= MaxAliveCount) return;

    AMonsterBase* Monster = GetInactiveMonster();
    if (!Monster) return;

    Monster->ActivateMonster(GetRandomSpawnLocation());
}