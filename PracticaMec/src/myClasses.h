#pragma once
#define gravity -9.8;
#define elasticC 0.1;
#define friction 0.1;
#include <vector>
#include <iostream>
#include "Includes.h"
#include "imgui\imgui.h"

enum solverMethod { euler, verlet };
struct coords { float x;float y;float z; };
enum emiterType {font, cascada};

class Particle {
	
	coords oldPos;
	coords actualPos;
	coords velocity;

	float mass;
	float force;

	float elasticCoef;
	float frictionCoef;	

	solverMethod sM;
public:
	
	Particle(solverMethod solvM, coords pos, coords vel, float laMassa);
	coords position;
	void Move(float dt);
	void DetectWall(coords n, int d, float dt);
};


class particleManager {

	float spawnCounter;
	float lifeCounter;
public:
	int emitterRate;
	coords pos1; //posicio del emissor
	coords dir; //vector de velocitat inicial de les particules
	emiterType eType; //tipus d'emissor
	float particleLife;
	solverMethod partsMethod;

	coords wallNormals[6];
	int wallDs[6];
	std::vector<Particle> particles;
	void SpawnParticles();
	void Update(float dt);
	
};


