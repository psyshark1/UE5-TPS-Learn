// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/LMAPlayerController.h"

void ALMAPlayerController::BeginSpectatingState()
{
	SetControlRotation(FRotator(-75.0f, 0.0f, 0.0f));
	Super::BeginSpectatingState();
}
