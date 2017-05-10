// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "State.h"

class ARCHON_API AEnemy;

class Patrolling : public State
{
private:

	Patrolling() {}

	Patrolling(const Patrolling&);
	Patrolling& operator=(const Patrolling&);

public:

	static Patrolling* Instance();

	virtual void Enter(AEnemy* enemy);
	virtual void Execute(AEnemy* enemy);
	virtual void Exit(AEnemy* enemy);
};

/*

-enemy will be in this state while chasing and attacking the player
-enemy exits this state when player is no longer detected

*/

class Pursuing : public State
{
private:

	Pursuing() {}

	Pursuing(const Pursuing&);
	Pursuing& operator=(const Pursuing&);

public:

	static Pursuing* Instance();

	virtual void Enter(AEnemy* enemy);
	virtual void Execute(AEnemy* enemy);
	virtual void Exit(AEnemy* enemy);
};

class Attacking : public State
{
private:

	Attacking() {}

	Attacking(const Attacking&);
	Attacking& operator=(const Attacking&);

public:

	static Attacking* Instance();

	virtual void Enter(AEnemy* enemy);
	virtual void Execute(AEnemy* enemy);
	virtual void Exit(AEnemy* enemy);
};

class Idle : public State
{
private:

	Idle() {}

	Idle(const Idle&);
	Idle& operator=(const Idle&);

public:
	static Idle* Instance();

	virtual void Enter(AEnemy* enemy);
	virtual void Execute(AEnemy* enemy);
	virtual void Exit(AEnemy* enemy);
};

class Dying : public State
{
private:

	Dying() {}

	Dying(const Dying&);
	Dying& operator=(const Dying&);

public:
	static Dying* Instance();

	virtual void Enter(AEnemy* enemy);
	virtual void Execute(AEnemy* enemy);
	virtual void Exit(AEnemy* enemy);
};