# Reverse Engineering Guide for Torchlight Infinity

This document contains instructions for reverse engineering Torchlight Infinity to configure the farming bot.

## Required Tools

### Primary Tools
- **Cheat Engine** - for memory scanning and address finding
- **x64dbg** - debugger for executable analysis
- **Process Hacker** - process and module monitoring
- **HxD** - hex editor for file analysis

### Additional Tools
- **ReClass.NET** - for creating data structures
- **IDA Pro** or **Ghidra** - static analysis
- **API Monitor** - API call tracking

## Reverse Engineering Steps

### 1. Finding Base Addresses

#### Finding Player Address
```cpp
// In GameState.cpp need to replace placeholder addresses with real ones

// Example search through Cheat Engine:
// 1. Find player coordinates (float values)
// 2. Find player health (float, max health)
// 3. Find player level (integer)
// 4. Create pointer chain to base address

// Current placeholder in GameState::scanForPlayerBase():
m_playerBaseAddress = 0x1000000; // REPLACE WITH REAL ADDRESS
```

#### Finding Entity List
```cpp
// In EntityManager.cpp replace:
m_entityListBase = 0x4000000; // REPLACE WITH REAL ADDRESS
m_entityListSize = 0x1000;    // REPLACE WITH REAL SIZE

// Entities are usually stored in array or linked list
// Each entity has ID, position, type, health
```

### 2. Determining Data Structures

#### Player Structure
```cpp
struct PlayerStruct {
    float x, y, z;           // +0x0, +0x4, +0x8
    float health;            // +0x10
    float maxHealth;         // +0x14
    float mana;              // +0x18
    float maxMana;           // +0x1C
    int level;               // +0x20
    bool inCombat;           // +0x24
    bool isDead;             // +0x28
    float movementSpeed;     // +0x2C
    int characterClass;      // +0x30
};
```

#### Entity Structure
```cpp
struct EntityStruct {
    uint64_t id;             // +0x8
    uint32_t type;           // +0x10
    uint32_t padding;        // +0x14
    float x, y, z;           // +0x20, +0x24, +0x28
    float health;            // +0x40
    float maxHealth;         // +0x44
    bool isAlive;            // +0x48
    bool isTargetable;       // +0x4C
    char* name;              // +0x50
    int level;               // +0x60
};
```

### 3. Address Finding Methods

#### 1. Value-based Search
```
Cheat Engine:
1. Scan Type: Exact Value
2. Value Type: Float/4 Bytes
3. Enter current value (e.g., HP)
4. First Scan
5. Change value in game
6. New Scan with new value
7. Repeat until 1-2 addresses
```

#### 2. Change-based Search
```
Cheat Engine:
1. Scan Type: Unknown initial value
2. First Scan
3. Change value in game
4. Scan Type: Changed value
5. Next Scan
6. Repeat process
```

#### 3. Pointer Scan
```
Cheat Engine:
1. Find value address
2. Right click -> Pointer scan for this address
3. Settings: Max level 5, Max different offsets 3
4. Start scanning
5. Check stability after game restart
```

### 4. Finding Entity List

#### Method 1: Through Monster Count
```
1. Find mob counter on screen
2. Pointer scan for this value
3. Investigate memory around found address
4. Look for array of entity pointers
```

#### Method 2: Through Mob Coordinates
```
1. Find coordinates of nearest mob
2. Find pointer chain
3. Investigate structure containing coordinates
4. Find list/array of such structures
```

#### Method 3: Through Entity IDs
```
1. Find unique mob ID (usually incrementing number)
2. Find this ID in memory
3. Investigate surrounding data
4. Find array with such IDs
```

### 5. Entity Type Analysis

#### Type Determination
```cpp
// In EntityManager::determineEntityType() replace placeholder:

switch (typeValue) {
    case 0x1001: return EntityType::MONSTER;
    case 0x2001: return EntityType::BOSS;  
    case 0x3001: return EntityType::ITEM;
    case 0x4001: return EntityType::CHEST;
    case 0x5001: return EntityType::PORTAL;
    case 0x6001: return EntityType::NPC;
    default: return EntityType::UNKNOWN;
}
```

### 6. Coordinate System

#### Finding Coordinate Transformation
```cpp
// In InputManager::worldToScreen() replace placeholder:

// Need to find:
// 1. Camera matrix (4x4)
// 2. Projection matrix (4x4)  
// 3. Viewport parameters
// 4. Screen resolution

// Example transformation function:
POINT worldToScreen(float worldX, float worldY, float worldZ) {
    // Multiply by view matrix
    // Multiply by projection matrix
    // Apply viewport transform
    // Return screen coordinates
}
```

### 7. Finding Addresses through Static Analysis

#### IDA Pro / Ghidra
```
1. Load main executable file
2. Find strings with variable names (Player, Entity, etc.)
3. Find references to these strings
4. Analyze code around these references
5. Find global variables and static addresses
```

#### API Hooking
```cpp
// Track DirectX/OpenGL calls for rendering
// Find where game draws entities
// Get addresses from rendering function parameters

// Example with SetTransform:
HRESULT WINAPI HookedSetTransform(
    IDirect3DDevice9* device,
    D3DTRANSFORMSTATETYPE state,
    CONST D3DMATRIX* matrix) {
    
    if (state == D3DTS_WORLD) {
        // matrix contains world coordinates of entity
        // Log or save address
    }
    
    return originalSetTransform(device, state, matrix);
}
```

### 8. Code Updates

#### After finding addresses, update:

1. **GameState.cpp**:
```cpp
bool GameState::scanForPlayerBase() {
    // Replace with real addresses/patterns
    m_playerBaseAddress = findPlayerBaseAddress();
    return m_playerBaseAddress != 0;
}
```

2. **EntityManager.cpp**:
```cpp
bool EntityManager::findEntityList() {
    // Replace with real addresses
    m_entityListBase = findEntityListAddress();
    m_entityListSize = getEntityListSize();
    return m_entityListBase != 0;
}
```

3. **InputManager.cpp**:
```cpp
InputManager::MousePosition InputManager::worldToScreen(float worldX, float worldY) const {
    // Replace with real calculations
    return calculateScreenPosition(worldX, worldY);
}
```

### 9. Testing and Validation

#### Address Tests
```cpp
// Add validation to code:
bool GameState::validateAddresses() {
    // Check that addresses are valid
    // Check that values are reasonable
    // Check stability after restart
    return true;
}
```

#### Debug Logging
```cpp
// Add detailed logging:
m_logger->debug("Player position: %.2f, %.2f, %.2f", x, y, z);
m_logger->debug("Found %d entities", entityCount);
m_logger->debug("Entity %d: type=%d, pos=(%.2f,%.2f)", id, type, x, y);
```

### 10. Automatic Updates

#### Pattern Scanning
```cpp
// Implement signature searching:
uintptr_t findPattern(const char* pattern, const char* mask) {
    // Search byte pattern in process memory
    // Returns address of found pattern
}

// Example usage:
uintptr_t playerBase = findPattern(
    "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00\x48\x8B\x40\x00", 
    "xxx????xxxx?xxx?"
);
```

## Security Notes

1. **Use separate game copy** for testing
2. **Don't run on main account** during development
3. **Make save backups**
4. **Test in offline mode** when possible

## Documenting Results

Create `addresses.txt` file with found addresses:
```
Player Base: 0x12345678
Entity List: 0x87654321
Camera Matrix: 0xABCDEF00
Game State: 0x11111111
```

## Supporting Game Updates

After game updates:
1. Check if old addresses work
2. Re-run pattern scanning if needed
3. Update constants in code
4. Test all bot functions
