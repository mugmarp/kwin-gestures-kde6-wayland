# KWin Gestures — KWin 6.3+ Compatible Fork

Fork of [gbytedev/kwin-gestures](https://github.com/gbytedev/kwin-gestures) with patches for KWin 6.3+ (Plasma 6.1+) API compatibility.

## 🎯 Problem Solved

**Upstream kwin-gestures 0.4.0 fails to build on KWin ≥ 6.3 (Plasma 6.1+)** due to API changes in `KWin::InputEventFilter` gesture handling methods.

| KWin Version | Gesture Method Signatures |
|--------------|---------------------------|
| **≤ 6.2** (legacy) | `bool holdGestureBegin(int fingerCount, std::chrono::microseconds time)` |
| **≥ 6.3** (current) | `bool holdGestureBegin(PointerHoldGestureBeginEvent *event)` |

The upstream code uses legacy signatures, causing **override errors** and **incomplete type errors** when building against KWin 6.3+ headers.

### ✅ Fixed Distributions
- **Kubuntu 26.04** (KWin 6.6.6)
- **Fedora 41+** (KWin 6.2+)
- **Arch Linux** (rolling, KWin 6.3+)
- **openSUSE Tumbleweed** (KWin 6.3+)

## 🔧 Changes (Summary)

| File | Change |
|------|--------|
| `src/kwin/inputfilter.h` | Conditional overloads for 12 gesture methods using `KWIN_6_3_OR_GREATER` |
| `src/kwin/inputfilter.cpp` | Dual API implementation, event struct field mapping, member function pointers for `processSpies/processFilters` |
| `src/kwin/CMakeLists.txt` | Unchanged — upstream already detects KWin version |

→ **[Full technical details](docs/technical-changes.md)** (event struct mapping, code diffs, hold gesture synthesis, screen lock handling)

## 📦 Build Instructions (Kubuntu 26.04)

```bash
# Dependencies
sudo apt install -y git cmake g++ extra-cmake-modules qt6-tools-dev \
  kwin-wayland kwin-dev libkf6configwidgets-dev gettext \
  libkf6kcmutils-dev libyaml-cpp-dev libxkbcommon-dev libdrm-dev

# Build from this fork
git clone https://github.com/mugmarp/kwin-gestures-kde6-wayland
cd kwin-gestures-kde6-wayland
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
| **Hold gesture `holdGestureUpdate`** | ⚠️ Partial | KWin 6.3+ has no event — synthesized via 1ms timer |
| **Screen lock handling** | ✅ Fixed | Guarded for older KWin |
| **X11 support** | ❌ Not supported | Upstream — Wayland only |
| **3/4-finger pinch on 2-finger touchpads** | ⚠️ Hardware limit | libinput interprets as swipe; move 2 fingers opposite |

## 🔄 Upstream Tracking

| Branch | Base | Status |
|--------|------|--------|
| `main` | `gbytedev/kwin-gestures@main` | ✅ Patched for KWin 6.3+ |
| `upstream/main` | — | Tracks upstream for rebasing |

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