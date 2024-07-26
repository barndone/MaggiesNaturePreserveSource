// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACTutorial.h"
#include "GameFramework/Actor.h"
#include "ACTutorialTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialEnter, FText, _text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialExit);

UCLASS()
class ACONTROL_API AACTutorialTrigger : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerArea;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UACTutorial*> PairedTutorials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText TutorialText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName TutorialName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool DisappearOnTriggerExit = false;
public:	
	// Sets default values for this actor's properties
	AACTutorialTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void TutorialTriggerEnter(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex,
			bool _bFromSweep,
			const FHitResult& _hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UBoxComponent* GetTriggerArea() const { return TriggerArea; }

	UPROPERTY(BlueprintAssignable)
	FOnTutorialEnter OnTutorialEnter;
	UPROPERTY(BlueprintAssignable)
	FOnTutorialExit OnTutorialExit;

	UFUNCTION(BlueprintCallable)
	TArray<UACTutorial*> GetTutorials() const { return PairedTutorials; }

};
