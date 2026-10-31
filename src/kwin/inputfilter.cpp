#include "inputfilter.h"
#include "input_event_spy.h"
#include <kwin/input_event.h>

#include "wayland_server.h"
#include <utility>

GestureInputEventFilter::GestureInputEventFilter()
#ifdef KWIN_6_2_OR_GREATER
    : KWin::InputEventFilter(KWin::InputFilterOrder::TabBox)
#endif
{
    connect(&m_touchpadHoldGestureTimer, &QTimer::timeout, this, [this]() {
        holdGestureUpdate(1);
    });
}

void GestureInputEventFilter::setTouchpadGestureRecognizer(const std::shared_ptr<libgestures::GestureRecognizer> &gestureRecognizer)
{
    m_touchpadGestureRecognizer = gestureRecognizer;
}

#ifdef KWIN_6_3_OR_GREATER
// KWin 6.3+ event-based API implementations

bool GestureInputEventFilter::holdGestureBegin(KWin::PointerHoldGestureBeginEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = event->fingerCount;
    m_touchpadGestureRecognizer->holdGestureBegin(event->fingerCount);
    m_touchpadHoldGestureTimer.start(1);
    return false;
}

void GestureInputEventFilter::holdGestureUpdate(const qreal &delta)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return;
#endif

    auto endedPrematurely = false;
    m_touchpadGestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
    if (!endedPrematurely)
        return;

    // TODO proper time
    KWin::PointerHoldGestureEndEvent endEvent{std::chrono::microseconds(0)};
    holdGestureEnd(&endEvent);
}

bool GestureInputEventFilter::holdGestureEnd(KWin::PointerHoldGestureEndEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadHoldGestureTimer.stop();

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->holdGestureEnd()) {
        KWin::PointerHoldGestureCancelEvent cancelEvent{event->time};
        KWin::input()->processSpies(&KWin::InputEventSpy::holdGestureCancelled, &cancelEvent);
        KWin::input()->processFilters(&KWin::InputEventFilter::holdGestureCancelled, &cancelEvent);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::holdGestureCancelled(KWin::PointerHoldGestureCancelEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadHoldGestureTimer.stop();

    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->holdGestureCancel();

    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(KWin::PointerSwipeGestureBeginEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = event->fingerCount;
    if (m_touchpadGestureFingerCount >= 3)
        m_touchpadGestureRecognizer->swipeGestureBegin(event->fingerCount);

    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(KWin::PointerSwipeGestureUpdateEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount < 3)
        return false;

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->swipeGestureUpdate(event->delta, endedPrematurely);
    if (endedPrematurely) {
        KWin::PointerSwipeGestureEndEvent endEvent{event->time};
        swipeGestureEnd(&endEvent);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::swipeGestureEnd(KWin::PointerSwipeGestureEndEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->swipeGestureEnd()) {
        KWin::PointerSwipeGestureCancelEvent cancelEvent{event->time};
        KWin::input()->processSpies(&KWin::InputEventSpy::swipeGestureCancelled, &cancelEvent);
        KWin::input()->processFilters(&KWin::InputEventFilter::swipeGestureCancelled, &cancelEvent);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::swipeGestureCancelled(KWin::PointerSwipeGestureCancelEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 3)
        m_touchpadGestureRecognizer->swipeGestureCancel();

    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(KWin::PointerPinchGestureBeginEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = event->fingerCount;
    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureBegin(event->fingerCount);

    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(KWin::PointerPinchGestureUpdateEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount < 2)
        return false;

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->pinchGestureUpdate(event->scale, event->angleDelta, event->delta, endedPrematurely);
    if (endedPrematurely) {
        KWin::PointerPinchGestureEndEvent endEvent{event->time};
        pinchGestureEnd(&endEvent);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::pinchGestureEnd(KWin::PointerPinchGestureEndEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 2 && m_touchpadGestureRecognizer->pinchGestureEnd()) {
        KWin::PointerPinchGestureCancelEvent cancelEvent{event->time};
        KWin::input()->processSpies(&KWin::InputEventSpy::pinchGestureCancelled, &cancelEvent);
        KWin::input()->processFilters(&KWin::InputEventFilter::pinchGestureCancelled, &cancelEvent);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::pinchGestureCancelled(KWin::PointerPinchGestureCancelEvent *event)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureCancel();

    return false;
}

#else
// Legacy KWin < 6.3 API implementations

bool GestureInputEventFilter::holdGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = fingerCount;
    m_touchpadGestureRecognizer->holdGestureBegin(fingerCount);
    m_touchpadHoldGestureTimer.start(1);
    return false;
}

void GestureInputEventFilter::holdGestureUpdate(const qreal &delta)
{
#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return;
#endif

    auto endedPrematurely = false;
    m_touchpadGestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
    if (!endedPrematurely)
        return;

    // TODO proper time
    holdGestureEnd(std::chrono::microseconds(0));
}

bool GestureInputEventFilter::holdGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->holdGestureEnd()) {
        KWin::input()->processSpies(&KWin::InputEventSpy::holdGestureCancelled, &time);
        KWin::input()->processFilters(&KWin::InputEventFilter::holdGestureCancelled, &time);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::holdGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

    m_touchpadHoldGestureTimer.stop();

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->holdGestureCancel();

    return false;
}

bool GestureInputEventFilter::swipeGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount >= 3)
        m_touchpadGestureRecognizer->swipeGestureBegin(fingerCount);

    return false;
}

bool GestureInputEventFilter::swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount < 3)
        return false;

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->swipeGestureUpdate(delta, endedPrematurely);
    if (endedPrematurely) {
        swipeGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::swipeGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 3 && m_touchpadGestureRecognizer->swipeGestureEnd()) {
        KWin::input()->processSpies(&KWin::InputEventSpy::swipeGestureCancelled, &time);
        KWin::input()->processFilters(&KWin::InputEventFilter::swipeGestureCancelled, &time);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::swipeGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 3)
        m_touchpadGestureRecognizer->swipeGestureCancel();

    return false;
}

bool GestureInputEventFilter::pinchGestureBegin(int fingerCount, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    m_touchpadGestureFingerCount = fingerCount;
    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureBegin(fingerCount);

    return false;
}

bool GestureInputEventFilter::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount < 2)
        return false;

    auto endedPrematurely = false;
    const auto filter = m_touchpadGestureRecognizer->pinchGestureUpdate(scale, angleDelta, delta, endedPrematurely);
    if (endedPrematurely) {
        pinchGestureEnd(time);
        return true;
    }

    return filter;
}

bool GestureInputEventFilter::pinchGestureEnd(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 2 && m_touchpadGestureRecognizer->pinchGestureEnd()) {
        KWin::input()->processSpies(&KWin::InputEventSpy::pinchGestureCancelled, &time);
        KWin::input()->processFilters(&KWin::InputEventFilter::pinchGestureCancelled, &time);
        return true;
    }

    m_touchpadGestureFingerCount = 0;
    return false;
}

bool GestureInputEventFilter::pinchGestureCancelled(std::chrono::microseconds time)
{
    Q_UNUSED(time)

#ifndef KWIN_6_2_OR_GREATER
    if (KWin::waylandServer()->isScreenLocked())
        return false;
#endif

    if (m_touchpadGestureFingerCount >= 2)
        m_touchpadGestureRecognizer->pinchGestureCancel();

    return false;
}

#endif

#include "moc_inputfilter.cpp"