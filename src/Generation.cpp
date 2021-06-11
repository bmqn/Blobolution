#include "Generation.h"

#include <iostream>
#include <map>
#include <random>
#include <algorithm>
#include <iterator>

void Generation::nextGeneration()
{
}

Generation::Generation(int carCount) :
	generationCount_(1), carCount_(carCount), world_(new b2World({ 0, -10 }))
{
	for (int i = 0; i < carCount_; i++)
		cars_.push_back(new Car(*world_, Car::randomCarData()));

	platform_ = new Platform(*world_, 500);
}

Generation::~Generation()
{
	for (auto c : cars_)
		delete c;

	delete platform_;

	delete world_;
}

glm::vec3 Generation::getPositionOfBestCar()
{
	glm::vec3 best = { cars_[0]->getPosition().x, cars_[0]->getPosition().y, 0 };


	for (auto& car : cars_)
	{
		auto pos = car->getPosition();

		if (pos.x > best.x)
		{
			best.x = pos.x;
			best.y = pos.y;
		}
	}

	return best;
}


//void Generation::nextGeneration()
//{
//	float generation_fitness = 1;
//	float average_fitness;
//	float best_fitness = 0;
//	CarData best_car_data;
//
//	for (auto c : cars_)
//	{
//		generation_fitness += c->get_fitness();
//
//		if (c->get_fitness() >= best_fitness)
//		{
//			best_fitness = c->get_fitness();
//			best_car_data = c->get_car_data();
//		}
//	}
//
//	average_fitness = generation_fitness / cars_.size();
//
//	std::map<int, CarData> generation_parents;
//	int index = 0;
//
//	for (auto c : cars_)
//	{
//	    float percentage_of_score = (float)c->get_fitness() / generation_fitness;
//	    for (int i = 0; i < (int)(percentage_of_score * 10.0f) + 1; i++)
//			generation_parents.emplace(index++, c->get_car_data());
//	}
//
//	for (auto c : cars_)
//		delete c;
//	cars_.clear();
//
//	for (int i = 0; i < carCount_; i++)
//	{
//		int parent1 = rand() % generation_parents.size();
//	    int parent2 = rand() % generation_parents.size();
//
//		CarData p1 = generation_parents[parent1];
//		CarData p2 = generation_parents[parent2];
//
//		CarData parents[2] = { p1, p2 };
//
//		float mutation = 0.05f;
//		CarData data;
//
//		for (int j = 0; j < 8; j++)
//		{
//			b2Vec2 pv = parents[rand() % 2].vertices[j];
//
//			b2Vec2 new_vert = { pv.x, pv.y };
//			if ((float)(rand() % 1000) / 1000 < mutation)
//			{
//				new_vert.x += ((float)(rand() % 1000 - 500) / 500);
//				new_vert.y += ((float)(rand() % 1000 - 500) / 500);
//			}
//			data.vertices[j].Set(new_vert.x, new_vert.y);
//		}
//
//		int wheels = std::min(parents[0].wheels.size(), parents[1].wheels.size());
//
//		if ((float)(rand() % 1000) / 1000 < mutation)
//			wheels += (rand() % 2) - 1;
//
//		if (wheels <= 1)
//			wheels = 1;
//
//		for (int j = 0; j < wheels; j++)
//		{
//			WheelData wheel;
//
//			if (j < parents[0].wheels.size() && j < parents[1].wheels.size())
//			{
//				wheel.motorSpeed = parents[rand() % 2].wheels[rand() % wheels].motorSpeed;
//				wheel.radius = parents[rand() % 2].wheels[rand() % wheels].radius;
//				wheel.vertex = parents[rand() % 2].wheels[rand() % wheels].vertex;
//				wheel.wheelDensity = parents[rand() % 2].wheels[rand() % wheels].wheelDensity;
//				wheel.wheelFriction = parents[rand() % 2].wheels[rand() % wheels].wheelFriction;
//				wheel.wheelRestitution = parents[rand() % 2].wheels[rand() % wheels].wheelRestitution;
//			}
//			else
//			{
//				wheel.radius = ((float)(rand() % 1000) / 1000) * (2 - 1) + 1;
//				wheel.wheelDensity = 10.0f;
//				wheel.wheelFriction = 0.5f;
//				wheel.wheelRestitution = 0.3f;
//				wheel.motorSpeed = ((float)(rand() % 1000) / 1000) * -50;
//				wheel.vertex = 7 - j;
//			}
//
//			if ((float)(rand() % 1000) / 1000 < mutation)
//				wheel.motorSpeed += +((float)(rand() % 1000 - 500) / 500);
//
//			if ((float)(rand() % 1000) / 1000 < mutation)
//				wheel.radius += +((float)(rand() % 1000 - 500) / 500);
//
//			if ((float)(rand() % 1000) / 1000 < mutation)
//				wheel.wheelDensity += +((float)(rand() % 1000 - 500) / 500);
//
//			if ((float)(rand() % 1000) / 1000 < mutation)
//				wheel.wheelFriction += +((float)(rand() % 1000 - 500) / 500);
//
//			if ((float)(rand() % 1000) / 1000 < mutation)
//				wheel.wheelRestitution += +((float)(rand() % 1000 - 500) / 500);
//
//			data.wheels.push_back(wheel);
//		}
//
//		data.chassisDensity = parents[rand() % 2].chassisDensity;
//		data.chassisFriction = parents[rand() % 2].chassisFriction;
//		data.chassisRestitution = parents[rand() % 2].chassisRestitution;
//
//		if ((float)(rand() % 1000) / 1000 < mutation)
//			data.chassisDensity += +((float)(rand() % 1000 - 500) / 500);
//
//		if ((float)(rand() % 1000) / 1000 < mutation)
//			data.chassisFriction += +((float)(rand() % 1000 - 500) / 500);
//
//		if ((float)(rand() % 1000) / 1000 < mutation)
//			data.chassisRestitution += +((float)(rand() % 1000 - 500) / 500);
//	
//		cars_.push_back(new Car(*world_, data));
//	}
//
//	printf("----------------------- Generation: %d, Average Fitness: %f -------------------------\n", generationCount_++, average_fitness);
//	printf("--- Best Car, Fitness: %f ---\n", best_fitness);
//	printf("--- Wheels: %d\n", best_car_data.wheels.size());
//	for (int i = 0; i < best_car_data.wheels.size(); i++)
//	{
//		printf("  --- Wheel %d ---\n", i + 1);
//		printf("    --- Motor Speed: %0.3f\n", best_car_data.wheels[i].motorSpeed);
//		printf("    --- Radius: %0.3f\n", best_car_data.wheels[i].radius);
//		printf("    --- Density: %0.3f\n", best_car_data.wheels[i].wheelDensity);
//		printf("    --- Friction: %0.3f\n", best_car_data.wheels[i].wheelFriction);
//		printf("    --- Restitution: %0.3f\n", best_car_data.wheels[i].wheelRestitution);
//	}
//	printf("--- Chassis Density: %0.3f\n", best_car_data.chassisDensity);
//	printf("--- Chassis Friction: %0.3f\n", best_car_data.chassisFriction);
//	printf("--- Chassis Restitution: %0.3f\n", best_car_data.chassisRestitution);
//}

//void Generation::drawImGui()
//{
//	ImGui::Begin("Generation");
//
//	// ImGui::SliderFloat("Time Scale", &mTimeScale, 0.1f, 20.0f);
//
//	ImGui::Spacing();
//
//	if (!ImGui::CollapsingHeader("Camera"))
//	{
//		ImGui::BeginChild("CameraControls", { 0, 100 }, true, ImGuiWindowFlags_AlwaysAutoResize);
//
//		// ImGui::DragFloat2("Position", &mCamPos.x, 0.5f);
//		// ImGui::DragFloat("Zoom", &mCamZoom, 0.0001f, 0.0001f, 0.5f);
//
//		ImGui::EndChild();
//	}
//
//	/*if (!ImGui::CollapsingHeader("Current Car") && mCurrentCar)
//	{
//			CarData best_car_data = mCurrentCar->get_car_data();
//
//			ImGui::Text("--- Wheels: %d\n", best_car_data.wheels.size());
//			for (int i = 0; i < best_car_data.wheels.size(); i++)
//			{
//				ImGui::Text("  --- Wheel %d ---\n", i + 1);
//				ImGui::Text("    --- Motor Speed: %0.3f\n", best_car_data.wheels[i].motorSpeed);
//				ImGui::Text("    --- Radius: %0.3f\n", best_car_data.wheels[i].radius);
//				ImGui::Text("    --- Density: %0.3f\n", best_car_data.wheels[i].wheelDensity);
//				ImGui::Text("    --- Friction: %0.3f\n", best_car_data.wheels[i].wheelFriction);
//				ImGui::Text("    --- Restitution: %0.3f\n", best_car_data.wheels[i].wheelRestitution);
//			}
//			ImGui::Text("--- Chassis Density: %0.3f\n", best_car_data.chassisDensity);
//			ImGui::Text("--- Chassis Friction: %0.3f\n", best_car_data.chassisFriction);
//			ImGui::Text("--- Chassis Restitution: %0.3f\n", best_car_data.chassisRestitution);
//	}*/
//
//	ImGui::End();
//}

void Generation::update(float delta)
{
	static float timer = 0;
	timer += delta;

	world_->Step(delta, 8, 6);

	int deadCount = 0;

	if (deadCount == cars_.size())
		nextGeneration();
}

void Generation::draw(Renderer& renderer)
{
	for (auto& car : cars_)
		car->draw(renderer);

	platform_->draw(renderer);
}
