// Fill out your copyright notice in the Description page of Project Settings.
#include "K2Node_CreateWindow.h"

#if WITH_EDITOR
	#include "GameFramework/PlayerController.h"
#endif // WITH_EDITOR
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "Blueprint/UserWidget.h"

#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "SGraphNodeCreateWindow.h"

#include "WindowSystemUtilityLibrary.h"
#include "WindowWidget.h"


#define LOCTEXT_NAMESPACE "UMG"

struct FK2Node_CreateWindowHelper
{
	static const FName OwningPlayerPinName;
};

const FName FK2Node_CreateWindowHelper::OwningPlayerPinName(TEXT("OwningPlayer"));

UK2Node_CreateWindow::UK2Node_CreateWindow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Creates a new window and adds it to the screen");
}

void UK2Node_CreateWindow::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// OwningPlayer pin
	UEdGraphPin* OwningPlayerPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, APlayerController::StaticClass(), FK2Node_CreateWindowHelper::OwningPlayerPinName);
	SetPinToolTip(*OwningPlayerPin, LOCTEXT("OwningPlayerPinDescription", "The player that 'owns' the widget."));
}

FText UK2Node_CreateWindow::GetBaseNodeTitle() const
{
	return LOCTEXT("CreateWindow_BaseTitle", "Create Window");
}

FText UK2Node_CreateWindow::GetNodeTitleFormat() const
{
	return LOCTEXT("CreateWindow", "Create {ClassName} Window");
}

UClass* UK2Node_CreateWindow::GetClassPinBaseClass() const
{
	return UWindowWidget::StaticClass();
}

FText UK2Node_CreateWindow::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::UserInterface);
}

FName UK2Node_CreateWindow::GetCornerIcon() const
{
	return TEXT("Graph.Replication.ClientEvent");
}

UEdGraphPin* UK2Node_CreateWindow::GetOwningPlayerPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_CreateWindowHelper::OwningPlayerPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

bool UK2Node_CreateWindow::IsSpawnVarPin(UEdGraphPin* Pin) const
{
	return(Super::IsSpawnVarPin(Pin) &&
		Pin->PinName != FK2Node_CreateWindowHelper::OwningPlayerPinName);
}

void UK2Node_CreateWindow::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName Create_FunctionName = GET_FUNCTION_NAME_CHECKED(UWindowSystemUtilityLibrary, CreateWindow);
	static const FName WorldContextObject_ParamName(TEXT("WorldContextObject"));
	static const FName WindowClass_ParamName(TEXT("WindowClass"));
	static const FName OwningPlayer_ParamName(TEXT("OwningPlayer"));
	
	UK2Node_CreateWindow* CreateWidgetNode = this;
	UEdGraphPin* SpawnNodeExec = CreateWidgetNode->GetExecPin();
	UEdGraphPin* SpawnWorldContextPin = CreateWidgetNode->GetWorldContextPin();
	UEdGraphPin* SpawnOwningPlayerPin = CreateWidgetNode->GetOwningPlayerPin();
	UEdGraphPin* SpawnClassPin = CreateWidgetNode->GetClassPin();
	UEdGraphPin* SpawnNodeThen = CreateWidgetNode->GetThenPin();
	UEdGraphPin* SpawnNodeResult = CreateWidgetNode->GetResultPin();
	
	UClass* SpawnClass = (SpawnClassPin != NULL) ? Cast<UClass>(SpawnClassPin->DefaultObject) : NULL;
	if (!SpawnClassPin || ((0 == SpawnClassPin->LinkedTo.Num()) && (NULL == SpawnClass)))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("CreateWidgetNodeMissingClass_Error", "Spawn node @@ must have a class specified.").ToString(), CreateWidgetNode);
		// we break exec links so this is the only error we get, don't want the CreateWidget node being considered and giving 'unexpected node' type warnings
		CreateWidgetNode->BreakAllNodeLinks();
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// create 'UWidgetBlueprintLibrary::Create' call node
	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CreateWidgetNode, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(Create_FunctionName, UWindowSystemUtilityLibrary::StaticClass());
	CallCreateNode->AllocateDefaultPins();
	
	// store off the class to spawn before we mutate pin connections:
	UClass* ClassToSpawn = GetClassToSpawn();
	
	UEdGraphPin* CallCreateExec = CallCreateNode->GetExecPin();
	UEdGraphPin* CallCreateWorldContextPin = CallCreateNode->FindPinChecked(WorldContextObject_ParamName);
	UEdGraphPin* CallCreateWidgetTypePin = CallCreateNode->FindPinChecked(WindowClass_ParamName);
	UEdGraphPin* CallCreateOwningPlayerPin = CallCreateNode->FindPinChecked(OwningPlayer_ParamName);
	UEdGraphPin* CallCreateResult = CallCreateNode->GetReturnValuePin();
	
	// Move 'exec' connection from create widget node to 'UWidgetBlueprintLibrary::Create'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallCreateExec);
	
	if (SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'blueprint' connection from the spawn node to 'UWidgetBlueprintLibrary::Create'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallCreateWidgetTypePin);
	}
	else
	{
		// Copy blueprint literal onto 'UWidgetBlueprintLibrary::Create' call
		CallCreateWidgetTypePin->DefaultObject = SpawnClass;
	}
	
	// Copy the world context connection from the spawn node to 'UWidgetBlueprintLibrary::Create' if necessary
	if (SpawnWorldContextPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallCreateWorldContextPin);
	}
	
	// Copy the 'Owning Player' connection from the spawn node to 'UWidgetBlueprintLibrary::Create'
	CompilerContext.MovePinLinksToIntermediate(*SpawnOwningPlayerPin, *CallCreateOwningPlayerPin);
	
	// Move result connection from spawn node to 'UWidgetBlueprintLibrary::Create'
	CallCreateResult->PinType = SpawnNodeResult->PinType; // Copy type so it uses the right actor subclass
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallCreateResult);
	
	//////////////////////////////////////////////////////////////////////////
	// create 'set var' nodes
	
	// Get 'result' pin from 'begin spawn', this is the actual actor we want to set properties on
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, CreateWidgetNode, CallCreateResult, ClassToSpawn);
	
	// Move 'then' connection from create widget node to the last 'then'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen);
	
	// Break any links to the expanded node
	CreateWidgetNode->BreakAllNodeLinks();
}

TSharedPtr<SGraphNode> UK2Node_CreateWindow::CreateVisualWidget()
{
	return SNew(SGraphNodeCreateWindow, this);
}

#undef LOCTEXT_NAMESPACE