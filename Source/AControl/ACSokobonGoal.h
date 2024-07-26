// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPiece.h"
#include "ACSokobonGoal.generated.h"

/*
 * AKA the point where animals become unpretrified
 */
UCLASS()
class ACONTROL_API AACSokobonGoal : public AACSokobonPiece
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideOnClear = false;
	
};
