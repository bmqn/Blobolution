#include "Application.h"
#include "SimLayer.h"

#include <memory>

int main()
{
	auto& app = Application::get();
	
	app.pushLayer(std::make_shared<SimLayer>());
	app.run();

	return 0;
}
