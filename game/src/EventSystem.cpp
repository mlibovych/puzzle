#include <EventSystem.h>

void EventSystem::Subscribe(EventType type, Object* receiver) noexcept
{
    m_subscribers.insert(decltype(m_subscribers)::value_type(type, receiver));
    // m_subscribers[type] = receiver;
}

void EventSystem::AddEvent(GameEvent&& event) noexcept
{
    m_events.push(std::move(event));
}

void EventSystem::ProccedEvent() noexcept
{   
    if (HaveEvents()) {
        auto& event = m_events.front();
        
        for (auto& [key, value] : m_subscribers) {
            if (key == event.eventType) {
                value->React(event);
            }
            // m_subscribers[event.eventType]->React(event);
        }
        m_events.pop();
    }
}

bool EventSystem::HaveEvents() noexcept
{
    return !m_events.empty();
}