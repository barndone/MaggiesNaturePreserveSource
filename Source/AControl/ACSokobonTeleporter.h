// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPiece.h"
#include "ACSokobonTeleporter.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportBlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportUnblocked);

UCLASS()
class ACONTROL_API AACSokobonTeleporter : public AACSokobonPiece
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AACSokobonTeleporter> OutTeleporter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TeleportArea = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* TeleportOutLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundWave* TeleportSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool AllowPuzzleBlockTeleporting = false;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	AACSokobonTeleporter();
	UFUNCTION()
	void TeleporterEnter(UPrimitiveComponent* _overlappedComponent,
		AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _hit);
	UFUNCTION()
	void TeleporterExit(UPrimitiveComponent* _overlappedComponent,
		AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool Blocked = false;

	UFUNCTION()
	void CheckForBlockage();

	void SetOutTeleporter(TWeakObjectPtr<AACSokobonTeleporter> _value);
	UFUNCTION(BlueprintCallable)
	AACSokobonTeleporter* GetOutTele() const { return OutTeleporter.Get(); }

	UPROPERTY(BlueprintAssignable)
	FOnTeleportStart OnTeleportStart;
	UPROPERTY(BlueprintAssignable)
	FOnTeleportBlocked OnTeleportBlocked;
	UPROPERTY(BlueprintAssignable)
	FOnTeleportUnblocked OnTeleportUnblocked;
};
