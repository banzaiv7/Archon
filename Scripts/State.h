// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"

/**
 * 
 */
//class Actor;

class ARCHON_API AEnemy;

class State
{
public:

	virtual ~State() {}
	
	virtual void Enter(AEnemy*) = 0;

	virtual void Execute(AEnemy*) = 0;

	virtual void Exit(AEnemy*) = 0;
};
