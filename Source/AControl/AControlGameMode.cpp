// Copyright Epic Games, Inc. All Rights Reserved.

#include "AControlGameMode.h"
#include "BaseAIAgent.h"
#include "AControlCharacter.h"
#include "ACSokobonGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "ACGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "ACGameHud.h"

AAControlGameMode::AAControlGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MBP/Core/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AAControlGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!instance)
	{
		instance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	TArray<AActor*> found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AACSokobonGrid::StaticClass(), found);

	if (instance->PuzzleProgress.Num() == 0)
	{
		instance->PuzzleProgress = PuzzleStatus;
	}
	else
	{
		PuzzleStatus = instance->PuzzleProgress;
	}

	if (found.Num() != 0)
	{
		for (auto& val : found)
		{
			AACSokobonGrid* puzzle = Cast<AACSokobonGrid>(val);
			Puzzles.Add(puzzle);

			//	if the puzzle has been marked as completed from the game instance
			//	don't forget to deref since it returns a pointer
			if (PuzzleStatus.Contains(puzzle->PuzzleName) && *PuzzleStatus.Find(puzzle->PuzzleName))
			{
				puzzle->InitPuzzleTeardown();
			}
			//	otherwise, bind oncomplete event
			else
			{
				puzzle->OnPuzzleComplete.AddDynamic(this, &AAControlGameMode::HandleCompletedPuzzle);
			}
		}
	}
}

AActor* AAControlGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (!instance)
	{
		instance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	return Super::FindPlayerStart_Implementation(Player, instance->PlayerStartTag);
}

TWeakObjectPtr<class AAControlCharacter> AAControlGameMode::GetCharRef() const
{
	return CharRef;
}

void AAControlGameMode::SetCharRef(TWeakObjectPtr<class AAControlCharacter> _charRef)
{
	CharRef = _charRef;
	OnCharRefInit.Broadcast();
}

void AAControlGameMode::HandleGameCompletion()
{
	TWeakObjectPtr<AACGameHud> hud = Cast<AACGameHud>(GetWorld()->GetFirstPlayerController()->GetHUD());
	hud->ToggleEndScrn();

	//	TODO: freeze player movement
	GetCharRef().Get()->FreezeMovement = true;
}

bool AAControlGameMode::CheckZoneCompletion()
{
	return false;
}

void AAControlGameMode::HandleCompletedPuzzle(class AACSokobonGrid* _grid)
{
	instance->SetPuzzleAsCompleted(_grid->PuzzleName);
	PuzzleStatus = instance->PuzzleProgress;

	if (instance->CheckGameComplete())
	{
		HandleGameCompletion();
	}
}

void AAControlGameMode::InitAnimalAI()
{
	ABaseAIAgent* spawnedAI = Cast<ABaseAIAgent>(GetWorld()->SpawnActor(AnimalAIBP, &AISpawnLoc->GetActorTransform()));
	SpawnedAI.Add(spawnedAI);
}

void AAControlGameMode::DispatchBegin()
{
	DispatchBeginPlay();
}
