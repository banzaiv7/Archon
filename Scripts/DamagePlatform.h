// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DamagePlatform.generated.h"

UCLASS()
class ARCHON_API ADamagePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagePlatform(const FObjectInitializer &ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//player colliding with this object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		UBoxComponent* DamageZone;

	//referenced by Tenny
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool IsActive;

	//per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float Damage;

	//checks if player is within damage zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool PlayerInZone;
	
	//***********************Collision****************************

	UFUNCTION()
		void OnEnterDamageZone(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnExitDamageZone(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//************************************************************

	void SetDamageActive(bool status);

	void DealDamage();
};
