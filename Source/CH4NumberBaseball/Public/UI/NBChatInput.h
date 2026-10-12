#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "NBChatInput.generated.h"

class UEditableTextBox;
class UScrollBox;

UCLASS()
class CH4NUMBERBASEBALL_API UNBChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddChatMessage(const FString& InMessageString);

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ChatLog;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	int32 MaxChatLogCount = 50;
};