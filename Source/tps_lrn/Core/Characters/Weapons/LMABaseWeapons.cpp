#include "Core/Characters/Weapons/LMABaseWeapons.h"

ALMABaseWeapons::ALMABaseWeapons()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);
}

void ALMABaseWeapons::Fire()
{
	Shoot();
}

void ALMABaseWeapons::ChangeClip()
{
	CurrentAmmoWeapon.Bullets = AmmoWeapon.Bullets;
	CurrClipCount.Broadcast(GetCurrentClipAmmo());
}

inline const int32& ALMABaseWeapons::GetClipAmmoCount()
{
	return AmmoWeapon.Bullets;
}

inline const int32& ALMABaseWeapons::GetCurrentClipAmmo()
{
	return CurrentAmmoWeapon.Bullets;
}

void ALMABaseWeapons::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoWeapon = AmmoWeapon;
}

void ALMABaseWeapons::Shoot()
{
	const FTransform SocketTransform = WeaponComponent->GetSocketTransform("Muzzle");
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	if (HitResult.bBlockingHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.0f, 24, FColor::Red, false, 1.0f);
	}

	DecrementBullets();
}

void ALMABaseWeapons::DecrementBullets()
{
	--CurrentAmmoWeapon.Bullets;
	CurrClipCount.Broadcast(GetCurrentClipAmmo());
	if (IsCurrentClipEmpty())
	{
		EmptyClip.Broadcast();
	}
}

inline bool ALMABaseWeapons::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

void ALMABaseWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}