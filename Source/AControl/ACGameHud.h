// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ACGameHud.generated.h"

UCLASS()
class ACONTROL_API AACGameHud : public AHUD
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget > GameUIClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget > EndScrnClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UUserWidget* GameUIWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UUserWidget* EndScrnWidget;

public:
	void ToggleEndScrn();
//	UUserWidget* GetTutorialPrompt() const;
protected:
	virtual void BeginPlay() override;
	
};
