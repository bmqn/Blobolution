#pragma once

#include "Event.h"

class Layer
{

public:
	
	virtual void onUpdate(float delta) = 0;

	virtual void onEvent(Event& e) = 0;
};

