#pragma once

#include "windowinfoprovider.h"
#include "gestures/swipedirection.h"
#include <QRegularExpression>
#include <optional>

namespace libgestures
{

class GestureRecognizer;

enum class ConditionType {
    WindowState,
    PreviousSwipeDirection
};

/**
 * Condition class for filtering gestures based on various criteria.
 */
class Condition
{
public:
    /**
     * @return @c false if no window is currently active. @c true if all specified subconditions are satisfied or if
     * none were specified.
     */
    bool isSatisfied() const;

    /**
     * @param windowClassRegex If empty, the subcondition will always be satisfied.
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowClass(const QRegularExpression &windowClassRegex);

    /**
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowState(const WindowStates &windowState);

    void setNegateWindowClass(const bool &negate);
    void setNegateWindowState(const bool &negate);

    /**
     * Sets the condition type.
     */
    void setConditionType(const ConditionType &type);

    /**
     * Sets the previous swipe direction to check for.
     * Only applicable when condition type is PreviousSwipeDirection.
     */
    void setPreviousSwipeDirection(const SwipeDirection &direction);

    /**
     * Sets the finger count to check for previous swipe direction.
     * Only applicable when condition type is PreviousSwipeDirection.
     */
    void setPreviousSwipeFingerCount(const uint8_t &fingerCount);

    /**
     * Sets a reference to the gesture recognizer for checking previous swipe direction.
     * Required when using PreviousSwipeDirection condition type.
     */
    void setGestureRecognizer(const GestureRecognizer *recognizer);

private:
    bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;
    bool isPreviousSwipeDirectionConditionSatisfied() const;

    ConditionType m_conditionType = ConditionType::WindowState;
    std::optional<QRegularExpression> m_windowClass = std::nullopt;
    bool m_negateWindowClass = false;
    WindowStates m_windowState = WindowState::All;
    bool m_negateWindowState = false;

    // For PreviousSwipeDirection condition
    SwipeDirection m_previousSwipeDirection = SwipeDirection::Any;
    uint8_t m_previousSwipeFingerCount = 0;
    const GestureRecognizer *m_gestureRecognizer = nullptr;

    friend class TestCondition;
};

}