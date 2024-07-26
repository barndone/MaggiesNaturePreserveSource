// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SokobonUtils.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "GameFramework/Actor.h"
#include "ACSokobonTile.generated.h"

UCLASS()
class ACONTROL_API AACSokobonTile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SokobonTileViz;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AACSokobonPiece>> BlockMovementOf;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	AACSokobonTile();

	class AACSokobonGrid* GridRef = nullptr;

	void Initialize();

	//	use weak pointers for class members that it does not have ownership of!
	//	do not pass around the weakpointer, get the underlying pointer itself
	UPROPERTY()
	TWeakObjectPtr<AACSokobonTile> NorthTile = nullptr;
	UPROPERTY()
	TWeakObjectPtr<AACSokobonTile> EastTile = nullptr;
	UPROPERTY()
	TWeakObjectPtr<AACSokobonTile> SouthTile = nullptr;
	UPROPERTY()
	TWeakObjectPtr<AACSokobonTile> WestTile = nullptr;

	//	used for accessing this tile in the SokobonGrid TMap
	UPROPERTY()
	FVector2D GridPos = { 0, 0 };

	UPROPERTY(VisibleAnywhere)
	bool Occupied = false;
	UPROPERTY(VisibleAnywhere)
	bool Navigatable = true;

	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<class AACSokobonPiece>> OccupyingPieces;

	AACSokobonTile* GetTileInDirection(Direction _dir) const;
	AACSokobonTile* GetValidTileInDirection(Direction _dir, bool _checkNext, bool _movingBkwrds, AACSokobonTile* _cur, class AACSokobonPiece* _piece) const;

	void SetTileExtents(float _tileSize);

	bool GetMovementBlockingOf(class AACSokobonPiece* _pieceToCheck) const;
	void AddBlockMovementOf(class AACSokobonPiece* _pieceToAdd);
	/// <summary>
	/// Updates the BlockMovementOf list of this tile after a piece has left this tile given the tile that is leaving.
	/// Should be called BEFORE the piece is removed from the list.
	/// If there are pieces occupying this tile other than the piece to be removed,
	/// will update block list accounting for other pieces BlockLists.
	/// </summary>
	void RemoveBlockMovementOf(class AACSokobonPiece* _pieceToRemove);

	void UpdateBlockMovementOf();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
	void ClearTile();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnPuzzlePieceHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnPuzzleTeleporterHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnSpecialtyTeleporterHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnWallHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnObjectiveHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnGoalHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnHoleHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnConveyorHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnIPipeHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnCWLPipeHere();

	UFUNCTION(CallInEditor, Category = "ACSokobonTile")
		void SpawnCCWLPipeHere();


};
