// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "EnemyOwnedStates.h"
#include "Enemy.h"
#include "CombatZone.h"

Patrolling* Patrolling::Instance()
{
	static Patrolling instance;

	return &instance;
}

void Patrolling::Enter(AEnemy* enemy)
{
	//set the appropriate speed
	enemy->SetSpeed(enemy->PatrolSpeed);

	//set the appropriate animations
	enemy->GetMesh()->SetAnimation(enemy->WalkAnim);
	enemy->GetMesh()->PlayAnimation(enemy->WalkAnim, true);
	
}

void Patrolling::Execute(AEnemy* enemy)
{
	/*
	new point = random point in reachable radius
	destination = new point

	//checks if the Waypoints array is null
	//if it is, then AI will move randomly in a reachable radius
	if(enemy->Waypoints == NULL)
	{
		pick a random location within radius of combat zone
		return;
	}

	//set the current destination
	if(enemy->wpTracker > enemy->Waypoints.Num()) wpTracker = 0;

	enemy->destinationPoint = Waypoints[enemy->wpTracker];


	*/
}

void Patrolling::Exit(AEnemy* enemy)
{
	/*
	destination = NULL
	*/
}

//***********************************************************

Pursuing* Pursuing::Instance()
{
	static Pursuing instance;

	return &instance;
}

void Pursuing::Enter(AEnemy* enemy)
{
	//set the appropriate speed
	enemy->SetSpeed(enemy->PursueSpeed);

	enemy->IsAttacking = false;

	//play monster sound
	enemy->Voice->Play();

	if (enemy->IsFrozen == false && enemy->IsDead == false)
	{
		//set the appropriate animations
		enemy->GetMesh()->SetAnimation(enemy->RunAnim);
		enemy->GetMesh()->PlayAnimation(enemy->RunAnim, true);
	}
}

void Pursuing::Execute(AEnemy* enemy)
{
	/*
	new point = player position
	destination = player position
	*/
	if (enemy->IsAttacking)
	{
		enemy->ChangeState(Attacking::Instance());
	}
}

void Pursuing::Exit(AEnemy* enemy)
{
	//destination = NULL
}

//***********************************************************

Attacking* Attacking::Instance()
{
	static Attacking instance;

	return &instance;
}

void Attacking::Enter(AEnemy* enemy)
{
	if (enemy->IsFrozen == false && enemy->IsDead == false)
	{
		enemy->IsAttacking = true;
	}
}

void Attacking::Execute(AEnemy* enemy)
{
	
}

void Attacking::Exit(AEnemy* enemy)
{
	enemy->IsAttacking = false;
}

//***********************************************************

Idle* Idle::Instance()
{
	static Idle instance;

	return &instance;
}

void Idle::Enter(AEnemy* enemy)
{

}

void Idle::Execute(AEnemy* enemy)
{

}

void Idle::Exit(AEnemy* enemy)
{

}

//***********************************************************

Dying* Dying::Instance()
{
	static Dying instance;

	return &instance;
}

void Dying::Enter(AEnemy* enemy)
{
	enemy->IsDead = true;

	//set and play death animation
	enemy->GetMesh()->SetAnimation(enemy->DeathAnim);
	enemy->GetMesh()->PlayAnimation(enemy->DeathAnim, false);

	//play death sound
	enemy->DeathSound->Play();

	//disable movement for actor
	enemy->GetCharacterMovement()->SetMovementMode(MOVE_None);

	//destroy actor after set amount of seconds
	enemy->SetLifeSpan(2.0);

	//actor will not attack
	enemy->IsAttacking = false;
}

void Dying::Execute(AEnemy* enemy)
{

}

void Dying::Exit(AEnemy* enemy)
{

}