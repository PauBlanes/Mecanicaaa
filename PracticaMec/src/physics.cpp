#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include "myClasses.h"
#include <iostream>
#include <vector>

bool show_test_window = false;
particleManager pM;

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//for play & stop
		static bool Play_simulation = false;
		ImGui::Checkbox("Play simulation", &Play_simulation);
		//for resert
		static bool Resert = false;
		if (ImGui::Button("Resert simulation")) {
			Resert ^= 1;
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
	//TODO
}
void PhysicsUpdate(float dt) {
	pM.Update(dt);
}
void PhysicsCleanup() {
	//TODO
}