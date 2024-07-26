// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPiece.h"
#include "ITeleportable.h"
#include "ACSokobonPuzzleBlock.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoleFill, class AACSokobonTile*, _curTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConveyorStart);

UCLASS()
class ACONTROL_API AACSokobonPuzzleBlock : public AACSokobonPiece, public ITeleportable
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector DropOffset;
	FVector tar;
	FRotator tarRot;
	FRotator startRot;

	float DropTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DropDuration = 0.25f;

protected:
	virtual void AdditionalMovementLogic() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(BlueprintAssignable)
	FOnHoleFill OnHoleFill;

	UPROPERTY(BlueprintAssignable)
		FOnConveyorStart OnConveyorStart;

	void FitIntoPlace();

	void CheckForOverlapPiece();
};
