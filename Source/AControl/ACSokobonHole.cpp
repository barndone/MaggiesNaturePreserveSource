// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonHole.h"
#include "Components/BoxComponent.h"
#include "ACSokobonTile.h"

AACSokobonHole::AACSokobonHole()
{
	Mesh->SetStaticMesh(UnfulfilledMesh);
	BlockMovement = false;
	Movable = false;

	PlayerBlockingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerBlocker"));
	PlayerBlockingCollision->SetupAttachment(RootComponent);

	DustPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MatPlane"));
	DustPlane->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AACSokobonHole::BeginPlay()
{
	Super::BeginPlay();
	UpdateMesh();

	BlockMovementOfPreFilled = BlockMovementOf;
	DustPlane->SetVisibility(false);
}

void AACSokobonHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowDust)
	{
		DustTimer += DeltaTime;
		if (DustTimer >= DustDur)
		{
			OnFadeDust.Broadcast();
			bShowDust = false;
		}
	}
}

void AACSokobonHole::FillHole(AACSokobonTile* _curTile)
{
	if (_curTile == CurrentTile)
	{
		CurrentTile->Navigatable = true;
		IsFilled = true;
		UpdateMesh();
		BlockMovementOf = BlockMovementOfPostFilled;
		CurrentTile->UpdateBlockMovementOf();
		SetActorEnableCollision(false);
		
	}

}

void AACSokobonHole::ResetHole()
{
	CurrentTile->Navigatable = false;
	IsFilled = false;
	UpdateMesh();
	SetActorEnableCollision(true);
	BlockMovementOf = BlockMovementOfPreFilled;
	CurrentTile->UpdateBlockMovementOf();
}

void AACSokobonHole::UpdateMesh()
{
	IsFilled ? Mesh->SetStaticMesh(FulfilledMesh) : Mesh->SetStaticMesh(UnfulfilledMesh);
}

void AACSokobonHole::ShowDustCloud(AACSokobonHole* _in)
{
	if (_in && _in == this)
	{
		ShowDust.Broadcast();
	}
}
