# KWin Gestures — KWin 6.3+ Compatible Fork

Fork of [gbytedev/kwin-gestures](https://github.com/gbytedev/kwin-gestures) with patches for KWin 6.3+ (Plasma 6.1+) API compatibility.

## 🎯 Problem Solved

**Upstream kwin-gestures 0.4.0 fails to build on KWin ≥ 6.3 (Plasma 6.1+)** due to API changes in `KWin::InputEventFilter` gesture handling methods.

### API Incompatibility

| KWin Version | Gesture Method Signatures |
|--------------|---------------------------|
| **≤ 6.2** (legacy) | `bool holdGestureBegin(int fingerCount, std::chrono::microseconds time)` |
| **≥ 6.3** (current) | `bool holdGestureBegin(PointerHoldGestureBeginEvent *event)` |

The upstream code uses the legacy signatures, causing **override errors** and **incomplete type errors** when building against KWin 6.3+ headers.

### Affected Distributions
- **Kubuntu 26.04** (KWin 6.6.6) ✅ Fixed
- **Fedora 41+** (KWin 6.2+) ✅ Fixed
- **Arch Linux** (rolling, KWin 6.3+) ✅ Fixed
- **openSUSE Tumbleweed** (KWin 6.3+) ✅ Fixed

## 🔧 Changes Made

### `src/kwin/inputfilter.h`
- Added `#ifdef KWIN_6_3_OR_GREATER` conditional overloads for all 12 gesture methods
- New signatures use KWin event struct pointers (`Pointer*Gesture*Event*`)

### `src/kwin/inputfilter.cpp`
- Implemented both API versions (legacy + KWin 6.3+)
- **Event struct field mapping:**
  ```cpp
  // PointerHoldGestureBeginEvent
  event->fingerCount, event->time
  
  // PointerSwipeGestureUpdateEvent  
  event->delta, event->time
  
  // PointerPinchGestureUpdateEvent
  event->scale, event->angleDelta, event->delta, event->time
  ```
- **Fixed `processSpies`/`processFilters` calls** — changed from lambdas to member function pointers:
  ```cpp
  // Old (broken on KWin 6.3+)
  KWin::input()->processSpies([&time](auto &&spy) { spy->holdGestureCancelled(time); });
  
  // New (works on all versions)
  KWin::input()->processSpies(&KWin::InputEventSpy::holdGestureCancelled, &cancelEvent);
  ```

### `src/kwin/CMakeLists.txt` (already present in upstream)
- Detects KWin version and defines `KWIN_6_2_OR_GREATER` / `KWIN_6_3_OR_GREATER`

## 📦 Build Instructions (Kubuntu 26.04)

```bash
# Dependencies
sudo apt install -y git cmake g++ extra-cmake-modules qt6-tools-dev \
  kwin-wayland kwin-dev libkf6configwidgets-dev gettext \
  libkf6kcmutils-dev libyaml-cpp-dev libxkbcommon-dev libdrm-dev

# Build from this fork
git clone https://github.com/mugmarp/kwin-gestures
cd kwin-gestures
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
sudo make install
```

## 🚀 Enable

1. **System Settings → Desktop Effects → Tools → Gestures** (enable)
2. **System Settings → Input Devices → Touchpad → Gestures** (configure)
3. Log out/in or `kwin_wayland --replace &`

## ⚠️ Known Limitations

| Limitation | Status | Notes |
|------------|--------|-------|
| **Hold gesture `holdGestureUpdate`** | ⚠️ Partial | KWin 6.3+ has no `holdGestureUpdate` event — synthesized via timer (1ms interval). May feel less responsive than native. |
| **Screen lock handling** | ✅ Fixed | Properly guarded with `#ifndef KWIN_6_2_OR_GREATER` for older KWin. |
| **X11 support** | ❌ Not supported | Upstream limitation — Wayland only. |
| **3/4-finger pinch on 2-finger touchpads** | ⚠️ Hardware limit | libinput interprets as swipe; workaround: move 2 fingers opposite. |

## 🔄 Upstream Tracking

| Branch | Base | Status |
|--------|------|--------|
| `main` | `gbytedev/kwin-gestures@main` | ✅ Patched for KWin 6.3+ |
| `upstream/main` | — | Tracks upstream for rebasing |

Check for upstream updates:
```bash
git fetch upstream
git log --oneline upstream/main..main
```

---

# Original Upstream README

Custom touchpad gestures for Plasma 6.

X11 is currently not supported.

https://github.com/user-attachments/assets/2c16790a-869b-44f3-a760-590137293759

[peterfajdiga/kwin4_effect_geometry_change](https://github.com/peterfajdiga/kwin4_effect_geometry_change) was used for tile animations.

# Features
- Gestures: hold, pinch, swipe
- Actions: run command, input (keyboard keys, mouse buttons, relative and absolute mouse movement, full control over when keys/buttons are pressed/released), invoke global shortcut
- Override/block built-in Plasma gestures
- Application-specific gestures
- Trigger actions before fingers are lifted for a more responsive feel
- Repeating actions with support for changing the direction mid-gesture
- Fast and slow gestures

# Installation
<details>
  <summary>NixOS (flakes)</summary>
  <br>

`flake.nix`:
```nix
{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

    kwin-gestures = {
      url = "github:taj-ny/kwin-gestures";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };
}
```

```nix
{ inputs, pkgs, ... }:

{
  environment.systemPackages = [
    inputs.kwin-gestures.packages.${pkgs.system}.default
  ];
}
```
</details>

# Building from source
### Dependencies
- CMake
- Extra CMake Modules
- Plasma 6
- Qt 6
- KF6
- KWin development packages

<details>
  <summary>Arch Linux</summary>
  <br>

  ```
  sudo pacman -S --needed base-devel git extra-cmake-modules qt6-tools kwin yaml-cpp
  ```
</details>

<details>
  <summary>Debian-based (KDE Neon, Kubuntu, Ubuntu)</summary>
  <br>

  ```
  sudo apt install git cmake g++ extra-cmake-modules qt6-tools-dev kwin-wayland kwin-dev libkf6configwidgets-dev gettext libkf6kcmutils-dev libyaml-cpp-dev libxkbcommon-dev
  ```
</details>

<details>
  <summary>Fedora 40, 41</summary>
  <br>

  ```
  sudo dnf install git cmake extra-cmake-modules gcc-g++ qt6-qtbase-devel kwin-devel kf6-ki18n-devel kf6-kguiaddons-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel qt6-qtbase kf6-kguiaddons kf6-ki18n wayland-devel yaml-cpp yaml-cpp-devel libepoxy-devel
  ```
</details>

<details>
  <summary>openSUSE</summary>
  <br>

  ```
  sudo zypper in git cmake-full gcc-c++ kf6-extra-cmake-modules kguiaddons-devel kconfigwidgets-devel ki18n-devel kcmutils-devel "cmake(KF6I18n)" "cmake(KF6KCMUtils)" "cmake(KF6WindowSystem)" "cmake(Qt6Core)" "cmake(Qt6DBus)" "cmake(Qt6Quick)" "cmake(Qt6Widgets)" libepoxy-devel kwin6-devel yaml-cpp-devel libxkbcommon-devel
  ```
</details>

### Building
```sh
git clone https://github.com/taj-ny/kwin-gestures
cd kwin-gestures
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```

Remove the *build* directory when rebuilding the effect.

# Usage
> [!NOTE]
> If the effect stops working after a system upgrade, you will need to rebuild it.

1. Install the plugin.
2. Open the Desktop Effects page in System Settings.
3. Enable the *Gestures* effect in the *Tools* category.

See [docs/configuration.md](docs/configuration.md) for instructions on how to configure this plugin.

# Gesture recognition issues
Before reporting any issues related to gesture recognition, run ``libinput debug-events`` as root and ensure the gesture is recognized properly. If it's not, there's nothing I can do.

Depending on the touchpad, 3 or 4-finger pinch gestures may sometimes be incorrectly interpreted as swipe gestures due to the touchpad only being able to track 2 fingers. As a workaround, move only 2 fingers in opposite directions. See https://wayland.freedesktop.org/libinput/doc/1.25.0/gestures.html#gestures-on-two-finger-touchpads for more information.

# Credits
- [KWin](https://invent.kde.org/plasma/kwin) - Gesture recognition code (parts of it), sending keystrokes

## License

GPL-2.0-or-later (same as upstream KWin)