// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TheAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class WB2023_API UTheAssetManager : public UAssetManager
{
	GENERATED_BODY()

	public:
	virtual void StartInitialLoading() override;
	
};
