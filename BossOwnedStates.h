// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BossState.h"

class ARCHON_API ATenny;

class Phase1 : public BossState
{
private:
	Phase1() {}

	Phase1(const Phase1&);
	Phase1& operator=(const Phase1&);

public:
	static Phase1* Instance();

	virtual void Enter(ATenny* tenny);
	virtual void Execute(ATenny* tenny);
	virtual void Exit(ATenny* tenny);
};

class Phase2 : public BossState
{
private:

	Phase2() {}
	Phase2(const Phase2&);
	Phase2& operator=(const Phase2&);

public:
	static Phase2* Instance();

	virtual void Enter(ATenny* tenny);
	virtual void Execute(ATenny* tenny);
	virtual void Exit(ATenny* tenny);
};

class Phase3 : public BossState
{
private:
	Phase3() {}

	Phase3(const Phase3&);
	Phase3& operator=(const Phase3&);

public:
	static Phase3* Instance();

	virtual void Enter(ATenny* tenny);
	virtual void Execute(ATenny* tenny);
	virtual void Exit(ATenny* tenny);
};

class Death : public BossState
{
private:
	Death() {}

	Death(const Death&);
	Death& operator=(const Death&);

public:
	static Death* Instance();

	virtual void Enter(ATenny* tenny);
	virtual void Execute(ATenny* tenny);
	virtual void Exit(ATenny* tenny);
};