#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include "myClasses.h"
#include <iostream>
#include <vector>

using namespace std;
bool show_test_window = false;
particleManager pM;

float* partVerts = new float[LilSpheres::maxParticles * 3];
int counter = 0;
static bool Play_simulation = true;
static bool Reset = false;
//posicion, vel i angle
coords newPos; coords newVel; coords newPos2;
static float second = 0;
static float pos[3]={ newPos.x,newPos.y,newPos.z };
static float pos2[3] = { newPos2.x,newPos2.y,newPos2.z };
static float dir[3] = { newVel.x,newVel.y,newVel.z };
static float angle = 0.0f;
static float Rad = 2*3.1415926/360;
static int EmissionRate=1; static int life=2;
static int Euler_Verlet = 0;
static float iela = 0.2, ifri = 0.1f;
//font cascada
static int Fout_Casca = 0;
//gravetat
static bool Gravity = true;
static int Forces = 0;
static float GravityAccel[3] = { 0.0f,-9.81f,0.0f };
//força
static bool Force_Field = true;
static float F1; static float F2;
//esfera
static bool SphereCollider = true;
static float SpherePos[3] = { 3.0f,1.0f,0.0f };
static float SphereRad = 1.0f;
coords spherePosition;
//capsura
static bool CapCollider = true;
static float CapPosA[3] = { -3.0f,2.0f,-2.0f };
static float CapPosB[3] = { -4.0f,2.0f,2.0f };
static float CapRad = 1.0f;
coords CapPositionA, CapPositionB;

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//for play & stop
		ImGui::Checkbox("Play simulation", &Play_simulation);
		//for resert
		if (ImGui::Button("Reset simulation")) {
			Reset ^= 1; 
		}
		//Emitter
		if (ImGui::CollapsingHeader("Emitter"))
		{
			//Emitter rate & Particle life
			ImGui::DragInt("Emitter rate", &EmissionRate, 1); //quantes particules per segon
			ImGui::DragInt("Particle life", &life, 2);
			//Faountain/Cascade
			
			ImGui::RadioButton("Fountain", &Fout_Casca, 0); ImGui::SameLine();
			ImGui::RadioButton("Cascade", &Fout_Casca, 1);
			//position
			
			ImGui::InputFloat3("position", pos);
			if (Fout_Casca == 1) { //nomes si és casdada mostrem la segona posicio i la direccio
				ImGui::InputFloat3("position2", pos2);
				ImGui::InputFloat3("direction", dir);
			}				
			ImGui::SliderAngle("angle", &angle);

			newPos.x = pos[0]; newPos.y = pos[1]; newPos.z = pos[2];
			newPos2.x = pos2[0]; newPos2.y = pos2[1]; newPos2.z = pos2[2];
		}
		//Integration
		if (ImGui::CollapsingHeader("Integration"))
		{
			
			ImGui::RadioButton("Euler", &Euler_Verlet, 0);
			ImGui::RadioButton("Verlet", &Euler_Verlet, 1);
		}
		//Elasticity & friction
		if (ImGui::CollapsingHeader("Elasticity & friction"))
		{
			
			ImGui::DragFloat("Elastic Coefficient", &iela, 0.001f);
			ImGui::DragFloat("Friction Coefficient", &ifri, 0.001f);
		}
		//Colliders
		if (ImGui::CollapsingHeader("Colliders"))
		{
			
			ImGui::Checkbox("Use Sphere Collider", &SphereCollider);
			//Sphere
			
			if (SphereCollider) {
				ImGui::InputFloat3("Sphere Position", SpherePos);
				ImGui::DragFloat("Sphere Radius", &SphereRad, 0.005f);
			}
			//Capsule
			ImGui::Checkbox("Use Capsule Collider", &CapCollider);
			
			if (CapCollider) {
				ImGui::InputFloat3("Capsule Pos A", CapPosA);
				ImGui::InputFloat3("Capsule Pos B", CapPosB);
				ImGui::DragFloat("Capsule Radius", &CapRad, 0.005f);
			}
		}
		//Forces
		if (ImGui::CollapsingHeader("Forces"))
		{
			
			ImGui::Checkbox("Use gravity", &Gravity);
			if (Gravity) {
				ImGui::InputFloat3("Gravity Accelation", GravityAccel);
			}
			//Use Force Field: Atraction,Repulsion.etc
			
			ImGui::Checkbox("Use Force Field", &Force_Field);
			
			if (Force_Field) {
				ImGui::RadioButton("Atraction", &Forces, 0);
				ImGui::RadioButton("Repulsion", &Forces, 1);
			}
		}
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

int ini = 0;

void PhysicsInit() {
	pM.wallNormals[0] = { 0,1,0 };
	pM.wallNormals[1] = { 0,-1,0 };
	pM.wallNormals[2] = { 1,0,0 };
	pM.wallNormals[3] = { -1,0,0 };
	pM.wallNormals[4] = { 0,0,1 };
	pM.wallNormals[5] = { 0,0,-1 };
	pM.wallDs[0] = 0;
	pM.wallDs[1] = 10;
	pM.wallDs[2] = 5;
	pM.wallDs[3] = 5;
	pM.wallDs[4] = -5;
	pM.wallDs[5] = -5;
}
void PhysicsUpdate(float dt) {	
	
	if (Play_simulation) {
		//actualitzar parametres del emissor
		pM.emitterRate = EmissionRate;
		pM.pos1.x = newPos.x;	pM.pos1.y = newPos.y;	pM.pos1.z = newPos.z;		
		pM.particleLife = life;
		if (Euler_Verlet == 0)
			pM.partsMethod = euler;
		else
			pM.partsMethod = verlet;
		
		pM.elasticCoef = iela;	pM.frictionCoef = ifri;
		
		//SPAWNS
		//fount
		if (Fout_Casca==0) {			
			pM.SpawnParticles(font);
			//newVel.y = dir[0] * tan(angle*Rad);
			//cout << newVel.y << endl;
			//newVel.x = dir[0]*cos(angle*Rad); newVel.y = dir[1] * sin(angle*Rad); newVel.z = dir[2] * cos(angle*Rad);
			//pM.pos1.y = newVel.y*
		}
		//cascada
		else {
			pM.pos2.x = newPos2.x;	pM.pos2.y = newPos2.y;	pM.pos2.z = newPos2.z;
			pM.dir.x = newVel.x;	pM.dir.y = newVel.y;	pM.dir.z = newVel.z;
			pM.SpawnParticles(cascada);
		}

		if (Gravity) {
			gravity = GravityAccel[1];
		}
		else{
			gravity = 0;
		}
		//força
		if (Force_Field) {
			if (Forces == 0) {
				//attraccion
				F1;
				F2;
			}
			else {
				//repulsion
				F1;
				F2;
			}
		}
		
		//colisio murs
		for (int i = 0; i < pM.particles.size(); i++) {
			for (int j = 0; j < 6; j++) {
				pM.particles[i].DetectWall(pM.wallNormals[j], pM.wallDs[j], dt);
			}
		}
		//colisio esfera
		if (SphereCollider) {
			renderSphere = true;
			spherePosition.x = SpherePos[0];	spherePosition.y = SpherePos[1];	spherePosition.z = SpherePos[2];
			Sphere::updateSphere(glm::vec3(SpherePos[0], SpherePos[1], SpherePos[2]), SphereRad);
			for (int i = 0; i < pM.particles.size(); i++) {
				pM.particles[i].DetectSphere(spherePosition, SphereRad, dt);
			}
		}
		else {
			renderSphere = false;
		}
		//capsure
		if (CapCollider) {
			renderCapsule = true;
			CapPositionA.x = CapPosA[0];	CapPositionA.y = CapPosA[1];	CapPositionA.z = CapPosA[2];
			CapPositionB.x = CapPosB[0];	CapPositionB.y = CapPosB[1];	CapPositionB.z = CapPosB[2];
			Capsule::updateCapsule(glm::vec3(CapPosA[0], CapPosA[1], CapPosA[2]), glm::vec3(CapPosB[0], CapPosB[1], CapPosB[2]), CapRad);
		}
		else {
			renderCapsule = false;
		}

		//moure particules
		pM.Update(dt);
	}
	if (Reset) {
		for(int i = 0; i < pM.particles.size(); i++) {
			pM.particles.clear(); //netejem l'array i com que el dels vertex nomes fem update tenint en compte el tamany d'aquest dons no tindrem problemes.
		}
		PhysicsInit();
		Reset = false;
		Play_simulation = true;
	}
	
}
void PhysicsCleanup() {
	delete[] partVerts;
}