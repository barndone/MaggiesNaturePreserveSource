// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPuzzleBlock.h"
#include "ACSokobonPipe.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPipeMove);
/**
 * 
 */
UCLASS()
class ACONTROL_API AACSokobonPipe : public AACSokobonPuzzleBlock
{
	GENERATED_BODY()
		

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void AdditionalMovementLogic() override;

	UFUNCTION()
		void PipeConnectionEntry(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex,
			bool _bFromSweep,
			const FHitResult& _hit);

	UFUNCTION()
		void PipeConnectionExit(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UACPipeConnection* Entry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UACPipeConnection* Exit;

	AACSokobonPipe();

	UPROPERTY(VisibleAnywhere)
	AACSokobonPipe* Previous = nullptr;
	UPROPERTY(VisibleAnywhere)
	AACSokobonPipe* Next = nullptr;

	UFUNCTION()
	void UpdateConnections();

	UFUNCTION(CallInEditor, Category = "ACSokobonPipe")
		void RotateClockwiseNinety();
	UFUNCTION(CallInEditor, Category = "ACSokobonPipe")
		void RotateCtClockwiseNinety();
	UFUNCTION(CallInEditor, Category = "ACSokobonPipe")
		void SetAsEntry();
	UFUNCTION(CallInEditor, Category = "ACSokobonPipe")
		void SetAsExit();

	UPROPERTY()
	FOnPipeMove OnPipeMove;
};
