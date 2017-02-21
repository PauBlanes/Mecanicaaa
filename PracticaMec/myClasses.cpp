
#define gravity -9.81;

enum solverMethod { euler, verlet };
struct coords { float x;float y;float z;};

class Particle {
	coords position;
	coords oldPos;
	coords velocity;
	
	float mass;
	float force;

	float elasticCoef;
	float frictionCoef;

	float particleLife;

	solverMethod sM;
public:
	Particle(solverMethod solvM, coords pos, coords vel, float laMassa) {
		sM = solvM;
		
		position = pos;
		oldPos = { 0,0,0 };
		velocity = vel;

		mass = laMassa;

		//calculem la força
		force = mass*gravity;
	};
	void Move(float dt) {
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
	void DetectWall() {

	}

};