# Lightweight Crash Tracking Usage Guide

## Overview
Since PDB files are too large to ship, we use a lightweight crash tracking system that provides context about what the code was doing when it crashed.

## How It Works
Instead of relying on debug symbols, the system tracks execution context through explicit zone markers. When a crash occurs, you get:
- The execution path that led to the crash
- Which modules were enabled
- Custom context information
- Timestamp and exception details

## Basic Usage

### 1. Simple Function Tracking
```cpp
void MyModule::onEnable() {
    CRASH_ZONE_SIMPLE();  // Automatically uses __FUNCTION__ and __LINE__

    // Your code here
    player->setPosition(pos);
}
```

### 2. Detailed Context Tracking
```cpp
void MyModule::handlePacket(Packet* packet) {
    CRASH_ZONE("Processing packet type: " + packet->getName());

    // Your code here
}
```

### 3. Module Operations
```cpp
void MyModule::render() {
    MODULE_CRASH_ZONE("MyModule", "Rendering UI");

    // Rendering code
}
```

### 4. Manual Context Management
```cpp
void complexOperation() {
    CrashTracker::pushContext("MyModule", "complexOperation", __LINE__, "Step 1: Init");
    // Do initialization

    CrashTracker::popContext();
    CrashTracker::pushContext("MyModule", "complexOperation", __LINE__, "Step 2: Process");
    // Do processing

    CrashTracker::popContext();
}
```

### 5. Setting Global Context
```cpp
void MyModule::onPlayerJoin(std::string serverIP) {
    CrashTracker::setGlobalContext("Server", serverIP);
    CrashTracker::setGlobalContext("Player Count", std::to_string(playerCount));
}
```

## Example Crash Report
```
========================================
    FLARIAL LIGHTWEIGHT CRASH REPORT
========================================

Timestamp: 2025-09-15 14:32:45

Global Context:
----------------------------------------
  Client Version: 2.00
  Build Type: Release
  Server: play.example.com
  Player Count: 24

Execution Context (Most Recent First):
----------------------------------------
  [0] Waypoints.cpp -> onRender (line 145)
      Context: Drawing waypoint marker
  [1] ModuleManager.cpp -> renderModules (line 78)
      Context: Rendering all enabled modules
  [2] RenderHook.cpp -> onPresent (line 234)

Active Modules:
----------------------------------------
  ✓ Waypoints
  ✓ ClickGUI
  ✓ Sprint
  ✓ Zoom

========================================
```

## Best Practices

1. **Add zones to crash-prone areas**: Rendering, packet handling, memory operations
2. **Use descriptive context**: Include relevant data like packet types, player names, coordinates
3. **Don't overuse**: Too many zones can impact performance
4. **Clean up properly**: CRASH_ZONE macros auto-cleanup, but manual push/pop must be balanced

## Performance Impact
- Minimal overhead: Just pushing/popping from a vector
- No file I/O during normal operation
- No symbol resolution overhead
- Typical overhead: < 0.001ms per zone

## Integration with Modules

```cpp
class MyModule : public Module {
public:
    void onEnable() override {
        CRASH_ZONE_SIMPLE();
        // Enable logic
    }

    void onTick() override {
        CRASH_ZONE("Tick #" + std::to_string(tickCount));
        // Tick logic
    }

    void onRender() override {
        MODULE_CRASH_ZONE(this->name, "Render");
        // Render logic
    }
};
```

## Debugging Tips

1. When users report crashes, ask for the `crash_lightweight_*.txt` file
2. The execution context shows the exact code path at crash time
3. Global context helps understand the game state
4. Active modules list helps reproduce the issue

## Advantages Over PDB Files
- No 50+ MB PDB files to distribute
- Works immediately without symbol servers
- Provides human-readable context
- Can include runtime values (coordinates, player names, etc.)
- Zero configuration for end users