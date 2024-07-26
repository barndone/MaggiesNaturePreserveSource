// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ITeleportable.h"
#include "AControlCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleResetWish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimalCapture, class ABaseAIAgent*, _capturedAnimal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraSwapWish);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDebugClearWish);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPieceBeginOverlap, bool, _NS, class AACSokobonPiece*, _overlappedPiece);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPieceEndOverlap, class AACSokobonPiece*, _overlappedPiece);

//	Events for sound hooks/animation integration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrab, bool, _shouldBeckon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLetGo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPush);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPull);

//	Events for movement tutorial prompt
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveTutorialComplete);

UCLASS(config=Game)
class AAControlCharacter : public ACharacter, public ITeleportable
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SokobonPieceAreaChecker;

	TArray<TWeakObjectPtr<class ABaseAIAgent>> AnimalsInRange;

	bool bPressedSneak = false;
	bool bPressedSprint = false;
	bool bPressedGrab = false;
	bool bBeckoning = false;
	bool bPulling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SokoStartDelay = 0.25f;
	float SokoStartTimer = 0.0f;

	TWeakObjectPtr<class UCameraComponent> TopDownCamRef = {};


	bool bHorizontalMovementLocked = false;
	bool bForwardMovementLocked = false;

	//	check for if the player is currently moving to a new sokobon tile
	bool bMoveInProgress = false;
	bool bLetGoWish = false;

	bool HoldingSokobonPiece = false;
	TWeakObjectPtr<class AACSokobonPiece> HeldSokobonPiece = nullptr;
	FVector sokoOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TWeakObjectPtr<class AACSokobonPiece>> NearbyPuzzlePieces = {};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess=true))
	TArray<TWeakObjectPtr<class UACSokobonPivotComponent>> OverlappingPivotComponents = {};

	bool PuzzlePieceComponentCheck = false;

	float ComponentCheckDelay = 0.5f;
	float ComponentCheckTimer = 0.0f;

	bool PuzzlePieceMoveWish = false;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float PuzzlePieceMoveDelay = 0.75f;
	
	float PuzzlePieceMoveTimer = 0.0f;

	FVector sokoDest = FVector::ZeroVector;

	void LerpCheck(float _elapsed, float _dur);
	bool BackwardsSokobonMovementCheck(FVector _movementDir);

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<class USoundWave*> FootstepSounds;

	FVector LerpStartPos;
	FRotator LerpStartRot;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	UFUNCTION()
	void StartSneaking();
	UFUNCTION()
	void StopSneaking();

	UFUNCTION()
	void StartSprinting();
	UFUNCTION()
	void StopSprinting();

	void Interact();

	void SokobonPieceInteract();

	void SwapCamera();



	UFUNCTION()
		void SokobonPuzzlePieceOnOverlap(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex,
			bool _bFromSweep,
			const FHitResult& _hit);

	UFUNCTION()
		void SokobonPuzzlePieceOnOverlapEnd(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex);

	UFUNCTION()
		bool CheckForOverlappingPuzzlePivotComponents();

	UFUNCTION()
		TWeakObjectPtr<class UACSokobonPivotComponent> GetClosestPuzzlePivotComponent() const;

	UFUNCTION()
		void PuzzleResetWish();

	UFUNCTION()
		void PuzzleClearWish();

	UFUNCTION()
	void AnimalBeginOverlap(class UPrimitiveComponent* _overlappedComponent,
		class AActor* _otherActor,
		class UPrimitiveComponent* _otherComponent,
		int32 _otherBodyIndex,
		bool _bFromSweep,
		const FHitResult& _hit);

	UFUNCTION()
		void AnimalEndOverlap(class UPrimitiveComponent* _overlappedComponent,
			class AActor* _otherActor,
			class UPrimitiveComponent* _otherComponent,
			int32 _otherBodyIndex);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AAControlCharacter();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SneakSpeed = 400;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1000;
	UPROPERTY(EditAnywhere)
	float GrabOffset = 20.0f;
	bool FreezeMovement = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AACSokobonGrid* CurPuzzle = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnPuzzleResetWish OnPuzzleResetWish;

	UPROPERTY(BlueprintAssignable)
	FOnAnimalCapture OnAnimalCapture;

	UPROPERTY(BlueprintAssignable)
	FOnCameraSwapWish OnCameraSwapWish;

	UPROPERTY(BlueprintAssignable)
	FOnGrab OnGrab;
	
	UPROPERTY(BlueprintAssignable)
	FOnLetGo OnLetGo;
	
	UPROPERTY(BlueprintAssignable)
	FOnPush OnPush;
	
	UPROPERTY(BlueprintAssignable)
	FOnPull OnPull;

	UPROPERTY(BlueprintAssignable)
	FOnDebugClearWish OnDebugClearWish;

	UPROPERTY(BlueprintAssignable)
	FOnMoveTutorialComplete OnMoveTutorialComplete;

	UFUNCTION()
	void ClearHeldPiece();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void MoveToOverTime(class AACSokobonTile* _newTile, Direction _dir, bool _pulled);

	UPROPERTY(BlueprintAssignable)
	FOnPieceBeginOverlap OnPieceBeginOverlap;
	UPROPERTY(BlueprintAssignable)
	FOnPieceEndOverlap OnPieceEndOverlap;

	void SetTopDownCamRef(class UCameraComponent* _cam);

	UFUNCTION()
		class UCameraComponent* GetTopDownCamRef() const { return TopDownCamRef.Get(); }

	UFUNCTION(BlueprintCallable)
	void CheckForPuzzleOverlapOnStart();

	TArray<class USoundWave*> GetFootstepList() const { return FootstepSounds; }

	void ReInitPlayerInput();
	
	UFUNCTION(BlueprintCallable)
	class AACSokobonPiece* GetHeldSokoPiece() const { return HeldSokobonPiece.Get(); }
};

