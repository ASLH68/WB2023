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
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

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

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, & AWB2023PlayerCharacter::MoveForward);
        EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &AWB2023PlayerCharacter::MoveRight);
        EnhancedInputComponent->BindAction(IA_TurnRate, ETriggerEvent::Triggered, this, &AWB2023PlayerCharacter::TurnRate);
        EnhancedInputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AWB2023PlayerCharacter::Turn);
        EnhancedInputComponent->BindAction(IA_LookUpRate, ETriggerEvent::Triggered, this, &AWB2023PlayerCharacter::LookUpRate);
        EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &AWB2023PlayerCharacter::LookUp);
    }
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

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(CharacterMappingContext, 0);
        }
    }
}

void AWB2023PlayerCharacter::LookUp(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();
        AddControllerPitchInput(FloatValue);
    }
}

void AWB2023PlayerCharacter::LookUpRate(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();
        AddControllerPitchInput(FloatValue * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AWB2023PlayerCharacter::Turn(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();
        AddControllerYawInput(FloatValue);
    }
}

void AWB2023PlayerCharacter::TurnRate(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();
        AddControllerPitchInput(FloatValue * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AWB2023PlayerCharacter::MoveForward(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();
        UE_LOG(LogTemp, Warning, TEXT("Text, %f"), FloatValue);
        FVector Forward = GetActorForwardVector();
        AddMovementInput(Forward, FloatValue);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Dead"));
    }
}

void AWB2023PlayerCharacter::MoveRight(const FInputActionValue& Instance)
{
    if (IsAlive())
    {
        float FloatValue = Instance.Get<float>();

        FVector Right = GetActorRightVector();
        AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), FloatValue);
    }
}

void AWB2023PlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AWB2023PlayerState* PS = GetPlayerState<AWB2023PlayerState>();
    if (PS)
    {
        InitializeStartingValues(PS);
        BindASCInput();
    }  
}

void AWB2023PlayerCharacter::InitializeStartingValues(AWB2023PlayerState* PS)
{
    // Set the ASC on the server, clients use OnRep_PlayerState
    AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    AbilitySystemComponent->InitAbilityActorInfo(PS, this);
    
    // Sets the attributeSetBase for convenience attribute functions
    AttributeSetBase = PS->GetAttributeSetBase();
    InitializeAttributes();
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
            FString("CancelTarget"), FString("/Script/WB2023.CharAbilityID"), static_cast<int32>(CharAbilityID::Confirm), static_cast<int32>(CharAbilityID::Cancel)));
    
        ASCInputBound = true;
    }
}
