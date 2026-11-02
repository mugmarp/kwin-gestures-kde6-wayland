#include "condition.h"
#include "gestures/gesturerecognizer.h"

namespace libgestures
{

bool Condition::isSatisfied() const
{
    switch (m_conditionType) {
    case ConditionType::PreviousSwipeDirection:
        return isPreviousSwipeDirectionConditionSatisfied();

    case ConditionType::WindowState:
    default: {
        const auto windowData = WindowInfoProvider::implementation()->activeWindow();
        if (!windowData)
            return false;

        return isWindowClassRegexSubConditionSatisfied(windowData.value())
            && isWindowStateSubConditionSatisfied(windowData.value());
    }
    }
}

bool Condition::isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const
{
    if (!m_windowClass || m_windowClass->pattern().isEmpty()) {
        return true;
    }

    return ((*m_windowClass).match(data.resourceClass()).hasMatch()
            || (*m_windowClass).match(data.resourceName()).hasMatch())
        == !m_negateWindowClass;
}

bool Condition::isWindowStateSubConditionSatisfied(const WindowInfo &data) const
{
    if (m_windowState == WindowState::All)
        return true;

    const bool satisfied =
        ((m_windowState & WindowState::Fullscreen) && (data.state() & WindowState::Fullscreen))
        || ((m_windowState & WindowState::Maximized) && (data.state() & WindowState::Maximized));
    return m_negateWindowState == !satisfied;
}

bool Condition::isPreviousSwipeDirectionConditionSatisfied() const
{
    if (!m_gestureRecognizer) {
        return false;
    }

    if (m_previousSwipeFingerCount == 0) {
        return false;
    }

    const auto lastDirection = m_gestureRecognizer->lastSwipeDirection(m_previousSwipeFingerCount);
    return lastDirection == m_previousSwipeDirection;
}

void Condition::setWindowClass(const QRegularExpression &windowClassRegex)
{
    m_windowClass = windowClassRegex;
}

void Condition::setWindowState(const WindowStates &windowState)
{
    m_windowState = windowState;
}

void Condition::setNegateWindowClass(const bool &negate)
{
    m_negateWindowClass = negate;
}

void Condition::setNegateWindowState(const bool &negate)
{
    m_negateWindowState = negate;
}

void Condition::setConditionType(const ConditionType &type)
{
    m_conditionType = type;
}

void Condition::setPreviousSwipeDirection(const SwipeDirection &direction)
{
    m_previousSwipeDirection = direction;
}

void Condition::setPreviousSwipeFingerCount(const uint8_t &fingerCount)
{
    m_previousSwipeFingerCount = fingerCount;
}

void Condition::setGestureRecognizer(const GestureRecognizer *recognizer)
{
    m_gestureRecognizer = recognizer;
}

}