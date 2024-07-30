// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ULMAWeaponComponent;
class ALMAPlayerController;

DECLARE_MULTICAST_DELEGATE(OnDeathDlg);
DECLARE_MULTICAST_DELEGATE(TimeOutDlg);
DECLARE_MULTICAST_DELEGATE(OnDeathDelWidget);
DECLARE_MULTICAST_DELEGATE_OneParam(DPlayTimer, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(Sprint, bool);

UCLASS()
class TPS_LRN_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PlayTime{600};

	UPROPERTY()
	float PlayTimeTimerRate{ 1.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement1 = GetCharacterMovement();

	UPROPERTY()
	ALMAPlayerController* PlayerCont = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool hasSprint{false};

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaChanged(float NewStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void GetCurrAmmo(int32 ammo);

	UFUNCTION(BlueprintImplementableEvent)
	void TimeOut();

	OnDeathDlg OnDeathD;
	Sprint HasSprint;
	DPlayTimer DPlayTimer;
	OnDeathDelWidget OnDeathDelWidget;
	TimeOutDlg TimeOutD;

	UFUNCTION(BlueprintImplementableEvent)
	void HideHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void SetPause();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayTimer(int32 PTime);

	UFUNCTION(BlueprintCallable)
	const int32& GetPlayTime() const { return PlayTime; }

	FTimerHandle PlayTimerHandle;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

private:	

	float YRotation = -75.0f;
	float ArmLength = 1400.0f;
	float FOV = 55.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CamZoom(float Value);
	void Sprint();
	void StopSprint();
	void SetTired(bool hSprint, float tiredSpeed);
	void ProcPlayTimer();

	void OnDeath();
	void RotationPlayerOnCursor();
};
