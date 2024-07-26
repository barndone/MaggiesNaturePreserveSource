// Fill out your copyright notice in the Description page of Project Settings.


#include "ACSokobonConveyor.h"
#include "ACSokobonTile.h"

void AACSokobonConveyor::MoveOverlappingPieceInFlowDir(AACSokobonPiece* _overlappingPiece)
{
	//	move to piece in FlowDir

	auto next = CurrentTile->GetValidTileInDirection(FlowDir, false, false, CurrentTile, _overlappingPiece);
	
	_overlappingPiece->Floating = true;

	if (next)
	{
		_overlappingPiece->MoveToOverTime(next, nullptr, Direction::NONE, false, false);
	}
}