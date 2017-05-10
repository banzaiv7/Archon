// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Tenny.generated.h"

UENUM(BlueprintType)
enum class EAbilitiesEnum : uint8
{
	EARTHQUAKE,
	ICE_NOVA,
	CALL_FOR_HELP,
	DEATH_METEOR,
	FIRE_BARRAGE
};

class ARCHON_API BossState;
class ARCHON_API ADamagePlatform;
class ARCHON_API AFireProjectile;

UCLASS()
class ARCHON_API ATenny : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATenny(const FObjectInitializer &ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		bool IsActive;

	//********************************************************************************************

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAudioComponent* Laugh;

	void Taunt();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		UAudioComponent* TennyTaunt;

	void AntyResponse();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
		UAudioComponent* AntyTaunt;

	//********************************************************************************************

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		USphereComponent* Shield;

	//********************************************************************************************

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States")
		bool InPhase1;

	//this float represents the percentage value of health
	//before switching to the second phase of the boss fight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States")
		float Phase2Mark;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States")
		bool InPhase2;

	//this float represents the percentage value of health
	//before switching to the third and final phase of the boss fight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States")
		float Phase3Mark;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States")
		bool InPhase3;

	//********************************************************************************************

	//enum that contains the name of the abilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		EAbilitiesEnum AbilitiesEnum;
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void ActivateAbility(EAbilitiesEnum ability);

	//********************************************************************************************

	//one of the abilities used by Tenny in the first phase of the boss battle
	//accesses the platform where the player and boss are battling and
	//deals damage to the player if they're on the platform
	UFUNCTION(BlueprintCallable, Category = "Phase 1 - Earthquake")
		void Earthquake();

	//time before earthquake is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		float EQCastTime;

	//platform that will cause damage to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		ADamagePlatform* Platform;

	//this actor will disappear when boss uses 'Earthquake'
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		AActor* Barricade;

	UFUNCTION(BlueprintCallable, Category = "Phase 1 - Earthquake")
		void DeactivateEarthquake();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		UAnimSequence* EarthquakeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		UParticleSystemComponent* EQParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Earthquake")
		UAudioComponent* EQVO;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 1 - Earthquake")
		void EQAssist();

	//********************************************************************************************

	//a radial explosion that deals damage to the player if caught in the radius
	//can be avoided by breaking line of sight
	UFUNCTION(BlueprintCallable, Category = "Phase 1 - Ice Nova")
		void IceNova();

	//base damage of ice nova
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		float IN_BaseDamage;

	//minimum damage of ice nova
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		float IN_MinDamage;

	//radius that covers base damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		float IN_InnerRadius;

	//radius that covers the minimum damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		float IN_OuterRadius;

	//ignore channel of actors
	//ice nova will not apply damage to these actors
	const TArray<AActor*> actorsToIgnore;

	//time before ice nova is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		float INCastTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		UAnimSequence* IceNovaAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		UParticleSystemComponent* INParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 1 - Ice Nova")
		UAudioComponent* IceVO;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 1 - Ice Nova")
		void IceAssist();

	//********************************************************************************************
	
	//spawns a shield that prevents Tenny from being damaged
	//spawns a handful of enemies for the player to deal with
	UFUNCTION(BlueprintCallable, Category = "Phase 2 - Help")
		void CallForHelp();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Phase 2 - Help")
		bool ShieldActive;

	UFUNCTION(BlueprintCallable, Category = "Phase 2 - Help")
		void ToggleShield(bool active);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 2 - Help")
		TArray<AActor*> Servants;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 2 - Help")
		AActor* TeleportDestination;

	void Teleport(AActor* destination);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 2 - Help")
		AActor* Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 2 - Help")
		UAudioComponent* HelpVO;

	//********************************************************************************************

	//spawns a shield that prevents damage taken
	//randomly references 2 obelisks, which must be deactivated in order to prevent major damage
	UFUNCTION(BlueprintCallable, Category = "Phase 3 - Death")
		void DeathMeteor();

	//time before meteor is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		float DMCastTime;

	//determines if meteor can drop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		bool MeteorReady;

	//obelisks
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 3 - Death")
		void ActivateObelisks();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 3 - Death")
		void ResetObelisks();

	//meteor drops on map
	void LaunchMeteor();

	//base damage for meteor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		float DM_Dmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		UAnimSequence* DeathMeteorAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		UAudioComponent* DMVO;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 3 - Death")
		void DMAssist();

	FTimerHandle UnusedHandle;

	bool canDM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Death")
		float DMCooldownTime;

	void EnableDM();

	//********************************************************************************************

	//"bullet hell" style shooting
	//spawns multiple projectiles towards player
	UFUNCTION(BlueprintCallable, Category = "Phase 3 - Barrage")
		void FireBarrage();

	UFUNCTION(BlueprintCallable, Category = "Phase 3 - Barrage")
		void SpawnFireball();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Phase 3 - Barrage")
		void FireSingle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Barrage")
		int NumFireBalls;

	//time before barrage is activated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Barrage")
		float FBCastTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Barrage")
		float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Barrage")
		UAnimSequence* BarrageAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase 3 - Barrage")
		UAudioComponent* BarrageVO;

	//********************************************************************************************

	//Unreal damage event
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* DeathAnim;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		void PlayOutro();

	//********************************************************************************************

	//handles state change
	BossState* m_pCurrentState;

	void ChangeState(BossState* pNewState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		UAnimSequence* ChangeStateAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
		bool IsDead;
};
