// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "EnhancedInputComponent.h"
#include "CharacterAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UCharacterAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

class UInputAction;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32 InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Category = "AbilityInput", meta = (BlueprintSpawnableComponent))
class WB2023_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearAbilityBindings(UInputAction* InputAction);


	bool CharacterAbilitiesGiven = false;
	bool StartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;	// Recieved damage event that is broadcasted to

	virtual void ReceiveDamage(UCharacterAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float Mitigated);

private:
	void OnAbilityInputPressed(UInputAction* InputAction);

	void OnAbilityInputReleased(UInputAction* InputAction);

	void RemoveEntry(UInputAction* InputAction);

	void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);

	virtual void BeginPlay() override;

	UPROPERTY(transient)
	TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;

	UPROPERTY(transient)
	UEnhancedInputComponent* InputComponent;

};
