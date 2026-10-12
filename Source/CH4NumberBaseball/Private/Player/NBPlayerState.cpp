#include "Player/NBPlayerState.h"

#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState()
	: CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ANBPlayerState::GetPlayerInfoString() const
{
	return FString::Printf(TEXT("%s [%d/%d]"), *GetPlayerName(), CurrentGuessCount, MaxGuessCount);
}

void ANBPlayerState::IncreaseGuessCount()
{
	++CurrentGuessCount;
}

void ANBPlayerState::ResetGuessCount()
{
	CurrentGuessCount = 0;
}