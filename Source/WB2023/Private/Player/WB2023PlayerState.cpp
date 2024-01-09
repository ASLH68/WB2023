// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/WB2023PlayerState.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"

AWB2023PlayerState::AWB2023PlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);

    // Only shows visual queues/ sfx to other players
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

    NetUpdateFrequency = 100.0f;

    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AWB2023PlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UCharacterAttributeSetBase* AWB2023PlayerState::GetAttributeSetBase() const
{
    return AttributeSetBase;
}

bool AWB2023PlayerState::IsAlive() const
{
    return GetHealth() > 0.0f;
}

void AWB2023PlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
    // TODO -- implement HUD later
}

float AWB2023PlayerState::GetHealth() const
{
    return AttributeSetBase->GetHealth();
}

float AWB2023PlayerState::GetMaxHealth() const
{
    return AttributeSetBase->GetMaxHealth();
}

float AWB2023PlayerState::GetMana() const
{
    return AttributeSetBase->GetMana();
}

float AWB2023PlayerState::GetMaxMana() const
{
    return AttributeSetBase->GetMaxMana();
}

int32 AWB2023PlayerState::GetCharacterLevel() const
{
    return AttributeSetBase->GetLevel();
}

void AWB2023PlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        // Linked the value changing event to the functions here
        HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWB2023PlayerState::HealthChanged);
        MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWB2023PlayerState::MaxHealthChanged);
        ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWB2023PlayerState::ManaChanged);
        MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWB2023PlayerState::MaxManaChanged);
        CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AWB2023PlayerState::CharacterLevelChanged);
    
        // Called if the stunned debuff is added or removed
        AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AWB2023PlayerState::StunTagChanged);
    }
}

void AWB2023PlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Health Changed!"));

    FString NewHealth = FString::SanitizeFloat(GetHealth());
    GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *NewHealth);
}

void AWB2023PlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Max Health Changed!"));
}

void AWB2023PlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Mana Changed!"));

    FString NewMana = FString::SanitizeFloat(GetMana());
    GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, *NewMana);
}

void AWB2023PlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Max Mana Changed!"));
}

void AWB2023PlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
    UE_LOG(LogTemp, Warning, TEXT("Character Level Changed!"));
}

void AWB2023PlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    // Want to cancel all abilities since stunned
    if (NewCount > 0)
    {
        FGameplayTagContainer AbilityTagsToCancel;
        AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

        FGameplayTagContainer AbilityTagsToIgnore;
        AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

        AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
    }
}
