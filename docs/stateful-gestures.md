# Stateful Gestures — Previous Swipe Direction Condition

kwin-gestures now supports **stateful gestures** via a new condition type: `previous_swipe_direction`.

This enables Windows 11-style toggle behavior where a gesture's action depends on the **previous swipe direction** with the same finger count.

## How It Works

1. **GestureRecognizer** tracks the last swipe direction per finger count
2. **Condition** type `previous_swipe_direction` checks if the last swipe matches
3. **YAML** configuration specifies the expected previous direction and finger count

## YAML Syntax

```yaml
touchpad:
  gestures:
    # Default 3-finger swipe UP → Overview
    - type: swipe
      fingers: 3
      direction: up
      actions:
        - plasma_shortcut: kwin,Overview

    # 3-finger swipe UP → Restore (only if previous was DOWN)
    - type: swipe
      fingers: 3
      direction: up
      conditions:
        - type: previous_swipe_direction
          direction: down
          fingers: 3
      actions:
        - plasma_shortcut: kwin,Show Desktop  # Toggle: restores minimized windows

    # 3-finger swipe DOWN → Show Desktop (minimize all)
    - type: swipe
      fingers: 3
      direction: down
      actions:
        - plasma_shortcut: kwin,Show Desktop
```

## Condition Properties

| Property | Type | Required | Description |
|----------|------|----------|-------------|
| `type` | string | **Yes** | Must be `previous_swipe_direction` |
| `direction` | SwipeDirection | **Yes** | Expected previous direction (`up`, `down`, `left`, `right`, `left_right`, `up_down`, `any`) |
| `fingers` | integer | **Yes** | Finger count to check (must match current gesture's finger count) |

## SwipeDirection Values

| Value | Meaning |
|-------|---------|
| `up` | Swipe up |
| `down` | Swipe down |
| `left` | Swipe left |
| `right` | Swipe right |
| `left_right` | Left or right |
| `up_down` | Up or down |
| `any` | Any direction |

## Complete Windows 11 Toggle Example

```yaml
touchpad:
  gestures:
    # 3-finger UP → Overview (default)
    - type: swipe
      fingers: 3
      direction: up
      actions:
        - plasma_shortcut: kwin,Overview

    # 3-finger UP → Restore (if previous was DOWN)
    - type: swipe
      fingers: 3
      direction: up
      conditions:
        - type: previous_swipe_direction
          direction: down
          fingers: 3
      actions:
        - plasma_shortcut: kwin,Show Desktop

    # 3-finger DOWN → Show Desktop (minimize all)
    - type: swipe
      fingers: 3
      direction: down
      actions:
        - plasma_shortcut: kwin,Show Desktop

    # 4-finger LEFT/RIGHT → Switch virtual desktops
    - type: swipe
      fingers: 4
      direction: left
      actions:
        - plasma_shortcut: kwin,Switch One Desktop to the Left

    - type: swipe
      fingers: 4
      direction: right
      actions:
        - plasma_shortcut: kwin,Switch One Desktop to the Right
```

## How the Toggle Works

| State | User Action | Result |
|-------|-------------|--------|
| Normal | 3-finger ↓ | Minimize all (Show Desktop) |
| Minimized | 3-finger ↑ | **Restore all** (condition matches: previous was ↓) |
| Normal | 3-finger ↑ | Overview (no condition match) |

The plugin records the direction when a swipe gesture **ends**, so the next swipe can check it.

## Notes

- Conditions are evaluated **before** the gesture action executes
- The `fingers` property in the condition must match the gesture's finger count
- Only swipe gestures update the history (hold/pinch don't affect it)
- Works with `left_right` and `up_down` direction aliases
- Compatible with existing `window_class` and `window_state` conditions

## Reload Config

```bash
qdbus6 org.kde.KWin /KWin reconfigure
```