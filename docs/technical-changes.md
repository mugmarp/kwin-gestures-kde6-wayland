# Technical Changes — KWin 6.3+ API Compatibility

Detailed documentation of patches applied to achieve KWin 6.3+ (Plasma 6.1+) compatibility.

## 📋 Summary

| File | Change Type | Lines Changed |
|------|-------------|---------------|
| `src/kwin/inputfilter.h` | Header: conditional overloads for 12 gesture methods | ~25 |
| `src/kwin/inputfilter.cpp` | Implementation: dual API + event struct mapping | ~200 |
| `src/kwin/CMakeLists.txt` | Unchanged (upstream already has version detection) | 0 |

## 🔄 API Incompatibility Details

### KWin ≤ 6.2 (Legacy)
```cpp
bool holdGestureBegin(int fingerCount, std::chrono::microseconds time);
bool holdGestureUpdate(const qreal &delta);
bool holdGestureEnd(std::chrono::microseconds time);
bool holdGestureCancelled(std::chrono::microseconds time);

bool swipeGestureBegin(int fingerCount, std::chrono::microseconds time);
bool swipeGestureUpdate(const QPointF &delta, std::chrono::microseconds time);
bool swipeGestureEnd(std::chrono::microseconds time);
bool swipeGestureCancelled(std::chrono::microseconds time);

bool pinchGestureBegin(int fingerCount, std::chrono::microseconds time);
bool pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta, std::chrono::microseconds time);
bool pinchGestureEnd(std::chrono::microseconds time);
bool pinchGestureCancelled(std::chrono::microseconds time);
```

### KWin ≥ 6.3 (Current)
```cpp
bool holdGestureBegin(PointerHoldGestureBeginEvent *event);
void holdGestureUpdate(const qreal &delta);  // Not in KWin API - synthesized
bool holdGestureEnd(PointerHoldGestureEndEvent *event);
bool holdGestureCancelled(PointerHoldGestureCancelEvent *event);

bool swipeGestureBegin(PointerSwipeGestureBeginEvent *event);
bool swipeGestureUpdate(PointerSwipeGestureUpdateEvent *event);
bool swipeGestureEnd(PointerSwipeGestureEndEvent *event);
bool swipeGestureCancelled(PointerSwipeGestureCancelEvent *event);

bool pinchGestureBegin(PointerPinchGestureBeginEvent *event);
bool pinchGestureUpdate(PointerPinchGestureUpdateEvent *event);
bool pinchGestureEnd(PointerPinchGestureEndEvent *event);
bool pinchGestureCancelled(PointerPinchGestureCancelEvent *event);
```

## 📁 File-by-File Changes

### `src/kwin/inputfilter.h`

Added `#ifdef KWIN_6_3_OR_GREATER` block with 12 new method declarations using event struct pointers. Legacy signatures preserved in `#else` block for KWin ≤ 6.2 compatibility.

Key additions:
- `KWIN_6_3_OR_GREATER` compile definition detection (from `CMakeLists.txt`)
- All 12 gesture methods overloaded with `KWin::Pointer*Gesture*Event*` parameters
- Private members unchanged (`m_touchpadGestureRecognizer`, `m_touchpadGestureFingerCount`, `m_touchpadHoldGestureTimer`)

### `src/kwin/inputfilter.cpp`

#### 1. Include Fix
```cpp
#include <kwin/input_event.h>  // Required for complete event struct definitions
```

#### 2. Event Struct Field Mapping

| Event Struct | Fields Used |
|--------------|-------------|
| `PointerHoldGestureBeginEvent` | `fingerCount`, `time` |
| `PointerHoldGestureEndEvent` | `time` |
| `PointerHoldGestureCancelEvent` | `time` |
| `PointerSwipeGestureBeginEvent` | `fingerCount`, `time` |
| `PointerSwipeGestureUpdateEvent` | `delta`, `time` |
| `PointerSwipeGestureEndEvent` | `time` |
| `PointerSwipeGestureCancelEvent` | `time` |
| `PointerPinchGestureBeginEvent` | `fingerCount`, `time` |
| `PointerPinchGestureUpdateEvent` | `scale`, `angleDelta`, `delta`, `time` |
| `PointerPinchGestureEndEvent` | `time` |
| `PointerPinchGestureCancelEvent` | `time` |

#### 3. `processSpies`/`processFilters` Fix

**Old (lambda - broken on KWin 6.3+):**
```cpp
KWin::input()->processSpies([&time](auto &&spy) {
    spy->holdGestureCancelled(time);
});
KWin::input()->processFilters([&time](auto &&filter) {
    return filter->holdGestureCancelled(time);
});
```

**New (member function pointer - works on all versions):**
```cpp
KWin::PointerHoldGestureCancelEvent cancelEvent{event->time};
KWin::input()->processSpies(&KWin::InputEventSpy::holdGestureCancelled, &cancelEvent);
KWin::input()->processFilters(&KWin::InputEventFilter::holdGestureCancelled, &cancelEvent);
```

Applied to all three gesture end handlers (hold, swipe, pinch).

#### 4. Hold Gesture Update Synthesis

KWin 6.3+ has **no `holdGestureUpdate` event**. Implemented via timer:
```cpp
// Constructor
connect(&m_touchpadHoldGestureTimer, &QTimer::timeout, this, [this]() {
    holdGestureUpdate(1);
});

// holdGestureBegin starts timer
m_touchpadHoldGestureTimer.start(1);

// holdGestureUpdate calls recognizer, synthesizes end event if ended prematurely
void holdGestureUpdate(const qreal &delta) {
    auto endedPrematurely = false;
    m_touchpadGestureRecognizer->holdGestureUpdate(delta, endedPrematurely);
    if (!endedPrematurely) return;
    KWin::PointerHoldGestureEndEvent endEvent{std::chrono::microseconds(0)};
    holdGestureEnd(&endEvent);
}
```

#### 5. Screen Lock Handling

Guarded with `#ifndef KWIN_6_2_OR_GREATER` for older KWin:
```cpp
#ifndef KWIN_6_2_OR_GREATER
if (KWin::waylandServer()->isScreenLocked())
    return false;  // or return; for void methods
#endif
```

## 🏗️ Build System

`src/kwin/CMakeLists.txt` (upstream, unchanged):
```cmake
if(${KWin_VERSION} VERSION_GREATER_EQUAL 6.1.90)
    add_compile_definitions(KWIN_6_2_OR_GREATER)
endif()
if(${KWin_VERSION} VERSION_GREATER_EQUAL 6.2.90)
    add_compile_definitions(KWIN_6_3_OR_GREATER)
endif()
```

## 🔗 Related

- [README.md](../README.md) — Quick start, install, enable
- [Upstream kwin-gestures](https://github.com/gbytedev/kwin-gestures)
- [KWin Input API](https://invent.kde.org/plasma/kwin/-/blob/master/src/input.h)