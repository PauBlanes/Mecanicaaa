#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include "myClasses.h"
#include <iostream>
#include <vector>

bool show_test_window = false;
particleManager pM;

static bool Play_simulation = true;
static bool Reset = false;
coords newPos;
coords newVel;
void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//for play & stop
		ImGui::Checkbox("Play simulation", &Play_simulation);
		//for resert
		if (ImGui::Button("Reset simulation")) {
			Reset ^= 1; //pq ^=??
			//reiniciar todo los datos
		}
		//Emitter
		if (ImGui::CollapsingHeader("Emitter"))
		{
			static int i1 = 1000, i2 = 5000;
			ImGui::DragInt("Emitter rate", &i1, 1);
			ImGui::DragInt("Particle life", &i2, 2);
			//Faountain/Cascade
			static int Fout_Casca = 0;
			ImGui::RadioButton("Fountain", &Fout_Casca, 0); ImGui::SameLine();
			ImGui::RadioButton("Cascade", &Fout_Casca, 1);
			//position
			static float pos[3] = { 0.0f,3.0f,0.0f };
			static float dir[3] = { 0.0f,7.11f,0.2f };
			static float angle = 0.0f;
			ImGui::InputFloat3("position", pos);
			ImGui::InputFloat3("direction", dir);
			ImGui::SliderAngle("angle", &angle);
		}
		//Integration
		if (ImGui::CollapsingHeader("Integration"))
		{
			static int Euler_Verlet = 0;
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
	for (int i = 0; i < 1; ++i) {
		newPos = { 0,5,0};
		newVel = { 2,0,0 };
		Particle temp(euler, newPos, newVel, 1.0);
		partVerts[i * 3 + 0] = temp.position.x;
		partVerts[i * 3 + 1] = temp.position.y;
		partVerts[i * 3 + 2] = temp.position.z;

		pM.AddPart(temp);

	}
	LilSpheres::updateParticles(0, pM.particles.size(), partVerts);

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
	pM.wallDs[4] = -5;
	pM.wallDs[5] = 5;
}
void PhysicsUpdate(float dt) {	
	for (int i = 0; i < pM.particles.size();i++) {
		
		for (int j = 0; j < 6;j++) {
			pM.particles[i].DetectWall(pM.wallNormals[j], pM.wallDs[j], dt);
		}
		
	}
	if (Play_simulation) {
		pM.Update(dt);
	}
	if (Reset) {
		PhysicsInit();
	}
	
}
void PhysicsCleanup() {
	//TODO
}