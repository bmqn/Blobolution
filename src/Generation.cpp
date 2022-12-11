#include "Generation.h"
#include "Random.h"
#include "Log.h"

#include <box2d/box2d.h>

template <typename T, typename U>
static T& mutate(T &value, U amount, T min, T max)
{
	value += amount;

	if (value < min)
		value = min;
	if (value > max)
		value = max;

	return value;
}

template <typename T>
static T& offset(T &value, T amount)
{
	value += amount;

	return value;
}

Generation::Generation()
	: m_World(nullptr)
	, m_Platform(nullptr)
	, m_Cars(0)
{
}

Generation::~Generation()
{
	if (!m_World)
	{
		return;
	}

	m_Platform->Destory();
	for (auto &car : m_Cars)
	{
		car->Destory();
	}
}

void Generation::Create(int numCars)
{
	m_World = std::make_unique<b2World>(b2Vec2{ 0.0f, -10.0f });

	m_Platform = std::make_unique<Platform>();
	m_Platform->Create(*m_World, 1024);
	
	m_Cars.resize(numCars);
	for (auto &car : m_Cars)
	{
		car = std::make_unique<Car>();
		car->Create(*m_World, Car::RandomProto());
	}
}

void Generation::Update(float delta)
{
	if (!m_World)
	{
		return;
	}
	
	m_World->Step(delta, 6, 2);
	int deadCount = 0;
	for (auto &car : m_Cars)
	{
		car->Update(delta);
		if (car->IsDead())
		{
			deadCount++;
		}
	}
	if (deadCount == m_Cars.size())
	{
		NextGeneration();
	}
}

void Generation::Draw() const
{
	if (!m_World)
	{
		return;
	}

	m_Platform->Draw();
	for (const auto &car : m_Cars)
	{
		car->Draw();
	}
}

const Car *Generation::GetBestCar() const
{
	if (!m_World)
	{
		return nullptr;
	}

	const Car *bestCar = nullptr;
	glm::vec3 bestPosition = { 0.0, 0.0, 0.0f };
	for (const auto& car : m_Cars)
	{
		b2Vec2 pos = car->GetPosition();

		if (pos.x > bestPosition.x && !car->IsDead())
		{
			bestPosition.x = pos.x;
			bestPosition.y = pos.y;
			bestCar = car.get();
		}
	}
	return bestCar;
}

void Generation::NextGeneration()
{
	if (!m_World)
	{
		return;
	}

	BL_LOG("Starting to create next generation");

	using RatioRange       = std::pair<double, double>;
	using CarProtoRoulette = std::vector<std::pair<RatioRange, CarProto>>;
	using ProtoList        = std::vector<CarProto>;

	BL_LOG("Ranking parents");

	CarProtoRoulette carProtosRoulette;
	ProtoList        parentProtos;

	size_t numCars    = m_Cars.size();
	size_t numParents = numCars / 2;

	float totalFitness = static_cast<float>(
		std::accumulate(m_Cars.begin(), m_Cars.end(), 0_zu, [](size_t i, const auto &car)
		{
			return i + car->GetFitness();
		})
	);
	
	{
		float cumlRatio = 0.;
		for (auto curr = m_Cars.cbegin(); curr != m_Cars.cend(); ++curr)
		{
			float ratio = static_cast<float>((*curr)->GetFitness()) / totalFitness;
			cumlRatio += ratio;

			carProtosRoulette.push_back({ {cumlRatio - ratio, cumlRatio}, (*curr)->GetProto() });
		}
	}

	for (int i = 0; i < numParents; i++)
	{
		float ratio = Random::Float(0.0f, 1.0f);

		for (const auto& segment : carProtosRoulette)
		{
			if (ratio >= segment.first.first && ratio <= segment.first.second)
			{
				parentProtos.push_back(segment.second);
				break;
			}
		}
		
	}

	BL_LOG("Crossing parents");

	for (auto& car : m_Cars)
	{
		CarProto newCarData;

		const CarProto& parentCarData1 = parentProtos[Random::Int(0_zu, numParents - 1)];
		const CarProto& parentCarData2 = parentProtos[Random::Int(0_zu, numParents - 1)];

		// Density
		{
			float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
			                 / std::log(1000.0f + 1.0f);

			if (Random::Bool())
			{
				mixRatio = 1.0f - mixRatio;
			}

			newCarData.Density =   parentCarData1.Density * mixRatio
			                     + parentCarData2.Density * (1.0f - mixRatio);

			if (Random::Bool())
			{
				offset(newCarData.Density, Random::Float(0.5f, 2.0f));
			}
		}

		// Friction
		{
			float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
			                 / std::log(1000.0f + 1.0f);

			if (Random::Bool())
			{
				mixRatio = 1.0f - mixRatio;
			}

			newCarData.Friction =   parentCarData1.Friction * mixRatio
			                      + parentCarData2.Friction * (1.0f - mixRatio);

			if (Random::Bool())
			{
				mutate(newCarData.Friction, Random::Float(0.05f, 0.3f), 0.1f, 1.0f);
			}
		}

		// Restitution
		{
			float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
			                 / std::log(1000.0f + 1.0f);

			if (Random::Bool())
			{
				mixRatio = 1.0f - mixRatio;
			}

			newCarData.Restitution =   parentCarData1.Restitution * mixRatio
			                         + parentCarData2.Restitution * (1.0f - mixRatio);

			if (Random::Bool())
			{
				mutate(newCarData.Restitution, Random::Float(0.5f, 2.0f), 0.1f, 1.0f);
			}
		}

		// Colour
		{
			float r =  (newCarData.Density               - CarConstants::kMinChassisDensity)
			         / (CarConstants::kMaxChassisDensity - CarConstants::kMinChassisDensity);
			float g = newCarData.Friction;
			float b = newCarData.Restitution;
			newCarData.Colour = {r, g, b, 1.0f };
		}

		// Vertices
		{
			for (int j = 0; j < CarConstants::kNumVertices; j++)
			{
				if (Random::Bool())
				{
					newCarData.Vertices[j] = parentCarData1.Vertices[j];
				}
				else
				{
					newCarData.Vertices[j] = parentCarData2.Vertices[j];
				}

				if (Random::Bool())
				{
					offset(newCarData.Vertices[j].x, Random::Float(0.5f, 2.0f));
					offset(newCarData.Vertices[j].y, Random::Float(0.5f, 2.0f));
				}
			}
		}

		// Wheels
		{
			newCarData.Wheels.resize(Random::Bool() ? parentCarData1.Wheels.size() :
			                                          parentCarData2.Wheels.size()
			);

			size_t maxIndex = std::min(parentCarData1.Wheels.size(),
			                           parentCarData2.Wheels.size()
			);
			maxIndex = maxIndex == 0 ? 0 : maxIndex - 1;

			BL_ASSERT(newCarData.Wheels.size() > 0, "A car has no wheels...");

			size_t i = 0;
			for (auto it = newCarData.Wheels.begin(); it != newCarData.Wheels.end(); )
			{
				{
					auto& wheel = *it;

					// Density
					{
						if (   i >= parentCarData1.Wheels.size()
						    || i >= parentCarData2.Wheels.size())
						{
							if (Random::Bool())
							{
								wheel.Density = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].Density;
							}
							else
							{
								wheel.Density = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].Density;
							}
						}
						else
						{
							float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
							                 / std::log(1000.0f + 1.0f);

							if (Random::Bool())
							{
								mixRatio = 1.0f - mixRatio;
							}

							wheel.Density =   parentCarData1.Wheels[i].Density * mixRatio
							                + parentCarData2.Wheels[i].Density * (1.0f - mixRatio);
						}

						if (Random::Bool())
						{
							offset(wheel.Density, Random::Float(0.5f, 2.0f));
						}
					}

					// Friction
					{
						if (   i >= parentCarData1.Wheels.size()
						    || i >= parentCarData2.Wheels.size())
						{
							if (Random::Bool())
							{
								wheel.Friction = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].Friction;
							}
							else
							{
								wheel.Friction = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].Friction;
							}
						}
						else
						{
							float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
							                 / std::log(1000.0f + 1.0f);

							if (Random::Bool())
							{
								mixRatio = 1.0f - mixRatio;
							}

							wheel.Friction =   parentCarData1.Wheels[i].Friction * mixRatio
							                 + parentCarData2.Wheels[i].Friction * (1.0f - mixRatio);
						}

						if (Random::Bool())
						{
							mutate(wheel.Friction, Random::Float(0.5f, 2.0f), 0.1f, 1.0f);
						}
					}

					// Restitution
					{
						if (   i >= parentCarData1.Wheels.size()
						    || i >= parentCarData2.Wheels.size())
						{
							if (Random::Bool())
							{
								wheel.Restitution = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].Restitution;
							}
							else
							{
								wheel.Restitution = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].Restitution;
							}
						}
						else
						{
							float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
							                 / std::log(1000.0f + 1.0f);

							if (Random::Bool())
							{
								mixRatio = 1.0f - mixRatio;
							}

							wheel.Restitution =   parentCarData1.Wheels[i].Restitution * mixRatio
							                    + parentCarData2.Wheels[i].Restitution * (1.0f - mixRatio);
						}

						if (Random::Bool())
						{
							mutate(wheel.Restitution, Random::Float(0.5f, 2.0f), 0.1f, 1.0f);
						}
					}

					// Radius
					{
						if (   i >= parentCarData1.Wheels.size()
						    || i >= parentCarData2.Wheels.size())
						{
							if (Random::Bool())
							{
								wheel.Radius = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].Radius;
							}
							else
							{
								wheel.Radius = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].Radius;
							}
						}
						else
						{
							float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
							                 / std::log(1000.0f + 1.0f);

							if (Random::Bool())
							{
								mixRatio = 1.0f - mixRatio;
							}

							wheel.Radius =   parentCarData1.Wheels[i].Radius * mixRatio
							               + parentCarData2.Wheels[i].Radius * (1.0f - mixRatio);
						}

						if (Random::Bool())
						{
							mutate(
								wheel.Radius, Random::Float(0.5f, 2.0f),
								CarConstants::kMinWheelRadius, CarConstants::kMaxWheelRadius
							);
						}
					}

					// Motor Speed
					{
						if (   i >= parentCarData1.Wheels.size()
						    || i >= parentCarData2.Wheels.size())
						{
							if (Random::Bool())
							{
								wheel.MotorSpeed = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].MotorSpeed;
							}
							else
							{
								wheel.MotorSpeed = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].MotorSpeed;
							}
						}
						else
						{
							float mixRatio =   std::log(Random::Float(0.0f, 1000.0f) + 1.0f)
							                 / std::log(1000.0f + 1.0f);

							if (Random::Bool())
							{
								mixRatio = 1.0f - mixRatio;
							}

							wheel.MotorSpeed =   parentCarData1.Wheels[i].MotorSpeed * mixRatio
							                   + parentCarData2.Wheels[i].MotorSpeed * (1.0f - mixRatio);
						}

						if (Random::Bool())
						{
							mutate(
								wheel.MotorSpeed, Random::Float(0.5f, 2.0f),
								-CarConstants::kMaxWheelMotorSpeed, -CarConstants::kMinWheelMotorSpeed
							);
						}
					}

					// Vertex
					{
						if (Random::Bool())
						{
							wheel.Vertex = parentCarData1.Wheels[Random::Int(0_zu, maxIndex)].Vertex;
						}
						else
						{
							wheel.Vertex = parentCarData2.Wheels[Random::Int(0_zu, maxIndex)].Vertex;
						}

						if (Random::Bool())
						{
							mutate(
								wheel.Vertex, Random::Int(-1, 1), 0_zu,
								CarConstants::kNumVertices - 1_zu
							);
						}
					}

					// Colour
					{
						float r =  (wheel.Density                  - CarConstants::kMinWheelDensity)
						         / (CarConstants::kMaxWheelDensity - CarConstants::kMinWheelDensity);
						float g = wheel.Friction;
						float b = wheel.Restitution;
						wheel.Colour = {r, g, b, 1.0f };
					}

					++it;
				}

				++i;
			}

			car->Destory();
			car->Create(*m_World, newCarData);
		}
	}

	BL_LOG("Finished creating next generation");
}
