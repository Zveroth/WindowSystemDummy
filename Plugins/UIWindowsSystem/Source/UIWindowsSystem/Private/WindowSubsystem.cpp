// Fill out your copyright notice in the Description page of Project Settings.
#include "WindowSubsystem.h"
#include "WindowWidget.h"



UWindowSubsystem* UWindowSubsystem::Get(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (!IsValid(World))
	{
		return nullptr;
	}

	return World->GetSubsystem<UWindowSubsystem>(World);
}

void UWindowSubsystem::AddWindow(UWindowWidget* Window)
{
	
	if (Window->IsExclusive())
	{
		for (UWindowWidget* StackWindow : WindowStack)
		{
			if (IsValid(StackWindow))
			{
				StackWindow->RemoveFromParent();
			}
		}

		ExclusiveWindowStack.Add(Window);
	}


	int32 WindowIndex = WindowStack.Add(Window);
	Window->SetWindowIndex(WindowIndex);
	Window->AddToViewport();
}

void UWindowSubsystem::RemoveWindow(UWindowWidget* Window)
{
	const int32 WindowIndex = Window->GetWindowIndex();
	WindowStack.RemoveAt(WindowIndex);

	if (Window->IsInViewport())
	{
		Window->RemoveFromParent();
	}

	if (Window->IsExclusive())
	{
		ExclusiveWindowStack.RemoveSingle(Window);

		if (ExclusiveWindowStack.Num() != 0)
		{
			for (int32 I = ExclusiveWindowStack.Last()->GetWindowIndex(); I < WindowStack.Num(); ++I)
			{
				UWindowWidget* const StackWindow = WindowStack[I];
				if (!StackWindow->IsInViewport())
				{
					StackWindow->AddToViewport();
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for (UWindowWidget* const StackWindow : WindowStack)
			{
				if (!StackWindow->IsInViewport())
				{
					StackWindow->AddToViewport();
				}
				else
				{
					break;
				}
			}
		}
	}
}

void UWindowSubsystem::CloseTopWindow()
{
	if (WindowStack.Num() != 0)
	{
		RemoveWindow(WindowStack.Last());
	}
}

void UWindowSubsystem::CloseAllWindows()
{
	for (UWindowWidget* const StackWindow : WindowStack)
	{
		StackWindow->RemoveFromParent();
	}

	WindowStack.Empty();
	ExclusiveWindowStack.Empty();
}

UWindowWidget* UWindowSubsystem::GetTopWindow() const
{
	if (WindowStack.Num() == 0)
	{
		return nullptr;
	}

	return WindowStack.Last();
}