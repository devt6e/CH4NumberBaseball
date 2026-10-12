#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

UCLASS()
class CH4NUMBERBASEBALL_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString() const;

	int32 GetCurrentGuessCount() const { return CurrentGuessCount; }
	int32 GetMaxGuessCount() const { return MaxGuessCount; }

	void IncreaseGuessCount();
	void ResetGuessCount();

	bool HasRemainingGuess() const { return CurrentGuessCount < MaxGuessCount; }

protected:
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
};