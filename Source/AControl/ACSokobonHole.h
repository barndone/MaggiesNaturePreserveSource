// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACSokobonPiece.h"
#include "ACSokobonHole.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeDust);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShowDust);
/**
 * 
 */
UCLASS()
class ACONTROL_API AACSokobonHole : public AACSokobonPiece
{
	GENERATED_BODY()
	
	//	TODO: swap meshes

	//	TODO: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* UnfulfilledMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* FulfilledMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PlayerBlockingCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DustPlane= nullptr;

	bool IsFilled = false;

	TArray<TSubclassOf<class AACSokobonPiece>> BlockMovementOfPreFilled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DustDur = 1.0f;
	float DustTimer = 0.0f;
	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//	float DustFadeOutDur = 0.5f;
	//	float DustFadeOutTimer = 0.0f;

	bool bShowDust = false;

	/// <summary>
	/// BlockList of sokobon pieces for AFTER the hole is filled
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AACSokobonPiece>> BlockMovementOfPostFilled;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Sets default values for this actor's properties
	AACSokobonHole();

	UFUNCTION()
	void FillHole(class AACSokobonTile* _curTile);

	UFUNCTION(BlueprintCallable)
	bool GetFilled() const { return IsFilled; }

	void ResetHole();

	void UpdateMesh();

	UPROPERTY(BlueprintAssignable)
	FOnFadeDust OnFadeDust;
	UPROPERTY(BlueprintAssignable)
	FShowDust ShowDust;

	UFUNCTION()
	void ShowDustCloud(AACSokobonHole* _in);
};
