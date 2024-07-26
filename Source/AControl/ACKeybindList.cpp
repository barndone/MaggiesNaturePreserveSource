// Fill out your copyright notice in the Description page of Project Settings.


#include "ACKeybindList.h"
#include "ACGameInstance.h"
#include "ACKeybindings.h"
#include "ACActionBindWidget.h"
#include "ACPressAnyKeyWidget.h"
#include "ACKeyConfirmationWidget.h"
#include "ACBindingButton.h"
#include "Components/ScrollBox.h"

void UACKeybindList::InitList()
{
	if (GetOwningPlayer() == GetWorld()->GetFirstPlayerController())
	{
		auto instance = Cast<UACGameInstance>(GetWorld()->GetGameInstance());
		bindingList = &instance->GetKeybindSettings()->Bindings;
		axisList = &instance->GetKeybindSettings()->AxisBindings;

		bindingList->Sort();

		if (!AnyKeyWidget)
		{
			SetupAnyKeyWidget();
		}

		for (auto& action : *bindingList)
		{
			//auto initAction = new FInputActionKeyMapping(action);
			//	check to add for KBM List
			if (!action.Key.IsGamepadKey() && KBM)
			{
				SetupKeybindWidget(&action);
			}

			//	check to add for Gamepad List
			else if (action.Key.IsGamepadKey() && !KBM)
			{
				SetupKeybindWidget(&action);
			}

			//	otherwise, do nothin gamer
		}

		for (auto& axis : *axisList)
		{
			//auto initAction = new FInputActionKeyMapping(action);
			//	check to add for KBM List
			if (!axis.Key.IsGamepadKey() && KBM)
			{
				SetupKeybindWidget(&axis);
			}

			//	check to add for Gamepad List
			else if (axis.Key.IsGamepadKey() && !KBM)
			{
				SetupKeybindWidget(&axis);
			}

			//	otherwise, do nothin gamer
		}
	}
}

void UACKeybindList::SetupAnyKeyWidget()
{
	AnyKeyWidget = CreateWidget<UACPressAnyKeyWidget>(GetWorld(), AnyKeyBP);
	AnyKeyWidget->SetOwningPlayer(GetOwningPlayer());
	AnyKeyWidget->AddToPlayerScreen();
	KeybindContainer->GetParent()->AddChild(AnyKeyWidget);
	AnyKeyWidget->ParentList = this;
	AnyKeyWidget->SetVisibility(ESlateVisibility::Hidden);
	AnyKeyWidget->CancelButton->SetVisibility(ESlateVisibility::Hidden);
	AnyKeyWidget->ConfirmButton->SetVisibility(ESlateVisibility::Hidden);
}

void UACKeybindList::SetupKeybindWidget(FInputActionKeyMapping* _action)
{
	auto widget = CreateWidget<UACActionBindWidget>(GetWorld(), KeybindWidget);

	widget->SetPairedAction(_action);
	widget->KeyButton->OnClicked.AddDynamic(widget->KeyButton, &UACBindingButton::TogglePressAnyKeyPrompt);
	widget->KeyButton->OnKeybindChangeWish.AddDynamic(AnyKeyWidget, &UACPressAnyKeyWidget::SetBindingToUpdate);
	KeybindContainer->AddChild(widget);
	BindingWidgets.AddUnique(widget);
}

void UACKeybindList::SetupKeybindWidget(FInputAxisKeyMapping* _axis)
{
	auto widget = CreateWidget<UACActionBindWidget>(GetWorld(), KeybindWidget);

	widget->SetPairedAxis(_axis);
	widget->KeyButton->OnClicked.AddDynamic(widget->KeyButton, &UACBindingButton::TogglePressAnyKeyPrompt);
	widget->KeyButton->OnKeybindChangeWish.AddDynamic(AnyKeyWidget, &UACPressAnyKeyWidget::SetBindingToUpdate);
	KeybindContainer->AddChild(widget);
	BindingWidgets.AddUnique(widget);
}

UACActionBindWidget* UACKeybindList::FindBindingByKey(FKey _newKey) const
{
	UACActionBindWidget* result = nullptr;
	for (auto& widget : BindingWidgets)
	{
		//	check if action
		if (widget->GetPairedAction() && widget->GetPairedAction()->Key == _newKey)
		{
			result = widget;
		}

		//	check if axis
		if (widget->GetPairedAxis() && widget->GetPairedAxis()->Key == _newKey)
		{
			result = widget;
		}
	}

	return result;
}

void UACKeybindList::ResetBindings()
{
	for (auto& binding : BindingWidgets)
	{
		binding->UpdatePairedAction();
	}
}

bool UACKeybindList::ValidateBindings()
{
	for (auto& binding : BindingWidgets)
	{
		auto action = binding->GetPairedAction();
		if (action)
		{
			if (!action->Key.IsValid())
			{
				binding->KeyButton->TogglePressAnyKeyPrompt();
				return false;
			}
		}
		auto axis = binding->GetPairedAxis();
		if (axis)
		{
			if (!axis->Key.IsValid())
			{
				binding->KeyButton->TogglePressAnyKeyPrompt();
				return false;
			}
		}
	}

	return true;
}
