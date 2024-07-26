// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonPiece.h"
#include "ACSokobonTile.h"
#include "ACSokobonPivotComponent.h"
#include "ACSokobonGrid.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AACSokobonPiece::AACSokobonPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto root = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	RootComponent = root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(root);

	SK = CreateDefaultSubobject<USkeletalMeshComponent>("SK");
	SK->SetupAttachment(root);

	InitSokoPivots();
}

void AACSokobonPiece::Initialize()
{
	CurrentTile->OccupyingPieces.Add(this);
	CurrentTile->AddBlockMovementOf(this);

}

// Called when the game starts or when spawned
void AACSokobonPiece::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

// Called every frame
void AACSokobonPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotateWish)
	{
		RotateTimer += DeltaTime;
		SK->SetWorldRotation(FMath::Lerp(LerpStartQuat, TarQuat, RotateTimer / RotateDelay));
		if (RotateTimer >= RotateDelay)
		{
			RotateWish = false;
			RotateTimer = 0.0f;
		}
	}
	if (bMoveInProgress)
	{
		LerpCheck(ConveyorMoveTimer, GridRef->GetConveyorMoveDur());
		ConveyorMoveTimer += DeltaTime;
	}

	if (moveWish)
	{
		//	OnMoveStart.Broadcast();
	}
}

bool AACSokobonPiece::MoveToTileInDirection(Direction _dir)
{
	//	if move successful, return true
	AACSokobonTile* newTile = CurrentTile->GetTileInDirection(_dir);

	if (newTile)
	{
		if (newTile->Navigatable)
		{	
			CurrentTile->Occupied = false;
			CurrentTile->Navigatable = true;
			CurrentTile->RemoveBlockMovementOf(this);
			CurrentTile->OccupyingPieces.Remove(this);

			CurrentTile = newTile;
			CurrentTile->Occupied = true;
			CurrentTile->OccupyingPieces.Add(this);
			
			if (BlockMovement)
			{
				CurrentTile->Navigatable = false;
			}

			OnPieceMove.Broadcast();
			AdditionalMovementLogic();
			UpdatePosition();
			
			return true;
		}
	}

	return false;
}

void AACSokobonPiece::InitPivotLocations(float _fullExtent)
{
	if (NorthPivot)
	{
		NorthPivot->SetRelativeLocation(FVector(_fullExtent / 2, 0, 50));
		//	get side of actor
		FVector actorPos = GetActorLocation();
		FVector pivotPos = NorthPivot->GetComponentLocation();

		FVector dir = (actorPos - pivotPos).GetSafeNormal();

		if (dir.X != 0.0)
		{
			NorthPivot->FwdAligned = true;
		}
		else if (dir.Y != 0.0)
		{
			NorthPivot->FwdAligned = false;
		}
		//	swap alignment
	}
	if (EastPivot)
	{
		EastPivot->SetRelativeLocation(FVector(0, _fullExtent / 2, 50));

		FVector actorPos = GetActorLocation();
		FVector pivotPos = EastPivot->GetComponentLocation();

		FVector dir = (actorPos - pivotPos).GetSafeNormal();

		if (dir.X != 0.0)
		{
			EastPivot->FwdAligned = true;
		}
		else if (dir.Y != 0.0)
		{
			EastPivot->FwdAligned = false;
		}
	}
	if (SouthPivot)
	{
		SouthPivot->SetRelativeLocation(FVector(-_fullExtent / 2, 0, 50));

		FVector actorPos = GetActorLocation();
		FVector pivotPos = SouthPivot->GetComponentLocation();

		FVector dir = (actorPos - pivotPos).GetSafeNormal();

		if (dir.X != 0.0)
		{
			SouthPivot->FwdAligned = true;
		}
		else if (dir.Y != 0.0)
		{
			SouthPivot->FwdAligned = false;
		}
	}
	if (WestPivot)
	{
		WestPivot->SetRelativeLocation(FVector(0, -_fullExtent / 2, 50));

		FVector actorPos = GetActorLocation();
		FVector pivotPos = WestPivot->GetComponentLocation();

		FVector dir = (actorPos - pivotPos).GetSafeNormal();

		if (dir.X != 0.0)
		{
			WestPivot->FwdAligned = true;
		}
		else if (dir.Y != 0.0)
		{
			WestPivot->FwdAligned = false;
		}
	}
}

void AACSokobonPiece::UpdatePosition()
{
	PlaceAtCoordinate(CurrentTile->GridPos);
}

bool AACSokobonPiece::GetMovementBlockingOf(AACSokobonPiece* _otherPiece) const
{
	UClass* otherPieceClass = _otherPiece->StaticClass();

	return BlockMovementOf.Contains(otherPieceClass);
}


void AACSokobonPiece::ResetPosition()
{
	PlaceAtCoordinate(StartingTileIdx);
}

void AACSokobonPiece::SaveCurrentPositionAsStart()
{
	StartingTileIdx = CurrentTile->GridPos;
}

void AACSokobonPiece::PlaceAtCoordinate(FVector2D _coordinate)
{
	AACSokobonTile* targetTile = *GridRef->SokobonTiles.Find(_coordinate);
	
	if (CurrentTile)
	{
		CurrentTile->Occupied = false;
		CurrentTile->Navigatable = true;
		CurrentTile->RemoveBlockMovementOf(this);
		CurrentTile->OccupyingPieces.Remove(this);
	}

	CurrentTile = targetTile;
	CurrentTile->Occupied = true;
	CurrentTile->OccupyingPieces.Add(this);
	CurrentTile->AddBlockMovementOf(this);
	
	if (BlockMovement)
	{
		CurrentTile->Navigatable = false;
	}

	SetActorLocation(targetTile->GetActorLocation());
	OnPieceMove.Broadcast();
	AdditionalMovementLogic();
}

void AACSokobonPiece::MoveNorth()
{
	MoveToTileInDirection(Direction::NORTH);
}

void AACSokobonPiece::MoveEast()
{
	MoveToTileInDirection(Direction::EAST);
}

void AACSokobonPiece::MoveSouth()
{
	MoveToTileInDirection(Direction::SOUTH);
}

void AACSokobonPiece::MoveWest()
{
	MoveToTileInDirection(Direction::WEST);
}

void AACSokobonPiece::DeleteSokobonPiece()
{
	CurrentTile->RemoveBlockMovementOf(this);
	CurrentTile->OccupyingPieces.Remove(this);
	CurrentTile->Occupied = (CurrentTile->OccupyingPieces.Num() == 0);
	CurrentTile->Navigatable = !CurrentTile->Occupied;
	GridRef->CleanUpDeletedPiece(this);
	OnPieceDrop.Broadcast();
}

void AACSokobonPiece::HandlePieceHiding()
{
	CurrentTile->RemoveBlockMovementOf(this);
	CurrentTile->OccupyingPieces.Remove(this);
	CurrentTile->Occupied = (CurrentTile->OccupyingPieces.Num() == 0);
	CurrentTile->Navigatable = !CurrentTile->Occupied;
	OnPieceDrop.Broadcast();
	Movable = false;

	if (!bLetFall)
	{
		SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, (this->GetActorLocation().Z - 1000.0f)));
	}
	else
	{
		bEasing = true;
	}
}

TArray<TWeakObjectPtr<class UACSokobonPivotComponent>> AACSokobonPiece::GetPivotComponents() const
{
	return TArray<TWeakObjectPtr<class UACSokobonPivotComponent>>() = { NorthPivot,
																		EastPivot,
																		SouthPivot,
																		WestPivot };
}

void AACSokobonPiece::AdditionalMovementLogic() { }

void AACSokobonPiece::MoveToOverTime(class AACSokobonTile* _newTile, class AAControlCharacter* _char, Direction _dir, bool _backwards, bool _rotateToDir)
{
	//	fields used to update movement outside of this function if needed
	nextTile = _newTile;
	character = _char;
	Pulled = _backwards;
	RotateWish = _rotateToDir;
	if (RotateWish)
	{
		LerpStartQuat = FQuat(SK->GetComponentRotation());
	}
	TarDir = _dir;
	LerpStartPos = GetActorLocation();
	LerpStartRot = Mesh->GetComponentRotation();

	switch (TarDir)
	{
		//	East West - x axis / roll
		//	North South - y axis / pitch
	case Direction::NORTH:
		if (RotateWish)
		{

			//	make piece rotate in direction they are moving
			TarQuat = FQuat::MakeFromEuler(FVector(0, 0, -90));
		}
		if (bRollingPiece)
		{
			//	rolling axis / dir
			//	RollingAxis = -FVector::YAxisVector;
			//	assign the rolling quaternion target
			StartRollDegree = CurrentRollDegreeTarget;
			CurrentRollDegreeTarget += (RollDegreeDelta * -1);
			Rolltator = FRotator(CurrentRollDegreeTarget, 0.0f, 0.0f);
		}
		break;
	case Direction::EAST:
		if (RotateWish)
		{
			//	make piece rotate in direction they are moving
			TarQuat = FQuat::MakeFromEuler(FVector(0, 0, 0));
		}
		if (bRollingPiece)
		{
			//	rolling axis / dir
			//	RollingAxis = FVector::XAxisVector;
			//	assign the rolling quaternion target
			StartRollDegree = CurrentRollDegreeTarget;

			CurrentRollDegreeTarget += RollDegreeDelta;
			Rolltator = FRotator(0.0f, 0.0f, CurrentRollDegreeTarget);
		}



		break;
	case Direction::SOUTH:
		if (RotateWish)
		{
			TarQuat = FQuat::MakeFromEuler(FVector(0, 0, -270));
		}
		if (bRollingPiece)
		{
			//	RollingAxis = FVector::YAxisVector;
			StartRollDegree = CurrentRollDegreeTarget;

			CurrentRollDegreeTarget += RollDegreeDelta;
			Rolltator = FRotator(CurrentRollDegreeTarget, 0.0f, 0.0f);
		}

		break;
	case Direction::WEST:
		if (RotateWish)
		{
			TarQuat = FQuat::MakeFromEuler(FVector(0, 0, 180));
		}
		if (bRollingPiece)
		{
			//	RollingAxis = -FVector::XAxisVector;
			StartRollDegree = CurrentRollDegreeTarget;

			CurrentRollDegreeTarget += (RollDegreeDelta * -1);
			Rolltator = FRotator(0.0f, 0.0f, CurrentRollDegreeTarget);
		}

		break;
	}

	//	if we are marked to move until we're stopped, weren't pulled, and our last assigned direction is none
	if (MoveTilStopped && !Pulled && LastPushDir == Direction::NONE)
	{
		//	set last push dir!
		LastPushDir = _dir;
	}

	//	clear our existence from the tile we start in
	CurrentTile->Occupied = false;
	CurrentTile->Navigatable = true;
	CurrentTile->RemoveBlockMovementOf(this);
	CurrentTile->OccupyingPieces.Remove(this);

	//	put us in the next tile
	CurrentTile = _newTile;
	CurrentTile->Occupied = true;
	CurrentTile->OccupyingPieces.Add(this);
	CurrentTile->AddBlockMovementOf(this);

	//	block movement check
	if (BlockMovement)
	{
		CurrentTile->Navigatable = false;
	}

	//	if char was null (conveyor) reset the timer
	if (!_char)
	{
		ConveyorMoveTimer = 0.0f;
		bMoveInProgress = true;
	}

	//	broadcast move start
	OnMoveStart.Broadcast();
	moveWish = true;
}

void AACSokobonPiece::LerpCheck(float _elapsed, float _dur)
{
	//	ensure we aren't null
	if (this)
	{
		if (_elapsed <= _dur)
		{
			//	uncomment for debugging
			//	FVector cur = GetActorLocation();
			FVector tar = CurrentTile->GetActorLocation();
			//	uncomment for debugging
			//	FVector dest = { CurrentTile->GetActorLocation().X, CurrentTile->GetActorLocation().Y, GetActorLocation().Z };

			//	change how we lerp positions based on bEasingIn
			bEasingIn ? this->SetActorLocation(FMath::InterpExpoIn(LerpStartPos, tar, _elapsed / _dur)) : this->SetActorLocation(FMath::Lerp(LerpStartPos, tar, _elapsed / _dur));
			
			if (bRollingPiece)
			{
				float rollyAlpha = _elapsed / _dur;
				bEasingIn ? CurrentRollDegree = FMath::InterpExpoIn(StartRollDegree, CurrentRollDegreeTarget, _elapsed / _dur) : CurrentRollDegree = FMath::Lerp(StartRollDegree, CurrentRollDegreeTarget, _elapsed / _dur);
				if (NSMovement)
				{
					Mesh->SetWorldRotation(FRotator(CurrentRollDegree, 0.0f, 0.0f));
				}
				else
				{
					Mesh->SetWorldRotation(FRotator(0.0f, 0.0f, CurrentRollDegree));

				}
				//	lerp with rotations was weird, updating pitch/roll/yaw directly then recreating an FRotator with that as the only nonzero value
				//	Mesh->SetWorldRotation(FQuat::Slerp(FQuat(LerpStartRot), FQuat(Rolltator), rollyAlpha));
			}
		}
		else
		{
			ConveyorMoveTimer = 0.0f;
			//	reset flags
			bMoveInProgress = false;
			moveWish = false;
			//	broadcast move complete
			OnPieceMove.Broadcast();
			this->SetActorLocation(CurrentTile->GetActorLocation());
			if (bRollingPiece)
			{
				Mesh->SetWorldRotation(Rolltator);
			}
			AdditionalMovementLogic();


			//	check if we should keep moving:
			//	are we set to move til stopped, were we pushed, and was our last push direction valid
			if (MoveTilStopped && !Pulled && LastPushDir != Direction::NONE)
			{
				//	drop the piece
				OnPieceDrop.Broadcast();
				//	stop ease in since we're already moving
				bEasingIn = false;
				//	get the next tile
				auto next = CurrentTile->GetValidTileInDirection(LastPushDir, false, false, CurrentTile, this);

				//	if valid, move to it
				if (next)
				{
					MoveToOverTime(next, nullptr, LastPushDir, Pulled, false);
				}

				//	otherwise, there is no move
				else
				{
					LastPushDir = Direction::NONE;
				}
			}
		}
	}
}

void AACSokobonPiece::InitSokoPivots()
{
	if (NSMovement)
	{
		//	check north
		if (!NorthPivot)
		{
			NorthPivot = CreateDefaultSubobject<UACSokobonPivotComponent>(TEXT("NorthPivot"));
			NorthPivot->SetupAttachment(RootComponent);
			NorthPivot->OwningPiece = this;
			NorthPivot->FwdAligned = true;
		}
		//	check south
		if (!SouthPivot)
		{ 
			SouthPivot = CreateDefaultSubobject<UACSokobonPivotComponent>(TEXT("SouthPivot"));
			SouthPivot->SetupAttachment(RootComponent);
			SouthPivot->OwningPiece = this;
			SouthPivot->FwdAligned = true;
		}
	}
	else
	{
		if (NorthPivot)
		{
			NorthPivot->DestroyComponent();
		}
		if (SouthPivot)
		{
			SouthPivot->DestroyComponent();
		}
	}

	if (EWMovement)
	{
		if (!EastPivot)
		{
			EastPivot = CreateDefaultSubobject<UACSokobonPivotComponent>(TEXT("EastPivot"));
			EastPivot->SetupAttachment(RootComponent);
			EastPivot->OwningPiece = this;
		}

		if (!WestPivot)
		{
			WestPivot = CreateDefaultSubobject<UACSokobonPivotComponent>(TEXT("WestPivot"));
			WestPivot->SetupAttachment(RootComponent);
			WestPivot->OwningPiece = this;
		}
	}
	else
	{
		if (EastPivot)
		{
			EastPivot->DestroyComponent();
		}

		if (WestPivot)
		{
			WestPivot->DestroyComponent();
		}
	}
}