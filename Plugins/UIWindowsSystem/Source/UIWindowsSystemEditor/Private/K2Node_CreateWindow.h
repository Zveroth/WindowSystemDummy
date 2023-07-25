// Fill out your copyright notice in the Description page of Project Settings.
//Copy of K2Node_CreateWidget

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "K2Node_CreateWindow.generated.h"



class UEdGraph;

UCLASS()
class UK2Node_CreateWindow : public UK2Node_ConstructObjectFromClass
{
	GENERATED_BODY()
	

public:

	UK2Node_CreateWindow(const FObjectInitializer& ObjectInitializer);

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual FText GetMenuCategory() const override;
	virtual FName GetCornerIcon() const override;
	//~ End UK2Node Interface.

	/** Get the owning player pin */
	UEdGraphPin* GetOwningPlayerPin() const;

protected:
	/** Gets the default node title when no class is selected */
	virtual FText GetBaseNodeTitle() const override;
	/** Gets the node title when a class has been selected. */
	virtual FText GetNodeTitleFormat() const override;
	/** Gets base class to use for the 'class' pin.  UObject by default. */
	virtual UClass* GetClassPinBaseClass() const override;
	/**  */
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) const override;
};