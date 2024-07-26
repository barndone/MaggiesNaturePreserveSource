// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



/**
 * 
 */
UENUM()
enum class Direction : uint8
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NONE
};

class ACONTROL_API SokobonUtils
{

public:
	SokobonUtils();
	~SokobonUtils();


};
