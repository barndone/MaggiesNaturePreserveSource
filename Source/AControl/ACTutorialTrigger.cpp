// Fill out your copyright notice in the Description page of Project Settings.


#include "ACTutorialTrigger.h"
#include "ACGameInstance.h"
#include "AControlCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AACTutorialTrigger::AACTutorialTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));

}

// Called when the game starts or when spawned
void AACTutorialTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &AACTutorialTrigger::TutorialTriggerEnter);
}

// Called every frame
void AACTutorialTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AACTutorialTrigger::TutorialTriggerEnter(UPrimitiveComponent* _overlappedComponent,
	AActor* _otherActor,
	UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _hit)
{
	if (_otherActor != this)
	{
		AAControlCharacter* character = Cast<AAControlCharacter>(_otherActor);
		if (character)
		{
			UCapsuleComponent* charColl = Cast<UCapsuleComponent>(_otherComponent);

			if (charColl)
			{
				UACGameInstance* instance = Cast<UACGameInstance>(GetGameInstance());

				//	iterate through list of tutorials (in the event this trigger has multiple tutorials bound to it)
				for (auto& tutorial : PairedTutorials)
				{
					instance->AddTutorial(tutorial);
				}
			}
		}
	}
}