// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SokobonUtils.h"
#include "ACSokobonPiece.h"
#include "ACSokobonConveyor.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API AACSokobonConveyor : public AACSokobonPiece
{
	GENERATED_BODY()



public:
	UPROPERTY(EditAnywhere)
	Direction FlowDir;

	void MoveOverlappingPieceInFlowDir(AACSokobonPiece* _overlappingPiece);
};
