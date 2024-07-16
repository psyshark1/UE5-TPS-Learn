// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapons;
class ALMADefaultCharacter;
class UAnimMontage;
class ULMAHealthComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_LRN_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULMAWeaponComponent();

	void Fire();
	void StopFire();
	void Reload();
	void OnDeathOwner();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALMABaseWeapons> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate{ 0.1f };

private:	
	UPROPERTY()
	ALMABaseWeapons* Weapon = nullptr;

	FTimerHandle FireRateHandle;

	bool AnimReloading = false;
	bool fired = false;;
	void SpawnWeapon();
	void InitAnimNotify();

	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
	void EmptyCLip();
	void EmptyCLipReload();
	bool CanReload() const;
	
};
