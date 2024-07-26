// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ACTutorial.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACTutorial : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName TutorialName;
	UPROPERTY(EditAnywhere)
	FText TutorialText;
	bool bAdded = false;

	UPROPERTY(EditAnywhere)
	FText ActorArrowTag;

};
