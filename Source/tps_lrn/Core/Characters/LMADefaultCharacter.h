// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ULMAWeaponComponent;

DECLARE_MULTICAST_DELEGATE(OnDeathDel);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;

	//UPROPERTY()
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool hasSprint{false};

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float NewHealth);

	OnDeathDel OnDeathD;
	Sprint HasSprint;

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

	void OnDeath();
	

	void RotationPlayerOnCursor();
};
