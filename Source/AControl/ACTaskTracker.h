// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ACTaskTracker.generated.h"

/**
 * 
 */
UCLASS()
class ACONTROL_API UACTaskTracker : public UUserWidget
{
	GENERATED_BODY()

		class AAControlGameMode* GameMode;

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TaskTitle;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TaskObjective;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TaskProgress;

	UFUNCTION(BlueprintCallable)
	void UpdateText(class AACSokobonGrid* _grid, bool _onlyUpdateProgress);
	void SetTitle(FText _title);
	void SetObjective(FText _obj);
	void SetProgress(const int _cur, const int _total);
	void SetRemainingPuzzles(class AAControlGameMode* _gameMode);
};