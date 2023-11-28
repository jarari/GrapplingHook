#include "Configs.h"
#include "MenuWatcher.h"

RE::BSEventNotifyControl MenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent& evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>* src) {
	if (!evn.opening) {
		if (evn.menuName == RE::BSFixedString("PauseMenu") || evn.menuName == RE::BSFixedString("LoadingMenu")) {
			Configs::LoadConfigs();
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}
