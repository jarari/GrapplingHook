#pragma once

class MenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>* src) override;
};
