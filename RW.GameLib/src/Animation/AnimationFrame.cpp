#include "Animation/AnimationFrame.h"

AnimationFrame::AnimationFrame()
{
}

AnimationFrame::~AnimationFrame()
{
	delete this->Current;
}

void AnimationFrame::Init(Json::Value &root) {
	this->Duration = root.get("duration", 100).asUInt();
	Frame* f = new Frame();
	f->Init(root["frame"]);
	this->Current = f;

	const Json::Value evts = root["events"];
	if (evts != Json::Value::null) {
		for (Json::ValueIterator it = evts.begin(); it != evts.end(); ++it) {
			Event ev;
			ev.Init(*it);
			this->Events.push_back(ev);
		}
	}

}
