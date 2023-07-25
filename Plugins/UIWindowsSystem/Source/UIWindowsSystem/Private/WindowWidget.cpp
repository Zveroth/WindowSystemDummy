// Fill out your copyright notice in the Description page of Project Settings.
#include "WindowWidget.h"
#include "WindowSubsystem.h"



UWindowWidget* UWindowWidget::CreateWindow(UObject* WorldContextObject, TSubclassOf<UWindowWidget> WindowClass, APlayerController* OwningPlayer)
{
	if (WindowClass == nullptr)
	{
		return nullptr;
	}

	UWindowWidget* Window = nullptr;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (!IsValid(World))
	{
		return nullptr;
	}

	if (OwningPlayer)
	{
		Window = CreateWidget<UWindowWidget>(OwningPlayer, WindowClass);
	}
	else if (APlayerController* ImpliedOwningPlayer = Cast<APlayerController>(WorldContextObject))
	{
		Window = CreateWidget<UWindowWidget>(ImpliedOwningPlayer, WindowClass);
	}
	else if (UUserWidget* OwningWidget = Cast<UUserWidget>(WorldContextObject))
	{
		Window = CreateWidget<UWindowWidget>(OwningWidget, WindowClass);
	}
	else
	{
		Window = CreateWidget<UWindowWidget>(World, WindowClass);
	}

	UWindowSubsystem* const WindowSubsystem = World->GetSubsystem<UWindowSubsystem>(World);

	if (!IsValid(WindowSubsystem))
	{
		return nullptr;
	}

	WindowSubsystem->AddWindow(Window);

	return Window;
}

void UWindowWidget::CloseWindow()
{
	UWindowSubsystem::Get(this)->RemoveWindow(this);
}