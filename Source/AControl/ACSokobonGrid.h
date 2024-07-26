// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SokobonUtils.h"
#include "GameFramework/Actor.h"
#include "ACSokobonGrid.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPuzzleComplete, AACSokobonGrid*, _completedPuzzle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateTasks, AACSokobonGrid*, _grid, bool, _updateObj);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleExit);

UCLASS()
class ACONTROL_API AACSokobonGrid : public AActor
{

	GENERATED_BODY()
	FActorSpawnParameters SpawnParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDelayGateOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PuzzleArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCam;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetKeyOptions = "GetKeyChars"))
		TMap<FString, TSubclassOf<AACSokobonPiece>> TextImportPairings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
		FString PuzzleImport;
	UPROPERTY(EditAnywhere)
		bool BSHackPuzzle = false;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		int GridLength = 10;

	//	corresponds to the Y direction length in tiles of the grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		int GridWidth = 10;

	// corresponds to the size in units that a (square) tile will take up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		float GrabPivotOffset = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float PuzzleAreaOverflow = 500.0f;

	int CompletedPuzzles = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ConveyorMoveDur = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	Direction dir = Direction::SOUTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundWave* PuzzleCompleteSound = nullptr;

	class AAControlGameMode* GameMode = nullptr;

	//	corresponds to the X direction length in tiles of the grid

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TSubclassOf<class AACSokobonTile> TileBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> PuzzlePieceBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> TeleporterPieceBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> SpecialtyTeleporterBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TSubclassOf<class AACSokobonPiece> ObstructionPieceBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> ObjectivePieceBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> GoalPieceBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> HoleBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "!BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> ConveyorBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> IPipeBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> CWLPipeBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, EditCondition = "BSHackPuzzle", EditConditionHides))
		TSubclassOf<class AACSokobonPiece> CCWLPipeBP;

	//	return the FVector position of a tile given a row and column index
	FVector CalcTilePosition(int _curRowIdx, int _curColIdx);

	void ResetGrid();
	UFUNCTION()
		void ResetPuzzle();

	UFUNCTION()
		void BindCharacterEvents();


	TWeakObjectPtr<class AACSokobonTeleporter> GetNextOpenPortal(TWeakObjectPtr<class AACSokobonTeleporter> _curPortal);

	void InitSokobonPiece(class AACSokobonPiece* _pieceToInit, FVector2D _pos = {0,0});

	/// <summary>
	/// Given a TArray of pointers to actors, mark all entries in the list for destroy and empty the TArray.
	/// </summary>
	template<class T>
	void ResetActorList(TArray<T*>& _in)
	{
		static_assert(std::is_base_of<class AACSokobonPiece, T>::value, "T must derive from AActor");

		if (_in.Num() != 0)
		{
			for (auto& val : _in)
			{
				val->Destroy();
			}

			_in.Empty();
		}
	}

	UFUNCTION(CallInEditor)
		void InitPuzzleArea();

	UFUNCTION()
		void HandleAnimalFreed();

	void AddPieceToList(class AACSokobonPiece* _piece);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void PlayerEnteredPuzzleArea(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex,
			bool _bFromSweep,
			const FHitResult& _hit);

	UFUNCTION()
		void PlayerLeftPuzzleArea(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex);
	
public:	
	UPROPERTY(EditAnywhere)
	FText PuzzleTitle;
	UPROPERTY(EditAnywhere)
	FText PuzzleObjective;

	int GetCurrentPuzzleCompletion() const { return CompletedPuzzles; }

	class UBoxComponent* GetPuzzleArea() const { return PuzzleArea; }


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = BSHackPuzzle, EditConditionHides))
	class AACSokobonPipe* Start = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = BSHackPuzzle, EditConditionHides))
	class AACSokobonPipe* End = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = BSHackPuzzle, EditConditionHides))
	TArray<class AACSokobonPipe*> Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PuzzleName;

	// Sets default values for this actor's properties
	AACSokobonGrid();

	//	return the private GridLength field
	int GetGridLength() const;

	float GetConveyorMoveDur() const { return ConveyorMoveDur; }

	//	return the private GridWidth field
	int GetGridWidth() const;

	//	return the private TileSize field
	float GetTileSize() const;

	//class

	UFUNCTION(CallInEditor)
	//	generate the Sokobon Grid
	void GenerateGrid();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnPuzzlePiece();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnPuzzleTeleporter();


	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnSpecialtyTeleporter();

	UFUNCTION(CallInEditor)
	void SpawnWall();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnObjective();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnGoal();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnHole();

	UFUNCTION(CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void SpawnConveyor();

	UFUNCTION(BlueprintCallable, CallInEditor, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	void LinkPortals();

	UFUNCTION(CallInEditor, meta = (EditCondition = "BSHackPuzzle", EditConditionHides))
	void SpawnIPipe();

	UFUNCTION(CallInEditor, meta = (EditCondition = "BSHackPuzzle", EditConditionHides))
	void SpawnCWLPipe();

	UFUNCTION(CallInEditor, meta = (EditCondition = "BSHackPuzzle", EditConditionHides))
		void SpawnCCWLPipe();

	UFUNCTION(CallInEditor)
	void ImportPuzzle();

	UFUNCTION()
	void CleanUpDeletedPiece(class AACSokobonPiece* _pieceToCleanUp);

	UFUNCTION()
	void CheckPipePuzzleCompletion();

	UPROPERTY()
	TMap<FVector2D, class AACSokobonTile*> SokobonTiles;

	UFUNCTION()
	TArray<FString> GetKeyChars() const
	{
		TArray<FString> CharOptions;

		for (char c{'!'}; c <= '~'; ++c)
		{
			CharOptions.Add(FString::Chr(TCHAR(c)));
		}

		return CharOptions;
	}

	UFUNCTION(CallInEditor, BlueprintCallable)
	void InitPuzzleTeardown();

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonPuzzleBlock*> SpawnedPuzzlePieces;

	UPROPERTY(EditAnywhere, meta=(EditFixedSize, EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonTeleporter*> SpawnedTeleporters;
	UPROPERTY(EditAnywhere, meta = (EditFixedSize, EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonTeleporter*> SpawnedSpecialTeleporters;

	UPROPERTY(EditAnywhere)
	bool TwoWayTeleportPair = false;

	UPROPERTY(EditAnywhere)
	TArray<class AACSokobonWall*> SpawnedWalls;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonObjective*> SpawnedObjectives;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonGoal*> SpawnedGoals;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonHole*> SpawnedHoles;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!BSHackPuzzle", EditConditionHides))
	TArray<class AACSokobonConveyor*> SpawnedConveyors;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "BSHackPuzzle", EditConditionHides))
		TArray<class AACSokobonPipe*> SpawnedPipes;

	UPROPERTY(BlueprintAssignable)
	FOnPuzzleComplete OnPuzzleComplete;
	UPROPERTY(BlueprintAssignable)
	FOnPuzzleEnter OnPuzzleEnter;
	UPROPERTY(BlueprintAssignable)
	FOnPuzzleExit OnPuzzleExit;

	FOnUpdateTasks OnUpdateTasks;

	void SpawnPuzzlePieceAtTile(FVector2D _tileCoord);
	void SpawnPuzzleTeleporterAtTile(FVector2D _tileCoord);
	void SpawnSpecialtyTeleporterAtTile(FVector2D _tileCoord);
	void SpawnWallAtTile(FVector2D _tileCoord);
	void SpawnObjectiveAtTile(FVector2D _tileCoord);
	void SpawnGoalAtTile(FVector2D _tileCoord);
	void SpawnHoleAtTile(FVector2D _tileCoord);
	void SpawnConveyorAtTile(FVector2D _tileCoord);
	void SpawnIPipeAtTile(FVector2D _tileCoord);
	void SpawnCWLPipeAtTile(FVector2D _tileCoord);
	void SpawnCCWLPipeAtTile(FVector2D _tileCoord);


};
