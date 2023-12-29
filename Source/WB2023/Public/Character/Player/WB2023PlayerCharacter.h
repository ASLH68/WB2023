// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharBase.h"
#include "Player/WB2023PlayerState.h"
#include "InputActionValue.h"
#include "WB2023PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class WB2023_API AWB2023PlayerCharacter : public ACharBase
{
	GENERATED_BODY()

public:
	AWB2023PlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// Sets up the enhanced input system and binds actions
	/// </summary>
	/// <param name="PlayerInputComponent"></param>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	// Used when camera boom is modified (casting ability with super large aoe effect to see more of the map then go back to normal)
	UFUNCTION(BlueprintCallable, Category = "WB2023|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "WB2023|Camera")
	FVector GetStartingCameraBoomLocation();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WB2023|Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WB2023|Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "WB2023|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "WB2023|Camera")
	FVector StartingCameraBoomLocation;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "WB2023|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "WB2023|Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_MoveForward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_MoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_TurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_Turn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_LookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WB2023|Input")
	class UInputAction* IA_LookUp;

	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	virtual void BeginPlay() override;

	void LookUp(const FInputActionValue& Value);

	void LookUpRate(const FInputActionValue& Value);

	void Turn(const FInputActionValue& Value);

	void TurnRate(const FInputActionValue& Value);

	void MoveForward(const FInputActionValue& Value);

	void MoveRight(const FInputActionValue& Value);

	virtual void OnRep_PlayerState() override;

	void InitializeStartingValues(AWB2023PlayerState* PS);

	void BindASCInput();
};
