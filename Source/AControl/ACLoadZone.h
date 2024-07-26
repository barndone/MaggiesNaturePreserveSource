// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACLoadZone.generated.h"

UCLASS()
class ACONTROL_API AACLoadZone : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UBoxComponent* LoadTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OverlapBegins(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex,
			bool _bFromSweep,
			const FHitResult& _hit);

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

public:	
	// Sets default values for this actor's properties
	AACLoadZone();
};
