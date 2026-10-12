#include "Game/NBGameStateBase.h"

#include "Player/NBPlayerController.h"

void ANBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	// 데디케이티드 서버에는 화면이 없으니 클라에서만 출력
	if (HasAuthority() == true)
	{
		return;
	}

	ANBPlayerController* LocalNBPlayerController = Cast<ANBPlayerController>(GetWorld()->GetFirstPlayerController());
	if (IsValid(LocalNBPlayerController) == true)
	{
		LocalNBPlayerController->PrintChatMessageString(InNameString + TEXT(" has joined the game."));
	}
}