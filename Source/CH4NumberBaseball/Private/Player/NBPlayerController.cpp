#include "Player/NBPlayerController.h"

#include "UI/NBChatInput.h"
#include "Game/NBGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "../CH4NumberBaseball.h"

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	NB_LOG(this, TEXT("IsLocal=%d, WidgetClass=%s"), IsLocalController(), *GetNameSafe(ChatInputWidgetClass));

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UNBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	NB_LOG(this, TEXT("IsLocal=%d"), IsLocalController());

	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	if (IsValid(ChatInputWidgetInstance) == true)
	{
		ChatInputWidgetInstance->AddChatMessage(InChatMessageString);
	}
}

void ANBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	NB_LOG(this, TEXT("GameMode=%s"), *GetNameSafe(GetWorld()->GetAuthGameMode()));

	ANBGameModeBase* NBGameMode = GetWorld()->GetAuthGameMode<ANBGameModeBase>();
	if (IsValid(NBGameMode) == true)
	{
		NBGameMode->ProcessChatMessage(this, InChatMessageString);
	}
}

void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	NB_LOG(this, TEXT("Received=%s"), *InChatMessageString);
	PrintChatMessageString(InChatMessageString);
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ANBPlayerController::SetNotificationText(const FText& InNotificationText)
{
	NotificationText = InNotificationText;
}