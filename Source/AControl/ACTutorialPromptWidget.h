// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACTutorialPromptWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetHide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideCompleted);

UCLASS()
class ACONTROL_API UACTutorialPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class URichTextBlock* TutorialPromptText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FText> InputFilterAtlas;

	//	pls dont sue me aggro crab :<
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* AcknowledgeKnowledgeButton;

	UFUNCTION(BlueprintCallable)
	void ShowNewPrompt(FText _inText);
	UFUNCTION(BlueprintCallable)
	void HidePrompt();

	UPROPERTY(BlueprintAssignable)
	FOnWidgetShow OnWidgetShow;
	UPROPERTY(BlueprintAssignable)
	FOnWidgetHide OnWidgetHide;

	UPROPERTY(BlueprintAssignable)
	FOnHideCompleted OnHideCompleted;

	UFUNCTION(BlueprintCallable)
	void WidgetHidden();

	virtual void HandleWidgetHidden();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveWidgetHidden();

	void FilterText(FText& _inText);

	void UpdateFilterAtlas();
};
