#pragma once

#include <functional>

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	KeyPressed, KeyReleased, KeyTyped,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

class Event
{
public:
	virtual EventType getEventType() const = 0;
public:
	bool handled_ = false;
};

class EventDispatcher
{
public:
	EventDispatcher(Event& e) : event_(e) {}

	template<typename T>
	bool dispatch(const std::function<bool(T& e)>& func)
	{
		if (event_.getEventType() == T::getStaticEvent())
		{
			event_.handled_ = func(static_cast<T&>(event_));
			return true;
		}
		return false;
	}

private:
	Event& event_;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;
	virtual EventType getEventType() const override { return EventType::WindowClose;  }
	static EventType getStaticEvent() { return EventType::WindowClose; }
};

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(int width, int height) : width_(width), height_(height) { }
	virtual EventType getEventType() const override { return EventType::WindowResize; }
	static EventType getStaticEvent() { return EventType::WindowResize; }

public:
	int width_, height_;
};

class KeyPressedEvent : public Event
{
public:
	KeyPressedEvent(int key, int mods) : key_(key), mods_(mods) { }
	virtual EventType getEventType() const override { return EventType::KeyPressed; }
	static EventType getStaticEvent() { return EventType::KeyPressed; }

public:
	int key_, mods_;
};

class MousePressedEvent : public Event
{
public:
	MousePressedEvent(int button, int action, int mods) : button_(button), action_(action), mods_(mods) { }
	virtual EventType getEventType() const override { return EventType::MouseButtonPressed; }
	static EventType getStaticEvent() { return EventType::MouseButtonPressed; }

public:
	int button_, action_, mods_;
};

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(double xpos, double ypos) : xpos_(xpos), ypos_(ypos) { }
	virtual EventType getEventType() const override { return EventType::MouseMoved; }
	static EventType getStaticEvent() { return EventType::MouseMoved; }

public:
	double xpos_, ypos_;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(double xoffset, double yoffset) : xoffset_(xoffset), yoffset_(yoffset) { }
	virtual EventType getEventType() const override { return EventType::MouseScrolled; }
	static EventType getStaticEvent() { return EventType::MouseScrolled; }

public:
	double xoffset_, yoffset_;
};

