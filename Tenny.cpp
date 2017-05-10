// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "Tenny.h"

#include "BossOwnedStates.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DamagePlatform.h"
#include "FireProjectile.h"

// Sets default values
ATenny::ATenny(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 750.0f;

	//laugh vo
	Laugh = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Laugh VO"));
	Laugh->SetupAttachment(RootComponent);
	Laugh->bAutoActivate = false;

	//other dialogue
	TennyTaunt = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Tenny Taunt"));
	TennyTaunt->SetupAttachment(RootComponent);
	TennyTaunt->bAutoActivate = false;

	AntyTaunt = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Anty VO"));
	AntyTaunt->SetupAttachment(RootComponent);
	AntyTaunt->bAutoActivate = false;

	//TennyTaunt->OnAudioFinished.AddDynamic(this, &ATenny::AntyResponse);

	IsActive = false;
	IsDead = false;

	InPhase1 = true;
	InPhase2 = false;
	InPhase3 = false;

	//health marks that trigger state change
	Phase2Mark = 500.0f;
	Phase3Mark = 250.0f;

	//earthquake variables
	EQCastTime = 5.0f;
	EQParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Earthquake Particle"));
	EQParticle->SetupAttachment(RootComponent);
	EQParticle->bAutoActivate = false;
	EQVO = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Earthquake VO"));
	EQVO->SetupAttachment(RootComponent);
	EQVO->bAutoActivate = false;

	//ice nova variables
	IN_BaseDamage = 35.0f;
	IN_MinDamage = 10.0f;
	IN_InnerRadius = 100.0f;
	IN_OuterRadius = 500.0f;
	INCastTime = 3.0f;
	INParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Ice Nova Particle"));
	INParticle->SetupAttachment(RootComponent);
	INParticle->bAutoActivate = false;
	IceVO = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Ice Nova VO"));
	IceVO->SetupAttachment(RootComponent);
	IceVO->bAutoActivate = false;

	//help variables
	HelpVO = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Call for Help VO"));
	HelpVO->SetupAttachment(RootComponent);
	HelpVO->bAutoActivate = false;

	//death meteor variables
	canDM = true;
	MeteorReady = false;
	DM_Dmg = 90.0f;
	DMVO = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Death Meteor VO"));
	DMVO->SetupAttachment(RootComponent);
	DMVO->bAutoActivate = false;
	DMCooldownTime = 10.0f;

	//barrage variables
	NumFireBalls = 5;
	FireRate = 1.0f;
	BarrageVO = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Fire Barrage VO"));
	BarrageVO->SetupAttachment(RootComponent);
	BarrageVO->bAutoActivate = false;

	//initialize the sphere object on Tenny
	Shield = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Shield"));
	Shield->SetupAttachment(RootComponent);
	ShieldActive = false;
}

// Called when the game starts or when spawned
void ATenny::BeginPlay()
{
	Super::BeginPlay();

	//boss begins in 'Phase1'
	m_pCurrentState = Phase1::Instance();
	
	//make sure the shield is disabled
	Shield->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
}

// Called every frame
void ATenny::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (m_pCurrentState)
	{
		m_pCurrentState->Execute(this);
	}
}

// Called to bind functionality to input
void ATenny::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}


//************************************************************************************
//ABILITY ACTVATION
//- activates the selected ability from an enum
//************************************************************************************
void ATenny::ActivateAbility(EAbilitiesEnum ability)
{
	switch (ability)
	{
	case EAbilitiesEnum::EARTHQUAKE:
		//play setup vo
		if (Laugh) Laugh->Play();
		EQAssist();
		UKismetSystemLibrary::K2_SetTimer(this, "Earthquake", EQCastTime, false);
		//Earthquake();
		break;

	case EAbilitiesEnum::ICE_NOVA:
		//play setup vo
		if (IceVO) IceVO->Play();
		IceAssist();
		UKismetSystemLibrary::K2_SetTimer(this, "IceNova", INCastTime, false);
		//IceNova();
		break;

	case EAbilitiesEnum::CALL_FOR_HELP:
		if (HelpVO) HelpVO->Play();
		CallForHelp();
		break;

	case EAbilitiesEnum::DEATH_METEOR:
		//activate only if Death Meteor isn't already active
		if (canDM)
		{
			if (Laugh) Laugh->Play();
			DMAssist();
			//UKismetSystemLibrary::K2_SetTimer(this, "DeathMeteor", 2.0f , false);
			DeathMeteor();
		}
		//otherwise, use Fire Barrage
		else
		{
			if (BarrageVO) BarrageVO->Play();
			UKismetSystemLibrary::K2_SetTimer(this, "FireBarrage", FBCastTime, false);
		}
		break;

	case EAbilitiesEnum::FIRE_BARRAGE:
		//play setup vo
		if (BarrageVO) BarrageVO->Play();
		UKismetSystemLibrary::K2_SetTimer(this, "FireBarrage", FBCastTime, false);
		//FireBarrage();
		break;
	}
}

#pragma region Phase 1 Abilities
//************************************************************************************
//EARTHQUAKE ABILITY
//************************************************************************************
void ATenny::Earthquake()
{
	if (EQVO) EQVO->Play();

	//if animation exists
	if (EarthquakeAnim != NULL)
	{
		//play animation
		GetMesh()->SetAnimation(EarthquakeAnim);
		GetMesh()->PlayAnimation(EarthquakeAnim, false);
	}

	//only called if a barricade is referenced
	if (Barricade != NULL)
	{
		//disable wall blocking separate platform
		Barricade->SetActorEnableCollision(false);
		Barricade->SetActorHiddenInGame(true);
		Barricade->SetActorTickEnabled(false);
	}

	//enable platform collision and events
	Platform->SetActorEnableCollision(true);
	Platform->SetActorTickEnabled(true);
	
	//enable platform to do damage
	Platform->SetDamageActive(true);

	//play particle
	EQParticle->Activate();

	//deactivate earthquake after 5 seconds
	UKismetSystemLibrary::K2_SetTimer(this, "DeactivateEarthquake", 5.0f, false);
}

void ATenny::DeactivateEarthquake()
{
	//turn off damage from platform
	Platform->SetDamageActive(false);
}


//************************************************************************************
//ICE NOVA ABILITY
//-radial damage with falloff
//************************************************************************************
void ATenny::IceNova()
{
	//if (IceVO) IceVO->Play();

	//if animation exists
	if (IceNovaAnim != NULL)
	{
		//play the animation
		GetMesh()->SetAnimation(IceNovaAnim);
		GetMesh()->PlayAnimation(IceNovaAnim, false);
	}

	//play particle effect
	INParticle->Activate();

	//radial force that is blocked by line of sight or visual obstructions
	//and deals damage with falloff
	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), IN_BaseDamage, IN_MinDamage, GetActorLocation(), IN_InnerRadius, IN_OuterRadius, 1.0f, NULL, actorsToIgnore, NULL, NULL);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), IN_BaseDamage, GetActorLocation(), IN_OuterRadius, NULL, actorsToIgnore, this, NULL, true);
}
#pragma endregion

#pragma region Phase 2 Abilities
//************************************************************************************
//CALL FOR HELP
//-teleport Tenny to a second location
//************************************************************************************
void ATenny::CallForHelp()
{
	if (ShieldActive == false)
	{
		ShieldActive = true;

		//teleport Tenebris to second location
		Teleport(TeleportDestination);

		//activate shield 
		//Shield->BodyInstance.SetResponseToChannel(ECC_Camera, ECR_Block);
		//Shield->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		ToggleShield(true);

		//spawn a set number of grunts (may not be needed due to level design changes)

		//deactivate shield when all grunts are destroyed
	}
}

void ATenny::ToggleShield(bool active)
{
	if (active == true)
	{
		//have the shield block camera traces (like the arcane beam)
		Shield->BodyInstance.SetResponseToChannel(ECC_Camera, ECR_Block);

		//shield blocks projectiles
		Shield->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

		//cannot be damaged
		bCanBeDamaged = false;
	}
	else
	{
		//do the reverse
		Shield->BodyInstance.SetResponseToChannel(ECC_Camera, ECR_Ignore);
		Shield->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		bCanBeDamaged = true;
	}
}

void ATenny::Teleport(AActor* destination)
{
	SetActorLocation(destination->GetActorLocation(), false, nullptr, ETeleportType::None);
}
#pragma endregion

#pragma region Phase 3 Abilities
//************************************************************************************
//DEATH METEOR
//************************************************************************************
void ATenny::DeathMeteor()
{
	if (DMVO) DMVO->Play();

	//if animation exists
	if (DeathMeteorAnim != NULL)
	{
		//play animation
		GetMesh()->SetAnimation(DeathMeteorAnim);
		GetMesh()->PlayAnimation(DeathMeteorAnim, false);
	}

	//activate shield
	ToggleShield(true);

	MeteorReady = true;

	//get references to 2 random obelisks and activate them
	ActivateObelisks();
	//ActivateObelisks_Implementation();

	//spawn meteor above boss platform
	//UKismetSystemLibrary::K2_SetTimer(this, "LaunchMeteor", DMCastTime, false);
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATenny::LaunchMeteor, DMCastTime, false);
}
/*
void ATenny::ActivateObelisks_Implementation()
{
	//empty function call
	//triggers event for blueprint
}*/

void ATenny::LaunchMeteor()
{
	//debug
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Death"));

	if (MeteorReady)
	{
		//play explosion particle?

		//damage player for 90% of max health
		UGameplayStatics::ApplyDamage(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), DM_Dmg, NULL, this, NULL);
	}

	ToggleShield(false);

	MeteorReady = false;

	canDM = false;

	ResetObelisks();

	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATenny::EnableDM, DMCooldownTime, false);
}

void ATenny::EnableDM()
{
	canDM = true;
}


//************************************************************************************
//FIRE BARRAGE
//************************************************************************************
void ATenny::FireBarrage()
{
	//spawn projectiles for a set duration
	for (int i = 0; i <= NumFireBalls; i++)
	{
		//UKismetSystemLibrary::K2_SetTimer(this, "SpawnFireball", FireRate, false);
		SpawnFireball();
	}
}

void ATenny::SpawnFireball()
{
	//if an animation exists
	if (BarrageAnim != NULL)
	{
		//play the animation
		GetMesh()->SetAnimation(BarrageAnim);
		GetMesh()->PlayAnimation(BarrageAnim, false);
	}

	//spawn params declaration
	//FActorSpawnParameters sp;

	//make sure the fireball always spawns
	//sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//spawn a fireball
	//this->GetWorld()->SpawnActor<AFireProjectile>(FireballBlueprint, GetActorLocation(), GetActorRotation(), sp);
	this->FireSingle();
}
#pragma endregion

//************************************************************************************
//EVENT RECEIVE DAMAGE
//- scripted version of the take damage event in blueprints
//************************************************************************************
float ATenny::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if ((ActualDamage > 0.f) && (DamageCauser == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if(bCanBeDamaged == true) Health -= ActualDamage;

		if (Health <= 0.0f && IsDead == false) ChangeState(Death::Instance());
	}

	return ActualDamage;
}


//************************************************************************************
//STATE MACHINE
//************************************************************************************
void ATenny::ChangeState(BossState* pNewState) //takes an instance of a state as input
{
	//call the exit function of state...
	m_pCurrentState->Exit(this);

	//change the current state to newly desire state
	m_pCurrentState = pNewState; 

	//call the enter function of the new state
	m_pCurrentState->Enter(this); 
}

//************************************************************************************
//OTHER DIALOGUE
//************************************************************************************
void ATenny::Taunt()
{
	TennyTaunt->Play();
	
	UKismetSystemLibrary::K2_SetTimer(this, "AntyResponse", 4.0f, false);
}

void ATenny::AntyResponse()
{
	AntyTaunt->Play();
}