#include "SimLayer.h"

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

bool SimLayer::onMouseMoved(MouseMovedEvent& e)
{
	if (mousePressed_ && !followCam_)
	{
		camPosition_ -= glm::vec3{ e.xpos_ - xpos_, ypos_ - e.ypos_, 0.0f } * (0.05f / camScale_);
	}

	xpos_ = e.xpos_;
	ypos_ = e.ypos_;

	return false;
}

bool SimLayer::onMousePressed(MousePressedEvent& e)
{
	if (e.button_ == GLFW_MOUSE_BUTTON_1)
	{
		if (e.action_ == GLFW_PRESS)
			mousePressed_ = true;
		else
			mousePressed_ = false;
	}

	return false;
}

bool SimLayer::onMouseScrolled(MouseScrolledEvent& e)
{
	camScale_ += e.yoffset_ * (0.3f * camScale_);

	if (camScale_ < 0.01f)
		camScale_ = 0.01f;
	if (camScale_ > 2.0f)
		camScale_ = 2.0f;

	return false;
}

SimLayer::SimLayer() :
	gen_(10), renderer_(), camPosition_(0, 0, 0), camScale_(1.0f), timeScale_(1.0f), xpos_(0.0), ypos_(0.0), mousePressed_(false), followCam_(true)
{

}

SimLayer::~SimLayer()
{
}

void SimLayer::onUpdate(float delta)
{
	gen_.update(delta * timeScale_);

	if (followCam_)
		camPosition_ = gen_.getPositionOfBestCar();

	float asp = 16.0f / 9.0f;

	auto viewProj =
		glm::ortho(-10.0f * asp, 10.0f * asp, -10.0f, 10.0f)
		* glm::scale(glm::mat4(1.0f), glm::vec3(camScale_))
		* glm::translate(glm::mat4(1.0f), -camPosition_);

	renderer_.clear();
	renderer_.beginScene(viewProj);

	gen_.draw(renderer_);

	renderer_.endScene();

	auto stats = renderer_.getRendererStats();

	// ImGui_ImplOpenGL3_NewFrame();
	// ImGui_ImplGlfw_NewFrame();
	// ImGui::NewFrame();

	// ImGui::Begin("Options");

	// ImGui::BeginChild("Infomation", ImVec2(0, 80));
	// ImGui::Text("Delta (ms) %f", delta);
	// ImGui::Text("Render Submissions %d", stats.rendererSubmissions);
	// ImGui::Text("Render Vertices %d", stats.rendererVertices);
	// ImGui::Text("Render Draws %d", stats.rendererDrawCalls);
	// ImGui::EndChild();

	// ImGui::Separator();

	// ImGui::BeginChild("Settings", ImVec2(0, 60));
	// ImGui::SliderFloat("Time Scale", &timeScale_, 0.1f, 5.0f);
	// ImGui::Checkbox("Follow Camera", &followCam_);
	// ImGui::EndChild();

	// ImGui::End();

	// ImGui::Render();
	// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SimLayer::onEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseMovedEvent>(std::bind(&SimLayer::onMouseMoved, this, std::placeholders::_1));
	dispatcher.dispatch<MouseScrolledEvent>(std::bind(&SimLayer::onMouseScrolled, this, std::placeholders::_1));
	dispatcher.dispatch<MousePressedEvent>(std::bind(&SimLayer::onMousePressed, this, std::placeholders::_1));
}
