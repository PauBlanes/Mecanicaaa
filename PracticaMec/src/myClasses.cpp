
#include "myClasses.h"


Particle::Particle(solverMethod solvM, coords pos, coords vel, float laMassa) {
	sM = solvM;
		
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;
	actualPos.x = pos.x;
	actualPos.y = pos.y;
	actualPos.z = pos.z;
	oldPos.x = position.x;
	oldPos.y = position.y;
	oldPos.z = position.z;
	velocity = vel;

	mass = laMassa;

	//calculem la força
	force = mass*gravity;

	elasticCoef = elasticC;
	frictionCoef = friction;
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
		position.x = actualPos.x + (actualPos.x - oldPos.x);
		position.y = actualPos.y + (actualPos.y - oldPos.y) + (force / mass)*dt*dt;
		position.z = actualPos.z + (actualPos.z - oldPos.z);
		oldPos.x = actualPos.x;
		oldPos.y = actualPos.y;
		oldPos.z = actualPos.z;
		actualPos.x = position.x;
		actualPos.y = position.y;
		actualPos.z = position.z;
	}
}
void Particle::DetectWall(coords n, int d, float dt) {
	//calculem quina seria la seva seguent posicio
	coords posCreuada = {0,0,0};
	if (sM == euler) {
		posCreuada.x = position.x + dt*velocity.x;
		posCreuada.y = position.y + dt*velocity.y;
		posCreuada.z = position.z + dt*velocity.z;

		//el rebot
		if ((n.x*position.x + n.y*position.y + n.z*position.z + d) * (n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z + d) <= 0) {
			std::cout << "colisio" << std::endl;
			float VperN = (n.x*velocity.x) + (n.y*velocity.y) + (n.y*velocity.y); // v*n
			//elasticidad
			velocity.x += -(1 + elasticCoef)*(n.x*VperN);
			velocity.y += -(1 + elasticCoef)*(n.y*VperN);
			velocity.z += -(1 + elasticCoef)*(n.z*VperN);
			//friccion
			coords vN;
			vN.x = VperN*n.x;
			vN.y = VperN*n.y;
			vN.z = VperN*n.z;			
			velocity.x += -frictionCoef * (velocity.x - vN.x); //-u*vT
			velocity.y += -frictionCoef * (velocity.y - vN.y);
			velocity.z += -frictionCoef * (velocity.z - vN.z);

		}
	}
	else {
		posCreuada.x = position.x + (position.x - oldPos.x);
		posCreuada.y += position.y + (position.y - oldPos.y) + (force / mass)*dt*dt;
		posCreuada.z += position.z + (position.z - oldPos.z);

		//el rebot
		if ((n.x*position.x + n.y*position.y + n.z*position.z + d) * (n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z + d) <= 0) {
			std::cout << "colisio" << std::endl;

			coords projectedPos;
			projectedPos.x = position.x - 2 * ((n.x*position.x + n.x*position.x + n.z*position.x) + d)*n.x;
			projectedPos.y = position.y - 2 * ((n.y*position.y + n.y*position.y + n.z*position.y) + d)*n.y;
			projectedPos.z = position.z - 2 * ((n.z*position.z + n.y*position.z + n.z*position.z) + d)*n.z;
			coords projectedCreuada;
			projectedCreuada.x = posCreuada.x - 2 * ((n.x*posCreuada.x + n.y*posCreuada.x + n.z*posCreuada.x) + d)*n.x;
			projectedCreuada.y = posCreuada.y - 2 * ((n.x*posCreuada.y + n.y*posCreuada.y + n.z*posCreuada.y) + d)*n.y;
			projectedCreuada.z = posCreuada.z - 2 * ((n.x*posCreuada.z + n.y*posCreuada.z + n.z*posCreuada.z) + d)*n.z;
			coords dirVector;
			dirVector.x = projectedCreuada.x - projectedPos.x;
			dirVector.y = projectedCreuada.y - projectedPos.y;
			dirVector.z = projectedCreuada.z - projectedPos.z;
			coords dirVectorN;
			dirVectorN.x = (dirVector.x*n.x + dirVector.x*n.x + dirVector.z*n.x)*n.x;
			dirVectorN.y = (dirVector.y*n.y + dirVector.y*n.y + dirVector.z*n.y)*n.y;
			dirVectorN.z = (dirVector.z*n.z + dirVector.z*n.z + dirVector.z*n.z)*n.z;

			//la nova pos
			position.x = projectedPos.x + (1 - elasticCoef)*dirVectorN.x + frictionCoef* (dirVector.x - dirVectorN.x);
			position.y = projectedPos.y + (1 - elasticCoef)*dirVectorN.y + frictionCoef* (dirVector.y - dirVectorN.y);
			position.z = projectedPos.z + (1 - elasticCoef)*dirVectorN.z + frictionCoef* (dirVector.z - dirVectorN.z);
			
		}
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

void particleManager::SpawnParticles() {
	
	if (emitterRate > 0) {
		spawnCounter += (1/ImGui::GetIO().Framerate);
		if (spawnCounter >= emitterRate) {
			spawnCounter = 0;
			std::cout << "SPAWN" << std::endl;
			Particle temp(euler, pos1, iVelocity, 1.0);
			particles.push_back(temp);
			partVerts[(particles.size()-1) * 3 + 0] = temp.position.x;
			partVerts[(particles.size() - 1) * 3 + 1] = temp.position.y;
			partVerts[(particles.size() - 1) * 3 + 2] = temp.position.z;
		}
	
	}
}



