// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AControlGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharRefInit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIterateStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIterateComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateInputPrompts);

UCLASS(minimalapi)
class AAControlGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<class AAControlCharacter> CharRef;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<FName, bool> PuzzleStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<class AACSokobonGrid*> Puzzles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TSubclassOf<class ABaseAIAgent> AnimalAIBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		AActor* AISpawnLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<class ABaseAIAgent*> SpawnedAI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT(""));

public:

	
	class UACGameInstance* instance;

	UPROPERTY(BlueprintAssignable)
	FOnCharRefInit OnCharRefInit;

	UPROPERTY(BlueprintAssignable)
		FOnIterateComplete OnIterateComplete;

	UPROPERTY(BlueprintAssignable)
	FOnIterateStart OnIterateStart;

	UPROPERTY(BlueprintAssignable)
	FUpdateInputPrompts UpdateInputPrompts;

	AAControlGameMode();
	
	TWeakObjectPtr<class AAControlCharacter> GetCharRef() const;
	UFUNCTION(BlueprintCallable)
	class AAControlCharacter* GetCharRefBP() const { return CharRef.Get(); }
	void SetCharRef(TWeakObjectPtr<class AAControlCharacter> _charRef);

	UFUNCTION(CallInEditor)
	void HandleGameCompletion();

	bool CheckZoneCompletion();

	UFUNCTION()
	void HandleCompletedPuzzle(class AACSokobonGrid* _grid);

	TArray<class AACSokobonGrid*> GetPuzzles() const { return Puzzles; }
	TMap<FName, bool> GetPuzzleStatus() const { return PuzzleStatus; }

	void InitAnimalAI();

	UFUNCTION(BlueprintCallable)
	void DispatchBegin();

	UFUNCTION(BlueprintCallable)
		void DispatchUpdateInputPrompts() { UpdateInputPrompts.Broadcast(); }
};



