// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "TypewriterTextWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayCompleted);

UCLASS(Abstract)
class EVERGREEN_API UTypewriterTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	FOnPlayCompleted OnPlayCompleted;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;

	UPROPERTY(Transient, meta = (BindWidget))
	USizeBox* RootSizeBox;

	UPROPERTY(Transient, meta = (BindWidget))
	UTextBlock* Text;
	
	UFUNCTION(BlueprintCallable, Category = "TypewriterText")
	void SetTypewriter(FText InText = FText(), float InDelay = 0.2, int InCharNumPerDelay = 1);

	UFUNCTION(BlueprintCallable, Category = "TypewriterText")
	void Show(bool bAutoPlay = false, bool bFadeIn = false);

	UFUNCTION(BlueprintCallable, Category = "TypewriterText")
	void Hide();

private:
	bool bPlaying = false;
	FString FullString;
	float StartTimestamp;
	float Delay;
	int CharNumPerDelay = 1;
	
	void WriteText();
	void BroadcastOnPlayFinished();
};