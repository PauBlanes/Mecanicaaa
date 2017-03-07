
#include "myClasses.h"


Particle::Particle(solverMethod solvM, coords pos, coords vel, float laMassa) {
	sM = solvM;
		
	position = pos;
	oldPos = { 0,0,0 };
	velocity = vel;

	mass = laMassa;

	//calculem la força
	force = mass*gravity;

	elasticCoef = elasticC;
};
void Particle::Move(float dt) {
	if (sM == euler) {			
		//noves posicions
		position.x += dt*velocity.x;
		position.y += dt*velocity.y;
		position.z += dt*velocity.z;
		
		//tant en la x com z la velocitat es manté igual pq l'acceleració només és la de la gravetat
		velocity.y += dt*(force / mass);
		
		
	}
	else {
		position.x += (position.x - oldPos.x);
		position.y += (position.y - oldPos.y) + (force / mass)*dt*dt;
		position.z += (position.z - oldPos.z);
		oldPos = position;
	}
}
void Particle::DetectWall(coords n, int d) {

	if (0.1 > n.x*position.x + n.y*position.y + n.z*position.z + d && -0.1 < n.x*position.x + n.y*position.y + n.z*position.z + d) {
		
		std::cout << "colisio" << std::endl;
		float VperN = (n.x*velocity.x) + (n.y*velocity.y) + (n.y*velocity.y); // v*n
		velocity.x += -(1 + elasticCoef)*(n.x*VperN);
		velocity.y += -(1 + elasticCoef)*(n.y*VperN);
		velocity.z += -(1 + elasticCoef)*(n.z*VperN);
		
	}
}


//MANAGER
void particleManager::Update(float dt) {
	
	for (int i = 0; i < particles.size(); ++i) {
		
		particles[i].Move(dt);
		
		partVerts[i * 3 + 0] = particles[i].position.x;
		partVerts[i * 3 + 1] = particles[i].position.y;
		partVerts[i * 3 + 2] = particles[i].position.z;		

	}
	LilSpheres::updateParticles(0, particles.size(), partVerts);
}
void particleManager::AddPart(Particle temp) {
	particles.push_back(temp);
}



