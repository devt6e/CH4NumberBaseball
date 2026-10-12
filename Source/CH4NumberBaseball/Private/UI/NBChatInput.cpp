#include "UI/NBChatInput.h"

#include "Blueprint/WidgetTree.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Player/NBPlayerController.h"

void UNBChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::AddChatMessage(const FString& InMessageString)
{
	UTextBlock* ChatTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	if (IsValid(ChatTextBlock) == false)
	{
		return;
	}

	ChatTextBlock->SetText(FText::FromString(InMessageString));
	ChatTextBlock->SetAutoWrapText(true);
	ChatTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ChatTextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
	ChatTextBlock->SetShadowColorAndOpacity(FLinearColor::Black);

	ScrollBox_ChatLog->AddChild(ChatTextBlock);

	// 오래된 메시지부터 정리해서 무한히 쌓이지 않게
	while (ScrollBox_ChatLog->GetChildrenCount() > MaxChatLogCount)
	{
		ScrollBox_ChatLog->RemoveChildAt(0);
	}

	ScrollBox_ChatLog->ScrollToEnd();
}

void UNBChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter || Text.IsEmpty() == true)
	{
		return;
	}

	ANBPlayerController* OwningNBPlayerController = Cast<ANBPlayerController>(GetOwningPlayer());
	if (IsValid(OwningNBPlayerController) == true)
	{
		OwningNBPlayerController->SetChatMessageString(Text.ToString());

		EditableTextBox_ChatInput->SetText(FText());
		EditableTextBox_ChatInput->SetKeyboardFocus();
	}
}

void UNBChatInput::ClearChatLog()
{
	ScrollBox_ChatLog->ClearChildren();
}