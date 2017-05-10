// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "DamagePlatform.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Archon_Player.h"

// Sets default values
ADamagePlatform::ADamagePlatform(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//creating a box trigger
	DamageZone = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("DamageZone"));
	DamageZone->SetupAttachment(RootComponent);

	//adding collision events
	DamageZone->OnComponentBeginOverlap.AddDynamic(this, &ADamagePlatform::OnEnterDamageZone);
	DamageZone->OnComponentEndOverlap.AddDynamic(this, &ADamagePlatform::OnExitDamageZone);

	//default values
	Damage = 20.0f;
	IsActive = false;
	PlayerInZone = false;
}

// Called when the game starts or when spawned
void ADamagePlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADamagePlatform::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//if the player is within the zone
	if (PlayerInZone)
	{
		//damage the player
		DealDamage();
	}
}

//called by the boss
//determines of the platform will do damage
void ADamagePlatform::SetDamageActive(bool status)
{
	IsActive = status;
	this->SetActorTickEnabled(status);
}

void ADamagePlatform::DealDamage()
{
	//damage the player only of the platform is active
	//and the player is within the damage zone
	if (IsActive == true && PlayerInZone == true)
	{
		UGameplayStatics::ApplyDamage(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), Damage, NULL, this, NULL);
	}
}

//detection for collision entrance
void ADamagePlatform::OnEnterDamageZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerInZone = true;
	//UKismetSystemLibrary::K2_SetTimer(this, "DealDamage", 1.0f, true);
}

//detection for collision exit
void ADamagePlatform::OnExitDamageZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerInZone = false;
}