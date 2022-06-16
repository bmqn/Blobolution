#pragma once

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
	KeyRepeat,
	KeyTyped,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled
};

class Event
{
public:
	virtual EventType GetType() const = 0;

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
		if (m_Event.GetType() == T::GetStaticType())
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
	virtual EventType GetType() const override { return EventType::WindowClose; }
	static EventType GetStaticType() { return EventType::WindowClose; }
};

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(int width, int height) : Width(width), Height(height) {}
	virtual EventType GetType() const override { return EventType::WindowResize; }
	static EventType GetStaticType() { return EventType::WindowResize; }

public:
	int Width, Height;
};

class KeyReleasedEvent : public Event
{
public:
	KeyReleasedEvent(int key, int mods) : Key(key), Mods(mods) {}
	virtual EventType GetType() const override { return EventType::KeyReleased; }
	static EventType GetStaticType() { return EventType::KeyReleased; }

public:
	int Key, Mods;
};

class KeyPressedEvent : public Event
{
public:
	KeyPressedEvent(int key, int mods) : Key(key), Mods(mods) {}
	virtual EventType GetType() const override { return EventType::KeyPressed; }
	static EventType GetStaticType() { return EventType::KeyPressed; }

public:
	int Key, Mods;
};

class KeyRepeatEvent : public Event
{
public:
	KeyRepeatEvent(int key, int mods) : Key(key), Mods(mods) {}
	virtual EventType GetType() const override { return EventType::KeyRepeat; }
	static EventType GetStaticType() { return EventType::KeyRepeat; }

public:
	int Key, Mods;
};

class MousePressedEvent : public Event
{
public:
	MousePressedEvent(int button, int mods) : Button(button), Mods(mods) {}
	virtual EventType GetType() const override { return EventType::MouseButtonPressed; }
	static EventType GetStaticType() { return EventType::MouseButtonPressed; }

public:
	int Button, Mods;
};

class MouseReleasedEvent : public Event
{
public:
	MouseReleasedEvent(int button, int mods) : Button(button), Mods(mods) {}
	virtual EventType GetType() const override { return EventType::MouseButtonReleased; }
	static EventType GetStaticType() { return EventType::MouseButtonReleased; }

public:
	int Button, Mods;
};

class MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(double xpos, double ypos) : Xpos(xpos), Ypos(ypos) {}
	virtual EventType GetType() const override { return EventType::MouseMoved; }
	static EventType GetStaticType() { return EventType::MouseMoved; }

public:
	double Xpos, Ypos;
};

class MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(double xoffset, double yoffset) : Xoffset(xoffset), Yoffset(yoffset) {}
	virtual EventType GetType() const override { return EventType::MouseScrolled; }
	static EventType GetStaticType() { return EventType::MouseScrolled; }

public:
	double Xoffset, Yoffset;
};
