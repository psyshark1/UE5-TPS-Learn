// LeaveMeAlone Game by Netologiya. All Rights Reserved.


#include "Core/Characters/LevelActors/LMAHealthPickup.h"
#include "Core/Characters/Components/LMAHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Core/Characters/LMADefaultCharacter.h"

ALMAHealthPickup::ALMAHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(SphereComponent);

}

void ALMAHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALMAHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	const auto Charcter = Cast<ALMADefaultCharacter>(OtherActor);
	if (GivePickup(Charcter))
	{
		PickupWasTaken();
	}
}

void ALMAHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALMAHealthPickup::GivePickup(ALMADefaultCharacter* Character)
{
	if (Character != nullptr)
	{
		const auto HealthComponent = Character->GetHealthComponent();
		if (!HealthComponent) return false;

		return HealthComponent->AddHealth(HealthFromPickup);
	}
	return false;
}

void ALMAHealthPickup::PickupWasTaken()
{
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetRootComponent()->SetVisibility(false,true);

	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALMAHealthPickup::RespawnPickup, RespawnTime);
}

void ALMAHealthPickup::RespawnPickup()
{
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GetRootComponent()->SetVisibility(true,true);
}
