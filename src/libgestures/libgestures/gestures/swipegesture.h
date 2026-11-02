#pragma once

#include "swipedirection.h"
#include "gesture.h"

namespace libgestures
{

class SwipeGesture : public Gesture
{
public:
    SwipeDirection direction() const
    {
        return m_direction;
    }

    void setDirection(const SwipeDirection &direction);

private:
    SwipeDirection m_direction = SwipeDirection::Left;
};

}