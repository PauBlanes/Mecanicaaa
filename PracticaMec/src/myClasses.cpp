﻿
#include "myClasses.h"

float gravity;
int NumPrticles = 0;
Particle::Particle(solverMethod solvM, coords pos, coords initAcc, float laMassa, float eC, float fC) {
	sM = solvM;
		
	position.x = pos.x;		position.y = pos.y;		position.z = pos.z;
	actualPos.x = pos.x;	actualPos.y = pos.y;	actualPos.z = pos.z;
	oldPos.x = pos.x;		oldPos.y = pos.y;		oldPos.z = pos.z;

	velocity = {0,0,0};
	mass = laMassa;

	//calculem la forca
	acc.x = initAcc.x;
	acc.y = gravity + initAcc.y;
	acc.z = initAcc.z;

	force.x = mass*acc.x;
	force.y = mass*acc.y;
	force.z = mass*acc.z;

	elasticCoef = eC;
	frictionCoef = fC;

	lifeCounter = 0;
};
void Particle::Move(float dt) {
	
	if (sM == euler) {			
		//noves posicions
		position.x += dt*velocity.x;
		position.y += dt*velocity.y;
		position.z += dt*velocity.z;
		
		//sumem acceleracions que ens dona l'emissor o en el cas d'un camp de for鏰
		velocity.x += dt*(force.x / mass);
		velocity.y += dt*(force.y / mass);
		velocity.z += dt*(force.z / mass);
		
		//anar actualitzant aixo per si canviem en mig de la simulacio que no surtin disparades
		oldPos.x = actualPos.x;
		oldPos.y = actualPos.y;
		oldPos.z = actualPos.z;
		actualPos.x = position.x;
		actualPos.y = position.y;
		actualPos.z = position.z;
		
	}
	else {
		position.x = actualPos.x + (actualPos.x - oldPos.x) + (force.x / mass)*dt*dt;
		position.y = actualPos.y + (actualPos.y - oldPos.y) + (force.y / mass)*dt*dt;
		position.z = actualPos.z + (actualPos.z - oldPos.z) + (force.z / mass)*dt*dt;
		oldPos.x = actualPos.x;
		oldPos.y = actualPos.y;
		oldPos.z = actualPos.z;
		actualPos.x = position.x;
		actualPos.y = position.y;
		actualPos.z = position.z;
	}
	//recalculem forces per si han canviat
	force.x = mass*acc.x;
	force.y = mass*acc.y;
	force.z = mass*acc.z;
}
void Particle::DetectWall(coords n, int d, float dt) {
	//calculem quina seria la seva seguent posicio
	coords posCreuada = {0,0,0};
	//colisio per metode euler
	if (sM == euler) {
		posCreuada.x = position.x + dt*velocity.x;
		posCreuada.y = position.y + dt*velocity.y;
		posCreuada.z = position.z + dt*velocity.z;

		//el rebot
		if ((n.x*position.x + n.y*position.y + n.z*position.z + d) * (n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z + d) <= 0) {
			//std::cout << "colisio" << std::endl;
			float VperN = (n.x*velocity.x) + (n.y*velocity.y) + (n.z*velocity.z); // v*n
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
	//colisio per verlet
	else {
		posCreuada.x = position.x + (position.x - oldPos.x) + (force.x / mass)*dt*dt;
		posCreuada.y += position.y + (position.y - oldPos.y) + (force.y / mass)*dt*dt;
		posCreuada.z += position.z + (position.z - oldPos.z) + (force.z / mass)*dt*dt;

		//el rebot
		if ((n.x*position.x + n.y*position.y + n.z*position.z + d) * (n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z + d) <= 0) {

			coords projectedPos;
			projectedPos.x = position.x - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.x;
			projectedPos.y = position.y - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.y;
			projectedPos.z = position.z - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.z;
			
			coords projectedCreuada;
			projectedCreuada.x = posCreuada.x - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.x;
			projectedCreuada.y = posCreuada.y - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.y;
			projectedCreuada.z = posCreuada.z - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.z;
			
			coords dirVector;
			dirVector.x = projectedCreuada.x - projectedPos.x;
			dirVector.y = projectedCreuada.y - projectedPos.y;
			dirVector.z = projectedCreuada.z - projectedPos.z;
			
			coords dirVectorN;
			dirVectorN.x = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.x;
			dirVectorN.y = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.y;
			dirVectorN.z = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.z;

			//la nova pos
			
			position.x = projectedPos.x + (1 - elasticCoef)*dirVectorN.x + frictionCoef* (dirVector.x - dirVectorN.x);
			position.y = projectedPos.y + (1 - elasticCoef)*dirVectorN.y + frictionCoef* (dirVector.y - dirVectorN.y);
			position.z = projectedPos.z + (1 - elasticCoef)*dirVectorN.z + frictionCoef* (dirVector.z - dirVectorN.z);
			actualPos = position;
			oldPos = projectedPos;
		}
	}	
}
void Particle::DetectSphere(coords Pos, float radius, float dt) {
	//calculem quina seria la seva seguent posicio
	coords posCreuada = {0,0,0};
	//colisio per metode euler
	if (sM == euler) {
		posCreuada.x = position.x + dt*velocity.x;
		posCreuada.y = position.y + dt*velocity.y;
		posCreuada.z = position.z + dt*velocity.z;
		coords distVector = { posCreuada.x - Pos.x, posCreuada.y - Pos.y, posCreuada.z - Pos.z };
		float dist = sqrt((distVector.x*distVector.x)+ (distVector.y*distVector.y)+ (distVector.z*distVector.z));
		if (dist < radius) {
			//std::cout << "CollShpere" << std::endl;
			coords n = { distVector.x / dist, distVector.y / dist, distVector.z / dist };
			float VperN = (n.x*velocity.x) + (n.y*velocity.y) + (n.z*velocity.z); // v*n
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
		posCreuada.x = position.x + (position.x - oldPos.x) + (force.x / mass)*dt*dt;
		posCreuada.y += position.y + (position.y - oldPos.y) + (force.y / mass)*dt*dt;
		posCreuada.z += position.z + (position.z - oldPos.z) + (force.z / mass)*dt*dt;
		coords distVector = { posCreuada.x - Pos.x, posCreuada.y - Pos.y, posCreuada.y - Pos.y };
		float dist = sqrt((distVector.x*distVector.x) + (distVector.y*distVector.y) + (distVector.y*distVector.y));
		if (dist < radius) {
			//std::cout << "CollShpere" << std::endl;
			coords n = { distVector.x / dist, distVector.y / dist, distVector.z / dist };
			coords p = { Pos.x + n.x*radius, Pos.y + n.y*radius ,Pos.z + n.z*radius };
			float d = -(p.x*n.x + p.y*n.y + p.z*n.z);
			
			coords projectedPos;
			projectedPos.x = position.x - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.x;
			projectedPos.y = position.y - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.y;
			projectedPos.z = position.z - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.z;

			coords projectedCreuada;
			projectedCreuada.x = posCreuada.x - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.x;
			projectedCreuada.y = posCreuada.y - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.y;
			projectedCreuada.z = posCreuada.z - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.z;

			coords dirVector;
			dirVector.x = projectedCreuada.x - projectedPos.x;
			dirVector.y = projectedCreuada.y - projectedPos.y;
			dirVector.z = projectedCreuada.z - projectedPos.z;

			coords dirVectorN;
			dirVectorN.x = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.x;
			dirVectorN.y = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.y;
			dirVectorN.z = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.z;

			//la nova pos
			position.x = projectedPos.x + (1 - elasticCoef)*dirVectorN.x + frictionCoef* (dirVector.x - dirVectorN.x);
			position.y = projectedPos.y + (1 - elasticCoef)*dirVectorN.y + frictionCoef* (dirVector.y - dirVectorN.y);
			position.z = projectedPos.z + (1 - elasticCoef)*dirVectorN.z + frictionCoef* (dirVector.z - dirVectorN.z);
			actualPos = position;
			oldPos = projectedPos;
		}
	}
}
void Particle::DetectCapsule(coords posA, coords posB, float radius, float dt) {
	coords posCreuada = { 0,0,0 };
	if (sM == euler) {
		posCreuada.x = position.x + dt*velocity.x;
		posCreuada.y = position.y + dt*velocity.y;
		posCreuada.z = position.z + dt*velocity.z;		
		coords AP = { posCreuada.x - posA.x,posCreuada.y - posA.y,posCreuada.z - posA.z };
		coords AB = { posB.x - posA.x,posB.y - posA.y,posB.z - posA.z };
		float distAB = sqrt((AB.x*AB.x) + (AB.y*AB.y) + (AB.z*AB.z));
		coords clamp = {(AP.x*AB.x/(distAB*distAB))*AB.x,(AP.y*AB.y / (distAB*distAB))*AB.y, (AP.z*AB.z / (distAB*distAB))*AB.z};
		
		coords Q = { posA.x + clamp.x,posA.y + clamp.y,posA.z + clamp.z };
		coords AQ = {Q.x-posA.x,Q.y-posA.y,Q.z - posA.z };
		float distAQ = sqrt((AQ.x*AQ.x)+ (AQ.y*AQ.y) + (AQ.z*AQ.z) );
		coords PQ = {Q.x- posCreuada.x,Q.y - posCreuada.y, Q.z - posCreuada.z};
		float distPQ = sqrt((PQ.x*PQ.x)+ (PQ.y*PQ.y)+ (PQ.z*PQ.z));
		if (distPQ < radius) {
			bool distMinor = false;
			if (distAQ < distAB)	distMinor = true;
			else distMinor = false;
			if (distMinor) {
				coords U = PQ;	float Uquadrat = (U.x*U.x)+(U.y*U.y)+(U.z*U.z);
				coords projuV;
				projuV.x = velocity.x - ((velocity.x*U.x / Uquadrat)*U.x);
				projuV.y = velocity.y - ((velocity.y*U.y / Uquadrat)*U.y);
				projuV.z = velocity.z - ((velocity.z*U.z / Uquadrat)*U.z);
				float projuVquadrat = ((projuV.x*projuV.x) + (projuV.y*projuV.y) + (projuV.z*projuV.z));
				coords projuV2;
				projuV2.x = velocity.x - ((velocity.x*projuV.x/projuVquadrat)*projuV.x);
				projuV2.y = velocity.y - ((velocity.y*projuV.y / projuVquadrat)*projuV.y);
				projuV2.z = velocity.z - ((velocity.z*projuV.z / projuVquadrat)*projuV.z);

				velocity.x = -projuV2.x + projuV.x;
				velocity.y = -projuV2.y + projuV.y;
				velocity.z = -projuV2.x + projuV.z;	

				float distVelocity=sqrt((velocity.x*velocity.x)+ (velocity.y*velocity.y) + (velocity.z*velocity.z));
				coords n = {velocity.x/distVelocity,velocity.y / distVelocity, velocity.z / distVelocity};
				float VperN = (n.x*velocity.x)+(n.y*velocity.y)+(n.z*velocity.z);
				
				velocity.x += (elasticCoef)*(velocity.x);
				velocity.y += (elasticCoef)*(velocity.y);
				velocity.z += (elasticCoef)*(velocity.z);

				//friccion
				coords vN;
				vN.x = VperN*n.x;
				vN.y = VperN*n.y;
				vN.z = VperN*n.z;
				velocity.x += -frictionCoef * (velocity.x - vN.x); //-u*vT
				velocity.y += -frictionCoef * (velocity.y - vN.y);
				velocity.z += -frictionCoef * (velocity.z - vN.z);
			}
			else {
				DetectSphere(posA,radius,dt);
				DetectSphere(posB, radius, dt);
			}
		}
	}
	else {
		posCreuada.x = position.x + (position.x - oldPos.x) + (force.x / mass)*dt*dt;
		posCreuada.y += position.y + (position.y - oldPos.y) + (force.y / mass)*dt*dt;
		posCreuada.z += position.z + (position.z - oldPos.z) + (force.z / mass)*dt*dt;
	
		coords AP = { posCreuada.x - posA.x,posCreuada.y - posA.y,posCreuada.z - posA.z };
		coords AB = { posB.x - posA.x,posB.y - posA.y,posB.z - posA.z };
		float distAB = sqrt((AB.x*AB.x) + (AB.y*AB.y) + (AB.z*AB.z));
		coords clamp = { (AP.x*AB.x / (distAB*distAB))*AB.x,(AP.y*AB.y / (distAB*distAB))*AB.y, (AP.z*AB.z / (distAB*distAB))*AB.z };

		coords Q = { posA.x + clamp.x,posA.y + clamp.y,posA.z + clamp.z };
		coords AQ = { Q.x - posA.x,Q.y - posA.y,Q.z - posA.z };
		float distAQ = sqrt((AQ.x*AQ.x) + (AQ.y*AQ.y) + (AQ.z*AQ.z));
		coords PQ = { Q.x - posCreuada.x,Q.y - posCreuada.y, Q.z - posCreuada.z };
		float distPQ = sqrt((PQ.x*PQ.x) + (PQ.y*PQ.y) + (PQ.z*PQ.z));

		if (distPQ < radius) {

			coords n = { PQ.x / distPQ, PQ.y / distPQ, PQ.z / distPQ };
			coords p = { Q.x + n.x*radius, Q.y + n.y*radius ,Q.z + n.z*radius };
			float d = -(p.x*n.x + p.y*n.y + p.z*n.z);

			coords projectedPos;
			projectedPos.x = position.x - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.x;
			projectedPos.y = position.y - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.y;
			projectedPos.z = position.z - 2 * ((n.x*position.x + n.y*position.y + n.z*position.z) + d)*n.z;

			coords projectedCreuada;
			projectedCreuada.x = posCreuada.x - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.x;
			projectedCreuada.y = posCreuada.y - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.y;
			projectedCreuada.z = posCreuada.z - 2 * ((n.x*posCreuada.x + n.y*posCreuada.y + n.z*posCreuada.z) + d)*n.z;

			coords dirVector;
			dirVector.x = projectedCreuada.x - projectedPos.x;
			dirVector.y = projectedCreuada.y - projectedPos.y;
			dirVector.z = projectedCreuada.z - projectedPos.z;

			coords dirVectorN;
			dirVectorN.x = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.x;
			dirVectorN.y = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.y;
			dirVectorN.z = (dirVector.x*n.x + dirVector.y*n.y + dirVector.z*n.z)*n.z;

			//la nova pos
			position.x = projectedPos.x + (1 - elasticCoef)*dirVectorN.x + frictionCoef* (dirVector.x - dirVectorN.x);
			position.y = projectedPos.y + (1 - elasticCoef)*dirVectorN.y + frictionCoef* (dirVector.y - dirVectorN.y);
			position.z = projectedPos.z + (1 - elasticCoef)*dirVectorN.z + frictionCoef* (dirVector.z - dirVectorN.z);
			actualPos = position;
			oldPos = projectedPos;
		}
	}
}
bool Particle::Die() {
	
	if (lifeCounter < particleLife) {
		lifeCounter += (1 / ImGui::GetIO().Framerate);
		return false;
	}
	else {
		lifeCounter = 0;
		return true;
	}
	
}

//MANAGER
void particleManager::Update(float dt) {
	
	//actualitzar el array de vertexs
	for (int i = 0; i < particles.size(); ++i) {
		
		particles[i].Move(dt);
		particles[i].sM = partsMethod;
		particles[i].particleLife = particleLife;

		particles[i].elasticCoef = elasticCoef;
		particles[i].frictionCoef = frictionCoef;

		partVerts[i * 3 + 0] = particles[i].position.x;
		partVerts[i * 3 + 1] = particles[i].position.y;
		partVerts[i * 3 + 2] = particles[i].position.z;

		if (particles[i].Die()) {
			particles.erase(particles.begin());
			
		}
		

	}	

	LilSpheres::updateParticles(0, particles.size(), partVerts);

	
}
void particleManager::SpawnParticles(emiterType spawnType) {
	if (emitterRate > 0) {
		spawnCounter += (1 / ImGui::GetIO().Framerate); //ho hem fet aixi pq tingui en compte que els frames no sempre van igual
		if (spawnCounter >= (1 / emitterRate)) {

			if (spawnType == font) {
				
				dir.z = 5 * (fontAngle / std::abs(fontAngle)); //aixi angle positiu sortiran cap a un costat i negatiu cap a l'altre

				float min = -(dir.z * tan(fontAngle * 3.141516 / 180.0))*100;
				float max = (dir.z * tan(fontAngle * 3.141516 / 180.0))*100;
				
				
				dir.x = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
				dir.y = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
				
				Particle temp(partsMethod, pos1, dir, 1.0, elasticCoef, frictionCoef);
				partVerts[(particles.size()) * 3 + 0] = temp.position.x;
				partVerts[(particles.size()) * 3 + 1] = temp.position.y;
				partVerts[(particles.size()) * 3 + 2] = temp.position.z;
				particles.push_back(temp);
			}
			else {
				coords finalPos;
				finalPos.x = pos1.x + (((float)rand()) / (float)RAND_MAX) * (pos2.x - pos1.x);				
				finalPos.y = pos1.y + (((float)rand()) / (float)RAND_MAX) * (pos2.y - pos1.y);
				finalPos.z = pos1.z + (((float)rand()) / (float)RAND_MAX) * (pos2.z - pos1.z);
				

				Particle temp(partsMethod, finalPos, dir, 1.0, elasticCoef, frictionCoef);
				partVerts[(particles.size()) * 3 + 0] = temp.position.x;
				partVerts[(particles.size()) * 3 + 1] = temp.position.y;
				partVerts[(particles.size()) * 3 + 2] = temp.position.z;
				particles.push_back(temp);
			}		
			spawnCounter = 0;
		}
	}
}



