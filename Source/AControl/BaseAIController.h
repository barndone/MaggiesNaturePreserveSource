// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShouldUpdateBlackboard);

UCLASS()
class ACONTROL_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAISenseConfig_Sight* AISenseConfigSight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAISenseConfig_Hearing* AISenseConfigHearing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USphereComponent* WanderRadius;

	bool IsMoving = false;

	//	UPROPERTY()
	//	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBehaviorTreeComponent* BehaviorComp;

	virtual void OnPossess(class APawn* InPawn) override;

	virtual void BeginInactiveState() override;

	FGenericTeamId TeamId = FGenericTeamId(0);

	UPROPERTY(EditAnywhere)
		float SightFOV = 45.0f;

	UPROPERTY(EditAnywhere)
		float SightRadius = 500.0f;

	float PathfindingAcceptanceRadius = 1.0f;
	//UPROPERTY(VisibleAnywhere)
	class ABaseAIAgent* pawn = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsAwake = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InitialSleepChance = 0.3f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InitialWakeUpChance = 0.2f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SleepDelta = 0.001f;
	UPROPERTY(BlueprintReadOnly)
	float CurSleepChance = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float CurWakeUpChance = 0.0f;
	
	UFUNCTION(BlueprintCallable)
	void IncrementSleepChances();

	UFUNCTION(BlueprintCallable)
	void HandleIsAwakeChange();

	UFUNCTION(BlueprintCallable)
	class ABaseAIAgent* GetAgent() const { return pawn; }

	UPROPERTY(BlueprintAssignable)
	FShouldUpdateBlackboard ShouldUpdateBlackboard;

	UFUNCTION()
	void BroadcastBlackboardUpdateWish();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus stimulus);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& other) const override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	FAIMoveRequest InitMoveRequest(const AActor* _targetActor);
	FAIMoveRequest InitMoveRequest(const FVector& _targetLocation);


	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& result) override;

};
