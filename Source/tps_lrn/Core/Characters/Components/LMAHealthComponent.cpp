
#include "Core/Characters/Components/LMAHealthComponent.h"

ULMAHealthComponent::ULMAHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULMAHealthComponent::IsLife() const
{
	return Health > 0.0f;
}

bool ULMAHealthComponent::AddHealth(float NewHealth)
{
	if (!IsLife() || IsHealthFull()) return false;

	Health = FMath::Clamp(Health+NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
	return true;
}

bool ULMAHealthComponent::IsHealthFull() const
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}

void ULMAHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Stamina = MaxStamina;
	OnHealthChanged.Broadcast(Health);

	AActor* OwnerComponent = GetOwner();
	if (OwnerComponent)
	{
		OwnerComponent->OnTakeAnyDamage.AddDynamic(this, &ULMAHealthComponent::OnTakeAnyDamage);
	}
}


void ULMAHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (!IsLife()) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
	
	if (!IsLife())
	{
		OnDeath.Broadcast();
	}
}

void ULMAHealthComponent::Sprinted(bool hSprint)
{
	if (hSprint)
	{
		if (GetOwner()->GetWorldTimerManager().IsTimerActive(GainStaminaTimerHandle)) { GetOwner()->GetWorldTimerManager().ClearTimer(GainStaminaTimerHandle); }
		GetOwner()->GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, &ULMAHealthComponent::DrainStamina, DrainStaminaTimerRate, true);
	}
	else
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
		GetOwner()->GetWorldTimerManager().SetTimer(GainStaminaTimerHandle, this, &ULMAHealthComponent::GainStamina, GainStaminaTimerRate, true);
	}
}

void ULMAHealthComponent::GainStamina()
{
	if (Stamina < MaxStamina)
	{
		++Stamina;
		OnStaminaChanged.Broadcast(Stamina);
	}
	else
	{
		//Stamina = MaxStamina;
		GetOwner()->GetWorldTimerManager().ClearTimer(GainStaminaTimerHandle);
	}
}

void ULMAHealthComponent::DrainStamina()
{
	if (Stamina > 0)
	{
		--Stamina;
		OnStaminaChanged.Broadcast(Stamina);
	}
	else
	{
		NoStamina.Broadcast(false, 300.0f);
	}
}
