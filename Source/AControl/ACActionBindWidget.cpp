// Fill out your copyright notice in the Description page of Project Settings.


#include "ACActionBindWidget.h"
#include "Components/TextBlock.h"
#include "ACBindingButton.h"
#include "InputCoreTypes.h"
#include <GameFramework/InputSettings.h>

void UACActionBindWidget::SetPairedAction(FInputActionKeyMapping* _val)
{
	PairedAxis = nullptr;
	PairedAction = _val;

	//	update text fields
	//	populate ActionName text with the FName of the action
	ActionName->Text = FText::FromName(PairedAction->ActionName);

	if (KeyButton->KeyText)
	{
		KeyButton->KeyText->SetText(FText::FromName(PairedAction->Key.GetFName()));
	}

	else
	{
		KeyButton->KeyText = NewObject<UTextBlock>(GetWorld(), UTextBlock::StaticClass());
		KeyButton->KeyText->bIsVariable = true;
		KeyButton->KeyText->SetText(FText::FromName(PairedAction->Key.GetFName()));
		KeyButton->AddChild(KeyButton->KeyText);
		KeyButton->BindWidget = this;
	}
	KeyButton->PairedAction = _val;
	KeyButton->OnInputActionUpdateWish.AddDynamic(this, &UACActionBindWidget::UpdatePairedAction);
}

void UACActionBindWidget::SetPairedAxis(FInputAxisKeyMapping* _val)
{
	PairedAction = nullptr;
	PairedAxis = _val;
	//	update text fields
	//	populate ActionName text with the FName of the action

	FText res = GetAxisMapName();

	ActionName->SetText(res);

	if (KeyButton->KeyText)
	{
		KeyButton->KeyText->SetText(FText::FromName(PairedAxis->Key.GetFName()));
	}

	else
	{
		KeyButton->KeyText = NewObject<UTextBlock>(GetWorld(), UTextBlock::StaticClass());
		KeyButton->KeyText->bIsVariable = true;
		KeyButton->KeyText->SetText(FText::FromName(PairedAxis->Key.GetFName()));
		KeyButton->AddChild(KeyButton->KeyText);
		KeyButton->BindWidget = this;
	}
	KeyButton->PairedAxis = _val;
	KeyButton->OnInputActionUpdateWish.AddDynamic(this, &UACActionBindWidget::UpdatePairedAction);
}

void UACActionBindWidget::UpdatePairedAction()
{
	if (PairedAction)
	{
		if (KeyButton->KeyText)
		{
			FText res = PairedAction->Key == EKeys::Invalid ? FText::FromString("NONE") : FText::FromName(PairedAction->Key.GetFName());
			KeyButton->KeyText->SetText(res);
		}

		else
		{
			KeyButton->KeyText = NewObject<UTextBlock>(GetWorld(), UTextBlock::StaticClass());
			KeyButton->KeyText->bIsVariable = true;
			FText res = PairedAction->Key == EKeys::Invalid ? FText::FromString("NONE") : FText::FromName(PairedAction->Key.GetFName());
			KeyButton->KeyText->SetText(res);
			KeyButton->AddChild(KeyButton->KeyText);
			KeyButton->BindWidget = this;
		}

	}
	else if (PairedAxis)
	{
		if (KeyButton->KeyText)
		{
			FText res = PairedAxis->Key == EKeys::Invalid ? FText::FromString("NONE") : FText::FromName(PairedAxis->Key.GetFName());
			KeyButton->KeyText->SetText(res);
		}

		else
		{
			KeyButton->KeyText = NewObject<UTextBlock>(GetWorld(), UTextBlock::StaticClass());
			KeyButton->KeyText->bIsVariable = true;
			FText res = PairedAxis->Key == EKeys::Invalid ? FText::FromString("NONE") : FText::FromName(PairedAxis->Key.GetFName());
			KeyButton->KeyText->SetText(res);
			KeyButton->AddChild(KeyButton->KeyText);
			KeyButton->BindWidget = this;
		}
	}
}

FText UACActionBindWidget::GetAxisMapName() const
{
	FText res = FText::FromString("");

	if (PairedAxis->AxisName == FName("MoveForward"))
	{
		PairedAxis->Scale > 0 ? res = FText::FromString("Forward") : res = FText::FromString("Back");
	}
	else if (PairedAxis->AxisName == FName("MoveRight"))
	{
		PairedAxis->Scale > 0 ? res = FText::FromString("Right") : res = FText::FromString("Left");
	}

	return res;
}

FString UACActionBindWidget::GetPairedName() const 
{
	if (PairedAction)
	{
		return PairedAction->ActionName.ToString();
	}
	else
	{
		return PairedAxis->AxisName.ToString();
	}
}

