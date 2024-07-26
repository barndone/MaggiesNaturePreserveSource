// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SokobonUtils.h"
#include "ACSokobonPiece.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPieceMove);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPieceDrop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPieceDelete, AACSokobonPiece*, _ToDelete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFallStart, class AACSokobonHole*, _hole);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPieceGrabbed);


UCLASS()
class ACONTROL_API AACSokobonPiece : public AActor
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FText ActorArrowTag;

	class AACSokobonTile* nextTile = nullptr;
	class AAControlCharacter* character = nullptr;
	bool moveWish = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ConveyorMoveTimer = 0.0f;

	bool RotateWish = false;
	Direction TarDir = Direction::NONE;
	FQuat TarQuat;
	FRotator Rolltator;
	FVector RollingAxis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RollDegreeDelta = 90.0f;
	float CurrentRollDegreeTarget = 0.0f;
	float StartRollDegree = 0.0f;
	float CurrentRollDegree = 0.0f;

	float charTimer = 0.0f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RotateDelay = 0.3f;
	float RotateTimer = 0.0f;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bRollingPiece = false;
	float charDur = 0.0f; 
	FVector LerpStartPos;
	FQuat LerpStartQuat;
	FRotator LerpStartRot;

	// Sets default values for this actor's properties
	AACSokobonPiece();

	bool bEasingIn = true;
	bool Pulled = false;
	UPROPERTY(EditAnywhere)
	FVector2D StartingTileIdx = { 0, 0 };
	UPROPERTY(VisibleAnywhere)
	class AACSokobonTile* CurrentTile = nullptr;


	bool IsGrabbed = false;
	UPROPERTY(EditAnywhere)
	bool Movable = true;

	UPROPERTY(EditAnywhere)
	bool OnlyPush = false;

	UPROPERTY(EditAnywhere)
	bool MoveTilStopped = false;

	UPROPERTY(EditAnywhere)
	bool BlockMovement = true;

	bool Floating = false;

	UPROPERTY(EditAnywhere)
	bool NSMovement = true;
	UPROPERTY(EditAnywhere)
	bool EWMovement = true;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AACSokobonPiece>> BlockMovementOf;

	bool bMoveInProgress = false;
	float floatTimer;

	FVector sokoDest = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UACSokobonPivotComponent* NorthPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UACSokobonPivotComponent* EastPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UACSokobonPivotComponent* SouthPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UACSokobonPivotComponent* WestPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SK;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<class AACSokobonGrid> GridRef = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnPieceMove OnPieceMove;

	UPROPERTY(BlueprintAssignable)
	FOnPieceDelete OnPieceDelete;

	UPROPERTY(BlueprintAssignable)
	FOnPieceDrop OnPieceDrop;

	UPROPERTY(BlueprintAssignable)
	FOnMoveStart OnMoveStart;

	UPROPERTY(BlueprintAssignable)
		FOnFallStart OnFallStart;
	UPROPERTY(BlueprintAssignable)
FPieceGrabbed PieceGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Movable", EditConditionHides))
	class USoundWave* PushPullSound = nullptr;


	Direction LastPushDir = Direction::NONE;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void AdditionalMovementLogic();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLetFall = false;
	bool bEasing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLog = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor = nullptr;

public:	

	void Initialize();

	bool MoveToTileInDirection(Direction _dir);

	void InitPivotLocations(float _fullExtent);

	//UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void UpdatePosition();

	bool GetMovementBlockingOf(AACSokobonPiece* _otherPiece) const;

	UFUNCTION()
	void ResetPosition();

	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void SaveCurrentPositionAsStart();

	void PlaceAtCoordinate(FVector2D _coordinate);

	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void MoveNorth();
	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void MoveEast();
	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void MoveSouth();
	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void MoveWest();
	UFUNCTION(CallInEditor, Category = "ACSokobonPiece")
	void DeleteSokobonPiece();

	UFUNCTION()
	void HandlePieceHiding();

	TArray<TWeakObjectPtr<class UACSokobonPivotComponent>> GetPivotComponents() const;

	void MoveToOverTime(class AACSokobonTile* _newTile, class AAControlCharacter* _char, Direction _dir, bool _backwards, bool _rotateToDir);
	void LerpCheck(float _elapsed, float _dur);

	UFUNCTION(BlueprintCallable)
	void InitSokoPivots();
};
