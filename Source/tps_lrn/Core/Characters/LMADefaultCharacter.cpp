
#include "Core/Characters/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Core/Controllers/LMAPlayerController.h"

ALMADefaultCharacter::ALMADefaultCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(CursorMaterial))
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::HideHUD);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
	HealthComponent->OnStaminaChanged.AddUObject(this, &ALMADefaultCharacter::OnStaminaChanged);
	HealthComponent->NoStamina.AddUObject(this, &ALMADefaultCharacter::SetTired);
	
	OnDeathD.AddUObject(WeaponComponent, &ULMAWeaponComponent::OnDeathOwner);
	HasSprint.AddUObject(HealthComponent, &ULMAHealthComponent::Sprinted);
	HasSprint.AddUObject(WeaponComponent, &ULMAWeaponComponent::isSprinted);

	DPlayTimer.AddUObject(this, &ALMADefaultCharacter::PlayTimer);
	GetWorldTimerManager().SetTimer(PlayTimerHandle, this, &ALMADefaultCharacter::ProcPlayTimer, PlayTimeTimerRate, true);
	TimeOutD.AddUObject(this, &ALMADefaultCharacter::TimeOut);

	PlayerCont = Cast<ALMAPlayerController>(GetController());

	OnDeathDelWidget.AddUObject(PlayerCont, &ALMAPlayerController::SetDeadWidget);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::CamZoom(float Value)
{
	ArmLength += Value;
	(ArmLength > 1400.0f) ? ArmLength = 1400.0f : 
		(ArmLength < 400.0f) ? ArmLength = 400.0f : true;
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::Sprint()
{
	if (CharacterMovement1->Velocity != FVector(0.0f, 0.0f, 0.0f))
	{
		if (!CharacterMovement1->IsFalling())
		{
			if (HealthComponent->GetStamina() > 0)
			{
				hasSprint = true;
				CharacterMovement1->MaxWalkSpeed = 600.0f;
				HasSprint.Broadcast(hasSprint);
			}
		}
	}
}

void ALMADefaultCharacter::StopSprint()
{
	SetTired(false, 300.0f);
	HasSprint.Broadcast(hasSprint);
}

void ALMADefaultCharacter::SetTired(bool hSprint, float tiredSpeed)
{
	hasSprint = hSprint;
	CharacterMovement1->MaxWalkSpeed = tiredSpeed;
}

void ALMADefaultCharacter::ProcPlayTimer()
{
	if (PlayTime > 0)
	{
		--PlayTime;
		DPlayTimer.Broadcast(GetPlayTime());
	}
	else
	{
		GetWorldTimerManager().ClearTimer(PlayTimerHandle);
		TimeOutD.Broadcast();
	}
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	OnDeathD.Broadcast();

	GetWorldTimerManager().ClearTimer(PlayTimerHandle);

	OnDeathDelWidget.Broadcast();

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

/*void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}*/

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
}

void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HealthComponent->IsLife())
	{
		if (GetWorld())
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (IsValid(PC))
			{
				FHitResult ResultHit;
				PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
				float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
				SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
				if (CurrentCursor)
				{
					CurrentCursor->SetWorldLocation(ResultHit.Location);
				}
			}
		}
	}
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("CameraZoom", this, &ALMADefaultCharacter::CamZoom);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ALMADefaultCharacter::SetPause);
}