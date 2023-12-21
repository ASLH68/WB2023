// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "WB2023/WB2023.h"
#include "CharBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharBase*, Character);

/**
 *
 */
UCLASS()
class WB2023_API ACharBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharBase(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FCharacterDiedDelegate OnCharacterDied;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual int32 GetAbilityLevel(CharAbilityID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();

	/**
	* Getters for attributes from GDAttributeSetBase
	**/

	UFUNCTION(BlueprintCallable, Category = "Character|Attribute")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Character|Attribute")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character|Attribute")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character|Attribute")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Character|Attribute")
	float GetMaxMana() const;



	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void FinishDying();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	// Describes the state of the object (example: stunned)
	// Can have multiple tags
	// Can set "rules" based on tags, example: can't use ability if has certain tag (like stunned)
	FGameplayTag DeadTag; //Indicates if player is dead
	FGameplayTag EffectRemoveOnDeathTag;	// called when player has died and can be removed

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character|Abilities")
	TArray<TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

	// Not an array since one gameplay effect can set all the attributes
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;	// Setting default values to the attributes

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	/// <summary>
	/// Adds abilities to the character
	/// </summary>
	virtual void AddCharacterAbilities();

	/// <summary>
	/// Initializes Attributes
	/// </summary>
	virtual void InitializeAttributes();

	/// <summary>
	/// Adds any startup effects
	/// </summary>
	virtual void AddStartupEffects();

	/**
	* Setters for Attributes. Only use these in special cases like Respawning, otherwise use a GE to change Attributes.
	* These change the Attribute's Base Value.
	*/
	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
};
