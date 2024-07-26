// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ITeleportable.h"
#include "BaseAIAgent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTagCatch, ABaseAIAgent*, self);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTagJoin, ABaseAIAgent*, self);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTagLeave, ABaseAIAgent*, self);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWakeupWish);

UCLASS()
class ACONTROL_API ABaseAIAgent : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool IsAwake = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Sets default values for this pawn's properties
	ABaseAIAgent();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY()
	FOnWakeupWish OnWakeupWish;

	UPROPERTY(BlueprintAssignable)
	FOnStateUpdate OnStateUpdate;

	void StartWakeup();


	UFUNCTION(BlueprintCallable)
	void SetAwake(bool _val) { IsAwake = _val; }
	UFUNCTION(BlueprintCallable)
	bool GetAwake() const { return IsAwake; }
};
