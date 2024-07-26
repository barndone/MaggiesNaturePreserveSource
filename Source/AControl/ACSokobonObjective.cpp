// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonObjective.h"
#include "ACSokobonTile.h"
#include "ACSokobonGoal.h"
#include "ACSokobonTeleporter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Particles/ParticleSystemComponent.h"

void AACSokobonObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMovingToTarget)
	{
		if (!bMoveComponent)
		{
			if (TargetMoveTimer >= TargetMoveDur)
			{
				SetActorTransform(GoalReachedLocaiton->GetTransform());
				bMovingToTarget = false;
				OnGoalReached.Broadcast();
			}
			else
			{
				float alpha = TargetMoveTimer / TargetMoveDur;
				SetActorLocation(FMath::InterpExpoIn(StartLoc, GoalReachedLocaiton->GetActorLocation(), alpha));
				SetActorRotation(FMath::InterpExpoIn(StartRot, GoalReachedLocaiton->GetActorRotation(), alpha));
				TargetMoveTimer += DeltaTime;
			}
		}
		else
		{
			if (TargetMoveTimer >= TargetMoveDur)
			{
				if (PS)
				{
					PS->SetWorldTransform(GoalReachedLocaiton->GetTransform());
				}
				bMovingToTarget = false;
				OnGoalReached.Broadcast();
			}
			else
			{
				if (PS)
				{
					float alpha = TargetMoveTimer / TargetMoveDur;
					PS->SetWorldLocation(FMath::InterpExpoIn(StartLoc, GoalReachedLocaiton->GetActorLocation(), alpha));
					PS->SetWorldRotation(FMath::InterpExpoIn(StartRot, GoalReachedLocaiton->GetActorRotation(), alpha));
				}
				TargetMoveTimer += DeltaTime;
			}
		}
	}
}

void AACSokobonObjective::AdditionalMovementLogic()
{
	CheckCurTile();
}

void AACSokobonObjective::FreeAnimal()
{
	this->HandleAnimalGoalReached();
}

void AACSokobonObjective::CheckCurTile()
{
	bool free = false;

	//	broad phase: is this tile occupied by another piece
	if (CurrentTile->OccupyingPieces.Num()>1)
	{
		//	if so: compare against the other piece(s)

		for (auto& val : CurrentTile->OccupyingPieces)
		{
			if (val == this) { continue; }

			TWeakObjectPtr<AACSokobonGoal> goal = Cast<AACSokobonGoal>(val);

			if (goal.Get())
			{
				if (SwapMeshOnFree)
				{
					SKSwap();
					bSwappingMesh = true;
				}
				if (goal.Get()->bHideOnClear)
				{
					OnGoalReached.AddDynamic(goal.Get(), &AACSokobonPiece::HandlePieceHiding);
				}
				free = true;
			}
		}
	}

	if (free && !bSwappingMesh)
	{
		FreeAnimal();
	}
}

void AACSokobonObjective::SKSwap()
{
	TArray<USkeletalMeshComponent*> _;
	this->GetComponents(_);
	
	if (_.Num() != 0)
	{
		_[0]->SetSkeletalMesh(SwapSK);
		_[0]->SetAnimInstanceClass(SwapABP.Get());
	}
}

void AACSokobonObjective::HandleAnimalGoalReached()
{
	CurrentTile->RemoveBlockMovementOf(this);
	CurrentTile->OccupyingPieces.Remove(this);
	CurrentTile->Occupied = (CurrentTile->OccupyingPieces.Num() == 0);
	CurrentTile->Navigatable = !CurrentTile->Occupied;
	OnPieceDrop.Broadcast();
	Movable = false;

	bMovingToTarget = true;

	if (bMoveComponent)
	{
		PS = Cast<UParticleSystemComponent>(GetComponentByClass(UParticleSystemComponent::StaticClass()));
	}

	StartLoc = GetActorLocation();
	StartRot = GetActorRotation();
}

void AACSokobonObjective::InitAnimalAI()
{
	SpawnParams.OverrideLevel = GetLevel();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABaseAIAgent* spawnedAI = Cast<ABaseAIAgent>(GetWorld()->SpawnActor(AnimalAIBP, &GoalReachedLocaiton->GetActorTransform(), SpawnParams));
	SetActorLocation(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, (this->GetActorLocation().Z - 1000.0f)));
}














