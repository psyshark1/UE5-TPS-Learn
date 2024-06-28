
#include "Core/Characters/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(CursorMaterial))
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
	Stamina = MaxStamina;
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
	if (CharacterMovement->Velocity != FVector(0.0f, 0.0f, 0.0f))
	{
		if (!CharacterMovement->IsFalling())
		{
			if (Stamina > 0)
			{
				hasSprint = true;
				CharacterMovement->MaxWalkSpeed = 600.0f;
				if (GetWorldTimerManager().IsTimerActive(GainStaminaTimerHandle)) { GetWorldTimerManager().ClearTimer(GainStaminaTimerHandle); }
				GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, &ALMADefaultCharacter::DrainStamina, DrainStaminaTimerRate, true);
			}
		}
	}
}

void ALMADefaultCharacter::StopSprint()
{
	hasSprint = false;
	CharacterMovement->MaxWalkSpeed = 300.0f;
	GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
	GetWorldTimerManager().SetTimer(GainStaminaTimerHandle, this, &ALMADefaultCharacter::GainStamina, GainStaminaTimerRate, true);
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
}

void ALMADefaultCharacter::GainStamina()
{
	if (Stamina < MaxStamina)
	{
		++Stamina;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(GainStaminaTimerHandle);
	}
}

void ALMADefaultCharacter::DrainStamina()
{
	if (Stamina > 0)
	{
		--Stamina;
	}
	else
	{
		hasSprint = false;
		CharacterMovement->MaxWalkSpeed = 300.0f;
	}
	
}

// Called every frame
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
}

