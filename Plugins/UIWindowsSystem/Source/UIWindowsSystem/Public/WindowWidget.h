// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UIWINDOWSSYSTEM_API UWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	static UWindowWidget* CreateWindow(UObject* WorldContextObject, TSubclassOf<UWindowWidget> WindowClass, APlayerController* OwningPlayer);

	void CloseWindow();

	bool IsExclusive() const { return bExclusiveWindow; }

	int32 GetWindowIndex() const { return m_WindowIndex; }
	void SetWindowIndex(uint8 InWindowIndex) { m_WindowIndex = InWindowIndex; }

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Window")
	bool bExclusiveWindow = false;

private:

	int32 m_WindowIndex = INDEX_NONE;
};