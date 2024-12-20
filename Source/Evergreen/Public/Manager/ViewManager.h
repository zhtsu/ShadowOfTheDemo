// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ViewManager.generated.h"

UCLASS()
class UViewManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UViewManager();

	UFUNCTION(BlueprintPure)
	static bool IsObserved(UObject* ObservableObject);

	UFUNCTION(BlueprintPure)
	static UObject* GetCurrentObservedObject() { return CurrentObservedObject; }

	UFUNCTION(BlueprintCallable)
	void Observe(UObject* ObservableObject, FViewTargetTransitionParams ViewTargetTransitionParams);

	UFUNCTION(BlueprintCallable)
	void SetToPlayerView(FViewTargetTransitionParams ViewTargetTransitionParams);

	UFUNCTION(BlueprintCallable)
	static void PlayCutscene(class ULevelSequence* LevelSequence, class ALevelSequenceActor*& LevelSequenceActor, class ULevelSequencePlayer*& LevelSequencePlayer);

	UFUNCTION(BlueprintCallable)
	void SetCameraOffsetFollowCursorEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure)
	bool GetCameraOffsetFollowCursorEnabled() const;

	UFUNCTION(BlueprintCallable)
	void SetCameraOffsetScale_X(float Scale);

	UFUNCTION(BlueprintCallable)
	void SetCameraOffsetScale_Y(float Scale);
	
	UFUNCTION(BlueprintPure)
	float GetCameraOffsetScale_X() const;

	UFUNCTION(BlueprintPure)
	float GetCameraOffsetScale_Y() const;

	UFUNCTION(BlueprintCallable)
	void RotateCharacterCameraBoomYaw(float Yaw, bool bAllowMove);

	UFUNCTION(BlueprintCallable)
	void AdjustCharacterCameraBoom(float Length = 600.f, float Pitch = -25.f, bool bAllowMove = false);

	UFUNCTION(BlueprintCallable)
	void SetCharacterCameraBoom(float Length = 600.f, float Pitch = -25.f);

	static void SetGamePlayers(class AEvergreenCharacter* InThirdPersonPlayer, class AEvergreenPawn* InInteractionPlayer)
	{
		ThirdPersonPlayer = InThirdPersonPlayer;
		InteractionPlayer = InInteractionPlayer;
	}
	
private:
	static AEvergreenCharacter* ThirdPersonPlayer;
	static AEvergreenPawn* InteractionPlayer;
	
	static UObject* CurrentObservedObject;
	FTimerHandle TimerHandle;
	
	void CallOnStartObserve();
	void CallOnAttainPlayerView();
};

UObject* UViewManager::CurrentObservedObject = nullptr;
AEvergreenCharacter* UViewManager::ThirdPersonPlayer = nullptr;
AEvergreenPawn* UViewManager::InteractionPlayer = nullptr;
