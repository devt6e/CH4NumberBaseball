#include "Game/NBGameModeBase.h"

#include "Game/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "TimerManager.h"
#include "../CH4NumberBaseball.h"

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	NB_LOG(this, TEXT("SecretNumber=%s"), *SecretNumberString);
}

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController) == false)
	{
		return;
	}

	AllPlayerControllers.Add(NBPlayerController);
	NBPlayerController->SetNotificationText(FText::FromString(TEXT("Connected to the game server.")));

	ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPlayerState) == true)
	{
		++PlayerNumberCounter;
		NBPlayerState->SetPlayerName(FString::Printf(TEXT("Player%d"), PlayerNumberCounter));

		ANBGameStateBase* NBGameState = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameState) == true)
		{
			NBGameState->MulticastRPCBroadcastLoginMessage(NBPlayerState->GetPlayerName());
		}
	}

	NB_LOG(this, TEXT("PlayerCount=%d"), AllPlayerControllers.Num());
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(Exiting);
	if (IsValid(NBPlayerController) == true)
	{
		AllPlayerControllers.Remove(NBPlayerController);
	}

	Super::Logout(Exiting);
}

void ANBGameModeBase::ProcessChatMessage(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	ANBPlayerState* NBPlayerState = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPlayerState) == false)
	{
		return;
	}

	// 1. 숫자가 없으면 일반 채팅
	if (IsGuessAttempt(InChatMessageString) == false)
	{
		BroadcastChatMessage(NBPlayerState->GetPlayerInfoString() + TEXT(": ") + InChatMessageString);
		return;
	}

	if (bIsRoundOver == true)
	{
		SendSystemMessage(InChattingPlayerController, TEXT("잠시 후 새 게임이 시작됩니다."));
		return;
	}

	// 2. 기회를 다 썼으면 차단
	if (NBPlayerState->HasRemainingGuess() == false)
	{
		SendSystemMessage(InChattingPlayerController, TEXT("기회를 모두 사용했습니다."));
		return;
	}

	// 3. 규칙에 맞지 않으면 안내만 하고 기회는 유지
	const FString GuessNumberString = InChatMessageString.TrimStartAndEnd();
	if (IsGuessNumberString(GuessNumberString) == false)
	{
		SendSystemMessage(InChattingPlayerController, TEXT("다시 입력하세요. (1~9 중 서로 다른 숫자 3개)"));
		return;
	}

	// 4. 유효한 추측: 기회 소진 후 판정
	NBPlayerState->IncreaseGuessCount();

	int32 StrikeCount = 0;
	const FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString, StrikeCount);

	BroadcastChatMessage(FString::Printf(TEXT("%s: %s -> %s"),
		*NBPlayerState->GetPlayerInfoString(), *GuessNumberString, *JudgeResultString));

	JudgeGame(InChattingPlayerController, StrikeCount);
}

void ANBGameModeBase::BroadcastChatMessage(const FString& InMessageString)
{
	for (const TObjectPtr<ANBPlayerController>& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == true)
		{
			NBPlayerController->ClientRPCPrintChatMessageString(InMessageString);
		}
	}
}

FString ANBGameModeBase::GenerateSecretNumber() const
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FRandomStream RandomStream(static_cast<int32>(FDateTime::Now().GetTicks()));

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		const int32 Index = RandomStream.RandRange(0, Numbers.Num() - 1);
		Result.AppendInt(Numbers[Index]);
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ANBGameModeBase::IsGuessNumberString(const FString& InNumberString) const
{
	if (InNumberString.Len() != 3)
	{
		return false;
	}

	TSet<TCHAR> UniqueDigits;
	for (const TCHAR C : InNumberString)
	{
		if (FChar::IsDigit(C) == false || C == TEXT('0'))
		{
			return false;
		}

		UniqueDigits.Add(C);
	}

	return UniqueDigits.Num() == 3;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString, int32& OutStrikeCount) const
{
	OutStrikeCount = 0;
	int32 BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			++OutStrikeCount;
			continue;
		}

		int32 FoundIndex = INDEX_NONE;
		if (InSecretNumberString.FindChar(InGuessNumberString[i], FoundIndex) == true)
		{
			++BallCount;
		}
	}

	if (OutStrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), OutStrikeCount, BallCount);
}

bool ANBGameModeBase::IsGuessAttempt(const FString& InMessageString) const
{
	for (const TCHAR C : InMessageString)
	{
		if (FChar::IsDigit(C) == true)
		{
			return true;
		}
	}

	return false;
}

void ANBGameModeBase::SendSystemMessage(ANBPlayerController* InTargetPlayerController, const FString& InMessageString)
{
	if (IsValid(InTargetPlayerController) == true)
	{
		InTargetPlayerController->ClientRPCPrintChatMessageString(TEXT("[System] ") + InMessageString);
	}
}

void ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int32 InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ANBPlayerState* WinnerPlayerState = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		const FString WinnerName = IsValid(WinnerPlayerState) == true ? WinnerPlayerState->GetPlayerName() : TEXT("Unknown");

		EndRound(WinnerName + TEXT(" has won the game!"));
		return;
	}

	if (IsDraw() == true)
	{
		EndRound(TEXT("Draw..."));
	}
}

bool ANBGameModeBase::IsDraw() const
{
	for (const TObjectPtr<ANBPlayerController>& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == false)
		{
			continue;
		}

		const ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState) == true && NBPlayerState->HasRemainingGuess() == true)
		{
			// 한 명이라도 기회가 남아 있으면 아직 무승부 아님
			return false;
		}
	}

	return true;
}

void ANBGameModeBase::EndRound(const FString& InResultMessageString)
{
	bIsRoundOver = true;

	SetNotificationTextToAll(InResultMessageString);

	GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ThisClass::ResetGame, ResetDelaySeconds, false);
}

void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	NB_LOG(this, TEXT("SecretNumber=%s"), *SecretNumberString);

	for (const TObjectPtr<ANBPlayerController>& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == false)
		{
			continue;
		}

		ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState) == true)
		{
			NBPlayerState->ResetGuessCount();
		}
	}

	bIsRoundOver = false;

	SetNotificationTextToAll(TEXT("New game started!"));
}

void ANBGameModeBase::SetNotificationTextToAll(const FString& InMessageString)
{
	const FText NotificationText = FText::FromString(InMessageString);

	for (const TObjectPtr<ANBPlayerController>& NBPlayerController : AllPlayerControllers)
	{
		if (IsValid(NBPlayerController) == true)
		{
			NBPlayerController->SetNotificationText(NotificationText);
		}
	}
}