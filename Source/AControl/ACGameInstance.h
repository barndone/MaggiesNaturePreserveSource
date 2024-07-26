// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/GameInstance.h"
#include "ACGameInstance.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAdditionalTutorialLogicBegin, FText, _ActorTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAdditionalTutorialLogicEnd, FText, _ActorTag);


UCLASS()
class ACONTROL_API UACGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

	UPROPERTY()
	TArray<int> HubPlayerStartIdx;
	int PlayerStartIdx = 0;

	UPROPERTY()
	class UACKeybindings* KeybindingSettings;

	TQueue<class UACTutorial*> TutorialQueue;
	TArray<class UACTutorial*> AllTutorials;
	class UACTutorialPromptWidget* TutorialWidget;

	void TutorialCheck(class UACTutorial* _tut);
public:
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, bool> PuzzleProgress;
	TMap<FName, bool> TutorialProgress;

	UPROPERTY(BlueprintReadWrite)
	FString PlayerStartTag = "tutorial";

	void SetPuzzleAsCompleted(FName _puzzleName);
	FORCEINLINE bool GetPuzzleCompletion(FName _puzzleName) const { return *PuzzleProgress.Find(_puzzleName); }

	UFUNCTION(BlueprintCallable)
	void InitTutorials(TArray<class UACTutorial*> _inNames);
	UFUNCTION(BlueprintCallable)
	void SetTutorialAsCompleted(FName _tutorialName);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetTutorialCompletion(FName _tutorialName) const { return *TutorialProgress.Find(_tutorialName); }

	bool CheckGameComplete();

	void SetHubPlayerStartIdx(const int _val);
	FORCEINLINE int GetHubPlayerStartIdx() const { return PlayerStartIdx; }

	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;

	/** virtual function to allow custom GameInstances an opportunity to do cleanup when shutting down */
	virtual void Shutdown() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& _mapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* _inLoadedWorld);
	
	void SetKeybindSettings(class UACKeybindings* _val) { KeybindingSettings = _val; }
	class UACKeybindings* GetKeybindSettings();

	void LoadKeybinds();
	void SaveKeybinds();
	UFUNCTION(BlueprintCallable)
	void ResetKeybinds();

	UFUNCTION(BlueprintCallable)
		void AddTutorial(class UACTutorial* _tut);

	UFUNCTION(BlueprintCallable)
		void PopTutorial();

	UFUNCTION(BlueprintCallable)
	void SetTutorialWidgetRef(class UACTutorialPromptWidget* _in) { TutorialWidget = _in; }

	UFUNCTION(BlueprintCallable)
	void AddTutorialToProgress(class UACTutorial* _tut);

	void AddTutorialToProgressByTrigger(class AACTutorialTrigger* _in);


	UPROPERTY(BlueprintAssignable)
	FAdditionalTutorialLogicBegin AdditionalTutorialLogicBegin;

	UPROPERTY(BlueprintAssignable)
		FAdditionalTutorialLogicEnd AdditionalTutorialLogicEnd;

	void AssignFwdKey(int& _posIdx, int& _negIdx, bool _pos, FInputAxisKeyMapping& _key);
	void AssignRghtKey(int& _posIdx, int& _negIdx, bool _pos, FInputAxisKeyMapping& _key);

	void ReinitBinding(FInputActionKeyMapping& _inAction, FKey _key);
	void ReinitAxis(FInputAxisKeyMapping& _inAxis, FKey _key);

	UFUNCTION(BlueprintCallable)
	void ResetProgress();


};
