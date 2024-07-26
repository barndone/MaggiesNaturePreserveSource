// Fill out your copyright notice in the Description page of Project Settings.


#include "ACPressAnyKeyWidget.h"
#include "ACActionBindWidget.h"
#include "ACKeybindList.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Framework/Application/SlateApplication.h"
#include <GameFramework/InputSettings.h>
#include "AControlCharacter.h"

void UACPressAnyKeyWidget::HideSelf()
{
	//	widget focusing is controlled by FSlateApplication class---
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
	SetVisibility(ESlateVisibility::Hidden);
}

void UACPressAnyKeyWidget::SetBindingToUpdate(UACActionBindWidget* _val)
{ 
	BindingToUpdate = _val; 

	//	additionally, reset state of widget
	PromptText->SetText(FText::FromString("Press any key..."));
	CancelButton->SetVisibility(ESlateVisibility::Hidden);
	ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
	AwaitKeyPress = true;
	SetVisibility(ESlateVisibility::Visible);
	SetFocus();
}

void UACPressAnyKeyWidget::AssignNewKey(FKey _new)
{
	auto action = BindingToUpdate->GetPairedAction();

	UInputSettings* MyInputSettings = UInputSettings::GetInputSettings();

	if (action)
	{
		TArray<FInputActionKeyMapping> OutMappings;

		MyInputSettings->GetActionMappingByName(BindingToUpdate->GetPairedAction()->ActionName, OutMappings);
		MyInputSettings->RemoveActionMapping(*BindingToUpdate->GetPairedAction(), false);
		//	will just changing the key of the binding actually do anything? find out next time
		action->Key = _new;
		MyInputSettings->AddActionMapping(*BindingToUpdate->GetPairedAction(), true);
		MyInputSettings->SaveConfig();
	}

	auto axis = BindingToUpdate->GetPairedAxis();

	if (axis)
	{
		TArray<FInputAxisKeyMapping> OutMappings;

		MyInputSettings->GetAxisMappingByName(BindingToUpdate->GetPairedAxis()->AxisName, OutMappings);
		MyInputSettings->RemoveAxisMapping(*BindingToUpdate->GetPairedAxis(), false);
		//	will just changing the key of the binding actually do anything? find out next time
		axis->Key = _new;
		
		MyInputSettings->AddAxisMapping(*BindingToUpdate->GetPairedAxis(), true);
		MyInputSettings->SaveConfig();
	}

	BindingToUpdate->UpdatePairedAction();
	BindingToUpdate = nullptr;
	
	HideSelf();
}

void UACPressAnyKeyWidget::RemoveOldKey(UACActionBindWidget* _oldBinding)
{
	auto action = _oldBinding->GetPairedAction();

	UInputSettings* MyInputSettings = UInputSettings::GetInputSettings();

	if (action)
	{
		TArray<FInputActionKeyMapping> OutMappings;

		MyInputSettings->GetActionMappingByName(action->ActionName, OutMappings);
		MyInputSettings->RemoveActionMapping(*action, false);
		//	will just changing the key of the binding actually do anything? find out next time
		action->Key = EKeys::Invalid;
		MyInputSettings->AddActionMapping(*action, true);
		MyInputSettings->SaveConfig();
	}

	auto axis = _oldBinding->GetPairedAxis();

	if (axis)
	{
		TArray<FInputAxisKeyMapping> OutMappings;

		MyInputSettings->GetAxisMappingByName(axis->AxisName, OutMappings);
		MyInputSettings->RemoveAxisMapping(*axis, false);
		//	will just changing the key of the binding actually do anything? find out next time
		axis->Key = EKeys::Invalid;

		MyInputSettings->AddAxisMapping(*axis, true);
		MyInputSettings->SaveConfig();
	}

	_oldBinding->UpdatePairedAction();
}

void UACPressAnyKeyWidget::ShowConfirmBindingPrompt()
{
	//  1: change prompt text
	PromptText->SetText(FText::FromString("Would you like to apply " + NewKey.ToString() + " to the " + BindingToUpdate->GetPairedName() + "binding?"));

	//	2: show buttons
	CancelButton->SetVisibility(ESlateVisibility::Visible);
	ConfirmButton->SetVisibility(ESlateVisibility::Visible);
}

void UACPressAnyKeyWidget::ConfirmRebind()
{
	UACActionBindWidget* found = ParentList->FindBindingByKey(NewKey);
	if (!found)
	{
		AssignNewKey(NewKey);
	}
	else
	{
		OverrideBinding(found);
	}
}

void UACPressAnyKeyWidget::CancelRebind()
{
	BindingToUpdate = nullptr;
	HideSelf();
}

void UACPressAnyKeyWidget::OverrideBinding(class UACActionBindWidget* _oldBinding)
{
	RemoveOldKey(_oldBinding);
	AssignNewKey(NewKey);
}

FReply UACPressAnyKeyWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (AwaitKeyPress)
	{
		NewKey = InKeyEvent.GetKey();
		AwaitKeyPress = false;
		ShowConfirmBindingPrompt();
	}
		
	return OnKeyDown(InGeometry, InKeyEvent).NativeReply;
}

void UACPressAnyKeyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CancelButton->OnClicked.AddDynamic(this, &UACPressAnyKeyWidget::CancelRebind);
	ConfirmButton->OnClicked.AddDynamic(this, &UACPressAnyKeyWidget::ConfirmRebind);
}

void UACPressAnyKeyWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	//	if we are awaiting a key press, do NOT allow focus to change
	if (AwaitKeyPress)
	{
		SetFocus();
	}
}
