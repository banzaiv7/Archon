// Fill out your copyright notice in the Description page of Project Settings.

#include "Archon.h"
#include "BossOwnedStates.h"
#include "Tenny.h"

Phase1* Phase1::Instance()
{
	static Phase1 instance;

	return &instance;
}

void Phase1::Enter(ATenny* tenny)
{
	tenny->InPhase1 = true;
}

void Phase1::Execute(ATenny* tenny)
{
	//change states when Tenebris reaches a certain health mark
	if (tenny->Health <= tenny->Phase2Mark)
	{
		tenny->ChangeState(Phase2::Instance());
	}
}

void Phase1::Exit(ATenny* tenny)
{
	tenny->InPhase1 = false;
	tenny->Teleport(tenny->TeleportDestination);
	tenny->Door->Destroy();
}

//*****************************************************************

Phase2* Phase2::Instance()
{
	static Phase2 instance;

	return &instance;
}

void Phase2::Enter(ATenny* tenny)
{
	tenny->InPhase2 = true;
	tenny->CallForHelp();
	tenny->Door->Destroy();
}

void Phase2::Execute(ATenny* tenny)
{
	//change states when Tenebris reaches a certain health mark
	if (tenny->Health <= tenny->Phase3Mark)
	{
		tenny->ChangeState(Phase3::Instance());
	}
}

void Phase2::Exit(ATenny* tenny)
{
	tenny->InPhase2 = false;
}

//*****************************************************************

Phase3* Phase3::Instance()
{
	static Phase3 instance;

	return &instance;
}

void Phase3::Enter(ATenny* tenny)
{
	tenny->InPhase3 = true;
	tenny->Taunt();
}

void Phase3::Execute(ATenny* tenny)
{
	if (tenny->Health <= 0)
	{
		tenny->ChangeState(Death::Instance());
	}
}

void Phase3::Exit(ATenny* tenny)
{
	tenny->InPhase3 = false;
}

//*****************************************************************

Death* Death::Instance()
{
	static Death instance;

	return &instance;
}

void Death::Enter(ATenny* tenny)
{
	tenny->IsDead = true;

	//stop use of abilities
	tenny->IsActive = false;

	//play death anim
	tenny->GetMesh()->SetAnimation(tenny->DeathAnim);
	tenny->GetMesh()->PlayAnimation(tenny->DeathAnim, false);

	//destroy actor
	tenny->SetLifeSpan(10.0f);
}

void Death::Execute(ATenny* tenny)
{

}

void Death::Exit(ATenny* tenny)
{

}
