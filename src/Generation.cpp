#include "Generation.h"

#include "Log.h"
#include "Random.h"

#include <iostream>
#include <map>
#include <random>
#include <algorithm>
#include <iterator>

#include <box2d/box2d.h>

template <typename T>
static T &mutate(T &value, T amount, T min, T max)
{
	value += amount;

	if (value < min)
		value = min;
	if (value > max)
		value = max;

	return value;
}

template <typename T>
static T &offset(T &value, T amount)
{
	value += amount;

	return value;
}

Generation::Generation(int cars)
	: m_Simulating(false)
{
	m_World = std::make_unique<b2World>(b2Vec2{0.0f, -10.0f});
	m_Platform = std::make_unique<Platform>();
	m_Cars.resize(cars);

	// m_Draw.SetFlags( b2Draw::e_shapeBit );
	// m_World->SetDebugDraw(&m_Draw);
}

Generation::~Generation()
{
}

void Generation::init()
{
	m_Platform->init(*m_World, 500);

	for (auto &car : m_Cars)
	{
		car.init(*m_World, Car::randomCarData());
	}
}

void Generation::beginSimulating()
{
	if (m_Simulating)
	{
		BL_LOG_WARN("Already simulating a generation.");
		return;
	}

	m_Simulating = true;
}

void Generation::nextGeneration()
{
	m_Simulating = false;

	int numCars = m_Cars.size();
	int numParents = 8;
	int numVertices = 8;

	BL_ASSERT(numCars > 0, "We must have at least one car to perform natural selection.");

	// Sort by fitness

	std::sort(m_Cars.begin(), m_Cars.end(), [](const Car &car1, const Car &car2)
			  { return car1.getFitness() < car2.getFitness(); });

	// Select parents

	BL_LOG_INFO("Selecting parents!");

	std::vector<int> cumulativeFitness;

	cumulativeFitness.push_back(m_Cars[0].getFitness());

	for (int i = 1; i < numCars; i++)
	{
		cumulativeFitness.push_back(cumulativeFitness[i - 1] + m_Cars[i].getFitness());
	}

	int totalFitness = cumulativeFitness.back();

	std::vector<CarData> parentsCarData;

	for (int i = 0; i < numParents; i++)
	{
		int fitness = totalFitness > 0 ? Random::UInt(0, totalFitness) : 0;

		auto upper = std::upper_bound(cumulativeFitness.begin(), cumulativeFitness.end(), fitness);
		int index = upper == cumulativeFitness.end() ? 0 : std::distance(cumulativeFitness.begin(), upper);

		BL_ASSERT(index >= 0 && index < numCars, "Invalid index (", index, ") selected for parent!");

		parentsCarData.push_back(m_Cars[index].getCarData());
	}

	BL_LOG_INFO("Crossing parents!");

	// Cross parents

	for (int i = 0; i < numCars; i++)
	{
		CarData parentCarData1 = parentsCarData[Random::UInt(0, numParents)];
		CarData parentCarData2 = parentsCarData[Random::UInt(0, numParents)];
		CarData newCarData;

		// Density
		{
			if (Random::Bool())
				newCarData.Density = parentCarData1.Density;
			else
				newCarData.Density = parentCarData2.Density;
		}

		if (Random::Bool())
			mutate(newCarData.Density, Random::Float() / 5.0f, 0.1f, 1.0f);

		// Friction
		{
			if (Random::Bool())
				newCarData.Friction = parentCarData1.Friction;
			else
				newCarData.Friction = parentCarData2.Friction;
		}

		if (Random::Bool())
			mutate(newCarData.Friction, Random::Float() / 5.0f, 0.1f, 1.0f);

		// Restitution
		{
			if (Random::Bool())
				newCarData.Restitution = parentCarData1.Restitution;
			else
				newCarData.Restitution = parentCarData2.Restitution;
		}

		if (Random::Bool())
			mutate(newCarData.Restitution, Random::Float() / 5.0f, 0.1f, 1.0f);

		// Vertices
		{
			for (int j = 0; j < numVertices; j++)
			{
				if (Random::Bool())
					newCarData.Vertices.push_back(parentCarData1.Vertices[j]);
				else
					newCarData.Vertices.push_back(parentCarData2.Vertices[j]);

				if (Random::Bool())
					offset(newCarData.Vertices[j].x, Random::Float() / 10.0f);

				if (Random::Bool())
					offset(newCarData.Vertices[j].y, Random::Float() / 10.0f);
			}
		}

		// Wheels
		{
			int numWheels = std::min(parentCarData1.Wheels.size(), parentCarData2.Wheels.size());

			for (int j = 0; j < numWheels; j++)
			{
				WheelData newWheelData;

				// Density
				{
					if (Random::Bool())
						newWheelData.Density = parentCarData1.Wheels[j].Density;
					else
						newWheelData.Density = parentCarData2.Wheels[j].Density;

					if (Random::Bool())
						mutate(newWheelData.Density, Random::Float() / 5.0f, 0.1f, 1.0f);
				}

				// Friction
				{
					if (Random::Bool())
						newWheelData.Friction = parentCarData1.Wheels[j].Friction;
					else
						newWheelData.Friction = parentCarData2.Wheels[j].Friction;

					if (Random::Bool())
						mutate(newWheelData.Friction, Random::Float() / 5.0f, 0.1f, 1.0f);
				}

				// Restitution
				{
					if (Random::Bool())
						newWheelData.Restitution = parentCarData1.Wheels[j].Restitution;
					else
						newWheelData.Restitution = parentCarData2.Wheels[j].Restitution;

					if (Random::Bool())
						mutate(newWheelData.Restitution, Random::Float() / 5.0f, 0.1f, 1.0f);
				}

				// Radius
				{
					if (Random::Bool())
						newWheelData.Radius = parentCarData1.Wheels[j].Radius;
					else
						newWheelData.Radius = parentCarData2.Wheels[j].Radius;

					if (Random::Bool())
						mutate(newWheelData.Radius, Random::Float() / 5.0f, MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS);
				}

				// Motor Speed
				{
					if (Random::Bool())
						newWheelData.MotorSpeed = parentCarData1.Wheels[j].MotorSpeed;
					else
						newWheelData.MotorSpeed = parentCarData2.Wheels[j].MotorSpeed;

					if (Random::Bool())
						mutate(newWheelData.MotorSpeed, Random::Float(), -MAX_WHEEL_MOTOR_SPEED, -MIN_WHEEL_MOTOR_SPEED);
				}

				// Vertex
				{
					if (Random::Bool())
						newWheelData.Vertex = parentCarData1.Wheels[j].Vertex;
					else
						newWheelData.Vertex = parentCarData2.Wheels[j].Vertex;

					if (Random::Bool())
						mutate(newWheelData.Vertex, Random::Int(-1, 1), 0, numVertices - 1);
				}

				newCarData.Wheels.push_back(newWheelData);
			}

			if (Random::Bool())
			{
				int extraWheels = Random::Int(0, 2);

				for (int j = 0; j < extraWheels; j++)
				{
					WheelData wheelData;

					wheelData.Density = Random::Float() * (5.f - 1.f) + 1.f;
					wheelData.Friction = Random::Float();
					wheelData.Restitution = Random::Float();
					wheelData.Radius = Random::Float() * (MAX_WHEEL_RADIUS - MIN_WHEEL_RADIUS) + MIN_WHEEL_RADIUS;
					wheelData.MotorSpeed = -Random::Float() * (MAX_WHEEL_MOTOR_SPEED - MIN_WHEEL_MOTOR_SPEED) + MIN_WHEEL_MOTOR_SPEED;
					wheelData.Vertex = 7 - i;

					newCarData.Wheels.push_back(wheelData);
				}
			}

			m_Cars[i].init(*m_World, newCarData);
		}

		m_Simulating = true;
	}
}

glm::vec3 Generation::getPositionOfBestCar()
{
	if (m_Cars.empty())
	{
		return {0.0f, 0.0f, 0.0f};
	}

	b2Vec2 pos = m_Cars.front().getPosition();

	glm::vec3 best = {pos.x, pos.y, 0.0f};

	for (const auto &car : m_Cars)
	{
		pos = car.getPosition();

		if (pos.x > best.x && !car.isDead())
		{
			best.x = pos.x;
			best.y = pos.y;
		}
	}

	return best;
}

// void Generation::drawImGui()
// {
// 	ImGui::Begin("Generation");

// 	// ImGui::SliderFloat("Time Scale", &mTimeScale, 0.1f, 20.0f);

// 	ImGui::Spacing();

// 	if (!ImGui::CollapsingHeader("Camera"))
// 	{
// 		ImGui::BeginChild("CameraControls", {0, 100}, true, ImGuiWindowFlags_AlwaysAutoResize);

// 		// ImGui::DragFloat2("Position", &mCamPos.x, 0.5f);
// 		// ImGui::DragFloat("Zoom", &mCamZoom, 0.0001f, 0.0001f, 0.5f);

// 		ImGui::EndChild();
// 	}

// 	/*if (!ImGui::CollapsingHeader("Current Car") && mCurrentCar)
// 	{
// 			CarData best_car_data = mCurrentCar->get_car_data();

// 			ImGui::Text("--- Wheels: %d\n", best_car_data.wheels.size());
// 			for (int i = 0; i < best_car_data.wheels.size(); i++)
// 			{
// 				ImGui::Text("  --- Wheel %d ---\n", i + 1);
// 				ImGui::Text("    --- Motor Speed: %0.3f\n", best_car_data.wheels[i].motorSpeed);
// 				ImGui::Text("    --- Radius: %0.3f\n", best_car_data.wheels[i].radius);
// 				ImGui::Text("    --- Density: %0.3f\n", best_car_data.wheels[i].wheelDensity);
// 				ImGui::Text("    --- Friction: %0.3f\n", best_car_data.wheels[i].wheelFriction);
// 				ImGui::Text("    --- Restitution: %0.3f\n", best_car_data.wheels[i].wheelRestitution);
// 			}
// 			ImGui::Text("--- Chassis Density: %0.3f\n", best_car_data.chassisDensity);
// 			ImGui::Text("--- Chassis Friction: %0.3f\n", best_car_data.chassisFriction);
// 			ImGui::Text("--- Chassis Restitution: %0.3f\n", best_car_data.chassisRestitution);
// 	}*/

// 	ImGui::End();
// }

void Generation::update(float delta)
{
	static float timer = 0.0f;

	if (m_Simulating)
	{
		m_World->Step(delta, 8, 6);

		int deadCount = 0;

		for (auto &car : m_Cars)
		{
			car.update(delta);

			if (car.isDead())
				deadCount++;
		}

		if (deadCount == m_Cars.size())
			nextGeneration();
	}

	timer += delta;
}

void Generation::draw(Renderer &renderer)
{
	for (const auto &car : m_Cars)
	{
		car.draw(renderer);
	}

	m_Platform->draw(renderer);
}
