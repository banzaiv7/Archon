// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class ARCHON_API State;
class ARCHON_API ACombatZone;
class ARCHON_API ATenny;

UCLASS()
class ARCHON_API AEnemy : public ACharacter
{
	GENERATED_BODY()

private:
	//class State;
	State* m_pCurrentState;

public:
	// Sets default values for this character's properties
	AEnemy(const FObjectInitializer &ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float PursueSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float PatrolSpeed;
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* WalkAnim;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* RunAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* AttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* WalkAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* DeathAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAudioComponent* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		UAudioComponent* Voice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		ACombatZone* CombatManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
		ATenny* Tenebris;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool IsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool IsFrozen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		bool IsDead;

	void SetSpeed(float newSpeed);

	void DestroyActor();

	//components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		USphereComponent* RangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		UStaticMeshComponent* SightCone;

	//range
	UFUNCTION()
		void OnRangeBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnRangeEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//sight
	UFUNCTION()
		void OnSightBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void DealDamage();

	//damage event
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	//list of waypoints for enemy to follow
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> Waypoints;

	//handles index of Waypoints array
	//wpTracker + 1 is the destination index
	int wpTracker;

	//AActor* currentWaypoint; //will be set to Waypoints[wpTracker]

	AActor* destinationPoint; //will be set to Waypoints[wpTracker]

	//State handler
	void ChangeState(State* pNewState);
};