// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACPressAnyKeyWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewKey, FKey, _key);

UCLASS()
class ACONTROL_API UACPressAnyKeyWidget : public UUserWidget
{
	GENERATED_BODY()

	class UACActionBindWidget* BindingToUpdate = nullptr;
	FKey NewKey;
	bool AwaitKeyPress = false;
	void HideSelf();

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* PromptText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* CancelButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ConfirmButton;

	UFUNCTION()
	void SetBindingToUpdate(class UACActionBindWidget* _val);
	void AssignNewKey(FKey _new);
	void RemoveOldKey(class UACActionBindWidget* _oldBinding);

	void ShowConfirmBindingPrompt();

	UFUNCTION()
	void ConfirmRebind();
	UFUNCTION()
	void CancelRebind();

	UPROPERTY(BlueprintAssignable)
	FOnNewKey OnNewKey;

	void OverrideBinding(class UACActionBindWidget* _oldBinding);

	class UACKeybindList* ParentList = nullptr;

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
};
