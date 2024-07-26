// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "Blueprint/UserWidget.h"
#include "ACInputPrompt.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACInputPrompt : public UUserWidget
{
	GENERATED_BODY()

	FInputActionKeyMapping* Interact;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		TMap<FKey, class UTexture2D*> KeyIconMap;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	class UTexture2D* NSIcon;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	class UTexture2D* EWIcon;

	class AACSokobonPiece* parent;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct();

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* InputIcon;

	UFUNCTION(BlueprintCallable)
	void BindCharacterEvents();

	UFUNCTION()
	void UpdateIcon();
	UFUNCTION()
	void UpdateAxis(bool _NS, class AACSokobonPiece* _in);
	UFUNCTION()
	void HideAxis(class AACSokobonPiece* _in);

	UFUNCTION(BlueprintCallable)
	void SetParent(class AACSokobonPiece* _parent) { parent = _parent; }
};
