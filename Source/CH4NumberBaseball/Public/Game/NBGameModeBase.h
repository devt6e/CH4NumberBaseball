#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

UCLASS()
class CH4NUMBERBASEBALL_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void ProcessChatMessage(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void ResetGame();

protected:
	void BroadcastChatMessage(const FString& InMessageString);

	FString GenerateSecretNumber() const;

	bool IsGuessNumberString(const FString& InNumberString) const;

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString, int32& OutStrikeCount) const;

	bool IsGuessAttempt(const FString& InMessageString) const;

	void SendSystemMessage(ANBPlayerController* InTargetPlayerController, const FString& InMessageString);

protected:
	void JudgeGame(ANBPlayerController* InChattingPlayerController, int32 InStrikeCount);

	bool IsDraw() const;

	void EndRound(const FString& InResultMessageString);

	void SetNotificationTextToAll(const FString& InMessageString);

protected:
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;

	int32 PlayerNumberCounter = 0;

	FString SecretNumberString;

	UPROPERTY(EditDefaultsOnly, Category = "NumberBaseball")
	float ResetDelaySeconds = 3.0f;

	FTimerHandle ResetTimerHandle;

	bool bIsRoundOver = false;
};