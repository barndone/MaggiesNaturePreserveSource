// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACKeybindList.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACKeybindList : public UUserWidget
{
	GENERATED_BODY()

		
	TArray<FInputActionKeyMapping>* bindingList = nullptr;
	TArray<FInputAxisKeyMapping>* axisList = nullptr;
	TArray<class UACActionBindWidget*> BindingWidgets;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UACActionBindWidget> KeybindWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UACPressAnyKeyWidget> AnyKeyBP;


	class UACPressAnyKeyWidget* AnyKeyWidget = nullptr;



	UPROPERTY(BlueprintReadWrite)
	bool KBM = true;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* KeybindContainer;

	UFUNCTION(BlueprintCallable)
	void InitList();

	UFUNCTION(BlueprintCallable)
	void SetupAnyKeyWidget();


	void SetupKeybindWidget(struct FInputActionKeyMapping* _action);
	void SetupKeybindWidget(struct FInputAxisKeyMapping* _axis);

	class UACActionBindWidget* FindBindingByKey(FKey _newKey) const;

	UFUNCTION(BlueprintCallable)
	void ResetBindings();

	UFUNCTION(BlueprintCallable)
	bool ValidateBindings();
};
