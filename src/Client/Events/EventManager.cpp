#include "EventManager.hpp"

EventManager eventMgr;

nes::event_dispatcher &EventManager::getDispatcher() {
    return dispatcher;
}