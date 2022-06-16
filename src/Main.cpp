#include "Application.h"
#include "SimLayer.h"

int main()
{
	auto &app = Application::Get();

	app.PushLayer(std::make_unique<SimLayer>());
	app.Run();

	return 0;
}
