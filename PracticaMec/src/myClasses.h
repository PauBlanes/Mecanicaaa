#pragma once
#define gravity -5;
#define elasticC 0.2;
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
	
	Particle(solverMethod solvM, coords pos, coords vel, float laMassa);
	coords position;
	void Move(float dt);
	void DetectWall(coords n, int d);
};


class particleManager {	
public:
	coords wallNormals[6];
	int wallDs[6];
	std::vector<Particle> particles;
	void AddPart(Particle temp);
	void Update(float dt);
};
