// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ACSokobonTeleporter.h"
#include "CoreMinimal.h"
#include "ITeleportable.generated.h"

/**
 * 
 */
UINTERFACE()
class ACONTROL_API UTeleportable : public UInterface
{
	GENERATED_BODY()
};

class ACONTROL_API ITeleportable
{
	GENERATED_BODY()
public:

	//	keep track of teleporter this piece entered from so it doesn't infinitely teleport
	TWeakObjectPtr<class AACSokobonTeleporter> TeleporterRef = nullptr;

	virtual void HandleTeleport(AACSokobonTeleporter*_inPortal)
	{
		if (!TeleporterRef.Get())
		{
			TeleporterRef = _inPortal;
		}
	}
	virtual void HandleTeleportEnd()
	{
		TeleporterRef = nullptr;
	}
};
