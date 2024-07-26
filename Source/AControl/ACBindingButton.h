// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "GameFramework/PlayerInput.h"
#include "ACBindingButton.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputActionUpdateWish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeybindChangeWish, class UACActionBindWidget*, _binding);

UCLASS()
class ACONTROL_API UACBindingButton : public UButton
{
	GENERATED_BODY()

	FKey* KeyPair;

public:
	UPROPERTY(BlueprintAssignable)
	FOnInputActionUpdateWish OnInputActionUpdateWish;

	UPROPERTY(BlueprintAssignable)
		FOnKeybindChangeWish OnKeybindChangeWish;

	//	idk slap a uproperty on that shit to see if it doesn't get garbage collected?
	UPROPERTY()
	class UACActionBindWidget* BindWidget;
	
	FInputActionKeyMapping* PairedAction;
	FInputAxisKeyMapping* PairedAxis;

	class UTextBlock* KeyText = nullptr;

	UACBindingButton();
	~UACBindingButton();

	void SetKeyPair(FKey* _val) { KeyPair = _val; }
	FORCEINLINE FKey* GetKeyPair() const { return KeyPair; }

	UFUNCTION()
	void TogglePressAnyKeyPrompt();
};
