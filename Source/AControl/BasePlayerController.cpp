// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "ACGameHud.h"

FGenericTeamId ABasePlayerController::GetGenericTeamId() const
{
	return TeamId;
}

//	UACTutorialPromptWidget* ABasePlayerController::GetTutorialPrompt() const
//	{
//		AACGameHud* hud = Cast<AACGameHud>(GetHUD());
//		UACTutorialPromptWidget* widget = Cast<UACTutorialPromptWidget>(hud->GetTutorialPrompt());
//	
//		int meow = 1;
//		return widget;
//	}
