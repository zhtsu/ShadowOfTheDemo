// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/EvergreenGameInstance.h"

#include "LevelSequencePlayer.h"
#include "GeneralUtility.h"
#include "Internationalization/StringTableRegistry.h"
#include "Item/ItemBase.h"

UEvergreenGameInstance* UEvergreenGameInstance::Singleton = nullptr;

UEvergreenGameInstance* UEvergreenGameInstance::GetEvergreenGameInstance()
{
	return Singleton;
}

UEvergreenGameInstance::UEvergreenGameInstance()
{
	Singleton = this;

	LoadStringTablesOnRuntime();
}

void UEvergreenGameInstance::SetEvergreenGameMode(EEvergreenGameMode InGameMode)
{
	GameMode = InGameMode;
	OnGameModeChanged.Broadcast(GameMode);
}

void UEvergreenGameInstance::PauseGame()
{
	SetCurrentGamePlayState(EGamePlayState::Paused);
}

void UEvergreenGameInstance::ResumeGame()
{
	ReturnPreviousGamePlayState();
}

void UEvergreenGameInstance::PlayCutscene(ULevelSequence* LevelSequence, ALevelSequenceActor*& LevelSequenceActor, ULevelSequencePlayer*& LevelSequencePlayer)
{
	SetCurrentGamePlayState(EGamePlayState::Cutscene);
	
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor);

	if (!LevelSequenceActor || !LevelSequencePlayer)
	{
		FAST_WARNING(TEXT("Unable to create level sequence player"));
		return;
	}
	
	LevelSequencePlayer->Play();
	LevelSequencePlayer->OnStop.AddDynamic(this, &UEvergreenGameInstance::ReturnPreviousGamePlayState);
}

void UEvergreenGameInstance::StartMiniGame(TSubclassOf<AMiniGameBase> MiniGameClass)
{
	SetCurrentGamePlayState(EGamePlayState::MiniGame);
}

void UEvergreenGameInstance::EndMiniGame(TSubclassOf<AMiniGameBase> MiniGameClass)
{
	SetCurrentGamePlayState(GamePlayState.PreviousGamePlayState);
}

bool UEvergreenGameInstance::IsAllowKeyboardInput() const
{
	return GamePlayState.CurrentGamePlayState != EGamePlayState::Cutscene
		&& GamePlayState.CurrentGamePlayState != EGamePlayState::Paused
		&& GamePlayState.CurrentGamePlayState != EGamePlayState::MiniGame;
}

bool UEvergreenGameInstance::IsAllowInput() const
{
	return GamePlayState.CurrentGamePlayState != EGamePlayState::Cutscene
		&& GamePlayState.CurrentGamePlayState != EGamePlayState::Paused;
}

void UEvergreenGameInstance::CollectItem(AEvergreenItemBase* Item)
{
	if (!Item) return;
	
	if (!CollectedItemIDArray.Contains(Item->ItemID))
	{
		CollectedItemIDArray.Add(Item->ItemID);
		OnItemCollected.Broadcast(Item->ItemID);
		Item->Destroy();
	}
}

bool UEvergreenGameInstance::HasItem(FString ItemID)
{
	return CollectedItemIDArray.Contains(ItemID);
}

void UEvergreenGameInstance::SetCurrentGamePlayState(EGamePlayState NewGamePlayState)
{
	if (NewGamePlayState == GamePlayState.CurrentGamePlayState)
	{
		return;
	}

	GamePlayState.PreviousGamePlayState = GamePlayState.CurrentGamePlayState;
	GamePlayState.CurrentGamePlayState = NewGamePlayState;
	OnGamePlayStateChanged.Broadcast(GamePlayState.CurrentGamePlayState);
}

void UEvergreenGameInstance::ReturnPreviousGamePlayState()
{
	SetCurrentGamePlayState(GamePlayState.PreviousGamePlayState);
}

void UEvergreenGameInstance::LoadStringTablesOnRuntime()
{
#ifndef WITH_EDITOR
	FStringTableRegistry::Get().UnregisterStringTable(FAssetPathHub::ST_ItemName_Reference);
	FStringTableRegistry::Get().Internal_LocTableFromFile(
		FAssetPathHub::ST_ItemName_Reference, TEXT("ST_ItemName"),
		FAssetPathHub::ST_ItemName_CSV_Path.ToString(), FPaths::ProjectContentDir());

	FStringTableRegistry::Get().UnregisterStringTable(FAssetPathHub::ST_ItemDescription_Reference);
	FStringTableRegistry::Get().Internal_LocTableFromFile(
		FAssetPathHub::ST_ItemDescription_Reference, TEXT("ST_ItemDescription"),
		FAssetPathHub::ST_ItemDescription_CSV_Path.ToString(), FPaths::ProjectContentDir());
#endif
}
