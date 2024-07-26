// Fill out your copyright notice in the Description page of Project Settings.


#include "ACLoadZone.h"
#include "ACGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "AControlCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AACLoadZone::AACLoadZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	auto root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	root->SetupAttachment(RootComponent);

	LoadTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("LoadZone"));
	LoadTrigger->SetupAttachment(root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(root);


}

// Called when the game starts or when spawned
void AACLoadZone::BeginPlay()
{
	Super::BeginPlay();
	
	LoadTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &AACLoadZone::OverlapBegins);
}

void AACLoadZone::OverlapBegins(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _hit)
{
	if (_overlappedComponent == LoadTrigger)
	{
		AAControlCharacter* charCheck = Cast<AAControlCharacter>(_otherActor);

		if (charCheck)
		{
			UCapsuleComponent* charCapsule = Cast<UCapsuleComponent>(_otherComponent);

			if (charCapsule)
			{
				if (LevelToLoad != "Hub_Map_greybox")
				{
					auto instance = Cast<UACGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
					instance->PlayerStartTag = LevelToLoad.ToString();
				}
				UGameplayStatics::OpenLevel((UObject*)GetGameInstance(), LevelToLoad);
			}
		}
	}
}

