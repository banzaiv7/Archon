// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "Enemy.h"

//to reference other scripts
#include "Archon_Player.h"
#include "CombatZone.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemyOwnedStates.h"
#include "Tenny.h"

// Sets default values
AEnemy::AEnemy(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//default values for blueprint
	Health = 100.0f;
	Damage = 10.0f;
	PursueSpeed = 600.0f;
	PatrolSpeed = 200.0f;
	AttackCooldown = 2.0f;
	IsAttacking = false;
	IsFrozen = false;
	IsDead = false;

	//audio
	Voice = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Grunt Voice"));
	DeathSound = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Death Sound"));

	//create a sphere that handles attack range and enable collision events
	RangeSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RangeSphere"));
	RangeSphere->SetupAttachment(RootComponent);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRangeBeginOverlap);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnRangeEndOverlap);

	//create a cone that handles detection and enable collision events
	SightCone = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("SightCone"));
	SightCone->SetupAttachment(RootComponent);
	SightCone->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnSightBeginOverlap); //no current need for 'EndOverlap'

	//set a default state
	m_pCurrentState = Idle::Instance();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//begin the patrolling route
	ChangeState(Patrolling::Instance());
}

// Called every frame
void AEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (m_pCurrentState) //if a state exists...
	{
		m_pCurrentState->Execute(this); //...execute the state every tick
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

#pragma region Collision Events
//************************************************************************************
//COLLISION DETECTION FOR RANGE
//-attack the player when in range
//-chase the player when out of range
//************************************************************************************
void AEnemy::OnRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) && (OtherActor != this)) //if the actor is the player and not itself...
	{
		ChangeState(Attacking::Instance()); //...change state to 'Attacking'...
		
		UKismetSystemLibrary::K2_SetTimer(this, "DealDamage", AttackCooldown, true); //...then deal damage
	}
}

void AEnemy::OnRangeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) && (OtherActor != this)) //if the actor is the player and not itself...
	{
		ChangeState(Pursuing::Instance()); //...change state to 'Pursuing'...

		UKismetSystemLibrary::K2_ClearTimer(this, "DealDamage");
	}
}

//************************************************************************************
//COLLISION DETECTION FOR SIGHT
//-notify the combat manager that the player is detected
//************************************************************************************
void AEnemy::OnSightBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) && (OtherActor != this)) //if the actor is the player and not itself...
	{
		//ChangeState(Pursuing::Instance()); //...change state to 'Pursuing'...

		//ACombatZone* cm = Cast<ACombatZone>(CombatManager);
		if (CombatManager->PlayerDetected == false)
		{
			CombatManager->SetPlayerDetected(true); //...use the combat manager to notify other enemies that the player is detected
			CombatManager->AlertEnemies();
		}
	}
}
#pragma endregion

#pragma region Damage Events
//************************************************************************************
//DEAL DAMAGE
//- deal damage to the player
//- play the attack animation
//************************************************************************************
void AEnemy::DealDamage()
{
	//only attack of the enemy is NOT frozen, IS attacking, and NOT dead
	if ((IsFrozen == false) && (IsAttacking == true) && (IsDead == false))
	{
		UGameplayStatics::ApplyDamage(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), Damage, NULL, this, NULL);
		GetMesh()->PlayAnimation(AttackAnim, false);
	}
}


//************************************************************************************
//SIMPLE DESTROY CALL
//************************************************************************************
void AEnemy::DestroyActor()
{
	Destroy();
}


//************************************************************************************
//EVENT RECEIVE DAMAGE
//- scripted version of take damage event from blueprint
//************************************************************************************
float AEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		//subtract damage from health
		Health -= ActualDamage;

		//have the enemies target the player
		//ACombatZone* cm = Cast<ACombatZone>(CombatManager);
		AArchon_Player* player = Cast<AArchon_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		player->IsInCombat = true;

		//if the player has not already been detected...
		if (CombatManager->PlayerDetected == false)
		{
			//tell the combat manager the player is detected
			CombatManager->SetPlayerDetected(true);

			//have all the enemies assigned to the combat manager pursue the player
			CombatManager->AlertEnemies();
		}

		//ChangeState(Pursuing::Instance());

		if (Health <= 0.f && IsDead == false)
		{
			//...decrement the enemy counter
			CombatManager->NumEnemies--;

			//if the associated combat manager is part of the boss
			if (CombatManager->IsBossZone == true)
			{
				//and there are no enemies left
				if (CombatManager->NumEnemies <= 0)
				{
					//disable Tenny's shield
					Tenebris->ToggleShield(false);
				}
			}

			//change state to dying
			ChangeState(Dying::Instance());

			//dsetroy itself once health reaches 0 and...
			//Destroy();
			//UKismetSystemLibrary::K2_SetTimer(this, "DestroyActor", AttackCooldown, false);
		}
	}
	
	return ActualDamage;
}
#pragma endregion

//************************************************************************************
//STATE MACHINE
//- called when enemy needs to switch states
//************************************************************************************
void AEnemy::ChangeState(State* pNewState) //takes an instance of a state as input
{
	m_pCurrentState->Exit(this); //call the exit function of state...
	m_pCurrentState = pNewState; //...change the current state to newly desire state...
	m_pCurrentState->Enter(this); //...call the enter function of the new state
}


//************************************************************************************
//SIMPLE SET MOVEMENT SPEED
//************************************************************************************
void AEnemy::SetSpeed(float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}