#pragma once

// Invisible message-only window for launcher detection.
// The launcher finds the injected client via FindWindowW(L"Flarial Client", nullptr).
namespace BeaconWindow {
    void create();
    void destroy();
}
