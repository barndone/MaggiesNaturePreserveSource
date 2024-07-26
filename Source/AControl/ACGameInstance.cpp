// Fill out your copyright notice in the Description page of Project Settings.


#include "ACGameInstance.h"
#include "MoviePlayer.h"
#include "ACKeybindings.h"
#include "ACTutorial.h"
#include "ACTutorialPromptWidget.h"
#include "ACTutorialTrigger.h"

#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>

void UACGameInstance::SetPuzzleAsCompleted(FName _puzzleName)
{
	if (PuzzleProgress.Contains(_puzzleName)) { PuzzleProgress[_puzzleName] = true; }
}

void UACGameInstance::SetTutorialAsCompleted(FName _tutorialName)
{
	if (TutorialProgress.Contains(_tutorialName)) { TutorialProgress[_tutorialName] = true; }
}

void UACGameInstance::InitTutorials(TArray<UACTutorial*> _inNames)
{
	for (auto& name : _inNames)
	{
		if (!TutorialProgress.Contains(name->TutorialName))
		{
			TutorialProgress.Add(name->TutorialName);
		}
	}
}

void UACGameInstance::SetHubPlayerStartIdx(const int _val)
{
	PlayerStartIdx = _val;
}

bool UACGameInstance::CheckGameComplete()
{
	bool result = true;

	for (auto& puzzle : PuzzleProgress)
	{
		if (!puzzle.Value)
		{
			result = false;
		}
	}

	return result;
}

void UACGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UACGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UACGameInstance::EndLoadingScreen);

	KeybindingSettings = Cast<UACKeybindings>(UGameplayStatics::CreateSaveGameObject(UACKeybindings::StaticClass()));
	LoadKeybinds();
	int meow = -1;
	//KeybindingSettings = Cast<UACKeybindings>(UGameplayStatics::CreateSaveGameObject(UACKeybindings::StaticClass()));

	LoadKeybinds();
}


void UACGameInstance::Shutdown()
{
	for (auto& tut : AllTutorials)
	{
		tut->bAdded = false;
	}
	SaveKeybinds();
}

void UACGameInstance::BeginLoadingScreen(const FString& _mapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UACGameInstance::EndLoadingScreen(UWorld* _inLoadedWorld)
{

}

void UACGameInstance::LoadKeybinds()
{
	auto load = UGameplayStatics::LoadGameFromSlot(KeybindingSettings->SaveSlotName, KeybindingSettings->UserIndex);
	
	if (load)
	{
		auto bindings = Cast<UACKeybindings>(load);

		if (bindings)
		{
			KeybindingSettings = bindings;
		}
		else
		{
			SaveKeybinds();
		}
	}
	else 
	{
		SaveKeybinds();
	}
}

void UACGameInstance::SaveKeybinds()
{
	auto inputSettings = UInputSettings::GetInputSettings();
	KeybindingSettings->Bindings = inputSettings->UInputSettings::GetActionMappings();
	KeybindingSettings->AxisBindings = inputSettings->UInputSettings::GetAxisMappings();

	bool result = UGameplayStatics::SaveGameToSlot(KeybindingSettings, KeybindingSettings->SaveSlotName, KeybindingSettings->UserIndex);
}

void UACGameInstance::ResetKeybinds()
{
	for (auto& binding : KeybindingSettings->Bindings)
	{
		//	if statement jank
		if (binding.ActionName == FName("Interact"))
		{
			ReinitBinding(binding, EKeys::E);

		}
		if (binding.ActionName == FName("Camera Swap"))
		{
			ReinitBinding(binding, EKeys::Tab);

		}
		if (binding.ActionName == FName("Pause"))
		{
			ReinitBinding(binding, EKeys::Escape);

		}
		if (binding.ActionName == FName("Reset"))
		{
			ReinitBinding(binding, EKeys::I);
		}
	}

	TArray<FInputAxisKeyMapping> FwdQueue;
	TArray<FInputAxisKeyMapping> RightQueue;

	for (auto& axis : KeybindingSettings->AxisBindings)
	{
		if (axis.AxisName == FName("MoveForward"))
		{
			FwdQueue.Add(axis);
		}
		if (axis.AxisName == FName("MoveRight"))
		{
			RightQueue.Add(axis);
		}
	}

	int internalPos = 0;
	int internalNeg = 0;
	int internal = 0;

	for (int i = 0; i < FwdQueue.Num(); ++i)
	{
		if (FwdQueue[i].Key.IsGamepadKey())
		{
			continue;
		}
		else
		{
			switch (internal) 
			{
			case 0:
				AssignFwdKey(internalPos, internalNeg, FwdQueue[i].Scale > 0, FwdQueue[i]);
				++internal;
				break;
			case 1:
				AssignFwdKey(internalPos, internalNeg, FwdQueue[i].Scale > 0, FwdQueue[i]);
				++internal;
				break;
			case 2:
				AssignFwdKey(internalPos, internalNeg, FwdQueue[i].Scale > 0, FwdQueue[i]);
				++internal;
				break;
			case 3:
				AssignFwdKey(internalPos, internalNeg, FwdQueue[i].Scale > 0, FwdQueue[i]);
				++internal;
				break;
			}
		}
	}

	internalPos = 0;
	internalNeg = 0;
	internal = 0;

	for (int i = 0; i < RightQueue.Num(); ++i)
	{
		if (RightQueue[i].Key.IsGamepadKey())
		{
			continue;
		}
		else
		{
			switch (internal)
			{
			case 0:
				AssignRghtKey(internalPos, internalNeg, RightQueue[i].Scale > 0, RightQueue[i]);
				++internal;
				break;
			case 1:
				AssignRghtKey(internalPos, internalNeg, RightQueue[i].Scale > 0, RightQueue[i]);
				++internal;
				break;
			case 2:
				AssignRghtKey(internalPos, internalNeg, RightQueue[i].Scale > 0, RightQueue[i]);
				++internal;
				break;
			case 3:
				AssignRghtKey(internalPos, internalNeg, RightQueue[i].Scale > 0, RightQueue[i]);
				++internal;
				break;
			}
		}
	}
}

UACKeybindings* UACGameInstance::GetKeybindSettings()
{
	if (!KeybindingSettings)
	{
		this->LoadKeybinds();
	}

	return KeybindingSettings; 
}

void UACGameInstance::TutorialCheck(class UACTutorial* _tut)
{
	//	check only exists for adding to an empty queue, otherwise will always succeed
	if (*TutorialQueue.Peek() == _tut)
	{
		//	check that the tutorial widget has been assigned
		if (TutorialWidget)
		{
			TutorialWidget->ShowNewPrompt(_tut->TutorialText);
			AdditionalTutorialLogicBegin.Broadcast(_tut->ActorArrowTag);
		}
	}
}

void UACGameInstance::AddTutorial(class UACTutorial* _tut)
{
	//	bAdded ensures no double adds if it hasn't been completed yet
	if (!*TutorialProgress.Find(_tut->TutorialName) && !_tut->bAdded)
	{
		_tut->bAdded = true;
		TutorialQueue.Enqueue(_tut);
		TutorialCheck(_tut);
	}
}

void UACGameInstance::PopTutorial()
{
	if (!TutorialQueue.IsEmpty())
	{
		//	mark the tutorial as seen
		auto& _toRem = *TutorialQueue.Peek();
		TutorialProgress[_toRem->TutorialName] = true;
		AdditionalTutorialLogicEnd.Broadcast(_toRem->ActorArrowTag);
		//	remove latest
		TutorialQueue.Pop();

		//	if we are not empty--
		if (!TutorialQueue.IsEmpty())
		{
			auto& _tut = *TutorialQueue.Peek();
			//	show the prompty
			TutorialCheck(_tut);
		}
	}
}

void UACGameInstance::AddTutorialToProgress(class UACTutorial* _tut)
{
	TutorialProgress.Add(_tut->TutorialName);
	AllTutorials.AddUnique(_tut);
}

void UACGameInstance::AddTutorialToProgressByTrigger(AACTutorialTrigger* _in)
{
	for (auto& tut : _in->GetTutorials())
	{
		AddTutorialToProgress(tut);
	}
}

void UACGameInstance::AssignFwdKey(int& posIdx, int& negIdx, bool pos, FInputAxisKeyMapping& key)
{
	if (pos)
	{
		switch (posIdx)
		{
		case 0:
			ReinitAxis(key, EKeys::W);
			posIdx++;
			break;
		case 1:
			ReinitAxis(key, EKeys::W);
			posIdx++;
			break;
		}
	}
	else
	{
		switch (negIdx)
		{
		case 0:
			ReinitAxis(key, EKeys::S);
			negIdx++;
			break;
		case 1:
			ReinitAxis(key, EKeys::S);
			negIdx++;
			break;
		}
	}
}

void UACGameInstance::AssignRghtKey(int& posIdx, int& negIdx, bool pos, FInputAxisKeyMapping& key)
{
	if (pos)
	{
		switch (posIdx)
		{
		case 0:
			ReinitAxis(key, EKeys::D);
			posIdx++;
			break;
		case 1:
			ReinitAxis(key, EKeys::D);
			posIdx++;
			break;
		}
	}
	else
	{
		switch (negIdx)
		{
		case 0:
			ReinitAxis(key, EKeys::A);
			negIdx++;
			break;
		case 1:
			ReinitAxis(key, EKeys::A);
			negIdx++;
			break;
		}
	}
}

void UACGameInstance::ReinitBinding(FInputActionKeyMapping& _inAction, FKey _key)
{
	UInputSettings* MyInputSettings = UInputSettings::GetInputSettings();

	//	clear old key
	MyInputSettings->RemoveActionMapping(_inAction, false);

	//	apply new key
	_inAction.Key = _key;
	MyInputSettings->AddActionMapping(_inAction, true);
	MyInputSettings->SaveConfig();
}

void UACGameInstance::ReinitAxis(FInputAxisKeyMapping& _inAxis, FKey _key)
{
	UInputSettings* MyInputSettings = UInputSettings::GetInputSettings();

	//	clear old key
	MyInputSettings->RemoveAxisMapping(_inAxis, false);

	//	apply new key
	_inAxis.Key = _key;
	MyInputSettings->AddAxisMapping(_inAxis, true);
	MyInputSettings->SaveConfig();
}

void UACGameInstance::ResetProgress()
{
	if (PuzzleProgress.Num() > 0)
	{
		for (auto& pair : PuzzleProgress)
		{
			pair.Value = false;
		}
	}

	if (TutorialProgress.Num() > 0)
	{
		for (auto& pair : TutorialProgress)
		{
			pair.Value = false;
		}
	}

	PlayerStartTag = "tutorial";

	for (auto& val : AllTutorials)
	{
		val->bAdded = false;
	}
}