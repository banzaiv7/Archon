// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "Archon_Player.h"

#include"Kismet/KismetSystemLibrary.h"

// Sets default values
AArchon_Player::AArchon_Player(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//default player values
	Health = 100.0f;
	Anima = 100.0f;
	RegenRate = 10.0f;
	AnimaRegenRate = 10.0f;

	//ult timers
	UltCooldown = 0.0f;
	UltCooldownTime = 15.0f;
	IsUltReady = true;

	//flash freeze
	RootTimer = 3.0f;

	//thunder drums
	DrumsPlaying = false;
	HighSpeed = 1200.0f;
	LowSpeed = 300.0f;
	CombatSpeed = 600.0f;
	NormalSpeed = 800.0f;

	/*//IsInCombat = false;
	//AnimaResetMark = 50.0f;
	//IsInAnimaCooldown = false;*/

	//create deadly fire radial force
	DeadlyFire = ObjectInitializer.CreateDefaultSubobject<URadialForceComponent>(this, TEXT("Deadly Fire"));
	DeadlyFire->SetupAttachment(RootComponent);
	DeadlyFire->bAutoActivate = false;

	//create flash freeze sphere collider
	FlashFreeze = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Flash Freze"));
	FlashFreeze->SetupAttachment(RootComponent);
	//FlashFreeze->SetSphereRadius(500.0f);
	FlashFreeze->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//mouse
	Sensitivity = 100.0f;

	//audio initialization
	FireSound = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Deadly Fire Audio"));
	FireSound->bAutoActivate = false;

	FreezeSound = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Flash Freeze Audio"));
	FreezeSound->bAutoActivate = false;

	DrumsSound = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Thunder Drums Audio"));
	DrumsSound->bAutoActivate = false;

	DamagedSound = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Player Damaged Audio"));
	DamagedSound->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AArchon_Player::BeginPlay()
{
	Super::BeginPlay();

	//player begins out of combat
	IsInCombat = false;
}

// Called every frame
void AArchon_Player::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//regen health
	Regen();

	//regen anima
	AnimaRegen();

	//set player speed
	SetPlayerSpeed();
}

#pragma region Player Input
// Called to bind functionality to input
void AArchon_Player::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//bind player movement to functions
	InputComponent->BindAxis("Forward", this, &AArchon_Player::MoveForward);
	InputComponent->BindAxis("Strafe", this, &AArchon_Player::Strafe);
	InputComponent->BindAxis("Yaw", this, &AArchon_Player::Yaw);
	InputComponent->BindAxis("Pitch", this, &AArchon_Player::Pitch);

	//bind input actions to special abilities
	InputComponent->BindAction("ActivateDeadlyFire", IE_Pressed, this, &AArchon_Player::ActivateDeadlyFire);
	InputComponent->BindAction("ActivateFlashFreeze", IE_Pressed, this, &AArchon_Player::ActivateFlashFreeze);
	InputComponent->BindAction("ActivateThunderDrums", IE_Pressed, this, &AArchon_Player::ActivateThunderDrums);
}


//************************************************************************************
//PLAYER INPUT
//************************************************************************************
void AArchon_Player::MoveForward(float amount)
{
	//if the player is being controlled
	//and there is input
	if (Controller && amount)
	{
		//move the player forward
		FVector fwd = GetActorForwardVector();
		AddMovementInput(fwd, amount);
	}
}

void AArchon_Player::Strafe(float amount)
{
	if (Controller && amount)
	{
		FVector strafe = GetActorRightVector();
		AddMovementInput(strafe, amount);
	}
}

void AArchon_Player::Yaw(float amount)
{
	AddControllerYawInput(Sensitivity * amount * GetWorld()->GetDeltaSeconds());
}

void AArchon_Player::Pitch(float amount)
{
	AddControllerPitchInput(Sensitivity * amount * GetWorld()->GetDeltaSeconds());
}
#pragma endregion

#pragma region Ultimate Abilities
//************************************************************************************
//DEADLY FIRE ABILITY
//************************************************************************************
void AArchon_Player::ActivateDeadlyFire()
{
	//if health is above threshold
	//anima is above threshold
	//and ult is ready
	if ((Health > 25.0f) && (Anima >= 50.0f) && (IsUltReady == true))
	{
		//spend anima
		Anima -= 50.0f;
		//Health -= 25.0f;

		//play hand animation

		//set the radial force component active
		DeadlyFire->Activate();

		//apply radial damage with player as source
		//indicate the player as the damage causer
		UGameplayStatics::ApplyRadialDamage(GetWorld(), 200.0f, GetActorLocation(), 2000.0f, NULL, actorsToIgnore, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		
		//apply damage to self
		UGameplayStatics::ApplyDamage(this, 25.0f, NULL, NULL, NULL);

		//spawn particle explosion
		if (ExplosionParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), FRotator::ZeroRotator, true);
		}
		
		//play sound
		FireSound->Play();

		//render the ult not ready
		DisableUlt();

		//begin cooldown
		UKismetSystemLibrary::K2_SetTimer(this, "EnableUlt", UltCooldownTime, false);
		//GetWorldTimerManager().SetTimer(UnusedHandle, this, &AArchon_Player::DFCont, 0.2f);

		//turn off the radial force component
		DeadlyFire->Deactivate();
	}
	else //messages that specify why ability cannot activate
	{
		if (Health < 25.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough health"));

		if (Anima < 50.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Not enough anima"));

		if (UltCooldown > GetWorld()->GetTimeSeconds())
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ability not ready"));
	}
}
/*
void AArchon_Player::DFCont()
{
	DeadlyFire->Deactivate();
}*/


//************************************************************************************
//FLASH FREEZE ABILITY
//************************************************************************************
void AArchon_Player::ActivateFlashFreeze()
{
	//if anima is above requirement
	//player is not jumping
	//ult is ready
	if ((Anima >= 50.0f) && (!GetCharacterMovement()->IsFlying()) && (IsUltReady == true))
	{
		Anima -= 50.0f;

		if (IceParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IceParticle, GetActorLocation(), FRotator::ZeroRotator, true);
		}

		//set the collision to query only (no physics collision
		FlashFreeze->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		//make the player stop moving
		GetCharacterMovement()->SetMovementMode(MOVE_None);

		//play sound
		FreezeSound->Play();

		DisableUlt();

		UKismetSystemLibrary::K2_SetTimer(this, "EnableUlt", UltCooldownTime, false);

		//disable collision for sphere collider
		FlashFreeze->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//call EnableMovement() after set amount of time
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AArchon_Player::EnableMovement, RootTimer);
		
	}
	else
	{
		if (Health < 25.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough health"));

		if (Anima < 50.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Not enough anima"));

		if (UltCooldown > GetWorld()->GetTimeSeconds())
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ability not ready"));
	}
}

//re-enable movement 
void AArchon_Player::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}


//************************************************************************************
//THUNDER DRUMS ABILITY
//************************************************************************************
void AArchon_Player::ActivateThunderDrums()
{
	//anima is above threshold
	//ult is ready
	if ((Anima >= 50.0f) && (IsUltReady == true))
	{
		Anima -= 50.0f;

		if (DrumsParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DrumsParticle, GetActorLocation(), FRotator::ZeroRotator, true);
		}

		//set the player speed to the higher mark
		GetCharacterMovement()->MaxWalkSpeed = HighSpeed;

		DrumsPlaying = true;

		//play sound
		DrumsSound->Play();

		DisableUlt();

		UKismetSystemLibrary::K2_SetTimer(this, "EnableUlt", UltCooldownTime, false);

		//call SwitchLow() after set amoung to time
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AArchon_Player::SwitchLow, 5.0f);
	}
	else
	{
		if (Health < 25.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough health"));

		if (Anima < 50.0f)
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Not enough anima"));

		if (UltCooldown > GetWorld()->GetTimeSeconds())
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ability not ready"));
	}
}

//set the player speed to lower mark
void AArchon_Player::SwitchLow()
{
	GetCharacterMovement()->MaxWalkSpeed = LowSpeed;
	DrumsPlaying = false;

	//call RestoreSpeed() after set amount of time
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AArchon_Player::RestoreSpeed, 7.0f);
}

//set speed to normal
void AArchon_Player::RestoreSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = CombatSpeed;
}

/*
void AArchon_Player::InitiateCooldown(float time)
{
	UltCooldown = time + (GetWorld()->GetTimeSeconds());
}*/


//************************************************************************************
//ULTIMATES ENABLE AND DISABLE
//************************************************************************************
void AArchon_Player::EnableUlt()
{
	IsUltReady = true;
}

void AArchon_Player::DisableUlt()
{
	IsUltReady = false;
}
#pragma endregion

#pragma region Regen
//************************************************************************************
//HEALTH REGEN
//************************************************************************************
void AArchon_Player::Regen()
{
	/*
	Anima += (AnimaRegenRate * GetWorld()->GetDeltaSeconds());

	if (Anima > 100)
	{
		Anima = 100;
	}
	*/

	//trigger regen when the player is not in combat
	//or if Thunder Drums is active
	if (!IsInCombat || DrumsPlaying)
	{
		if (Health < 100.0f)
		{
			//multiply RegenRate by delta seconds in order to regen health every second
			Health += (RegenRate * GetWorld()->GetDeltaSeconds());
		}
	}

	//cap health at 100
	if (Health > 100)
	{
		Health = 100;
	}
	if (Health < 0)
	{
		Health = 0;
	}
}


//************************************************************************************
//ANIMA REGEN
//-regenerate anima every second
//************************************************************************************
void AArchon_Player::AnimaRegen()
{
	Anima += (AnimaRegenRate * GetWorld()->GetDeltaSeconds());

	//cap anima at 100
	if (Anima > 100)
	{
		Anima = 100;
	}
}


//************************************************************************************
//PLAYER MOVEMENT SPEED RESET
//************************************************************************************
void AArchon_Player::SetPlayerSpeed()
{
	if (DrumsPlaying == false)
	{
		if (!IsInCombat)
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}
		else
		{
			return;
		}
	}
}
/*
void AArchon_Player::CheckHealth()
{
	if (Health <= 0)
	{
		Reset();
	}
}*/
/*
void AArchon_Player::AnimaCooldown()
{

}
*/
#pragma endregion

#pragma region Level and Events
//************************************************************************************
//LEVEL RESET
//- called when player dies
//- resets the level
//************************************************************************************
void AArchon_Player::Reset()
{
	UGameplayStatics::OpenLevel(GetWorld(), "World");
}


//************************************************************************************
//EVENT RECEIVE DAMAGE
//- scripted version of TakeDamage event from blueprint
//************************************************************************************
float AArchon_Player::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if ((ActualDamage > 0.0f) && (DamageCauser != this))
	{
		DamagedSound->Play();

		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			Reset();
		}
	}
	
	return ActualDamage;
}


//************************************************************************************
//UI FUNCTIONALITY
//- will be called by UI to get remaing time until Ultimate is ready
//************************************************************************************
float AArchon_Player::UltTimeRemain()
{
	float timeRemain = UltCooldownTime - GetWorld()->GetTimeSeconds();
	if (timeRemain < 0)
	{
		timeRemain = 0;
	}
	return timeRemain;
}
#pragma endregion
