
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMABaseWeapons.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FAmmoWeapon
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Bullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Clips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool Infinite;
};

DECLARE_MULTICAST_DELEGATE(EmptyClip);
DECLARE_MULTICAST_DELEGATE_OneParam(FCurrClipcount, int32);

UCLASS()
class TPS_LRN_API ALMABaseWeapons : public AActor
{
	GENERATED_BODY()
	
public:	
	ALMABaseWeapons();
	void Fire();
	void ChangeClip();
	const int32& GetClipAmmoCount();
	const int32& GetCurrentClipAmmo();
	FCurrClipcount CurrClipCount;
	EmptyClip EmptyClip;

	virtual void Tick(float DeltaTime) override;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FAmmoWeapon AmmoWeapon{ 30, 0, true };

	void Shoot();

	void DecrementBullets();
	bool IsCurrentClipEmpty() const;

	virtual void BeginPlay() override;

private:	
	FAmmoWeapon CurrentAmmoWeapon;

};
