// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/SaveGame.h"
#include "ACKeybindings.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACKeybindings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FInputActionKeyMapping> Bindings;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		TArray<FInputAxisKeyMapping> AxisBindings;

	UPROPERTY(VisibleAnywhere, Category = Basic)
		FString SaveSlotName = "Keybindings";

	UPROPERTY(VisibleAnywhere, Category = Basic)
		uint32 UserIndex = 0;

};
