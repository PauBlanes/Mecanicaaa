#pragma once

#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <vector>
#include <iostream>
#include "Includes.h"
#include "imgui\imgui.h"
#include <time.h>

//Boolean variables allow to show/hide the primitives
extern bool renderSphere;
extern bool renderCapsule;
extern bool renderParticles;

namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);


}
namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}
namespace Capsule {
	extern void setupCapsule(glm::vec3 posA = glm::vec3(-3.f, 2.f, -2.f), glm::vec3 posB = glm::vec3(-4.f, 2.f, 2.f), float radius = 1.f);
	extern void cleanupCapsule();
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB, float radius = 1.f);
	extern void drawCapsule();
}
extern float *partVerts;


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
	float fontAngle; //l'angle amb que surten les particules a la font

	coords wallNormals[6];
	int wallDs[6];
	std::vector<Particle> particles;
	void SpawnParticles(emiterType spawnType);
	void Update(float dt);
	
};


