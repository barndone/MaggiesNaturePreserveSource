// Fill out your copyright notice in the Description page of Project Settings.


#include "ACInputPrompt.h"
#include "Components/Image.h"
#include "ACKeybindings.h"
#include "ACGameInstance.h"
#include "ACSokobonPiece.h"
#include "AControlGameMode.h"
#include "AControlCharacter.h"


void UACInputPrompt::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//	get instance
	auto instance = Cast<UACGameInstance>(GetWorld()->GetGameInstance());
	//	get keybinds
	auto& bindingList = instance->GetKeybindSettings()->Bindings;

	//	assign interact pointer to sokointeract interact
	for (auto& binding : bindingList)
	{
		if (binding.ActionName == FName("SokobonInteract"))
		{
			Interact = &binding;
			break;
		}
	}
	auto gm = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
	gm->UpdateInputPrompts.AddDynamic(this, &UACInputPrompt::UpdateIcon);
	//	force call the event to update icon from default to current binding

	//	TODO: bind to character
	UpdateIcon();
}

void UACInputPrompt::NativePreConstruct()
{
	if (KeyIconMap.Num() == 0)
	{
		TArray<FKey> Out;

		EKeys::GetAllKeys(Out);

		for (auto& key : Out)
		{
			KeyIconMap.Add(key);
		}
	}
}

void UACInputPrompt::BindCharacterEvents()
{
	auto gm = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
	auto character = Cast<AAControlCharacter>(gm->GetCharRefBP());

	if (character)
	{
		character->OnPieceBeginOverlap.AddDynamic(this, &UACInputPrompt::UpdateAxis);
		character->OnPieceEndOverlap.AddDynamic(this, &UACInputPrompt::HideAxis);
	}
	else
	{
		gm->OnCharRefInit.AddDynamic(this, &UACInputPrompt::BindCharacterEvents);
	}

}

void UACInputPrompt::UpdateIcon()
{

	//	get instance
	auto instance = Cast<UACGameInstance>(GetWorld()->GetGameInstance());

	//	get keybinds
	auto& bindingList = instance->GetKeybindSettings()->Bindings;

	//	assign interact pointer to sokointeract interact
	for (auto& binding : bindingList)
	{
		if (binding.ActionName == FName("Interact"))
		{
			Interact = &binding;
			break;
		}
	}

	if (Interact->Key != EKeys::Invalid)
	{
		InputIcon->SetBrushFromTexture(*KeyIconMap.Find(Interact->Key));
	}
}

void UACInputPrompt::UpdateAxis(bool _NS, AACSokobonPiece* _in)
{
	if (_in == parent && _NS)
	{
		InputIcon->SetBrushFromTexture(NSIcon);
	}
	else if (_in == parent && !_NS)
	{
		InputIcon->SetBrushFromTexture(EWIcon);
	}
}

void UACInputPrompt::HideAxis(AACSokobonPiece* _in)
{
	if (_in == parent)
	{
		InputIcon->SetBrushFromTexture(*KeyIconMap.Find(Interact->Key));
	}
}
