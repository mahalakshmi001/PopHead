#pragma once

#include "widget.hpp"
#include "Events/event.hpp"
#include <vector>

namespace ph {

class Interface 
{
public:
	Interface(const char* name);

	void handleEvent(const Event&);
	void update(float dt);

	void show();
	void hide();

	Widget* addChildWidget(Widget* widget);
	Widget* getWidget(const char* name);

	bool isActive() { return mIsActive; }
	const char* getName() { return mName; }

private:
	std::vector<std::unique_ptr<Widget>> mWidgetChildren;
	char mName[50];
	bool mIsActive = true;
};

}

