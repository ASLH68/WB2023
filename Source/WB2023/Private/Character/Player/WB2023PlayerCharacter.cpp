// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/WB2023PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "AI/PlayerAIController.h"
#include "Player/WB2023PlayerState.h"
#include "Character/Player/WB2023PlayerCharacter.h"
#include "Player/WB2023PlayerController.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWB2023PlayerCharacter::AWB2023PlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0, 0, 70));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->FieldOfView = 80.0f;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));

    AIControllerClass = APlayerAIController::StaticClass();

    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AWB2023PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AWB2023PlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("IA_MoveRight", this, &AWB2023PlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("IA_LookUp", this, &AWB2023PlayerCharacter::LookUp);
    PlayerInputComponent->BindAxis("IA_LookUpRate", this, &AWB2023PlayerCharacter::LookUpRate);
    PlayerInputComponent->BindAxis("IA_Turn", this, &AWB2023PlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("IA_TurnRate", this, &AWB2023PlayerCharacter::TurnRate);
    // Bind player input to the ability system component
    BindASCInput();
}

void AWB2023PlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AWB2023PlayerState* PS = GetPlayerState<AWB2023PlayerState>();
    if (PS)
    {
        InitializeStartingValues(PS);
        InitializeAttributes();
        AddStartupEffects();
        AddCharacterAbilities();
    }
}

USpringArmComponent* AWB2023PlayerCharacter::GetCameraBoom()
{
    return CameraBoom;
}

UCameraComponent* AWB2023PlayerCharacter::GetFollowCamera()
{
    return FollowCamera;
}

float AWB2023PlayerCharacter::GetStartingCameraBoomArmLength()
{
    return StartingCameraBoomArmLength;
}

FVector AWB2023PlayerCharacter::GetStartingCameraBoomLocation()
{
    return StartingCameraBoomLocation;
}

void AWB2023PlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
    StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();
}

void AWB2023PlayerCharacter::LookUp(float Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value);
    }
}

void AWB2023PlayerCharacter::LookUpRate(float Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AWB2023PlayerCharacter::Turn(float Value)
{
    if (IsAlive())
    {
        AddControllerYawInput(Value);
    }
}

void AWB2023PlayerCharacter::TurnRate(float Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AWB2023PlayerCharacter::MoveForward(float Value)
{
    AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void AWB2023PlayerCharacter::MoveRight(float Value)
{
    AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
}

void AWB2023PlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AWB2023PlayerState* PS = GetPlayerState<AWB2023PlayerState>();
    if (PS)
    {
        InitializeStartingValues(PS);
        BindASCInput();
        InitializeAttributes();
    }  
}

void AWB2023PlayerCharacter::InitializeStartingValues(AWB2023PlayerState* PS)
{
    // Set the ASC on the server, clients use OnRep_PlayerState
    AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
    
    // Sets the attributeSetBase for convenience attribute functions
    AttributeSetBase = PS->GetAttributeSetBase();
    AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
    SetHealth(GetMaxHealth());
    SetMana(GetMaxMana());
}

void AWB2023PlayerCharacter::BindASCInput()
{
    if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
    {
        // Tells the ability system component to map confirm target to the enum, with the rest casting to the charabilityid
        AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), 
            FString("CancelTarget"), FString("CharAbilityID"), static_cast<int32>(CharAbilityID::Confirm), static_cast<int32>(CharAbilityID::Cancel)));
    
        ASCInputBound = true;
    }
}
