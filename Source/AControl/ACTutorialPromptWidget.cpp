// Fill out your copyright notice in the Description page of Project Settings.


#include "ACTutorialPromptWidget.h"
#include "Components/RichTextBlock.h"
#include "ACGameInstance.h"
#include "ACTutorial.h"
#include "ACGameInstance.h"
#include "ACKeybindings.h"

void UACTutorialPromptWidget::ShowNewPrompt(FText _inText)
{
	UpdateFilterAtlas();
	FilterText(_inText);
	TutorialPromptText->SetText(_inText);
	OnWidgetShow.Broadcast();
}

void UACTutorialPromptWidget::HidePrompt() 
{
	OnWidgetHide.Broadcast();
}

void UACTutorialPromptWidget::WidgetHidden()
{
	//	C++ ownership
	HandleWidgetHidden();

	//	BP ownership
	ReceiveWidgetHidden();

	//	~and the rest~
	OnHideCompleted.Broadcast();
}

void UACTutorialPromptWidget::HandleWidgetHidden()
{
	UACGameInstance* instance = Cast<UACGameInstance>(GetGameInstance());
	instance->PopTutorial();
}

void UACTutorialPromptWidget::FilterText(FText& _inText)
{
	FString out = _inText.ToString();
	for (auto& pair : InputFilterAtlas)
	{
		out = out.Replace(*pair.Key, *pair.Value.ToString());
	}

	_inText = FText::FromString(out);
}

void UACTutorialPromptWidget::UpdateFilterAtlas()
{
	UACGameInstance* instance = Cast<UACGameInstance>(GetGameInstance());
	TArray<FInputActionKeyMapping> actions = instance->GetKeybindSettings()->Bindings;
	TArray<FInputAxisKeyMapping> axes = instance->GetKeybindSettings()->AxisBindings;

	//	phase 1: actions
	for (auto& action : actions)
	{
		FString workingString = action.ActionName.ToString();
		if (action.ActionName == "Camera Swap")
		{
			workingString = "Camera";
		}
		for (auto& pair : InputFilterAtlas)
		{
			if (pair.Key.Contains(workingString))
			{
				pair.Value = action.Key.GetDisplayName();
			}
		}
	}

	int axisBtnIdx = 0;
	//	phase 2: axes
	for (auto& axis : axes)
	{
		if (axisBtnIdx < 4)
		{
			//	phase 2.5:
			if (axis.AxisName == "MoveForward")
			{
				//	step 1: positive fwd
				if (axis.Scale > 0)
				{
					if (axis.Key == EKeys::Up || axis.Key.IsGamepadKey())
					{
						continue;
					}
					InputFilterAtlas["[Up]"] = axis.Key.GetDisplayName();
					++axisBtnIdx;
				}
				//	step 2: negative fwd
				if (axis.Scale < 0)
				{
					if (axis.Key == EKeys::Down || axis.Key.IsGamepadKey())
					{
						continue;
					}
					InputFilterAtlas["[Down]"] = axis.Key.GetDisplayName();
					++axisBtnIdx;
				}
			}
			//	phase 2.6:
			if (axis.AxisName == "MoveRight")
			{
				//	step 1: positive right
				if (axis.Scale > 0)
				{
					if (axis.Key == EKeys::Right || axis.Key.IsGamepadKey())
					{
						continue;
					}
					InputFilterAtlas["[Right]"] = axis.Key.GetDisplayName();
					++axisBtnIdx;
				}
				//	step 2: negative right
				if (axis.Scale < 0)
				{
					if (axis.Key == EKeys::Left || axis.Key.IsGamepadKey())
					{
						continue;
					}
					InputFilterAtlas["[Left]"] = axis.Key.GetDisplayName();
					++axisBtnIdx;
				}
			}
		}
		else
		{
			break;
		}
	}
}