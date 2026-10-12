#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNB, Log, All);

#define NB_NETMODE_STRING(Actor) \
	((Actor)->GetNetMode() == NM_Client ? TEXT("Client") : \
	((Actor)->GetNetMode() == NM_Standalone ? TEXT("Standalone") : TEXT("Server")))

#define NB_LOG(Actor, Format, ...) \
	UE_LOG(LogNB, Warning, TEXT("[%s] %s: ") Format, NB_NETMODE_STRING(Actor), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)