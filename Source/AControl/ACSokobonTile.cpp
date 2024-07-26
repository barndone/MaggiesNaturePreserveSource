// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonTile.h"
#include "ACSokobonPiece.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "ACPipeConnection.h"
#include "ACSokobonGoal.h"
#include "ACSokobonHole.h"
#include "ACSokobonGrid.h"
#include "ACSokobonPipe.h"

// Sets default values
AACSokobonTile::AACSokobonTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SokobonTileViz = CreateDefaultSubobject<UBoxComponent>(TEXT("Tile Bounds"));
	SokobonTileViz->SetupAttachment(RootComponent);
}

void AACSokobonTile::Initialize()
{
	OccupyingPieces.Empty();
}

// Called when the game starts or when spawned
void AACSokobonTile::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
}

void AACSokobonTile::ClearTile()
{
	for (auto& val : OccupyingPieces)
	{
		if (val.Get())
		{
			val.Get()->Destroy();
		}
	}

	OccupyingPieces.Empty();

	Occupied = false;
	Navigatable = true;
}

AACSokobonTile* AACSokobonTile::GetTileInDirection(Direction _dir) const
{
	switch (_dir)
	{
		case Direction::NORTH:
			return NorthTile.Get();
			break;
		case Direction::EAST:
			return EastTile.Get();
			break;
		case Direction::SOUTH:
			return SouthTile.Get();
			break;
		case Direction::WEST:
			return WestTile.Get();
			break;
	}

	return nullptr;
}

AACSokobonTile* AACSokobonTile::GetValidTileInDirection(Direction _dir, bool _checkNext, bool _movingBkwrds, 
														AACSokobonTile* _cur, AACSokobonPiece* _piece) const
{
	AACSokobonTile* result = nullptr;
	auto nextTile = _cur->GetTileInDirection(_dir);

	AACSokobonPipe* pipeCheck = Cast<AACSokobonPipe>(_piece);
	if (pipeCheck)
	{
		_checkNext = false;
	}

	//	check that next tile is valid, if it is, assign it to result
	if (nextTile && !nextTile->GetMovementBlockingOf(_piece))
	{
		if (nextTile->OccupyingPieces.Num() > 0)
		{
			AACSokobonHole* hole;
			AACSokobonGoal* goal;
			bool canPull = true;
			for (auto& piece : nextTile->OccupyingPieces)
			{
				hole = Cast<AACSokobonHole>(piece);
				goal = Cast<AACSokobonGoal>(piece);

				if (hole)
				{
					if (hole->GetFilled())
					{
						result = nextTile;
						canPull = false;
					}
					else
					{
						if (!_movingBkwrds)
						{
							result = nextTile;
							
						}
						canPull = false;
					}
				}
				else if (goal)
				{
					if (!_movingBkwrds)
					{
						result = nextTile;
					}
					canPull = false;
				}
				if (!canPull)
				{
					break;
				}

			}
			if (canPull)
			{
				result = nextTile;
			}
		}
		else
		{
			result = nextTile;
		}

		if (_checkNext && result) 
		{ 
			//	grid end check
			auto endCheck = result->GetTileInDirection(_dir);
			if (!endCheck)
			{
				result = nullptr;
			}
			else
			{
				result = nextTile->GetValidTileInDirection(_dir, false, _movingBkwrds, nextTile, _piece);
			}
		}
	}

	return result;
}

void AACSokobonTile::SetTileExtents(float _tileSize)
{
	SokobonTileViz->SetBoxExtent(FVector(_tileSize / 2, _tileSize / 2, 10));
}

bool AACSokobonTile::GetMovementBlockingOf(AACSokobonPiece* _pieceToCheck) const
{
	UClass* pieceClass = _pieceToCheck->GetClass();

	bool result = false;

	for (auto& val : BlockMovementOf)
	{
		result = UKismetMathLibrary::ClassIsChildOf(pieceClass, val);

		if (result)
		{
			break;
		}
	}
	
	return result;
}

void AACSokobonTile::AddBlockMovementOf(AACSokobonPiece* _pieceToAdd)
{
	if (_pieceToAdd->BlockMovementOf.Num() != 0)
	{
		for (auto& val : _pieceToAdd->BlockMovementOf)
		{
			this->BlockMovementOf.AddUnique(val);
		}
	}
}

void AACSokobonTile::RemoveBlockMovementOf(AACSokobonPiece* _pieceToRemove)
{
	//	check that this piece actually occupies this tile
	if (OccupyingPieces.Contains(_pieceToRemove))
	{
		//	case 1: only one piece exists on this tile
		if (OccupyingPieces.Num() == 1)
		{
			for (auto& val : _pieceToRemove->BlockMovementOf)
			{
				this->BlockMovementOf.Remove(val);
			}
		}

		//	case 2: more than one piece exists on this tile
		else
		{
			for (auto& val : _pieceToRemove->BlockMovementOf)
			{
				this->BlockMovementOf.Remove(val);
			}

			for (auto& val : OccupyingPieces)
			{
				if (val == _pieceToRemove)
				{
					continue;
				}

				AddBlockMovementOf(val.Get());
			}
		}
	}
	//	otherwise we do nothing
}

void AACSokobonTile::UpdateBlockMovementOf()
{
	BlockMovementOf.Empty();

	for (auto& val : OccupyingPieces)
	{
		AddBlockMovementOf(val.Get());
	}
}

void AACSokobonTile::SpawnPuzzlePieceHere()
{
	if (GridRef)
	{
		GridRef->SpawnPuzzlePieceAtTile(GridPos);
	}
	else
	{	
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnPuzzlePieceAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnPuzzleTeleporterHere()
{
	if (GridRef)
	{
		GridRef->SpawnPuzzleTeleporterAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnPuzzleTeleporterAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnSpecialtyTeleporterHere()
{
	if (GridRef)
	{
		GridRef->SpawnSpecialtyTeleporterAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnSpecialtyTeleporterAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnWallHere()
{
	if (GridRef)
	{
		GridRef->SpawnWallAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnWallAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnObjectiveHere()
{
	if (GridRef)
	{
		GridRef->SpawnObjectiveAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnObjectiveAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnGoalHere()
{
	if (GridRef)
	{
		GridRef->SpawnGoalAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnGoalAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnHoleHere()
{
	if (GridRef)
	{
		GridRef->SpawnHoleAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnHoleAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnConveyorHere()
{
	if (GridRef)
	{
		GridRef->SpawnConveyorAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnConveyorAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnIPipeHere()
{
	if (GridRef)
	{
		GridRef->SpawnIPipeAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnIPipeAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnCWLPipeHere()
{
	if (GridRef)
	{
		GridRef->SpawnCWLPipeAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnCWLPipeAtTile(GridPos);
	}
}

void AACSokobonTile::SpawnCCWLPipeHere()
{
	if (GridRef)
	{
		GridRef->SpawnCCWLPipeAtTile(GridPos);
	}
	else
	{
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->SpawnCCWLPipeAtTile(GridPos);
	}
}