#pragma once
#define gravity -9.81;
#include <vector>
#include <iostream>
#include "Includes.h"


enum solverMethod { euler, verlet };
struct coords { float x;float y;float z; };



class Particle {
	
	coords oldPos;
	coords velocity;

	float mass;
	float force;

	float elasticCoef;
	float frictionCoef;

	float particleLife;

	solverMethod sM;
public:
	coords position;
	Particle(solverMethod solvM, coords pos, coords vel, float laMassa);
	void Move(float dt);
	void DetectWall();
};


class particleManager {
	std::vector<Particle> particles;
	
public:
	void AddPart(Particle temp);
	void Update(float dt);
};
