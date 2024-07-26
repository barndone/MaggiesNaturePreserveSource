// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseAIAgent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/SphereComponent.h"

ABaseAIController::ABaseAIController()
{
	//	create an AI Perception component on this controller
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");

	//	create a sight sense config for this controller
	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>("SenseSight");
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = false;

	AISenseConfigSight->PeripheralVisionAngleDegrees = SightFOV;
	AISenseConfigSight->SightRadius = SightRadius;

	//	create a hearing sense config for this controller
	AISenseConfigHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("SenseHearing");
	AISenseConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigHearing->DetectionByAffiliation.bDetectNeutrals = false;

	//	configure the senses of this perception component
	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigHearing);

	//	set the dominant sense to sight
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTree");
	WanderRadius = CreateDefaultSubobject<USphereComponent>("WanderRadius");
	WanderRadius->SetupAttachment(RootComponent);
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	pawn = Cast<ABaseAIAgent>(GetPawn());
	check(pawn && "Paired pawn is not derived of BaseAIAgent");

	pawn->OnWakeupWish.AddDynamic(this, &ABaseAIController::HandleIsAwakeChange);
}

void ABaseAIController::BeginInactiveState()
{
	Super::BeginInactiveState();
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, 
		&ABaseAIController::OnTargetPerceptionUpdated_Delegate);

	CurSleepChance = InitialSleepChance;
	CurWakeUpChance = InitialWakeUpChance;
}

void ABaseAIController::OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus stimulus)
{
	switch (stimulus.Type)
	{
	case 0:

		if (!IsMoving)
		{
			//	MoveTo(InitMoveRequest());
			//IsMoving = true;
		}
		//	TODO: react to sight

#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Reacting To Sight");
		}
#endif


		break;

	case 1:

		//	TODO: react to hearing

#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Reacting To Hearing");
		}
#endif



		break;
	}
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& other) const
{
	if (APawn const* otherPawn = Cast<APawn>(&other))
	{
		if (auto const teamAgent = Cast<IGenericTeamAgentInterface>(otherPawn->GetController()))
		{
			if (teamAgent->GetGenericTeamId() == FGenericTeamId(0))
			{
				return ETeamAttitude::Friendly;
			}
			else { return ETeamAttitude::Hostile; }
		}
	}

	return ETeamAttitude::Neutral;
}

FGenericTeamId ABaseAIController::GetGenericTeamId() const
{
	return TeamId;
}

FAIMoveRequest ABaseAIController::InitMoveRequest(const AActor* _targetActor)
{
	FAIMoveRequest moveRequest;

	moveRequest.SetGoalActor(_targetActor);
	moveRequest.SetAcceptanceRadius(PathfindingAcceptanceRadius);
	moveRequest.SetReachTestIncludesAgentRadius(true);
	moveRequest.SetUsePathfinding(true);
	moveRequest.SetCanStrafe(true);
	moveRequest.SetAllowPartialPath(true);

	return moveRequest;
}

FAIMoveRequest ABaseAIController::InitMoveRequest(const FVector& _targetLocation)
{
	FAIMoveRequest moveRequest;

	moveRequest.SetGoalLocation(_targetLocation);
	moveRequest.SetAcceptanceRadius(PathfindingAcceptanceRadius);
	moveRequest.SetReachTestIncludesAgentRadius(true);
	moveRequest.SetUsePathfinding(true);
	moveRequest.SetCanStrafe(true);
	moveRequest.SetAllowPartialPath(true);

	return moveRequest;
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& result)
{
//	
}

void ABaseAIController::IncrementSleepChances()
{
	pawn->GetAwake() ? CurSleepChance += SleepDelta : CurWakeUpChance += SleepDelta;
}

void ABaseAIController::HandleIsAwakeChange()
{
	pawn->SetAwake(!pawn->GetAwake());
	pawn->GetAwake() ? CurSleepChance = InitialSleepChance : CurWakeUpChance = InitialWakeUpChance;
	BroadcastBlackboardUpdateWish();
}

void ABaseAIController::BroadcastBlackboardUpdateWish()
{
	ShouldUpdateBlackboard.Broadcast();
}
