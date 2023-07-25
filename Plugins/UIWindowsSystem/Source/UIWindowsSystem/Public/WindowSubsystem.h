// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WindowSubsystem.generated.h"



class UWindowWidget;

UCLASS()
class UIWINDOWSSYSTEM_API UWindowSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:

	static UWindowSubsystem* Get(UObject* WorldContextObject);

	void AddWindow(UWindowWidget* Window);
	void RemoveWindow(UWindowWidget* Window);

	void CloseTopWindow();
	void CloseAllWindows();

	UWindowWidget* GetTopWindow() const;

private:

	UPROPERTY()
	TArray<UWindowWidget*> WindowStack;

	UPROPERTY()
	TArray<UWindowWidget*> ExclusiveWindowStack;
};