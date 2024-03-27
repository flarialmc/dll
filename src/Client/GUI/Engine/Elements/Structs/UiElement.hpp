#pragma once
// Base class for all UI elements
class UIElement {
public:
    virtual void render() = 0;
    // Other common methods and properties
};

// Specific UI elements
class Button : public UIElement {
public:
    virtual void render() override {
        // Render button rectangle and text using rendering functions from the engine
    }
    // Button-specific methods and properties
};

class RoundedButton : public Button {
public:
    virtual void render() override {
        // Render rounded button using rendering functions from the engine
    }
    // Rounded button-specific methods and properties
};

// Other UI elements and effects classes...
