#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include "myClasses.h"
#include <iostream>
#include <vector>

using namespace std;
bool show_test_window = false;
particleManager pM;

float* partVerts = new float[LilSpheres::maxParticles * 3];

static bool Play_simulation = true;
static bool Reset = false;
coords newPos;coords newVel;
static float second = 0;
static float pos[3]={ newPos.x,newPos.y,newPos.z };
static float dir[3] = { newVel.x,newVel.y,newVel.z };
static float angle = 0.0f;
static float Rad = 2*3.1415926/360;
static int EmissionRate; static int life;
static int Euler_Verlet = 0;

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
//		if (second <= 1)	second += 1000 / ImGui::GetIO().Framerate / 1000;
//		else				second = 0.0f;
//		cout << second << endl;

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
			
			ImGui::DragInt("Emitter rate", &EmissionRate, 1);
			ImGui::DragInt("Particle life", &life, 2);
			//Faountain/Cascade
			static int Fout_Casca = 0;
			ImGui::RadioButton("Fountain", &Fout_Casca, 0); ImGui::SameLine();
			ImGui::RadioButton("Cascade", &Fout_Casca, 1);
			//position
			
			static float angle = 0.0f;
			ImGui::InputFloat3("position", pos);
			ImGui::InputFloat3("direction", dir);
			ImGui::SliderAngle("angle", &angle);

			newPos.x = pos[0]; newPos.y = pos[1]; newPos.z = pos[2];

			
			newVel.x = dir[0]*cos(angle*Rad); newVel.y = dir[1] * sin(angle*Rad); newVel.z = dir[2] * cos(angle*Rad);
			
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
			static float iela = 0.689f, ifri = 0.2f;
			ImGui::DragFloat("Elastic Coefficient", &iela, 0.001f);
			ImGui::DragFloat("Friction Coefficient", &ifri, 0.001f);
		}
		//Colliders
		if (ImGui::CollapsingHeader("Colliders"))
		{
			static bool SphereCollider = true;
			ImGui::Checkbox("Use Sphere Collider", &SphereCollider);
			//Sphere
			static float SpherePos[3] = { 3.0f,1.0f,0.0f };
			static float SphereRad = 1.0f;
			if (SphereCollider) {
				ImGui::InputFloat3("Sphere Position", SpherePos);
				ImGui::DragFloat("Sphere Radius", &SphereRad, 0.005f);
			}
			//Capsule
			static bool CapCollider = true;
			ImGui::Checkbox("Use Capsule Collider", &CapCollider);
			static float CapPosA[3] = { -3.0f,2.0f,-2.0f };
			static float CapPosB[3] = { -4.0f,2.0f,2.0f };
			static float CapRad = 1.0f;
			if (CapCollider) {
				ImGui::InputFloat3("Capsule Pos A", CapPosA);
				ImGui::InputFloat3("Capsule Pos B", CapPosB);
				ImGui::DragFloat("Capsule Radius", &SphereRad, 0.005f);
			}
		}
		//Forces
		if (ImGui::CollapsingHeader("Forces"))
		{
			static bool Gravity = true;
			ImGui::Checkbox("Use gravity", &Gravity);
			if (Gravity) {
				static float GravityAccel[3] = { 0.0f,-9.81f,0.0f };
				ImGui::InputFloat3("Gravity Accelation", GravityAccel);
			}
			//Use Force Field: Atraction,Repulsion.etc
			static bool Force_Field = true;
			ImGui::Checkbox("Use Force Field", &Force_Field);

			static int Forces = 0;
			if (Force_Field) {
				ImGui::RadioButton("Atraction", &Forces, 0);
				ImGui::RadioButton("Repulsion", &Forces, 1);
				ImGui::RadioButton("Repulsor ", &Forces, 2);
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



void PhysicsInit() {
	pM.wallNormals[0] = { 0,1,0 };
	pM.wallNormals[1] = { 0,-1,0 };
	pM.wallNormals[2] = { 1,0,0 };
	pM.wallNormals[3] = { -1,0,0 };
	pM.wallNormals[4] = { 0,0,1 };
	pM.wallNormals[5] = { 0,0,-1 };
	pM.wallDs[0] = 0;
	pM.wallDs[1] = 10;
	pM.wallDs[2] = -5;
	pM.wallDs[3] = 5;
	pM.wallDs[4] = 5;
	pM.wallDs[5] = -5;
}
void PhysicsUpdate(float dt) {	
	
	if (Play_simulation) {
		//actualitzar parametres del emissor
		pM.emitterRate = EmissionRate;
		pM.pos1.x = newPos.x;
		pM.pos1.y = newPos.y;
		pM.pos1.z = newPos.z;
		pM.dir.x = newVel.x;
		pM.dir.y = newVel.y;
		pM.dir.z = newVel.z;
		pM.particleLife = life;
		if (Euler_Verlet == 0)
			pM.partsMethod = euler;
		else
			pM.partsMethod = verlet;
		pM.SpawnParticles();

		//detectar murs
		for (int i = 0; i < pM.particles.size();i++) {
			for (int j = 0; j < 6;j++) {
				pM.particles[i].DetectWall(pM.wallNormals[j], pM.wallDs[j], dt);
			}
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

}