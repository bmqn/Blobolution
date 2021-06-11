#include <iostream>

#include "Application.h"

#include "SimLayer.h"

int main()
{
	Application app (1280, 720, "Blobolution");
	app.pushLayer(std::make_shared<SimLayer>());
	app.run();

	return 0;
}
