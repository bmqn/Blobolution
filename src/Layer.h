#pragma once

#include "Event.h"

class Layer
{
public:
	virtual ~Layer() {}

	virtual void OnUpdate(float delta) = 0;
	virtual void OnDrawImGui() = 0;
	virtual void OnEvent(Event &e) = 0;

	const std::string &GetName() const { return m_Name; }

protected:
	Layer(const std::string &name) : m_Name(name) {}

protected:
	std::string m_Name;
};
