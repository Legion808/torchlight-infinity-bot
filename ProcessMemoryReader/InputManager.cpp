#include "InputManager.h"
#include <random>
#include <thread>

InputManager::InputManager() : m_gameWindow(nullptr), m_currentMousePos(0, 0) {
    updateWindowRect();
}

InputManager::~InputManager() {
    // Release any held keys
    for (const auto& [key, pressed] : m_keyStates) {
        if (pressed) {
            sendKeyUp(key);
        }
    }
}

bool InputManager::initialize() {
    return findGameWindow();
}

bool InputManager::findGameWindow() {
    // Try to find Torchlight Infinity window
    std::vector<std::string> windowTitles = {
        "Torchlight Infinity",
        "Torchlight 3",
        "TL3",
        "Game Window"
    };
    
    for (const auto& title : windowTitles) {
        HWND window = FindWindowA(nullptr, title.c_str());
        if (window != nullptr) {
            m_gameWindow = window;
            updateWindowRect();
            return true;
        }
    }
    
    return false;
}

void InputManager::pressKey(int virtualKey) {
    waitForKeyDelay();
    sendKeyDown(virtualKey);
    m_keyStates[virtualKey] = true;
    m_keyPressTimes[virtualKey] = std::chrono::steady_clock::now();
}

void InputManager::releaseKey(int virtualKey) {
    sendKeyUp(virtualKey);
    m_keyStates[virtualKey] = false;
}

void InputManager::holdKey(int virtualKey, int durationMs) {
    pressKey(virtualKey);
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    releaseKey(virtualKey);
}

void InputManager::typeKey(int virtualKey) {
    pressKey(virtualKey);
    addRandomDelay(50, 25);
    releaseKey(virtualKey);
}

bool InputManager::isKeyPressed(int virtualKey) const {
    auto it = m_keyStates.find(virtualKey);
    return it != m_keyStates.end() && it->second;
}

void InputManager::moveMouse(int x, int y, bool relative) {
    waitForMouseDelay();
    
    if (relative) {
        POINT currentPos;
        GetCursorPos(&currentPos);
        x += currentPos.x;
        y += currentPos.y;
    }
    
    sendMouseMove(x, y);
    m_currentMousePos = MousePosition(x, y);
}

void InputManager::moveMouseSmooth(int x, int y, int durationMs) {
    POINT start;
    GetCursorPos(&start);
    
    MousePosition startPos(start.x, start.y);
    MousePosition endPos(x, y);
    
    auto path = generateSmoothPath(startPos, endPos, durationMs / 50);
    
    for (const auto& point : path) {
        sendMouseMove(point.x, point.y);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    m_currentMousePos = endPos;
}

void InputManager::clickMouse(ClickType type) {
    waitForMouseDelay();
    sendMouseDown(type);
    addRandomDelay(50, 25);
    sendMouseUp(type);
}

void InputManager::clickAt(int x, int y, ClickType type) {
    moveMouse(x, y);
    addRandomDelay(100, 50);
    clickMouse(type);
}

void InputManager::dragMouse(int fromX, int fromY, int toX, int toY, int durationMs) {
    moveMouse(fromX, fromY);
    sendMouseDown(ClickType::LEFT_CLICK);
    
    addRandomDelay(100, 50);
    moveMouseSmooth(toX, toY, durationMs);
    
    sendMouseUp(ClickType::LEFT_CLICK);
}

void InputManager::moveToPosition(float worldX, float worldY) {
    auto screenPos = worldToScreen(worldX, worldY);
    clickAt(screenPos.x, screenPos.y, ClickType::RIGHT_CLICK);
}

void InputManager::attackMove(float worldX, float worldY) {
    auto screenPos = worldToScreen(worldX, worldY);
    
    // Hold shift for force move, then right click
    pressKey(VK_SHIFT);
    addRandomDelay(50, 25);
    clickAt(screenPos.x, screenPos.y, ClickType::RIGHT_CLICK);
    releaseKey(VK_SHIFT);
}

void InputManager::useAbility(int abilityKey, float targetX, float targetY) {
    if (targetX >= 0 && targetY >= 0) {
        auto screenPos = worldToScreen(targetX, targetY);
        moveMouse(screenPos.x, screenPos.y);
    }
    
    typeKey(abilityKey);
}

void InputManager::openInventory() {
    typeKey('I');
}

void InputManager::pickupItem(float itemX, float itemY) {
    auto screenPos = worldToScreen(itemX, itemY);
    clickAt(screenPos.x, screenPos.y, ClickType::LEFT_CLICK);
}

void InputManager::usePortal() {
    typeKey('T');
}

void InputManager::openMap() {
    typeKey('M');
}

void InputManager::castAbilityAtTarget(int abilityKey, float targetX, float targetY) {
    auto screenPos = worldToScreen(targetX, targetY);
    moveMouse(screenPos.x, screenPos.y);
    addRandomDelay(100, 50);
    typeKey(abilityKey);
}

void InputManager::lootNearbyItems(const std::vector<std::pair<float, float>>& itemPositions) {
    for (const auto& [x, y] : itemPositions) {
        pickupItem(x, y);
        addRandomDelay(200, 100);
    }
}

void InputManager::navigateToPoint(float x, float y) {
    moveToPosition(x, y);
}

InputManager::MousePosition InputManager::worldToScreen(float worldX, float worldY) const {
    // This is a placeholder conversion - would need actual game camera/projection matrix
    // For now, return a basic conversion assuming a simple mapping
    
    int screenX = static_cast<int>(worldX * 10 + m_windowRect.left + 400);
    int screenY = static_cast<int>(worldY * 10 + m_windowRect.top + 300);
    
    return MousePosition(screenX, screenY);
}

std::pair<float, float> InputManager::screenToWorld(int screenX, int screenY) const {
    // Placeholder reverse conversion
    float worldX = (screenX - m_windowRect.left - 400) / 10.0f;
    float worldY = (screenY - m_windowRect.top - 300) / 10.0f;
    
    return {worldX, worldY};
}

bool InputManager::isGameWindowActive() const {
    return GetForegroundWindow() == m_gameWindow;
}

void InputManager::activateGameWindow() {
    if (m_gameWindow) {
        SetForegroundWindow(m_gameWindow);
        SetActiveWindow(m_gameWindow);
    }
}

void InputManager::setKeyDelayRange(int minMs, int maxMs) {
    m_minKeyDelay = minMs;
    m_maxKeyDelay = maxMs;
}

void InputManager::setMouseDelayRange(int minMs, int maxMs) {
    m_minMouseDelay = minMs;
    m_maxMouseDelay = maxMs;
}

void InputManager::addRandomDelay(int baseMs, int variationMs) {
    int delay = getRandomDelay(baseMs - variationMs, baseMs + variationMs);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

bool InputManager::isValidScreenPosition(int x, int y) const {
    return x >= m_windowRect.left && x <= m_windowRect.right &&
           y >= m_windowRect.top && y <= m_windowRect.bottom;
}

bool InputManager::isValidWorldPosition(float x, float y) const {
    // Basic validation - would need actual game world bounds
    return x >= -1000 && x <= 1000 && y >= -1000 && y <= 1000;
}

void InputManager::sendKeyDown(int virtualKey) {
    if (!isKeyValid(virtualKey)) return;
    
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = 0;
    
    SendInput(1, &input, sizeof(INPUT));
    m_lastKeyPress = std::chrono::steady_clock::now();
}

void InputManager::sendKeyUp(int virtualKey) {
    if (!isKeyValid(virtualKey)) return;
    
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(1, &input, sizeof(INPUT));
}

void InputManager::sendMouseDown(ClickType type) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    
    switch (type) {
        case ClickType::LEFT_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            m_mouseButtonsPressed[0] = true;
            break;
        case ClickType::RIGHT_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            m_mouseButtonsPressed[1] = true;
            break;
        case ClickType::MIDDLE_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            m_mouseButtonsPressed[2] = true;
            break;
    }
    
    SendInput(1, &input, sizeof(INPUT));
    m_lastMouseClick = std::chrono::steady_clock::now();
}

void InputManager::sendMouseUp(ClickType type) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    
    switch (type) {
        case ClickType::LEFT_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            m_mouseButtonsPressed[0] = false;
            break;
        case ClickType::RIGHT_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            m_mouseButtonsPressed[1] = false;
            break;
        case ClickType::MIDDLE_CLICK:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            m_mouseButtonsPressed[2] = false;
            break;
    }
    
    SendInput(1, &input, sizeof(INPUT));
}

void InputManager::sendMouseMove(int x, int y) {
    SetCursorPos(x, y);
    m_lastMouseMove = std::chrono::steady_clock::now();
}

void InputManager::waitForKeyDelay() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastKeyPress);
    int minDelay = getRandomDelay(m_minKeyDelay, m_maxKeyDelay);
    
    if (elapsed.count() < minDelay) {
        std::this_thread::sleep_for(std::chrono::milliseconds(minDelay - elapsed.count()));
    }
}

void InputManager::waitForMouseDelay() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastMouseClick);
    int minDelay = getRandomDelay(m_minMouseDelay, m_maxMouseDelay);
    
    if (elapsed.count() < minDelay) {
        std::this_thread::sleep_for(std::chrono::milliseconds(minDelay - elapsed.count()));
    }
}

int InputManager::getRandomDelay(int min, int max) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

bool InputManager::updateWindowRect() {
    if (m_gameWindow) {
        return GetClientRect(m_gameWindow, &m_windowRect) != 0;
    }
    return false;
}

POINT InputManager::clientToScreen(int clientX, int clientY) const {
    POINT point = {clientX, clientY};
    ClientToScreen(m_gameWindow, &point);
    return point;
}

POINT InputManager::screenToClient(int screenX, int screenY) const {
    POINT point = {screenX, screenY};
    ScreenToClient(m_gameWindow, &point);
    return point;
}

std::vector<InputManager::MousePosition> InputManager::generateSmoothPath(
    const MousePosition& start, const MousePosition& end, int steps) const {
    
    std::vector<MousePosition> path;
    path.reserve(steps);
    
    for (int i = 0; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        
        // Add some curve to make movement more natural
        float curveT = t * t * (3.0f - 2.0f * t); // Smoothstep
        
        int x = static_cast<int>(start.x + (end.x - start.x) * curveT);
        int y = static_cast<int>(start.y + (end.y - start.y) * curveT);
        
        path.emplace_back(x, y);
    }
    
    return path;
}

bool InputManager::isKeyValid(int virtualKey) const {
    return virtualKey >= 0 && virtualKey <= 255;
}

bool InputManager::isWindowValid() const {
    return m_gameWindow != nullptr && IsWindow(m_gameWindow);
}

InputManager::GameCoordinates InputManager::getGameCoordinates() const {
    GameCoordinates coords;
    coords.minX = -100.0f;
    coords.maxX = 100.0f;
    coords.minY = -100.0f;
    coords.maxY = 100.0f;
    coords.screenWidth = m_windowRect.right - m_windowRect.left;
    coords.screenHeight = m_windowRect.bottom - m_windowRect.top;
    
    return coords;
}
