// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"



// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay Owner:%s Ptr:%p Cur:%.1f"),
	*GetOwner()->GetName(), this, CurrentHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);   // 초기값 알림
	// ...
	OnExpChanged.Broadcast(CurrentExp, GetRequiredExp(), Level);  
}

void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	CurrentHealth = MaxHealth;
}

void UStatComponent::ApplyDamage(float DamageAmount, AActor* Attacker)
{
	if (IsDead()) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDamaged.Broadcast(Attacker);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

bool UStatComponent::IsDead() const
{
	return CurrentHealth <= 0.f;
}

int32 UStatComponent::GetRequiredExp() const
{
	return 100 + (Level - 1) * 50;
}

void UStatComponent::AddExp(int32 Amount)
{
	if (Amount <= 0 || IsDead()) return;

	CurrentExp += Amount;

	while (CurrentExp >= GetRequiredExp())
	{
		CurrentExp -= GetRequiredExp();
		LevelUp();
	}

	OnExpChanged.Broadcast(CurrentExp, GetRequiredExp(), Level);
}

void UStatComponent::LevelUp()
{
	Level++;

	MaxHealth += 20.f;
	AttackPower += 3.f;
	CurrentHealth = MaxHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnLevelUp.Broadcast(Level);

	UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! Lv.%d HP:%.0f ATK:%.0f"),
		Level, MaxHealth, AttackPower);
}

void UStatComponent::ResetStat()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

