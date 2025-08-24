# Dynamic Offset Implementation Guide

## Overview
This document explains how to implement dynamic offset calculations using the formula:
```
new_address = base_address + offset
```

## Key Concepts

### 1. Basic Offset Calculation
The fundamental formula for memory address calculation:
```cpp
uintptr_t final_address = base_address + offset;
```

**Example:**
```cpp
uintptr_t playerBase = 0x20000000;    // Player object base address
uintptr_t healthOffset = 0x10;        // Health is at offset 0x10
uintptr_t healthAddress = playerBase + healthOffset;  // Result: 0x20000010

// Read the health value
float health = memory->read<float>(healthAddress);
```

### 2. Dynamic Offset Management
The `OffsetManager` class provides several ways to handle offsets:

#### Method 1: Using OffsetManager (Recommended)
```cpp
// Register an offset
offsetManager->registerOffset("player_health", 0x10, OffsetType::PLAYER);

// Calculate address using offset name
uintptr_t healthAddress = offsetManager->calculateAddress(playerBase, "player_health");
```

#### Method 2: Manual Calculation
```cpp
// Get offset value and calculate manually
uintptr_t healthOffset = offsetManager->getOffset("player_health");
uintptr_t healthAddress = playerBase + healthOffset;  // new_address = base_address + offset
```

#### Method 3: Static Helper
```cpp
// Direct calculation with known offset
uintptr_t healthAddress = OffsetManager::calculateAddress(playerBase, 0x10);
```

### 3. Dynamic Offset Adjustment
For offsets that change at runtime (like array elements):

```cpp
// Example: Inventory slots (each slot is 0x20 bytes apart)
for (int slot = 0; slot < 10; ++slot) {
    // Calculate dynamic adjustment
    uintptr_t slotAdjustment = slot * 0x20;
    
    // Update the dynamic offset
    offsetManager->updateDynamicOffset("inventory_slot", slotAdjustment);
    
    // Calculate final address: base + static_offset + dynamic_adjustment
    uintptr_t slotAddress = offsetManager->calculateAddress(inventoryBase, "inventory_slot");
    
    // Alternative manual calculation:
    uintptr_t manualAddress = inventoryBase + 0x100 + slotAdjustment;
}
```

### 4. Array Element Access
For accessing elements in arrays:

```cpp
// Formula: address = base + (index * element_size) + offset
uintptr_t monsterBase = 0x40000000;
size_t monsterSize = 0x100;        // Each monster is 0x100 bytes
int monsterIndex = 5;              // Access 6th monster (0-based)

// Calculate monster address
uintptr_t monsterAddress = monsterBase + (monsterIndex * monsterSize);

// Calculate monster health address
uintptr_t healthOffset = 0x20;
uintptr_t monsterHealthAddress = monsterAddress + healthOffset;

// Using OffsetManager helper
uintptr_t healthAddress2 = offsetManager->calculateArrayAddress(
    monsterBase, monsterIndex, monsterSize, "entity_health"
);
```

### 5. Multi-Level Pointer Chains
For following pointer chains through memory:

```cpp
// Chain: GameBase -> PlayerPtr -> StatsPtr -> HealthValue
// Registers the chain offsets
offsetManager->registerOffset("player_pointer", 0x50);
offsetManager->registerOffset("stats_pointer", 0x80);
offsetManager->registerOffset("detailed_health", 0x10);

// Follow the chain automatically
std::vector<std::string> chain = {"player_pointer", "stats_pointer", "detailed_health"};
uintptr_t finalAddress = offsetManager->calculatePointerChain(gameBase, chain);

// Manual equivalent:
uintptr_t step1 = gameBase + 0x50;                      // Game + player pointer offset
uintptr_t playerPtr = memory->read<uintptr_t>(step1);   // Read player pointer value
uintptr_t step2 = playerPtr + 0x80;                     // Player + stats pointer offset
uintptr_t statsPtr = memory->read<uintptr_t>(step2);    // Read stats pointer value
uintptr_t finalManual = statsPtr + 0x10;                // Stats + health offset
```

### 6. Runtime Offset Updates
Adjusting offsets when game updates change memory layout:

```cpp
// Original offset
std::cout << "Original: 0x" << std::hex << offsetManager->getOffset("player_health") << std::endl;

// Game update changed the offset from 0x10 to 0x18
offsetManager->removeOffset("player_health");
offsetManager->registerOffset("player_health", 0x18, OffsetType::PLAYER);

// All future calculations now use the new offset automatically
uintptr_t newHealthAddress = offsetManager->calculateAddress(playerBase, "player_health");
```

## Practical Examples

### Reading Player Data
```cpp
class PlayerDataReader {
private:
    std::unique_ptr<OffsetManager> offsetManager;
    const Memory* memory;
    uintptr_t playerBase;

public:
    void readPlayerStats() {
        // Method 1: Using offset names
        float health = memory->read<float>(
            offsetManager->calculateAddress(playerBase, "player_health")
        );
        
        // Method 2: Manual calculation (your requested formula)
        uintptr_t healthOffset = offsetManager->getOffset("player_health");
        uintptr_t healthAddress = playerBase + healthOffset;  // new_address = base_address + offset
        float health2 = memory->read<float>(healthAddress);
        
        // Method 3: Direct offset
        float health3 = memory->read<float>(playerBase + 0x10);
    }
};
```

### Reading Inventory Items
```cpp
void readInventorySlots() {
    uintptr_t inventoryBase = 0x30000000;
    
    for (int slot = 0; slot < 40; ++slot) {
        // Each slot is 0x20 bytes apart, starting at offset 0x100
        uintptr_t slotOffset = 0x100 + (slot * 0x20);
        uintptr_t slotAddress = inventoryBase + slotOffset;  // new_address = base_address + offset
        
        // Read item data at this slot
        int itemId = memory->read<int>(slotAddress);
        int itemCount = memory->read<int>(slotAddress + 0x8);
        
        if (itemId != 0) {
            std::cout << "Slot " << slot << ": Item " << itemId << " x" << itemCount << std::endl;
        }
    }
}
```

### Monster Scanning
```cpp
void scanMonsters() {
    uintptr_t monsterArrayBase = 0x50000000;
    size_t maxMonsters = 100;
    size_t monsterSize = 0x200;  // Each monster structure is 0x200 bytes
    
    for (size_t i = 0; i < maxMonsters; ++i) {
        // Calculate monster base address
        uintptr_t monsterAddress = monsterArrayBase + (i * monsterSize);
        
        // Read monster data using offsets
        bool isAlive = memory->read<bool>(monsterAddress + 0x44);
        if (!isAlive) continue;
        
        float health = memory->read<float>(monsterAddress + 0x20);
        float x = memory->read<float>(monsterAddress + 0x0);
        float y = memory->read<float>(monsterAddress + 0x4);
        
        std::cout << "Monster " << i << " at (" << x << ", " << y << ") with " << health << " HP" << std::endl;
    }
}
```

## Summary

The key formula you asked about is:
```cpp
new_address = base_address + offset
```

This is implemented in several ways:
1. **Manual**: `uintptr_t address = base + offset;`
2. **OffsetManager**: `offsetManager->calculateAddress(base, "offset_name");`
3. **Dynamic**: `offsetManager->updateDynamicOffset("name", adjustment);`

The OffsetManager provides a flexible system for:
- Managing static offsets
- Handling dynamic offset adjustments
- Following pointer chains
- Runtime offset updates
- Address validation

This system allows you to easily adapt to game updates and manage complex memory structures while maintaining clean, readable code.
