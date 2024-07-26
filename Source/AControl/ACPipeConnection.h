// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ACPipeConnection.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACONTROL_API UACPipeConnection : public USphereComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UACPipeConnection* Connection = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class AACSokobonPipe* Pipe = nullptr;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Sets default values for this component's properties
	UACPipeConnection();

	void SetConnection(UACPipeConnection* _val) { Connection = _val; }
	FORCEINLINE UACPipeConnection* GetConnection() const { return Connection; }

	void SetPipe(class AACSokobonPipe* _val) { Pipe = _val; }
	FORCEINLINE class AACSokobonPipe* GetPipe() const { return Pipe; }
};
