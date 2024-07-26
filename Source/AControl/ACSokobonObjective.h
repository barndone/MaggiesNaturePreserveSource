// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPiece.h"
#include "ITeleportable.h"
#include "ACSokobonObjective.generated.h"

/*
 * AKA the animal representation on the sokobon tile system
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGoalReached);

UCLASS()
class ACONTROL_API AACSokobonObjective : public AACSokobonPiece
{
	FActorSpawnParameters SpawnParams;
	GENERATED_BODY()

	bool IsFrozen = false;
	bool Freeing = false;

	bool bMovingToTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TargetMoveDur = 1.0f;
	float TargetMoveTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool SwapMeshOnFree = false;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//	class USkeletalMesh* InitialMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "SwapMeshOnFree", EditConditionHides))
	class USkeletalMesh* SwapSK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", EditCondition = "SwapMeshOnFree", EditConditionHides))
		TSubclassOf<class UAnimInstance> SwapABP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundWave* GoalReachedSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* GoalReachedLocaiton = nullptr;
	FVector StartLoc;
	FRotator StartRot;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bMoveComponent = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PS = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TSubclassOf<class ABaseAIAgent> AnimalAIBP;

	bool bSwappingMesh = false;
protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void AdditionalMovementLogic() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BeckonMe = false;
	
	UFUNCTION(BlueprintCallable)
	void FreeAnimal();

	//	checks the current tile after this piece is moved, if it is a goal tile, it will call the FreeAnimal func
	void CheckCurTile();

	void SKSwap();
	void HandleAnimalGoalReached();
	void InitAnimalAI();

	UPROPERTY(BlueprintAssignable)
		FOnGoalReached OnGoalReached;
};
