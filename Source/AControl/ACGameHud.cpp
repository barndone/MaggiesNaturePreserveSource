// Fill out your copyright notice in the Description page of Project Settings.


#include "ACGameHud.h"
#include "Blueprint/WidgetTree.h"

#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"

void AACGameHud::ToggleEndScrn()
{
	GameUIWidget->SetVisibility(ESlateVisibility::Hidden);
	EndScrnWidget->SetVisibility(ESlateVisibility::Visible);
}

void AACGameHud::BeginPlay()
{
	Super::BeginPlay();

	GameUIWidget = CreateWidget<UUserWidget>(GetWorld(), GameUIClass);
	GameUIWidget->SetOwningPlayer(GetWorld()->GetFirstPlayerController());
	GameUIWidget->AddToPlayerScreen();
	GameUIWidget->SetVisibility(ESlateVisibility::Visible);
	
	EndScrnWidget = CreateWidget<UUserWidget>(GetWorld(), EndScrnClass);
	EndScrnWidget->SetOwningPlayer(GetWorld()->GetFirstPlayerController());
	EndScrnWidget->AddToPlayerScreen();
	EndScrnWidget->SetVisibility(ESlateVisibility::Hidden);
}
