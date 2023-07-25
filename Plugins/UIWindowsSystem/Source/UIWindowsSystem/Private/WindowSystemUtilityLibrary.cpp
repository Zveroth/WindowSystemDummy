// Fill out your copyright notice in the Description page of Project Settings.
#include "WindowSystemUtilityLibrary.h"
#include "WindowSubsystem.h"
#include "WindowWidget.h"



UWindowWidget* UWindowSystemUtilityLibrary::CreateWindow(UObject* WorldContextObject, TSubclassOf<UWindowWidget> WindowClass, APlayerController* OwningPlayer)
{
	return UWindowWidget::CreateWindow(WorldContextObject, WindowClass, OwningPlayer);
}

void UWindowSystemUtilityLibrary::CloseTopWindow(UObject* WorldContextObject)
{
	UWindowSubsystem::Get(WorldContextObject)->CloseTopWindow();
}

void UWindowSystemUtilityLibrary::CloseAllWindows(UObject* WorldContextObject)
{
	UWindowSubsystem::Get(WorldContextObject)->CloseAllWindows();
}