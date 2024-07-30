// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(SoTired, bool, float);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_LRN_API ULMAHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULMAHealthComponent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	const float& GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintCallable)
	bool IsLife() const;

	bool AddHealth(float NewHealth);
	bool IsHealthFull() const;
	
	UPROPERTY()
	float GainStaminaTimerRate{ 0.7f };

	UPROPERTY()
	float DrainStaminaTimerRate{ 0.1f };

	FTimerHandle GainStaminaTimerHandle;
	FTimerHandle DrainStaminaTimerHandle;

	void Sprinted(bool hSprint);

	FOnDeath OnDeath;
	FOnHealthChanged OnHealthChanged;
	FOnStaminaChanged OnStaminaChanged;
	SoTired NoStamina;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth{ 100.0f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxStamina{ 100.0f };

	virtual void BeginPlay() override;

private:
	float Health{ 0.0f };
	float Stamina{ 0.0f };
	
	void GainStamina();
	void DrainStamina();

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
