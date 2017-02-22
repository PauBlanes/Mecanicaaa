#pragma once
#define gravity -9.81;

enum solverMethod { euler, verlet };
struct coords { float x;float y;float z; };

std::vector<Particle> particles;

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