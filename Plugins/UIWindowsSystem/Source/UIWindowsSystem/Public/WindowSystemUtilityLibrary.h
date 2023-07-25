// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindowSystemUtilityLibrary.generated.h"



class UWindowWidget;

UCLASS()
class UIWINDOWSSYSTEM_API UWindowSystemUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UWindowWidget* CreateWindow(UObject* WorldContextObject, TSubclassOf<UWindowWidget> WindowClass, APlayerController* OwningPlayer);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void CloseTopWindow(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void CloseAllWindows(UObject* WorldContextObject);
};