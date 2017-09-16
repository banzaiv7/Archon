// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Archon_Player.generated.h"

UCLASS()
class ARCHON_API AArchon_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArchon_Player(const FObjectInitializer &ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void MoveForward(float amount);
	void Strafe(float amount);
	void Yaw(float amount);
	void Pitch(float amount);

	const TArray<AActor*> actorsToIgnore;
	float UltCooldown;
	FTimerHandle UnusedHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float Anima;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Freeze")
		float RootTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thunder Drums")
		float HighSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thunder Drums")
		float LowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thunder Drums")
		float CombatSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		bool IsInCombat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thunder Drums")
		bool DrumsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float RegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float AnimaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float AnimaResetMark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float Sensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAudioComponent* DamagedSound;

	UPROPERTY(EditAnywhere, Category = "DeadlyFire")
		UParticleSystem* ExplosionParticle;

	UPROPERTY(EditAnywhere, Category = "Flash Freeze")
		UParticleSystem* IceParticle;

	UPROPERTY(EditAnywhere, Category = "Thunder Drums")
		UParticleSystem* DrumsParticle;

	//Ultimate Abilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyFire")
		URadialForceComponent* DeadlyFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeadlyFire")
		UAudioComponent* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Freeze")
		USphereComponent* FlashFreeze;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash Freeze")
		UAudioComponent* FreezeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thunder Drums")
		UAudioComponent* DrumsSound;
	
	//hand animations
	//sets for right and left hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* RIdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* RCastAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* RWalkAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* RNiceAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* LIdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* LCastAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* LNiceAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimSequence* LWalkAnim;

	//custom events for hand anims
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Animations")
		void PlayRHandAnim();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Animations")
		void PlayLHandAnim();

	//UI purposes
	UPROPERTY(BlueprintReadWrite)
		float UltCooldownTime;

	UFUNCTION(BlueprintCallable, Category = "UI")
		float UltTimeRemain();

	UFUNCTION(BlueprintCallable, Category = "Ult")
		void EnableUlt();

	UFUNCTION(BlueprintCallable, Category = "Ult")
		void DisableUlt();

	bool IsInAnimaCooldown;

	UPROPERTY(BlueprintReadOnly, Category = "Ult")
		bool IsUltReady;

	void ActivateDeadlyFire();
	void ActivateFlashFreeze();
	void ActivateThunderDrums();
	void InitiateCooldown(float time);
	void DFCont();
	void EnableMovement();
	void SwitchLow();
	void RestoreSpeed();
	void Regen();
	void AnimaRegen();
	void SetPlayerSpeed();
	void CheckHealth();
	void AnimaCooldown();
	void Reset();
	
	//damage event
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
};
