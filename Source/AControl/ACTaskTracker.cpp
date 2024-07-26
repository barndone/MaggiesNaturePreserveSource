// Fill out your copyright notice in the Description page of Project Settings.


#include "ACTaskTracker.h"
#include "ACSokobonGrid.h"
#include "AControlGameMode.h"
#include "Components/TextBlock.h"

void UACTaskTracker::NativeConstruct()
{
	Super::NativeConstruct();
	GameMode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());

	for (auto& puzzle : GameMode->GetPuzzles())
	{
		puzzle->OnUpdateTasks.AddDynamic(this, &UACTaskTracker::UpdateText);
	}
}

void UACTaskTracker::UpdateText(AACSokobonGrid* _grid, bool _onlyUpdateProgress)
{
	//	if a grid was passed in from the event
	if (_grid)
	{
		if (!_onlyUpdateProgress)
		{
			SetTitle(_grid->PuzzleTitle);
			SetObjective(_grid->PuzzleObjective);
		}

		SetProgress(_grid->GetCurrentPuzzleCompletion(), _grid->SpawnedObjectives.Num());
	}
	//	otherwise, we have exited a grid and have to show which puzzles are left
	else
	{
		SetTitle(FText::FromString("Remaining Animals That Need Help:"));
		SetRemainingPuzzles(GameMode);
	}
}

void UACTaskTracker::SetTitle(FText _title)
{
	TaskTitle->SetText(_title);
}

void UACTaskTracker::SetObjective(FText _obj)
{
	TaskObjective->SetText(_obj);
}

void UACTaskTracker::SetProgress(const int _cur, const int _total)
{
	FString res = FString::FromInt(_cur) + " / " + FString::FromInt(_total);
	TaskProgress->SetText(FText::FromString(res));
}

void UACTaskTracker::SetRemainingPuzzles(AAControlGameMode* _gameMode)
{
	FString res;
	for (auto& pair : _gameMode->GetPuzzleStatus())
	{
		//	if the puzzle hasn't been marked as completed
		if (!pair.Value)
		{
			res += pair.Key.ToString() + "\n";
		}
	}
	TaskObjective->SetText(FText::FromString(res));
	TaskProgress->SetText(FText());
}