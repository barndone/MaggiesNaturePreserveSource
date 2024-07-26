// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonPipe.h"
#include "ACSokobonGrid.h"
#include "ACPipeConnection.h"

AACSokobonPipe::AACSokobonPipe()
{
	Entry = CreateDefaultSubobject<UACPipeConnection>(TEXT("EntryConnection"));
	Entry->SetupAttachment(Mesh);
	Exit = CreateDefaultSubobject<UACPipeConnection>(TEXT("ExitConnection"));
	Exit->SetupAttachment(Mesh);
}

void AACSokobonPipe::BeginPlay()
{
	Super::BeginPlay();
	Entry->SetPipe(this);
	Exit->SetPipe(this);

	Entry->OnComponentBeginOverlap.AddDynamic(this, &AACSokobonPipe::PipeConnectionEntry);
	Exit->OnComponentBeginOverlap.AddDynamic(this, &AACSokobonPipe::PipeConnectionEntry);

	Entry->OnComponentEndOverlap.AddDynamic(this, &AACSokobonPipe::PipeConnectionExit);
	Exit->OnComponentEndOverlap.AddDynamic(this, &AACSokobonPipe::PipeConnectionExit);

}

void AACSokobonPipe::PipeConnectionEntry(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _hit)
{
	if (_otherActor != this)
	{
		UACPipeConnection* otherConnection = Cast<UACPipeConnection>(_otherComponent);
		UACPipeConnection* thisConnection = Cast<UACPipeConnection>(_overlappedComponent);

		if (thisConnection && otherConnection && thisConnection != otherConnection)
		{
			otherConnection->SetConnection(thisConnection);
			thisConnection->SetConnection(otherConnection);
			UpdateConnections();
		}
	}
}

void AACSokobonPipe::PipeConnectionExit(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex)
{
	if (_otherActor != this)
	{
		UACPipeConnection* otherConnection = Cast<UACPipeConnection>(_otherComponent);
		UACPipeConnection* thisConnection = Cast<UACPipeConnection>(_overlappedComponent);

		if (thisConnection && otherConnection && thisConnection != otherConnection)
		{
			otherConnection->SetConnection(nullptr);
			thisConnection->SetConnection(nullptr);
			UpdateConnections();
		}
	}
}

void AACSokobonPipe::AdditionalMovementLogic()
{
	UpdateConnections();
	OnPipeMove.Broadcast();
}

void AACSokobonPipe::UpdateConnections()
{
	Entry->GetConnection() ? Previous = Entry->GetConnection()->GetPipe() : Previous = nullptr;
	Exit->GetConnection() ? Next = Exit->GetConnection()->GetPipe() : Next = nullptr;
}

void AACSokobonPipe::RotateClockwiseNinety()
{
	//	z axis :3
	Mesh->AddLocalRotation(FQuat::MakeFromEuler(FVector(0, 0, 90)));
	//	AddActorWorldRotation(FQuat::MakeFromEuler(FVector(0,0,90)));
}

void AACSokobonPipe::RotateCtClockwiseNinety()
{
	//	z axis :3
	Mesh->AddLocalRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)));
	//	AddActorWorldRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)));
}

void AACSokobonPipe::SetAsEntry()
{
	if(GridRef.Get())
	{
		GridRef->Start = this;
	}
	else
	{
		//	get grid :3
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->Start = this;
	}
}

void AACSokobonPipe::SetAsExit()
{
	if (GridRef.Get())
	{
		GridRef->End = this;
	}
	else
	{
		//	get grid :3
		GridRef = Cast<AACSokobonGrid>(RootComponent->GetAttachParent()->GetAttachmentRootActor());
		GridRef->End = this;
	}

}