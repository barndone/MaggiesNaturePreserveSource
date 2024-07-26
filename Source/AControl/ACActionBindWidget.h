// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerInput.h"
#include "ACActionBindWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACActionBindWidget : public UUserWidget
{
	GENERATED_BODY()
	FInputActionKeyMapping* PairedAction;
	FInputAxisKeyMapping* PairedAxis;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ActionName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UACBindingButton* KeyButton;

	void SetPairedAction(FInputActionKeyMapping* _val);
	FORCEINLINE FInputActionKeyMapping* GetPairedAction() const { return PairedAction; }

	void SetPairedAxis(FInputAxisKeyMapping* _val);
	FORCEINLINE FInputAxisKeyMapping* GetPairedAxis() const { return PairedAxis; }

	UFUNCTION()
	void UpdatePairedAction();

	FText GetAxisMapName() const;

	FString GetPairedName() const;
};