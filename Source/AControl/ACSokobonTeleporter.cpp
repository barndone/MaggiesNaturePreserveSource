// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonTeleporter.h"
#include "ACSokobonTile.h"
#include "Components/BoxComponent.h"
#include "BaseAIAgent.h"
#include "ACSokobonGrid.h"
#include "ITeleportable.h"
#include "AControlCharacter.h"
#include "ACSokobonPuzzleBlock.h"
#include "ACSokobonObjective.h"
#include "Components/CapsuleComponent.h"

AACSokobonTeleporter::AACSokobonTeleporter()
{
	//	TODO bind on overlap event for Teleport Area to TeleportAnimal
	TeleportArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TeleportZone"));
	TeleportArea->SetupAttachment(RootComponent);


	TeleportOutLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportOutLocation"));
	TeleportOutLocation->SetupAttachment(TeleportArea);
}

void AACSokobonTeleporter::BeginPlay()
{
	Super::BeginPlay();

	TeleportArea->OnComponentBeginOverlap.AddUniqueDynamic(this, &AACSokobonTeleporter::TeleporterEnter);
	TeleportArea->OnComponentEndOverlap.AddUniqueDynamic(this, &AACSokobonTeleporter::TeleporterExit);

	//CheckForBlockage();
}

void AACSokobonTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AACSokobonTeleporter::TeleporterEnter(UPrimitiveComponent* _overlappedComponent,
	AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _hit)
{
	//	ABaseAIAgent

	//	phase 1: check that it was an overlapping animal
	ITeleportable* overlappingAnimal = Cast<ITeleportable>(_otherActor);
	if (overlappingAnimal)
	{
		AAControlCharacter* charTest = Cast<AAControlCharacter>(_otherActor);

		if (charTest)
		{
			UCapsuleComponent* playerCapsule = Cast<UCapsuleComponent>(_otherComponent);

			if (playerCapsule)
			{
				if (OutTeleporter.Get())
				{
					//	handle teleportation
					overlappingAnimal->HandleTeleport(this);
				}

				//	check that this teleporter is the one that initiated the teleportation
				if ((overlappingAnimal->TeleporterRef.Get() == this) && OutTeleporter.Get())
				{
					OnTeleportStart.Broadcast();
					_otherActor->SetActorLocationAndRotation(
						OutTeleporter.Get()->TeleportOutLocation->GetComponentLocation(),			//	location of teleport out loc
						OutTeleporter.Get()->TeleportOutLocation->GetComponentRotation());			//	rotation of teleport out loc
				}
			}
		}

		else
		{
			AACSokobonPuzzleBlock* block = Cast<AACSokobonPuzzleBlock>(_otherActor);
			if (block && AllowPuzzleBlockTeleporting)
			{
				if (OutTeleporter.Get())
				{
					//	handle teleportation
					overlappingAnimal->HandleTeleport(this);
				}
			}
			else if (!block)
			{
				if (OutTeleporter.Get())
				{
					//	handle teleportation
					overlappingAnimal->HandleTeleport(this);
				}
			}


			//	check that this teleporter is the one that initiated the teleportation
			if (overlappingAnimal->TeleporterRef.Get() == this && OutTeleporter.Get())
			{
				AACSokobonPiece* piece = Cast<AACSokobonPiece>(_otherActor);
				if (piece)
				{
					OnTeleportStart.Broadcast();
					piece->OnPieceDrop.Broadcast();
					piece->PlaceAtCoordinate(OutTeleporter.Get()->CurrentTile->GridPos);
				}
			}
		}
	}
}

void AACSokobonTeleporter::TeleporterExit(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex)
{
	ITeleportable* overlappingAnimal = Cast<ITeleportable>(_otherActor);
	if (overlappingAnimal)
	{
		AAControlCharacter* charTest = Cast<AAControlCharacter>(_otherActor);

		if (charTest)
		{
			UCapsuleComponent* playerCapsule = Cast<UCapsuleComponent>(_otherComponent);

			if (playerCapsule)
			{
				//	phase 2: check that this isn't the teleporter that started teleportation
				if (charTest->TeleporterRef.Get() != this)
				{
					charTest->HandleTeleportEnd();
				}
			}
		}
		else
		{
			AACSokobonPuzzleBlock* block = Cast<AACSokobonPuzzleBlock>(_otherActor);
			if (block && AllowPuzzleBlockTeleporting)
			{
				//	phase 2: check that this isn't the teleporter that started teleportation
				if (overlappingAnimal->TeleporterRef.Get() != this)
				{
					overlappingAnimal->HandleTeleportEnd();
				}
			}
			else if (!block)
			{
				//	phase 2: check that this isn't the teleporter that started teleportation
				if (overlappingAnimal->TeleporterRef.Get() != this)
				{
					overlappingAnimal->HandleTeleportEnd();
				}
			}
		}
	}
}

void AACSokobonTeleporter::CheckForBlockage()
{
	if (CurrentTile->OccupyingPieces.Num() > 1)
	{
		for (auto& val: CurrentTile->OccupyingPieces)
		{
			AACSokobonPuzzleBlock* blocker = Cast<AACSokobonPuzzleBlock>(val);

			if (blocker)
			{
				if (!Blocked)
				{
					OnTeleportBlocked.Broadcast();
				}
				Blocked = true;
				GridRef->LinkPortals();
			}
		}

	}
	else
	{
		if (Blocked)
		{
			OnTeleportUnblocked.Broadcast();
		}
		Blocked = false;
		GridRef->LinkPortals();
	}
}

void AACSokobonTeleporter::SetOutTeleporter(TWeakObjectPtr<AACSokobonTeleporter> _value)
{
	OutTeleporter = _value;
}
