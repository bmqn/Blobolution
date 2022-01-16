#pragma once

#include <functional>

enum class EventType
{
	None = 0,
	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,
	KeyPressed,
	KeyReleased,
	KeyTyped,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled
};

class Event
{
public:
	virtual EventType getEventType() const = 0;

public:
	bool Handled = false;
};

class EventDispatcher
{
public:
	EventDispatcher(Event &e) : m_Event(e) {}

	template <typename T>
	bool dispatch(const std::function<bool(T &e)> &func)
	{
		if (m_Event.getEventType() == T::getStaticEvent())
		{
			m_Event.Handled = func(static_cast<T &>(m_Event));
			return true;
		}
		return false;
	}

private:
	Event &m_Event;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() = default;
	virtual EventType getEventType() const override { return EventType::WindowClose; }
	static EventType getStaticEvent() { return EventType::WindowClose; }
};

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(int width, int height) : Width(width), Height(height) {}
	virtual EventType getEventType() const override { return EventType::WindowResize; }
	static EventType getStaticEvent() { return EventType::WindowResize; }

public:
	int Width, Height;
};

class KeyPressedEvent : public Event
{
public:
	KeyPressedEvent(int key, int mods) : Key(key), Mods(mods) {}
	virtual EventType getEventType() const override { return EventType::KeyPressed; }
	static EventType getStaticEvent() { return EventType::KeyPressed; }

public:
	int Key, Mods;
};

class MousePressedEvent : public Event
{
public:
	MousePressedEvent(int button, int action, int mods) : Button(button), Action(action), Mods(mods) {}
	virtual EventType getEventType() const override { return EventType::MouseButtonPressed; }
	static EventType getStaticEvent() { return EventType::MouseButtonPressed; }

public:
	int Button, Action, Mods;
};

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(double xpos, double ypos) : Xpos(xpos), Ypos(ypos) {}
	virtual EventType getEventType() const override { return EventType::MouseMoved; }
	static EventType getStaticEvent() { return EventType::MouseMoved; }

public:
	double Xpos, Ypos;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(double xoffset, double yoffset) : Xoffset(xoffset), Yoffset(yoffset) {}
	virtual EventType getEventType() const override { return EventType::MouseScrolled; }
	static EventType getStaticEvent() { return EventType::MouseScrolled; }

public:
	double Xoffset, Yoffset;
};
