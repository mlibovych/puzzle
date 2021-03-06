#pragma once

#include <vector>
#include <queue>
#include <unordered_map>

enum class EventType
{
    BLOCK_SET_EVENT,
    LINES_COMPLETED_EVENT,
    NONE
};

struct GameEvent
{
    EventType eventType;
    std::vector<int> lines;
};

class Object
{
public:
    virtual ~Object()
    {

    }

    virtual void React(const GameEvent& event) noexcept = 0;
};

class EventSystem
{
    std::unordered_multimap<EventType, Object*> m_subscribers;
    std::queue<GameEvent> m_events;
public:
    void Subscribe(EventType type, Object* receiver) noexcept;
    void AddEvent(GameEvent&& event) noexcept;
    void ProccedEvent() noexcept;
    bool HaveEvents() noexcept;

    EventType GetNextEventType() noexcept
    {
        if (HaveEvents()) {
            return m_events.front().eventType;
        }
        return EventType::NONE;
    }
};