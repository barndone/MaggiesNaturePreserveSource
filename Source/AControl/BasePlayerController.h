// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API ABasePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	FGenericTeamId TeamId = FGenericTeamId(1);
	virtual FGenericTeamId GetGenericTeamId() const override;

	//	class UACTutorialPromptWidget* GetTutorialPrompt() const;
};
