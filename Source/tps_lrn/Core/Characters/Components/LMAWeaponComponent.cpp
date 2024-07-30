
#include "Core/Characters/Components/LMAWeaponComponent.h"
#include "Core/Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Core/Characters/Weapons/LMABaseWeapons.h"
#include "Core/Characters/LMADefaultCharacter.h"


ULMAWeaponComponent::ULMAWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULMAWeaponComponent::Fire()
{
	fired = true;
	if (Weapon && !AnimReloading && !sprinted)
	{
		if (!GetOwner()->GetWorldTimerManager().IsTimerActive(FireRateHandle))
		{
			Weapon->Fire();
			GetOwner()->GetWorldTimerManager().SetTimer(FireRateHandle, Weapon, &ALMABaseWeapons::Fire, FireRate, true);
		}
	}
}

void ULMAWeaponComponent::StopFire()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(FireRateHandle);
	fired = false;
}

void ULMAWeaponComponent::Reload()
{
	if (Weapon->GetCurrentClipAmmo() < Weapon->GetClipAmmoCount())
	{	
		(GetOwner()->GetWorldTimerManager().IsTimerActive(FireRateHandle)) ? EmptyCLip() : EmptyCLipReload();
	}
}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapons>(WeaponClass);
	if (Weapon)
	{
		PlayerChar = Cast<ALMADefaultCharacter>(GetOwner());
		if (PlayerChar)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(PlayerChar->GetMesh(), AttachmentRules, WeaponSocket);
			Weapon->EmptyClip.AddUObject(this, &ULMAWeaponComponent::EmptyCLip);
			Weapon->CurrClipCount.AddUObject(PlayerChar, &ALMADefaultCharacter::GetCurrAmmo);
		}
	}
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage)return;

	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	if (PlayerChar->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
		Weapon->ChangeClip();
		if (fired) {GetOwner()->GetWorldTimerManager().SetTimer(FireRateHandle, Weapon, &ALMABaseWeapons::Fire, FireRate, true);}
	}
}

void ULMAWeaponComponent::EmptyCLip()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(FireRateHandle);
	EmptyCLipReload();
}

inline void ULMAWeaponComponent::EmptyCLipReload()
{
	if (!CanReload()) return;
	AnimReloading = true;
	PlayerChar->PlayAnimMontage(ReloadMontage);
}

bool ULMAWeaponComponent::CanReload() const
{
	return !AnimReloading;
}

void ULMAWeaponComponent::OnDeathOwner()
{
	Weapon->SetLifeSpan(5.0f);
	AnimReloading = true;
	StopFire();
}

void ULMAWeaponComponent::isSprinted(bool hSprint)
{
	sprinted = hSprint;
	if (sprinted && fired) {StopFire();}
}

void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapon();
	InitAnimNotify();
}

const int32& ULMAWeaponComponent::GetFullClipAmmo()
{
	return Weapon->GetClipAmmoCount();
}

void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}