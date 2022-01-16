#pragma once

#include "Event.h"

#include <string>

class Layer
{
public:
	virtual void onUpdate(float delta) = 0;
	virtual void onEvent(Event &e) = 0;

	const std::string& getName() const { return m_Name; }

protected:
	Layer(const std::string& name) : m_Name(name) {}

protected:
	std::string m_Name;
};
