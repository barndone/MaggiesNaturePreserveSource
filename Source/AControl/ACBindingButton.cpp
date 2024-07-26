// Fill out your copyright notice in the Description page of Project Settings.


#include "ACBindingButton.h"
#include "Components/TextBlock.h"


UACBindingButton::UACBindingButton()
{

}

UACBindingButton::~UACBindingButton()
{
	//	new called on button binding
	//delete PairedAction;
}

void UACBindingButton::TogglePressAnyKeyPrompt()
{
	OnKeybindChangeWish.Broadcast(BindWidget);
}
