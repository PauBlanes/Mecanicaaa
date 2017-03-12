#pragma once



#include <vector>
#include <iostream>
#include "Includes.h"
#include "imgui\imgui.h"

enum solverMethod { euler, verlet };
struct coords { float x;float y;float z; };
enum emiterType {font, cascada};
extern float gravity;

class Particle {
	friend class particleManager;
	coords oldPos;
	coords actualPos;
	coords velocity;

	float mass;
	coords force;
	coords acc;

	float elasticCoef;
	float frictionCoef;	

	float lifeCounter;

	solverMethod sM;
public:
	float particleLife;
	Particle(solverMethod solvM, coords pos, coords initAcc, float laMassa, float eC, float fC);
	coords position;
	void Move(float dt);
	void DetectWall(coords n, int d, float dt);
	void DetectSphere(coords pos, float radius, float dt);
	bool Die();
};


class particleManager {

	float spawnCounter;
	
public:
	int emitterRate;
	coords pos1; //posicio del emissor
	coords pos2; //posicio del emissor per cascada
	coords dir; //vector de velocitat inicial de les particules
	emiterType eType; //tipus d'emissor
	float particleLife;
	solverMethod partsMethod;
	float elasticCoef;
	float frictionCoef;

	coords wallNormals[6];
	int wallDs[6];
	std::vector<Particle> particles;
	void SpawnParticles(emiterType spawnType);
	void Update(float dt);
	
};


