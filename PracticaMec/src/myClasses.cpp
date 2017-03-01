
#include "myClasses.h"


Particle::Particle(solverMethod solvM, coords pos, coords vel, float laMassa) {
	sM = solvM;
		
	position = pos;
	oldPos = { 0,0,0 };
	velocity = vel;

	mass = laMassa;

	//calculem la força
	force = mass*gravity;
};
void Particle::Move(float dt) {
	if (sM == euler) {			
		//tant en la x com z la velocitat es manté igual pq l'acceleració només és la de la gravetat
		velocity.y += dt*(force / mass);
		
		//noves posicions
		position.x += dt*velocity.x;
		position.y += dt*velocity.y;
		position.z += dt*velocity.z;
		
	}
	else {
		position.x += (position.x - oldPos.x) + (force / mass)*dt*dt;
		oldPos = position;
	}
}
void Particle::DetectWall(coords n, int d) {
	if (n.x*position.x + n.y*position.y + n.z*position.z + d == 0) {
		//colisionaaaaaaaaaaaaant wololooooooooooooo
	}
}

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



