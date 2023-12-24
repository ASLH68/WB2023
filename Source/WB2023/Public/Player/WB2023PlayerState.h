// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "WB2023PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WB2023_API AWB2023PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AWB2023PlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCharacterAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState")
	bool IsAlive() const;

	/// <summary>
	/// Shows ability placement confirmation text
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "WB2023|WB2023PlayerState|Attributes")
	int32 GetCharacterLevel() const;

protected:
	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;	

	UPROPERTY()
	class UCharacterAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;	// is dead or alive

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void BeginPlay() override;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
