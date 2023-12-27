// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/WB2023PlayerController.h"
#include "Player/WB2023PlayerState.h"
#include "AbilitySystemComponent.h"

void AWB2023PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AWB2023PlayerState* PS = GetPlayerState<AWB2023PlayerState>();

    if (PS)
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
    }
}

// TODO -- Add HUD Stuff
