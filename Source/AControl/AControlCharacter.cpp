// Copyright Epic Games, Inc. All Rights Reserved.

#include "AControlCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"

#include "BaseAIAgent.h"

#include "ACSokobonGrid.h"
#include "ACSokobonPiece.h"
#include "ACSokobonTile.h"
#include "ACSokobonPivotComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "AControlGameMode.h"
#include "ACGameInstance.h"

//	#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "ACTutorialTrigger.h"
#include "ACSokobonObjective.h"

//////////////////////////////////////////////////////////////////////////
// AAControlCharacter

AAControlCharacter::AAControlCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	SokobonPieceAreaChecker = CreateDefaultSubobject<UBoxComponent>(TEXT("SokobonPromptArea"));
	SokobonPieceAreaChecker->SetupAttachment(RootComponent);


}

//////////////////////////////////////////////////////////////////////////
// Input

void AAControlCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAControlCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAControlCharacter::MoveRight);

	//	bind sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAControlCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAControlCharacter::StopSprinting);

	//	bind sneak
	PlayerInputComponent->BindAction("Sneak", IE_Pressed, this, &AAControlCharacter::StartSneaking);
	PlayerInputComponent->BindAction("Sneak", IE_Released, this, &AAControlCharacter::StopSneaking);

	//	bind SokobonInteract
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AAControlCharacter::SokobonPieceInteract);

	//	bind SokobonReset
	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AAControlCharacter::PuzzleResetWish);

	PlayerInputComponent->BindAction("DEV_PuzzleClear", IE_Pressed, this, &AAControlCharacter::PuzzleClearWish);

	PlayerInputComponent->BindAction("Camera Swap", IE_Pressed, this, &AAControlCharacter::SwapCamera);
}

void AAControlCharacter::Tick(float DeltaTime)
{
	if (PuzzlePieceComponentCheck)
	{
		if (ComponentCheckTimer >= ComponentCheckDelay)
		{
			//	TODO CALL METHOD
			if (CheckForOverlappingPuzzlePivotComponents())
			{
				//	TODO show input prompt
			}
			ComponentCheckTimer = 0.0f;
		}
		else
		{
			ComponentCheckTimer += DeltaTime;
		}
	}

	if (PuzzlePieceMoveWish)
	{
		PuzzlePieceMoveTimer += DeltaTime;
	}

	//	if a movement is in progress time to L E R P 
	if (bMoveInProgress)
	{
		//	woowooo
		LerpCheck(PuzzlePieceMoveTimer, PuzzlePieceMoveDelay);
	}
}

void AAControlCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto gameMode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->SetCharRef(this);

	//	bind events to check for sokobon puzzle piece overlap
	SokobonPieceAreaChecker->OnComponentBeginOverlap.AddDynamic(this, &AAControlCharacter::SokobonPuzzlePieceOnOverlap);
	SokobonPieceAreaChecker->OnComponentEndOverlap.AddDynamic(this, &AAControlCharacter::SokobonPuzzlePieceOnOverlapEnd);

	//	HACK: utilizing same coll as SokobonPuzzlePiece checks for now
	//	bind events to check for animal overlaps
	SokobonPieceAreaChecker->OnComponentBeginOverlap.AddDynamic(this, &AAControlCharacter::AnimalBeginOverlap);
	SokobonPieceAreaChecker->OnComponentEndOverlap.AddDynamic(this, &AAControlCharacter::AnimalEndOverlap);

	PuzzlePieceMoveTimer = PuzzlePieceMoveDelay;

	//	CheckForPuzzleOverlapOnStart();
}

void AAControlCharacter::MoveToOverTime(AACSokobonTile* _newTile, Direction _dir, bool _pulled)
{
	bPulling = _pulled;
	//	step 1: get desired position (_newTile.pos - offset)
	sokoDest = FVector(_newTile->GetActorLocation().X - sokoOffset.X,
		_newTile->GetActorLocation().Y - sokoOffset.Y, this->GetActorLocation().Z);
	//	step 2: stop listening for input
	bMoveInProgress = true;
	LerpStartPos = GetActorLocation();
	HeldSokobonPiece->MoveToOverTime(_newTile, this, _dir, _pulled, bBeckoning);
}

void AAControlCharacter::SokobonPuzzlePieceOnOverlap(UPrimitiveComponent* _overlappedComponent,
	AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _hit)
{
	TWeakObjectPtr<AACSokobonPiece> overlappedPiece = Cast<AACSokobonPiece>(_otherActor);

	if (overlappedPiece.Get() && overlappedPiece.Get()->Movable)
	{
		NearbyPuzzlePieces.AddUnique(overlappedPiece);
		
		PuzzlePieceComponentCheck = true;
		
	}
	//	HACK: piggy back off the event that binds successfully
	else if (!overlappedPiece.Get())
	{
		AnimalBeginOverlap(_overlappedComponent, _otherActor, _otherComponent, _otherBodyIndex, _bFromSweep, _hit);
	}
}

void AAControlCharacter::SokobonPuzzlePieceOnOverlapEnd(UPrimitiveComponent* _overlappedComponent, 
	AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex)
{
	TWeakObjectPtr<AACSokobonPiece> overlappedPiece = Cast<AACSokobonPiece>(_otherActor);

	if (overlappedPiece.Get() && overlappedPiece.Get()->Movable)
	{
		NearbyPuzzlePieces.RemoveSingle(overlappedPiece);

		ComponentCheckTimer = ComponentCheckDelay;
		if (NearbyPuzzlePieces.Num() == 0)
		{
			PuzzlePieceComponentCheck = false;
		}

		CheckForOverlappingPuzzlePivotComponents();
	}
	//	HACK: piggy back off the event that binds successfully
	else if (!overlappedPiece.Get())
	{
		AnimalEndOverlap(_overlappedComponent, _otherActor, _otherComponent, _otherBodyIndex);
	}
}

bool AAControlCharacter::CheckForOverlappingPuzzlePivotComponents()
{
	bool result = false;

	OverlappingPivotComponents.Empty();

	//	overlap box location in world space
	FVector OverlapLocation = SokobonPieceAreaChecker->GetComponentLocation();
	FVector OverlapMin = OverlapLocation - FVector(SokobonPieceAreaChecker->Bounds.BoxExtent);
	FVector OverlapMax = OverlapLocation + FVector(SokobonPieceAreaChecker->Bounds.BoxExtent);


	for (auto& puzzlePiece : NearbyPuzzlePieces)
	{
		auto pivotsToCheck = puzzlePiece.Get()->GetPivotComponents();
		
		for (auto& pivot : pivotsToCheck)
		{
			FVector componentLocation = pivot.Get()->GetComponentLocation();

			//	get box location
			
			//	add box extents

			//	min / max

			if (componentLocation.X >= OverlapMin.X && componentLocation.Y >= OverlapMin.Y && componentLocation.Z >= OverlapMin.Z &&
				componentLocation.X <= OverlapMax.X && componentLocation.Y <= OverlapMax.Y && componentLocation.Z <= OverlapMax.Z)
			{
				FVector pieceLoc = puzzlePiece->GetActorLocation();
				//	1: check which direction (side) the pivot is on
				FVector pivotSide = (componentLocation - pieceLoc).GetSafeNormal();


				//	2.a., check along x axis
				if (puzzlePiece->NSMovement && pivotSide.X != 0)
				{
					OverlappingPivotComponents.AddUnique(pivot);
					result = true;
				}
				//	2.b., check along y axis
				if (puzzlePiece->EWMovement && pivotSide.Y != 0)
				{
					OverlappingPivotComponents.AddUnique(pivot);
					result = true;
				}
			}
		}
	}

	if (!result)
	{
		//	todo hide input prompt

	}

	return result;
}

TWeakObjectPtr<UACSokobonPivotComponent> AAControlCharacter::GetClosestPuzzlePivotComponent() const
{
	TWeakObjectPtr<UACSokobonPivotComponent> curClosestPivotComponent = nullptr;

	//	if our list of overlapping pivot components has been populated
	if (OverlappingPivotComponents.Num() != 0)
	{
		//	cache the distance between the character and the first component location
		float curClosestDistance = (this->GetActorLocation() - OverlappingPivotComponents[0].Get()->GetComponentLocation()).Size();
		//	set the current closest pivot to the first entry in the list
		curClosestPivotComponent = OverlappingPivotComponents[0];

		//	iterate through the pivot list
		for (auto& pivot : OverlappingPivotComponents)
		{
			//	get the distance between the character and the current pivot
			float curPivotDistance = (this->GetActorLocation() - pivot.Get()->GetComponentLocation()).Size();

			//	compare distance
			if (curPivotDistance < curClosestDistance)
			{
				//	replace ref
				curClosestPivotComponent = pivot;
				//	update distance
				curClosestDistance = curPivotDistance;
			}
		}
	}

	//	will return nullptr if list is empty
	return curClosestPivotComponent;
}

void AAControlCharacter::PuzzleResetWish()
{
	ClearHeldPiece();
	OnPuzzleResetWish.Broadcast();
}

void AAControlCharacter::PuzzleClearWish()
{
	ClearHeldPiece();
	OnDebugClearWish.Broadcast();
}

void AAControlCharacter::AnimalBeginOverlap(class UPrimitiveComponent* _overlappedComponent,
	class AActor* _otherActor,
	class UPrimitiveComponent* _otherComponent,
	int32 _otherBodyIndex,
	bool _bFromSweep,
	const FHitResult& _hit)
{
	TWeakObjectPtr<ABaseAIAgent> overlappingAnimal = Cast<ABaseAIAgent>(_otherActor);

	//	if not null
	if (overlappingAnimal.Get())
	{
		AnimalsInRange.Add(overlappingAnimal);
	}
}

void AAControlCharacter::AnimalEndOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex)
{
	TWeakObjectPtr<ABaseAIAgent> overlappingAnimal = Cast<ABaseAIAgent>(_otherActor);

	//	if not null
	if (overlappingAnimal.Get())
	{
		AnimalsInRange.Remove(overlappingAnimal);
	}
}

void AAControlCharacter::LerpCheck(float _elapsed, float _dur)
{
	if (HeldSokobonPiece.Get())
	{
		//	HeldSokobonPiece->charDur = _dur;
		HeldSokobonPiece->LerpCheck(_elapsed, _dur);
	}

	if (HeldSokobonPiece.Get() && !HeldSokobonPiece->MoveTilStopped)
	{
		if (_elapsed <= _dur)
		{
			this->SetActorRelativeLocation(FMath::InterpExpoIn(LerpStartPos, sokoDest, _elapsed / _dur));
		}
		else
		{
			this->SetActorRelativeLocation(sokoDest);
			bMoveInProgress = false;
			if (bLetGoWish)
			{
				SokobonPieceInteract();
			}

		}
	}
	if (!HeldSokobonPiece.Get())
	{
		bMoveInProgress = false;
	}
}

bool AAControlCharacter::BackwardsSokobonMovementCheck(FVector _movementDir)
{
	float dot = FVector::DotProduct(_movementDir, GetActorForwardVector());

	return dot < 0;
}

void AAControlCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bForwardMovementLocked && !bMoveInProgress && !FreezeMovement)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FVector::ForwardVector;

		if (HoldingSokobonPiece && PuzzlePieceMoveTimer >= PuzzlePieceMoveDelay)
		{
			bool bCheckTwoTiles = BackwardsSokobonMovementCheck(Direction * Value);

			//	also check that any combination of (OnlyPush and movingBackwards) is false
			if (!(HeldSokobonPiece->OnlyPush && bCheckTwoTiles))
			{
				//	move sokobon piece North
				if (Value > 0.0f)
				{
					auto destTile = HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::NORTH, bCheckTwoTiles, bCheckTwoTiles, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get());

					if (destTile)
					{
						bCheckTwoTiles ? OnPull.Broadcast() : OnPush.Broadcast();
						MoveToOverTime(HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::NORTH, false, false, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get()), Direction::NORTH, bCheckTwoTiles);
					}
				}
				//	move sokobon piece South
				else
				{
					auto destTile = HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::SOUTH, bCheckTwoTiles, bCheckTwoTiles, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get());

					if (destTile)
					{
						bCheckTwoTiles ? OnPull.Broadcast() : OnPush.Broadcast();
						MoveToOverTime(HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::SOUTH, false, false, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get()), Direction::SOUTH, bCheckTwoTiles);
					}
				}
				PuzzlePieceMoveTimer = 0.0f;
			}

		}

		if (!HoldingSokobonPiece)
		{
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void AAControlCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && !bHorizontalMovementLocked && !bMoveInProgress && !FreezeMovement)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FVector::RightVector;

		if (HoldingSokobonPiece && PuzzlePieceMoveTimer >= PuzzlePieceMoveDelay)
		{
			bool bCheckTwoTiles = BackwardsSokobonMovementCheck(Direction * Value);

			//	also check that any combination of (OnlyPush and movingBackwards) is false
			if (!(HeldSokobonPiece->OnlyPush && bCheckTwoTiles))
			{
				//	move sokobon piece east
				if (Value > 0.0f)
				{

					auto destTile = HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::EAST, bCheckTwoTiles, bCheckTwoTiles, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get());

					if (destTile)
					{
						bCheckTwoTiles ? OnPull.Broadcast() : OnPush.Broadcast();
						MoveToOverTime(HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::EAST, false, false, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get()), Direction::EAST, bCheckTwoTiles);
					}
				}
				//	move sokobon piece west
				else
				{
					auto destTile = HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::WEST, bCheckTwoTiles, bCheckTwoTiles, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get());

					if (destTile)
					{
						bCheckTwoTiles ? OnPull.Broadcast() : OnPush.Broadcast();
						MoveToOverTime(HeldSokobonPiece->CurrentTile->GetValidTileInDirection(Direction::WEST, false, false, HeldSokobonPiece->CurrentTile, HeldSokobonPiece.Get()), Direction::WEST, bCheckTwoTiles);
					}
				}
				PuzzlePieceMoveTimer = 0.0f;
			}
		}

		if (!HoldingSokobonPiece) 
		{
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}
}

void AAControlCharacter::StartSneaking()
{
	bPressedSneak = true;
	GetCharacterMovement()->MaxWalkSpeed = SneakSpeed;
}

void AAControlCharacter::StopSneaking()
{
	bPressedSneak = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAControlCharacter::StartSprinting()
{
	bPressedSprint = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AAControlCharacter::StopSprinting()
{
	bPressedSprint = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAControlCharacter::Interact()
{
	//	TODO Replace SokobonPieceAreaChecker with a collider toggled on and off during an animation
	//	Check if any BaseAIAgent actors are in the bounds of SokobonPieceAreaChecker
	FMTDResult _;

	if (AnimalsInRange.Num() != 0)
	{
		//	TODO: replace first animal in list with closest check
		bool hit = SokobonPieceAreaChecker->ComputePenetration(_, 
			AnimalsInRange[0].Get()->GetCapsuleComponent()->GetCollisionShape(),
			AnimalsInRange[0].Get()->GetCapsuleComponent()->GetComponentLocation(),
			FQuat(AnimalsInRange[0].Get()->GetCapsuleComponent()->GetComponentRotation()));

		if (hit)
		{
			OnAnimalCapture.Broadcast(AnimalsInRange[0].Get());
			//	TODO:
			//	remove from animalsinrange list
		}
	}
}

void AAControlCharacter::SokobonPieceInteract()
{
	//	if we are already holding a piece
	if (HoldingSokobonPiece)
	{
		if (bMoveInProgress)
		{
			bLetGoWish = true;
		}
		else
		{
			//HeldSokobonPiece.Get()->OnPieceDrop.RemoveDynamic(this, &AAControlCharacter::ClearHeldPiece);
			//	take care of letting go
			OnPieceEndOverlap.Broadcast(HeldSokobonPiece.Get());
			HoldingSokobonPiece = false;
			HeldSokobonPiece = nullptr;
			bHorizontalMovementLocked = false;
			bForwardMovementLocked = false;
			PuzzlePieceMoveWish = false;
			sokoOffset = FVector::ZeroVector;
			bLetGoWish = false;
			bBeckoning = false;
			OnLetGo.Broadcast();

		}

	}
	//	otherwise we should grab on
	else
	{
		auto pieceToGrab = GetClosestPuzzlePivotComponent();

		//	if the return value of GetClosestPuzzlePivotComponent is not null
		if (pieceToGrab.Get() && pieceToGrab.Get()->OwningPiece->Movable)
		{
			//	mark that we are holding a piece
			HoldingSokobonPiece = true;
			PuzzlePieceMoveWish = true;

			//	mark that it is the held piece
			HeldSokobonPiece = pieceToGrab.Get()->OwningPiece;

			//	determine side of the piece the pivot is located on side = (piece loc - pieceToGrab loc)
			FVector pivotLoc = pieceToGrab.Get()->GetComponentLocation();
			FVector pieceLoc = HeldSokobonPiece.Get()->GetActorLocation();
			FVector pivotSide = (pieceLoc - pivotLoc).GetSafeNormal();

			//	player location after grab will be:
			//	componentloc x + (pivotSide * offset), componentloc y + (pivotSide * offset), actor z)
			FVector target = { pivotLoc.X + (-pivotSide.X * GrabOffset), pivotLoc.Y + (-pivotSide.Y * GrabOffset), GetActorLocation().Z };

			//	move the player to "grab" it
			this->SetActorLocation(target);

			//	make the player look at the piece we are grabbing
			FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), 
				FVector(HeldSokobonPiece.Get()->GetActorLocation().X, HeldSokobonPiece.Get()->GetActorLocation().Y, GetActorLocation().Z));
			this->SetActorRotation(lookAtRot);

			//	lock movement to axis player is facing along
			pieceToGrab.Get()->FwdAligned ? bHorizontalMovementLocked = true : bForwardMovementLocked = true;

			sokoOffset = HeldSokobonPiece.Get()->GetActorLocation() - GetActorLocation();
			//	HeldSokobonPiece.Get()->OnPieceDrop.AddDynamic(this, &AAControlCharacter::ClearHeldPiece);

			auto animal = Cast<AACSokobonObjective>(HeldSokobonPiece);
			if (animal)
			{
				bBeckoning = animal->BeckonMe;
				OnGrab.Broadcast(animal->BeckonMe);

			}
			else
			{
				bBeckoning = false;
				OnGrab.Broadcast(false);

			}
			OnPieceBeginOverlap.Broadcast(bHorizontalMovementLocked, HeldSokobonPiece.Get());
			HeldSokobonPiece.Get()->PieceGrabbed.Broadcast();
		}
	}
}

void AAControlCharacter::SwapCamera()
{
	OnCameraSwapWish.Broadcast();
}

void AAControlCharacter::ClearHeldPiece()
{
	HeldSokobonPiece = nullptr;
	HoldingSokobonPiece = false;
	bHorizontalMovementLocked = bForwardMovementLocked = false;
	OnLetGo.Broadcast();
}

void AAControlCharacter::SetTopDownCamRef(UCameraComponent* _cam)
{
	TopDownCamRef = _cam;
}

void AAControlCharacter::CheckForPuzzleOverlapOnStart()
{
	FMTDResult _;

	auto capsuleShape = GetCapsuleComponent()->GetCollisionShape();
	auto capsuleLoc = GetCapsuleComponent()->K2_GetComponentLocation();
	auto capsuleRot = FQuat(GetCapsuleComponent()->K2_GetComponentRotation());

	if (!CurPuzzle)
	{
		//	phase 1: check for puzzle overlap
		auto gamemode = Cast<AAControlGameMode>(GetWorld()->GetAuthGameMode());

		if (gamemode)
		{
			gamemode->DispatchBeginPlay();
			for (auto& puzzle : gamemode->GetPuzzles())
			{
				bool result = puzzle->GetPuzzleArea()->ComputePenetration(_, capsuleShape,
					capsuleLoc,
					capsuleRot);
				if (result)
				{
					CurPuzzle = puzzle;
					CurPuzzle->OnUpdateTasks.Broadcast(CurPuzzle, false);
					OnDebugClearWish.AddDynamic(CurPuzzle, &AACSokobonGrid::InitPuzzleTeardown);
					break;
				}
			}

			if (!CurPuzzle)
			{
				if (gamemode->GetPuzzles().Num() != 0)
				{
					//	we haven't spawned in a puzzle!
					gamemode->GetPuzzles()[0]->OnUpdateTasks.Broadcast(nullptr, true);
				}
				else
				{
					//gamemode->GetPuzzles()[0]->OnUpdateTasks.Broadcast(nullptr, true);
				}
			}
		}
	}
	//	hijacking- phase 2: check for tutorial zone overlap
	UACGameInstance* instance = Cast<UACGameInstance>(GetGameInstance());
	TArray<AActor*> TutorialTriggers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AACTutorialTrigger::StaticClass(), TutorialTriggers);
	
	//	in the event tutorial progress hasn't been initialized-
	if (instance->TutorialProgress.Num() == 0)
	{
		for (auto& triggers : TutorialTriggers)
		{
			AACTutorialTrigger* trigger = Cast<AACTutorialTrigger>(triggers);
			instance->AddTutorialToProgressByTrigger(trigger);
		}
	}
	
	for (auto& triggers : TutorialTriggers)
	{
		AACTutorialTrigger* trigger = Cast<AACTutorialTrigger>(triggers);
		bool overlap = trigger->GetTriggerArea()->ComputePenetration(_, capsuleShape, capsuleLoc, capsuleRot);
		if (overlap)
		{
			for (auto& tut : trigger->GetTutorials())
			{
				instance->AddTutorial(tut);
			}
		}
	}														   	
}																   	

void AAControlCharacter::ReInitPlayerInput()
{
	SetupPlayerInputComponent(InputComponent);
}
