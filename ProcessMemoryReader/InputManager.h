#pragma once

#include <Windows.h>
#include <vector>
#include <chrono>
#include <unordered_map>

/**
 * @brief Handles input simulation and mouse/keyboard control
 */
class InputManager {
public:
    enum class ClickType {
        LEFT_CLICK,
        RIGHT_CLICK,
        MIDDLE_CLICK
    };

    struct MousePosition {
        int x, y;
        MousePosition(int x = 0, int y = 0) : x(x), y(y) {}
    };

private:
    HWND m_gameWindow;
    RECT m_windowRect;
    
    // Input timing and humanization
    std::chrono::steady_clock::time_point m_lastKeyPress;
    std::chrono::steady_clock::time_point m_lastMouseClick;
    std::chrono::steady_clock::time_point m_lastMouseMove;
    
    // Humanization parameters
    int m_minKeyDelay = 50;        // Minimum delay between key presses (ms)
    int m_maxKeyDelay = 150;       // Maximum delay between key presses (ms)
    int m_minMouseDelay = 100;     // Minimum delay between mouse actions (ms)
    int m_maxMouseDelay = 200;     // Maximum delay between mouse actions (ms)
    
    // Key state tracking
    std::unordered_map<int, bool> m_keyStates;
    std::unordered_map<int, std::chrono::steady_clock::time_point> m_keyPressTimes;
    
    // Mouse state
    MousePosition m_currentMousePos;
    bool m_mouseButtonsPressed[3] = {false}; // Left, Right, Middle

public:
    InputManager();
    ~InputManager();
    
    // Initialization
    bool initialize();
    bool findGameWindow();
    void setGameWindow(HWND window) { m_gameWindow = window; }
    
    // Keyboard input
    void pressKey(int virtualKey);
    void releaseKey(int virtualKey);
    void holdKey(int virtualKey, int durationMs);
    void typeKey(int virtualKey);
    bool isKeyPressed(int virtualKey) const;
    
    // Mouse input
    void moveMouse(int x, int y, bool relative = false);
    void moveMouseSmooth(int x, int y, int durationMs = 200);
    void clickMouse(ClickType type = ClickType::LEFT_CLICK);
    void clickAt(int x, int y, ClickType type = ClickType::LEFT_CLICK);
    void dragMouse(int fromX, int fromY, int toX, int toY, int durationMs = 500);
    
    // Game-specific input methods
    void moveToPosition(float worldX, float worldY);
    void attackMove(float worldX, float worldY);
    void useAbility(int abilityKey, float targetX = -1, float targetY = -1);
    void openInventory();
    void pickupItem(float itemX, float itemY);
    void usePortal();
    void openMap();
    
    // Input sequences
    void castAbilityAtTarget(int abilityKey, float targetX, float targetY);
    void lootNearbyItems(const std::vector<std::pair<float, float>>& itemPositions);
    void navigateToPoint(float x, float y);
    
    // Utility methods
    MousePosition getMousePosition() const { return m_currentMousePos; }
    MousePosition worldToScreen(float worldX, float worldY) const;
    std::pair<float, float> screenToWorld(int screenX, int screenY) const;
    bool isGameWindowActive() const;
    void activateGameWindow();
    
    // Humanization
    void setKeyDelayRange(int minMs, int maxMs);
    void setMouseDelayRange(int minMs, int maxMs);
    void addRandomDelay(int baseMs, int variationMs = 50);
    
    // Input validation
    bool isValidScreenPosition(int x, int y) const;
    bool isValidWorldPosition(float x, float y) const;

private:
    // Core input methods
    void sendKeyDown(int virtualKey);
    void sendKeyUp(int virtualKey);
    void sendMouseDown(ClickType type);
    void sendMouseUp(ClickType type);
    void sendMouseMove(int x, int y);
    
    // Timing and delays
    void waitForKeyDelay();
    void waitForMouseDelay();
    int getRandomDelay(int min, int max) const;
    
    // Window management
    bool updateWindowRect();
    POINT clientToScreen(int clientX, int clientY) const;
    POINT screenToClient(int screenX, int screenY) const;
    
    // Smooth movement
    std::vector<MousePosition> generateSmoothPath(
        const MousePosition& start, 
        const MousePosition& end, 
        int steps = 10
    ) const;
    
    // Input validation helpers
    bool isKeyValid(int virtualKey) const;
    bool isWindowValid() const;
    
    // Game-specific coordinate conversion
    struct GameCoordinates {
        float minX, maxX, minY, maxY;
        int screenWidth, screenHeight;
    };
    
    GameCoordinates getGameCoordinates() const;
};
