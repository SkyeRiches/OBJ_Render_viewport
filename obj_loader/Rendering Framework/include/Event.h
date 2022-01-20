#pragma once

// Event class
// an abstract base class for concrete event classes to inherit from

class Event 
{
public:
	// defualt constructor
	// constructs a base event class object, sets handled to false
	Event() : m_bHandled(false) {}
	// Destructor
	virtual ~Event() {};
	// using the "using" command to create an alias for const char*
	using DescriptorType = const char*;
	// returns the descriptor type of the event
	virtual DescriptorType type() const = 0;
	// function used to set if an event has been handled. events that are handled do not report to any subsequent observers
	void Handled() { m_bHandled = true; }
	// Function to set that an event has been handled
	bool IsHandled() { return m_bHandled; }

private:
	bool m_bHandled;
};