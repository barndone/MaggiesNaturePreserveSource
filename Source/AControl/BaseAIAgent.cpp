// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIAgent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "CritterPatrolPoint.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseAIAgent::ABaseAIAgent()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseAIAgent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseAIAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseAIAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ABaseAIAgent::StartWakeup()
{
	OnWakeupWish.Broadcast();
}
