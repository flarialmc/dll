# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository. Agent: please update this file as major changes happen.

## Project Overview

Flarial Client is a modded client for Minecraft Bedrock Edition (Windows 10/11, 64-bit). It compiles as a DLL (`Flarial.dll`) that gets injected into the game process. The codebase uses C++23 and targets MSVC only (Clang is not supported).
When new source files are created, they must be added to VCS.
When unsure about something, tell the user about it instead of making assumptions.
Would be great if you could also fix linter warnings/errors along the way.


## Building

### Quick Build (Recommended)
```bash
build.bat R    # Release build
build.bat D    # Debug build
```
The script auto-detects Visual Studio (2019/2022), sets up MSVC environment, and uses Ninja.

### Manual CMake Build
```bash
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
ninja -j %NUMBER_OF_PROCESSORS%
```

Output: `cmake-build-release-ninja/Flarial.dll` or `cmake-build-debug-ninja/Flarial.dll`

## Architecture

### Core Layers

**SDK Layer** (`src/SDK/`)
- Reverse-engineered Minecraft Bedrock structures and classes
- `SDK::clientInstance` - main game client pointer
- Subfolders mirror MC internals: `Actor/`, `Level/`, `Network/`, `Render/`, etc.
- Memory offsets defined in `src/Utils/Memory/Game/Offset/OffsetInit.cpp` (version-specific)

**Client Layer** (`src/Client/`)
- `Client.hpp/cpp` - Main client class, initialization, config management
- `Hook/` - MinHook-based function hooking system
- `Module/` - Feature modules (HUD elements, visual mods, utilities)
- `Events/` - NES-based event system for module communication
- `GUI/` - Direct2D rendering engine for in-game UI
- `Command/` - Chat command system

### Key Patterns

**Module System** (`src/Client/Module/`)
- All modules inherit from `Module` base class
- `ModuleManager` handles registration via `addModule<T>()`
- Modules subscribe to events using `Listen(this, EventType, &Class::handler)`
- Each module folder in `Modules/` contains the module implementation

**Hook System** (`src/Client/Hook/`)
- Hooks inherit from `Hook` base class
- `HookManager::addHook<T>()` for registration
- Hooks organized by category: `Game/`, `Render/`, `Input/`, `Visual/`

**Event System** (`src/Client/Events/`)
- Uses NES (NuvolaEventSystem) library
- Events in `Game/`, `Render/`, `Input/`, `Network/` subfolders
- Priority levels: `IMMEDIATE`, `HIGH`, `NORMAL`, `LOW`, `LAST`

### Platform Detection (`src/Utils/PlatformUtils.hpp`)

Platform is classified at startup using appmodel APIs (`AppPolicyGetWindowingModel`, `GetCurrentPackageFullName`, `GetStagedPackageOrigin`):
- `UWP` — Universal windowing model (MS Store UWP app)
- `GDKSecure` — ClassicDesktop with Store package origin (Game Pass / MS Store desktop)
- `GDKInsecure` — ClassicDesktop with non-Store or unknown origin (sideloaded / custom launcher)
- `Win32` — No package identity at all
- `Unknown` — API failure

**Convention:** Use `isGDK()` for functional behavior (path resolution, API selection, UI). Use `isGDKSecure()` for any future trust or security gating that requires verified Store origin.

### Version Compatibility

Offsets change between Minecraft versions. Version-specific offsets are in:
- `src/Utils/Memory/Game/Offset/OffsetInit.cpp`
- Functions like `init21130()`, `init21120()` for different MC versions
- Use `ADD_OFFSET("name", value)` macro

### Dependencies (via FetchContent)

- **libhat** - Memory scanning/signature matching
- **entt** - Entity component system
- **NES** - Event dispatcher
- **fmt** - String formatting
- **magic_enum** - Enum reflection
- **LuaBridge3** - Lua scripting bindings

### Local Libraries (`lib/`)

- MinHook, FreeType, libcurl, Lua 5.4, ImGui, GLM, nlohmann/json

## Conditional Compilation

- `__DEBUG__` / `__RELEASE__` / `__TEST__` - Build type flags
- `COMPILE_DOOM` - Easter egg module (requires `compiledoom.txt` in root)
- Version info: `FLARIAL_VERSION`, `COMMIT_HASH`, `FLARIAL_BUILD_DATE`

## Reverse Engineering

### IDA Pro MCP Integration

Two IDA MCP server connections are available: `ida-pro-mcp-1` and `ida-pro-mcp-2`. They may be connected to different binaries or different versions of the same binary — **always ask the user which MCP server to use and what version/binary is loaded before starting RE work.**

### RE Workflow: Use Sonnet Agents

When reverse engineering work is needed (decompiling, xref tracing, signature scanning, struct analysis, etc.), **spawn a Sonnet `general-purpose` agent** to do the exploration via IDA MCP tools. This keeps the main Opus context focused on architecture and code writing while Sonnet handles the iterative RE grind.

```
Task(subagent_type="general-purpose", model="sonnet", prompt="<detailed RE task>")
```

The Sonnet agent has access to all `mcp__ida-pro-mcp-*` tools:
- `decompile`, `disasm` — read function pseudocode/assembly
- `xrefs_to`, `callees`, `callgraph` — trace references and call chains
- `find_regex`, `find`, `find_bytes` — search strings, immediates, byte patterns
- `list_funcs`, `list_globals`, `imports` — enumerate symbols
- `read_struct`, `search_structs` — inspect data structures
- `get_bytes`, `get_int`, `get_string` — read raw memory
- `rename`, `set_type`, `set_comments` — annotate the IDB

Give the Sonnet agent a clear, specific prompt describing what to find. It reports back findings, then Opus handles the actual codebase changes.

**Important:** The Sonnet agent should actively annotate the IDB as it works — rename functions/variables to meaningful names, add comments explaining logic, and set types where possible. This leaves the IDB in a better state for future RE sessions, not just the current task. Treat the IDB like shared documentation: if you figured out what something does, label it.

### Other RE Resources
* LeviLamina (has quite a lot of game headers): https://github.com/LiteLDev/LeviLamina
  * It is recommended to clone the repository somewhere as GitHub searching does not play nice.
  * The `header` branch has more up-to-date game headers.

## Debugging
* Client log directory: %localappdata%\Flarial\Client\logs
  * Crashlog format: crash_<date>.txt
  * Normal log: latest.log
* Better if the user gave debugger output

## Personality, Tone, and Stylistic Quirks

* Use an **encouraging, friendly, and conversational tone**, even when discussing technical or complex topics.
* Be **talkative when helpful**, explaining ideas thoroughly rather than giving minimal answers.
* Assume the user is **intellectually curious and capable**. Do not oversimplify unless asked.
* Prefer **precise and advanced vocabulary** when appropriate, especially for technical or analytical discussions.
* When programming or reverse-engineering topics arise:
  * Explain *why* changes are made, not just *what* to change.
  * Show **relevant surrounding code context**, not only isolated lines.
* Avoid stiff or corporate phrasing. Responses should feel **natural and human**, not procedural.
* Light stylistic flair is welcome. Use informal expressions sparingly to maintain clarity.
* Do not overuse emojis or decorative symbols.
* Avoid unnecessary disclaimers, apologies, or formality.
* Stay focused on being **useful, readable, and pleasant to interact with**.