// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonGrid.h"
#include "ACSokobonTile.h"
#include "ACSokobonPiece.h"
#include "ACSokobonTeleporter.h"
#include "ACSokobonWall.h"
#include "ACSokobonObjective.h"
#include "ACSokobonGoal.h"
#include "ACSokobonHole.h"
#include "ACSokobonPuzzleBlock.h"
#include "ACSokobonConveyor.h" 
#include "ACSokobonPipe.h"
#include "ACPipeConnection.h"

#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Containers/UnrealString.h"

#include "AControlGameMode.h"
#include "AControlCharacter.h"

#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AACSokobonGrid::AACSokobonGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	auto root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	root->SetupAttachment(RootComponent);

	PuzzleArea = CreateDefaultSubobject<UBoxComponent>(TEXT("PuzzleArea"));
	PuzzleArea->SetupAttachment(root);

	TopDownCam = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCam->SetupAttachment(root);
	InitPuzzleArea();
}

// Called when the game starts or when spawned
void AACSokobonGrid::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode->GetCharRefBP())
	{
		BindCharacterEvents();
	}
	else
	{
		GameMode->OnCharRefInit.AddUniqueDynamic(this, &AACSokobonGrid::BindCharacterEvents);
	}
	PuzzleArea->OnComponentBeginOverlap.AddDynamic(this, &AACSokobonGrid::PlayerEnteredPuzzleArea);
	PuzzleArea->OnComponentEndOverlap.AddDynamic(this, &AACSokobonGrid::PlayerLeftPuzzleArea);

	SpawnParams.OverrideLevel = GetLevel();
	InitPuzzleArea();

	//	if we are a sliding pipe puzzle
	if (BSHackPuzzle)
	{

		//	HACK: hard coded list entry since we do not have time to build a more effective solution
		End->Entry->SetConnection(SpawnedPipes[24]->Exit);
		SpawnedPipes[24]->Exit->SetConnection(End->Entry);
		End->UpdateConnections();
		SpawnedPipes[24]->UpdateConnections();
	}
}

void AACSokobonGrid::PlayerEnteredPuzzleArea(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _hit)
{
	if (_overlappedComponent == PuzzleArea)
	{
			AAControlCharacter* charCheck = Cast<AAControlCharacter>(_otherActor);

		if (charCheck)
		{
			UCapsuleComponent* charCapsule = Cast<UCapsuleComponent>(_otherComponent);

			if (charCapsule)
			{
				//	it was actually the part of the player we care about (capsule coll)
				//	swap the game mode task to micro win con (this puzzle goal)
				charCheck->SetTopDownCamRef(TopDownCam);
				charCheck->CurPuzzle = this;
				OnPuzzleEnter.Broadcast();
				CompletedPuzzles >= SpawnedObjectives.Num() ? OnUpdateTasks.Broadcast(nullptr, true) : OnUpdateTasks.Broadcast(this, false);
				charCheck->OnDebugClearWish.AddDynamic(this, &AACSokobonGrid::InitPuzzleTeardown);
			}
		}
	}
}

void AACSokobonGrid::PlayerLeftPuzzleArea(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex)
{
	if (_overlappedComponent == PuzzleArea)
	{
		AAControlCharacter* charCheck = Cast<AAControlCharacter>(_otherActor);

		if (charCheck)
		{
			UCapsuleComponent* charCapsule = Cast<UCapsuleComponent>(_otherComponent);

			if (charCapsule)
			{
				//	it was actually the part of the player we care about (capsule coll)
				//	swap the game mode task to macro win con (puzzles completed / total)
				charCheck->SetTopDownCamRef(nullptr);
				charCheck->CurPuzzle = nullptr;
				OnPuzzleExit.Broadcast();
				OnUpdateTasks.Broadcast(nullptr, true);
				charCheck->OnDebugClearWish.RemoveDynamic(this, &AACSokobonGrid::InitPuzzleTeardown);
			}
		}
	}
}

int AACSokobonGrid::GetGridLength() const
{
	return GridLength;
}

int AACSokobonGrid::GetGridWidth() const
{
	return GridWidth;
}

float AACSokobonGrid::GetTileSize() const
{
	return TileSize;
}

void AACSokobonGrid::GenerateGrid()
{
	ResetGrid();
	InitPuzzleArea();

	for (int row = 0; row < GridLength; ++row)
	{
		for (int col = 0; col < GridWidth; ++col)
		{
			//	step 1: gen tile
			AACSokobonTile* curTile = Cast<AACSokobonTile>(GetWorld()->SpawnActor(
															TileBP,
															&this->GetTransform(), SpawnParams));

			//	attach the tile to the grid generating it
			curTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			
			//	step 2: add to SokobonTiles map
			SokobonTiles.Add(FVector2D(row, col), curTile);
			curTile->GridPos = FVector2D(row, col);
			curTile->SetActorLocation(CalcTilePosition(row, col));
			curTile->SetTileExtents(TileSize);

			//	step 3: pair to adjacent tiles
			//	if row is > 0 pair N/S tiles
			if (row > 0)
			{
				AACSokobonTile* prevTile = *SokobonTiles.Find(FVector2D(row-1, col));

				prevTile->NorthTile = curTile;
				curTile->SouthTile = prevTile;
			}

			//	if col > 0 pair to E/W tiles
			if (col > 0)
			{
				AACSokobonTile* prevTile = *SokobonTiles.Find(FVector2D(row, col - 1));

				prevTile->EastTile = curTile;
				curTile->WestTile = prevTile;
			}
		}
	}
}

void AACSokobonGrid::SpawnPuzzlePiece()
{
	AACSokobonPuzzleBlock* newPiece = Cast<AACSokobonPuzzleBlock>(GetWorld()->SpawnActor(PuzzlePieceBP, &this->GetTransform(), SpawnParams));
	//newPiece->InitPivotLocations(TileSize);
	SpawnedPuzzlePieces.Add(newPiece);

	InitSokobonPiece(newPiece);

	if (SpawnedTeleporters.Num() != 0)
	{
		for (auto& val : SpawnedTeleporters)
		{
			newPiece->OnPieceMove.AddUniqueDynamic(val, &AACSokobonTeleporter::CheckForBlockage);
		}
	}

	if (SpawnedSpecialTeleporters.Num() != 0)
	{
		for (auto& val : SpawnedSpecialTeleporters)
		{
			newPiece->OnPieceMove.AddUniqueDynamic(val, &AACSokobonTeleporter::CheckForBlockage);
		}
	}

	if (SpawnedHoles.Num() != 0)
	{
		for (auto& val : SpawnedHoles)
		{
			newPiece->OnHoleFill.AddUniqueDynamic(val, &AACSokobonHole::FillHole);
		}
	}

	//	newPiece->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnPuzzleTeleporter()
{
	AACSokobonTeleporter* newTeleporter = Cast<AACSokobonTeleporter>(GetWorld()->SpawnActor(TeleporterPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedTeleporters.Add(newTeleporter);

	InitSokobonPiece(newTeleporter);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto &val : SpawnedPuzzlePieces)
		{
			val->OnPieceMove.AddUniqueDynamic(newTeleporter, &AACSokobonTeleporter::CheckForBlockage);
		}
	}

	//	newTeleporter->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnSpecialtyTeleporter()
{
	AACSokobonTeleporter* newTeleporter = Cast<AACSokobonTeleporter>(GetWorld()->SpawnActor(SpecialtyTeleporterBP, &this->GetTransform(), SpawnParams));
	SpawnedSpecialTeleporters.Add(newTeleporter);

	InitSokobonPiece(newTeleporter);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto& val : SpawnedPuzzlePieces)
		{
			val->OnPieceMove.AddUniqueDynamic(newTeleporter, &AACSokobonTeleporter::CheckForBlockage);
		}
	}

	//	newTeleporter->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnWall()
{
	AACSokobonWall* newWall = Cast<AACSokobonWall>(GetWorld()->SpawnActor(ObstructionPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedWalls.Add(newWall);

	InitSokobonPiece(newWall);

	//	newWall->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnObjective()
{
	AACSokobonObjective* newObj = Cast<AACSokobonObjective>(GetWorld()->SpawnActor(ObjectivePieceBP, &this->GetTransform(), SpawnParams));
	SpawnedObjectives.Add(newObj);
	//newObj->InitPivotLocations(TileSize);

	InitSokobonPiece(newObj);

	//	newObj->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnGoal()
{
	AACSokobonGoal* newGoal = Cast<AACSokobonGoal>(GetWorld()->SpawnActor(GoalPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedGoals.Add(newGoal);

	InitSokobonPiece(newGoal);

	//	newGoal->OnPieceDelete.AddDynamic(this, &AACSokobonGrid::CleanUpDeletedPiece);
}

void AACSokobonGrid::SpawnHole()
{
	AACSokobonHole* newHole = Cast<AACSokobonHole>(GetWorld()->SpawnActor(HoleBP, &this->GetTransform(), SpawnParams));
	SpawnedHoles.Add(newHole);

	InitSokobonPiece(newHole);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto& val : SpawnedPuzzlePieces)
		{
			val->OnHoleFill.AddUniqueDynamic(newHole, &AACSokobonHole::FillHole);
		}
	}
}

void AACSokobonGrid::SpawnConveyor()
{
	AACSokobonConveyor* newConveyor = Cast<AACSokobonConveyor>(GetWorld()->SpawnActor(ConveyorBP, &this->GetTransform(), SpawnParams));
	SpawnedConveyors.Add(newConveyor);
	newConveyor->FlowDir = dir;
	InitSokobonPiece(newConveyor);
}

void AACSokobonGrid::SpawnIPipe()
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(IPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}

void AACSokobonGrid::SpawnCWLPipe()
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(CWLPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}


void AACSokobonGrid::SpawnCCWLPipe()
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(CCWLPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}

FVector AACSokobonGrid::CalcTilePosition(int _curRowIdx, int _curColIdx)
{
	//	cache the curent transform
	FVector curPos = this->GetTransform().GetLocation();
	//	cache the offset of the tile
	FVector tileOffset = FVector(_curRowIdx * TileSize, _curColIdx * TileSize, 0);

	//	return cur transform of grid actor + tile offset
	return curPos + tileOffset;
}

void AACSokobonGrid::ResetGrid()
{
	if (SokobonTiles.Num() != 0)
	{
		for (TPair<FVector2D, AACSokobonTile*>& pair : SokobonTiles)
		{
			pair.Value->Destroy();
		}

		SokobonTiles.Empty();
	}

	ResetActorList(SpawnedPuzzlePieces);
	ResetActorList(SpawnedTeleporters);
	ResetActorList(SpawnedWalls);
	ResetActorList(SpawnedObjectives);
	ResetActorList(SpawnedGoals);
	ResetActorList(SpawnedHoles);
	ResetActorList(SpawnedPipes);
}

void AACSokobonGrid::ResetPuzzle()
{

	UGameplayStatics::OpenLevel((UObject*)GetGameInstance(), FName(UGameplayStatics::GetCurrentLevelName(GetWorld())));
}

void AACSokobonGrid::BindCharacterEvents()
{
	//	get the game mode and cast to AAControlGameMode
	GameMode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
	//	bind the puzzle reset event to the resetpuzzle method
	GameMode->GetCharRef()->OnPuzzleResetWish.AddDynamic(this, &AACSokobonGrid::ResetPuzzle);

	for (auto& puzzlePiece : SpawnedPuzzlePieces)
	{
		puzzlePiece->InitPivotLocations(GrabPivotOffset);
		puzzlePiece->GridRef = this;

		puzzlePiece->OnPieceDrop.AddUniqueDynamic(GameMode->GetCharRef().Get(), &AAControlCharacter::ClearHeldPiece);

		if (SpawnedTeleporters.Num() != 0)
		{
			for (auto& teleporter : SpawnedTeleporters)
			{
				puzzlePiece->OnPieceMove.AddUniqueDynamic(teleporter, &AACSokobonTeleporter::CheckForBlockage);
			}
		}

		if (SpawnedHoles.Num() != 0)
		{
			for (auto& hole : SpawnedHoles)
			{
				puzzlePiece->OnHoleFill.AddUniqueDynamic(hole, &AACSokobonHole::FillHole);
				puzzlePiece->OnFallStart.AddUniqueDynamic(hole, &AACSokobonHole::ShowDustCloud);
			}
		}
	}

	for (auto& teleporter : SpawnedTeleporters)
	{
		teleporter->GridRef = this;
	}

	for (auto& val : SpawnedWalls)
	{
		val->GridRef = this;
	}

	for (auto& val : SpawnedObjectives)
	{
		val->InitPivotLocations(GrabPivotOffset);
		val->GridRef = this;

		val->OnPieceDrop.AddUniqueDynamic(GameMode->GetCharRef().Get(), &AAControlCharacter::ClearHeldPiece);
		val->OnGoalReached.AddUniqueDynamic(this, &AACSokobonGrid::HandleAnimalFreed);
	}

	for (auto& val : SpawnedGoals)
	{
		val->GridRef = this;
	}

	for (auto& hole : SpawnedHoles)
	{
		hole->GridRef = this;
	}

	for (auto& pipe : SpawnedPipes)
	{
		pipe->GridRef = this;
		pipe->InitPivotLocations(GrabPivotOffset);
		pipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
		for (auto& val : SpawnedPipes)
		{
			if (val == pipe)
			{
				continue;
			}

			pipe->OnPieceMove.AddUniqueDynamic(val, &AACSokobonPipe::UpdateConnections);
		}
	}

	//	ResetPuzzle();
	LinkPortals();

	//	clear the temp OnCharRefInit binding
	GameMode->OnCharRefInit.RemoveAll(this);
}

void AACSokobonGrid::LinkPortals()
{
	if (SpawnedTeleporters.Num() == 1)
	{
		SpawnedTeleporters[0]->SetOutTeleporter(nullptr);
	}
	else if (SpawnedTeleporters.Num() == 2)
	{
		if (SpawnedTeleporters[1]->Blocked)
		{
			if (SpawnedTeleporters[0]->GetOutTele())
			{
				SpawnedTeleporters[0]->OnTeleportBlocked.Broadcast();
			}
			SpawnedTeleporters[0]->SetOutTeleporter(nullptr);
		}
		else
		{
			if (!SpawnedTeleporters[0]->GetOutTele())
			{
				SpawnedTeleporters[0]->OnTeleportUnblocked.Broadcast();
			}
			SpawnedTeleporters[0]->SetOutTeleporter(SpawnedTeleporters[1]);
		}

		if (TwoWayTeleportPair && !SpawnedTeleporters[0]->Blocked)
		{
			if (!SpawnedTeleporters[1]->GetOutTele())
			{
				SpawnedTeleporters[1]->OnTeleportUnblocked.Broadcast();
			}
			SpawnedTeleporters[1]->SetOutTeleporter(SpawnedTeleporters[0]);
		}
		else
		{
			if (SpawnedTeleporters[1]->GetOutTele())
			{
				SpawnedTeleporters[1]->OnTeleportBlocked.Broadcast();
			}
			SpawnedTeleporters[1]->SetOutTeleporter(nullptr);
		}
	}
	else
	{
		for (int curPortal = 0; curPortal < SpawnedTeleporters.Num(); ++curPortal)
		{
			if (!SpawnedTeleporters[curPortal]->Blocked)
			{
				if (!SpawnedTeleporters[curPortal]->GetOutTele())
				{
					SpawnedTeleporters[curPortal]->OnTeleportBlocked.Broadcast();
				}
				SpawnedTeleporters[curPortal]->SetOutTeleporter(GetNextOpenPortal(SpawnedTeleporters[curPortal]));
			}
			else
			{
				if (SpawnedTeleporters[curPortal]->GetOutTele())
				{
					SpawnedTeleporters[curPortal]->OnTeleportBlocked.Broadcast();
				}
				SpawnedTeleporters[curPortal]->SetOutTeleporter(nullptr);
			}
		}
	}
}

TWeakObjectPtr<AACSokobonTeleporter> AACSokobonGrid::GetNextOpenPortal(
	TWeakObjectPtr<AACSokobonTeleporter> _curPortal)
{
	//	get index of current portal
	int portalIdx = SpawnedTeleporters.IndexOfByKey(_curPortal);
	//	get next index
	int nextIdx = portalIdx + 1;

	//	account for overflow
	if (nextIdx >= SpawnedTeleporters.Num())
	{
		nextIdx = 0;
	}

	bool earlyExit = false;

	//	look for the next valid portal while:
	//	nextIdx is NOT portalIdx OR while early exit is false
	while (nextIdx != portalIdx || !earlyExit)
	{
		if (SpawnedTeleporters[nextIdx]->Blocked)
		{
			++nextIdx;

			//	account for overflow
			if (nextIdx >= SpawnedTeleporters.Num())
			{
				nextIdx = 0;
			}
		}
		else
		{
			if (!(_curPortal == SpawnedTeleporters[nextIdx]))
			{
				earlyExit = true;
				return SpawnedTeleporters[nextIdx];
			}
		}
	}

	return nullptr;
}

void AACSokobonGrid::InitSokobonPiece(AACSokobonPiece* _pieceToInit, FVector2D _pos)
{
	_pieceToInit->GridRef = this;

	if (SokobonTiles.Num() == 0)
	{
		GenerateGrid();
	}

	_pieceToInit->CurrentTile = *SokobonTiles.Find(_pos);
	_pieceToInit->StartingTileIdx = _pos;
	_pieceToInit->CurrentTile->Occupied = true;
	_pieceToInit->CurrentTile->OccupyingPieces.Add(_pieceToInit);
	_pieceToInit->SetActorLocation(_pieceToInit->CurrentTile->GetActorLocation());

	if (_pieceToInit->BlockMovement)
	{
		_pieceToInit->CurrentTile->Navigatable = false;
	}

	//	attach the tile to the grid generating it
	_pieceToInit->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AACSokobonGrid::InitPuzzleArea()
{
	//	step 1: update position to middle of grid
	//	x / y -> ((GridL/W - 1) * TileSize) / 2
	//	z -> 0

	FVector center = FVector(((GridLength - 1) * TileSize) * 0.5f, ((GridWidth - 1) * TileSize) * 0.5f, 0.0f);

	PuzzleArea->SetRelativeLocation(center);

	//	step 2: update extents of PuzzleArea
	PuzzleArea->SetBoxExtent(FVector((GridLength * TileSize) * 0.5f + PuzzleAreaOverflow, (GridWidth * TileSize) * 0.5f + PuzzleAreaOverflow, 1000.0f));
	//TopDownCam->

	auto dToFitWidth = GridWidth * TileSize / UKismetMathLibrary::Tan(TopDownCam->FieldOfView * TopDownCam->AspectRatio * 0.5f);
	auto dToFitHeight = GridLength * TileSize / UKismetMathLibrary::Tan(TopDownCam->FieldOfView * 0.5f);

	//	ref: https://forum.unity.com/threads/camera-distance-to-fit-a-procedurally-generated-gameobject-of-varying-dimensions.356085/
	//	addtl. ref: https://docs.unity3d.com/Manual/FrustumSizeAtDistance.html

	//center = TopDownCam->GetRelativeLocation() - TopDownCam->GetRelativeTransform().GetRotation().GetForwardVector() * UKismetMathLibrary::Max(dToFitWidth, dToFitHeight);

	TopDownCam->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.0f, -90.0f, 0.0f)));
	TopDownCam->SetRelativeLocation(center + FVector(0,0, UKismetMathLibrary::Max(dToFitWidth, dToFitHeight) * 1.75));
	//	TopDownCam->SetRelativeLocation(center - 
	//		TopDownCam->GetRelativeTransform().GetRotation().GetUpVector() * (GridLength * TileSize * 0.5f) - 
	//		TopDownCam->GetRelativeTransform().GetRotation().GetRightVector() * (GridWidth * TileSize * 0.5f));
}

void AACSokobonGrid::HandleAnimalFreed()
{
	++CompletedPuzzles;

	//	check for grid puzzle completion
	if (CompletedPuzzles >= SpawnedObjectives.Num())
	{
		OnPuzzleComplete.Broadcast(this);
		OnUpdateTasks.Broadcast(nullptr, true);

		if (!GameMode)
		{
			GameMode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
		}

		for (int i = 0; i < SpawnedObjectives.Num(); ++i)
		{
			SpawnedObjectives[i]->InitAnimalAI();
		}
	}

	else
	{
		OnUpdateTasks.Broadcast(this, true);
	}
}

void AACSokobonGrid::ImportPuzzle()
{
	if (!PuzzleImport.IsEmpty())
	{
		//	get number of rows (lines)
		int rowCt = 0;
		//	get number of columns (chars per line)
		int colCt = 0;
		TArray<FString> splitString;

		PuzzleImport.ParseIntoArrayLines(splitString);

		//	row count will be equal to the number of entries in splitString
		rowCt = splitString.Num();
		//	assume colCt will always be uniform across rows
		colCt = splitString[0].Len();

		//	assign row/col ct to assigned things
		GridLength = rowCt;
		GridWidth = colCt;
		//	generate grid of correct size
		GenerateGrid();

		//	TIME TO ITERATE BAYBEE

		//	rows
		for (int row = rowCt - 1; row >= 0; --row)
		{
			for (int col = 0; col < colCt; ++col)
			{
				auto ch = splitString[row][col];
				FString key = FString::Chr(ch);
				key = key.ToUpper();

				if (key == ".")
				{
					continue;
				}

				//	hard coded double value to key pairings
				if (key == "@")
				{
					//	also spawn a goal
					AACSokobonGoal* goal = Cast<AACSokobonGoal>(GetWorld()->SpawnActor(GoalPieceBP, &this->GetTransform(), SpawnParams));
					AddPieceToList(goal);
					InitSokobonPiece(goal);
					goal->PlaceAtCoordinate(FVector2D((rowCt - 1) - row, col));
					goal->SaveCurrentPositionAsStart();
				}

				//	hard coded double value to key pairings
				if (key == "K")
				{
					//	also spawn a tele
					AACSokobonTeleporter* tele = Cast<AACSokobonTeleporter>(GetWorld()->SpawnActor(TeleporterPieceBP, &this->GetTransform(), SpawnParams));
					AddPieceToList(tele);
					InitSokobonPiece(tele);
					tele->PlaceAtCoordinate(FVector2D((rowCt - 1) - row, col));
					tele->SaveCurrentPositionAsStart();
				}


				//	get spawnable class ref
				auto pieceToSpawn = TextImportPairings.Find(key);

				//	spawn the actor
				auto piece = Cast<AACSokobonPiece>(GetWorld()->SpawnActor(*pieceToSpawn, &this->GetTransform(), SpawnParams));
				AddPieceToList(piece);
				InitSokobonPiece(piece);
				piece->PlaceAtCoordinate(FVector2D((rowCt - 1) - row, col));
				piece->SaveCurrentPositionAsStart();
			}
		}
		//	start at bottom left
		//	work left to right, increasing rows by one
	}
}

void AACSokobonGrid::CleanUpDeletedPiece(AACSokobonPiece* _pieceToCleanUp)
{
	//	potentially jank? definitely jank.
	//	BRUTE FORCE WOOOOOOO
	AACSokobonPuzzleBlock* pb = Cast<AACSokobonPuzzleBlock>(_pieceToCleanUp);
	AACSokobonTeleporter* t = Cast<AACSokobonTeleporter>(_pieceToCleanUp);
	AACSokobonWall* w = Cast<AACSokobonWall>(_pieceToCleanUp);
	AACSokobonObjective* o = Cast<AACSokobonObjective>(_pieceToCleanUp);
	AACSokobonGoal* g = Cast<AACSokobonGoal>(_pieceToCleanUp);
	AACSokobonHole* h = Cast<AACSokobonHole>(_pieceToCleanUp);

	if (pb)
	{
		SpawnedPuzzlePieces.Remove(pb);
	}

	if (t)
	{
		SpawnedTeleporters.Remove(t);
	}

	if (w)
	{
		SpawnedWalls.Remove(w);
	}

	if (o)
	{
		SpawnedObjectives.Remove(o);
	}

	if (g)
	{
		SpawnedGoals.Remove(g);
	}
	if (h)
	{
		SpawnedHoles.Remove(h);
	}

	_pieceToCleanUp->Destroy();
}

void AACSokobonGrid::InitPuzzleTeardown()
{
	for (auto& tile : SokobonTiles)
	{
		tile.Value->Initialize();
	}
	for (auto& piece : SpawnedPuzzlePieces)
	{
		piece->Initialize();
		piece->HandlePieceHiding();
	}

	for (auto& obj : SpawnedObjectives)
	{
		obj->Initialize();
		obj->HandlePieceHiding();
		HandleAnimalFreed();
	}

	for (auto& hole : SpawnedHoles)
	{
		hole->Initialize();
		hole->FillHole(hole->CurrentTile);
	}

	OnPuzzleComplete.Broadcast(this);
	bDelayGateOpen = true;
}

void AACSokobonGrid::AddPieceToList(AACSokobonPiece* _piece)
{
	UClass* pieceClass = _piece->GetClass();

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonPuzzleBlock::StaticClass()))
	{
		AACSokobonPuzzleBlock* piece = Cast<AACSokobonPuzzleBlock>(_piece);
		SpawnedPuzzlePieces.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonTeleporter::StaticClass()))
	{
		AACSokobonTeleporter* piece = Cast<AACSokobonTeleporter>(_piece);
		SpawnedTeleporters.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonWall::StaticClass()))
	{
		AACSokobonWall* piece = Cast<AACSokobonWall>(_piece);
		SpawnedWalls.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonObjective::StaticClass()))
	{
		AACSokobonObjective* piece = Cast<AACSokobonObjective>(_piece);
		SpawnedObjectives.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonGoal::StaticClass()))
	{
		AACSokobonGoal* piece = Cast<AACSokobonGoal>(_piece);
		SpawnedGoals.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonHole::StaticClass()))
	{
		AACSokobonHole* piece = Cast<AACSokobonHole>(_piece);
		SpawnedHoles.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonConveyor::StaticClass()))
	{
		AACSokobonConveyor* piece = Cast<AACSokobonConveyor>(_piece);
		SpawnedConveyors.Add(piece);
	}

	if (UKismetMathLibrary::ClassIsChildOf(pieceClass, AACSokobonPipe::StaticClass()))
	{
		AACSokobonPipe* piece = Cast<AACSokobonPipe>(_piece);
		SpawnedPipes.Add(piece);
	}
}

void AACSokobonGrid::CheckPipePuzzleCompletion()
{
	AACSokobonPipe* cur = Start;
	int loops = 0;
	do
	{
		cur = cur->Next;
		++loops;
		if (cur == End)
		{
			break;
		}
	} while (cur != nullptr && loops < 100);

	if (cur == End)
	{
		OnPuzzleComplete.Broadcast(this);
		OnUpdateTasks.Broadcast(nullptr, true);
	}
}

void AACSokobonGrid::SpawnPuzzlePieceAtTile(FVector2D _tileCoord)
{
	AACSokobonPuzzleBlock* newPiece = Cast<AACSokobonPuzzleBlock>(GetWorld()->SpawnActor(PuzzlePieceBP, &this->GetTransform(), SpawnParams));
	//newPiece->InitPivotLocations(TileSize);
	SpawnedPuzzlePieces.Add(newPiece);

	InitSokobonPiece(newPiece, _tileCoord);

	if (SpawnedTeleporters.Num() != 0)
	{
		for (auto& val : SpawnedTeleporters)
		{
			newPiece->OnPieceMove.AddUniqueDynamic(val, &AACSokobonTeleporter::CheckForBlockage);
		}
	}

	if (SpawnedHoles.Num() != 0)
	{
		for (auto& val : SpawnedHoles)
		{
			newPiece->OnHoleFill.AddUniqueDynamic(val, &AACSokobonHole::FillHole);
		}
	}
}
void AACSokobonGrid::SpawnPuzzleTeleporterAtTile(FVector2D _tileCoord)
{
	AACSokobonTeleporter* newTeleporter = Cast<AACSokobonTeleporter>(GetWorld()->SpawnActor(TeleporterPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedTeleporters.Add(newTeleporter);

	InitSokobonPiece(newTeleporter, _tileCoord);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto& val : SpawnedPuzzlePieces)
		{
			val->OnPieceMove.AddUniqueDynamic(newTeleporter, &AACSokobonTeleporter::CheckForBlockage);
		}
	}
}

void AACSokobonGrid::SpawnSpecialtyTeleporterAtTile(FVector2D _tileCoord)
{
	AACSokobonTeleporter* newTeleporter = Cast<AACSokobonTeleporter>(GetWorld()->SpawnActor(SpecialtyTeleporterBP, &this->GetTransform(), SpawnParams));
	SpawnedSpecialTeleporters.Add(newTeleporter);

	InitSokobonPiece(newTeleporter, _tileCoord);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto& val : SpawnedPuzzlePieces)
		{
			val->OnPieceMove.AddUniqueDynamic(newTeleporter, &AACSokobonTeleporter::CheckForBlockage);
		}
	}
}

void AACSokobonGrid::SpawnWallAtTile(FVector2D _tileCoord)
{
	AACSokobonWall* newWall = Cast<AACSokobonWall>(GetWorld()->SpawnActor(ObstructionPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedWalls.Add(newWall);

	InitSokobonPiece(newWall, _tileCoord);
}
void AACSokobonGrid::SpawnObjectiveAtTile(FVector2D _tileCoord)
{
	AACSokobonObjective* newObj = Cast<AACSokobonObjective>(GetWorld()->SpawnActor(ObjectivePieceBP, &this->GetTransform(), SpawnParams));
	SpawnedObjectives.Add(newObj);
	//newObj->InitPivotLocations(TileSize);

	InitSokobonPiece(newObj, _tileCoord);
}
void AACSokobonGrid::SpawnGoalAtTile(FVector2D _tileCoord)
{
	AACSokobonGoal* newGoal = Cast<AACSokobonGoal>(GetWorld()->SpawnActor(GoalPieceBP, &this->GetTransform(), SpawnParams));
	SpawnedGoals.Add(newGoal);

	InitSokobonPiece(newGoal, _tileCoord);
}
void AACSokobonGrid::SpawnHoleAtTile(FVector2D _tileCoord)
{
	AACSokobonHole* newHole = Cast<AACSokobonHole>(GetWorld()->SpawnActor(HoleBP, &this->GetTransform(), SpawnParams));
	SpawnedHoles.Add(newHole);

	InitSokobonPiece(newHole, _tileCoord);

	if (SpawnedPuzzlePieces.Num() != 0)
	{
		for (auto& val : SpawnedPuzzlePieces)
		{
			val->OnHoleFill.AddUniqueDynamic(newHole, &AACSokobonHole::FillHole);
		}
	}
}
void AACSokobonGrid::SpawnConveyorAtTile(FVector2D _tileCoord)
{
	AACSokobonConveyor* newConveyor = Cast<AACSokobonConveyor>(GetWorld()->SpawnActor(ConveyorBP, &this->GetTransform(), SpawnParams));
	SpawnedConveyors.Add(newConveyor);
	newConveyor->FlowDir = dir;
	InitSokobonPiece(newConveyor, _tileCoord);
}
void AACSokobonGrid::SpawnIPipeAtTile(FVector2D _tileCoord)
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(IPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe, _tileCoord);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}
void AACSokobonGrid::SpawnCWLPipeAtTile(FVector2D _tileCoord)
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(CWLPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe, _tileCoord);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}

void AACSokobonGrid::SpawnCCWLPipeAtTile(FVector2D _tileCoord)
{
	AACSokobonPipe* newPipe = Cast<AACSokobonPipe>(GetWorld()->SpawnActor(CCWLPipeBP, &this->GetTransform(), SpawnParams));
	SpawnedPipes.Add(newPipe);
	InitSokobonPiece(newPipe, _tileCoord);
	newPipe->OnPipeMove.AddUniqueDynamic(this, &AACSokobonGrid::CheckPipePuzzleCompletion);
}








