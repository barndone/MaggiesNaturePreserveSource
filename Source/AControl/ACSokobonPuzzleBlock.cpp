// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonPuzzleBlock.h"
#include "ACSokobonHole.h"
#include "ACSokobonTile.h"
#include "ACSokobonConveyor.h"

void AACSokobonPuzzleBlock::FitIntoPlace()
{
	HandlePieceHiding();
	if (!bEasing)
	{
		OnHoleFill.Broadcast(CurrentTile);
	}

	if (bLog)
	{
		tar = TargetActor->GetActorLocation();
		tarRot = TargetActor->GetActorRotation();
		startRot = GetActorRotation();
	}
	else
	{
		tar = DropOffset + GetActorLocation();
	}
	//tar = DropOffset + GetActorLocation();
	LerpStartPos = GetActorLocation();

}

void AACSokobonPuzzleBlock::AdditionalMovementLogic()
{
	Super::AdditionalMovementLogic();

	CheckForOverlapPiece();
}

void AACSokobonPuzzleBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEasing)
	{
		DropTimer += DeltaTime;

		if (DropTimer >= DropDuration)
		{
			bEasing = false;
			OnHoleFill.Broadcast(CurrentTile);
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 1000));
			bEasing = false;
		}
		else
		{
			SetActorLocation(FMath::InterpExpoIn(LerpStartPos, tar, DropTimer / DropDuration));
			if (bLog)
			{
				SetActorRotation(FMath::InterpExpoIn(startRot, tarRot, DropTimer / DropDuration));
			}
		}
	}
}

void AACSokobonPuzzleBlock::CheckForOverlapPiece()
{
	if (CurrentTile->OccupyingPieces.Num() > 1)
	{
		for (auto& val : CurrentTile->OccupyingPieces)
		{
			AACSokobonHole* blocker = Cast<AACSokobonHole>(val);

			if (blocker && !blocker->GetFilled())
			{
				if (MoveTilStopped)
				{
					LastPushDir = Direction::NONE;
				}

				OnFallStart.Broadcast(blocker);
				FitIntoPlace();
				break;
			}

			AACSokobonConveyor* conveyor = Cast<AACSokobonConveyor>(val);

			if (conveyor)
			{

				OnPieceDrop.Broadcast();
				if (MoveTilStopped)
				{
					LastPushDir = Direction::NONE;
				}

				bRollingPiece = false;
				bEasingIn = false;
				conveyor->MoveOverlappingPieceInFlowDir(this);
				OnConveyorStart.Broadcast();
				break;
			}
		}
	}
}
