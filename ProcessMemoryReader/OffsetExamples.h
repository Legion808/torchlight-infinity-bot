#pragma once
#include "OffsetManager.h"
#include "Memory.h"
#include <iostream>

/**
 * PRACTICAL EXAMPLES: Dynamic Offset Implementation
 * 
 * This file demonstrates how to implement the offset calculations you asked about:
 * "new_address = base_address + offset" and dynamic offset adjustments
 */

class OffsetExamples {
private:
    std::unique_ptr<OffsetManager> m_offsetManager;
    const Memory* m_memory;
    uintptr_t m_playerBaseAddress;
    uintptr_t m_inventoryBaseAddress;
    uintptr_t m_gameBaseAddress;

public:
    OffsetExamples(const Memory* memory) : m_memory(memory) {
        m_offsetManager = std::make_unique<OffsetManager>(memory);
        
        // Example base addresses (these would be found through pattern scanning)
        m_gameBaseAddress = 0x140000000;      // Game module base
        m_playerBaseAddress = 0x20000000;     // Player object base
        m_inventoryBaseAddress = 0x30000000;  // Inventory base
        
        m_offsetManager->setGameBaseAddress(m_gameBaseAddress);
    }

    // ============ EXAMPLE 1: Basic Offset Calculation ============
    void example1_BasicOffsetCalculation() {
        std::cout << "=== EXAMPLE 1: Basic Offset Calculation ===" << std::endl;
        
        // Method 1: Using OffsetManager
        uintptr_t healthAddress = m_offsetManager->calculateAddress(m_playerBaseAddress, "player_health");
        std::cout << "Player Health Address (Method 1): 0x" << std::hex << healthAddress << std::endl;
        
        // Method 2: Manual calculation - this is what you asked about!
        uintptr_t healthOffset = m_offsetManager->getOffset("player_health");
        uintptr_t healthAddressManual = m_playerBaseAddress + healthOffset;  // new_address = base_address + offset
        std::cout << "Player Health Address (Method 2): 0x" << std::hex << healthAddressManual << std::endl;
        
        // Method 3: Static helper function
        uintptr_t healthAddressStatic = OffsetManager::calculateAddress(m_playerBaseAddress, 0x10);
        std::cout << "Player Health Address (Method 3): 0x" << std::hex << healthAddressStatic << std::endl;
        
        // All three methods should give the same result!
        std::cout << std::dec; // Reset to decimal
    }

    // ============ EXAMPLE 2: Dynamic Offset Adjustment ============
    void example2_DynamicOffsetAdjustment() {
        std::cout << "\n=== EXAMPLE 2: Dynamic Offset Adjustment ===" << std::endl;
        
        // Scenario: Accessing different inventory slots
        // Each slot is 0x20 bytes apart, starting at base + 0x100
        
        for (int slotIndex = 0; slotIndex < 5; ++slotIndex) {
            // Calculate dynamic adjustment for this slot
            uintptr_t slotAdjustment = slotIndex * 0x20;  // Each slot is 0x20 bytes
            
            // Update the dynamic offset
            m_offsetManager->updateDynamicOffset("inventory_slot", slotAdjustment);
            
            // Now calculate the final address
            uintptr_t slotAddress = m_offsetManager->calculateAddress(m_inventoryBaseAddress, "inventory_slot");
            
            std::cout << "Inventory Slot " << slotIndex << " Address: 0x" << std::hex << slotAddress << std::endl;
            
            // Alternative manual calculation:
            uintptr_t baseSlotOffset = 0x100;  // Base inventory offset
            uintptr_t manualSlotAddress = m_inventoryBaseAddress + baseSlotOffset + slotAdjustment;
            std::cout << "Manual calculation: 0x" << std::hex << manualSlotAddress << std::endl;
        }
        std::cout << std::dec;
    }

    // ============ EXAMPLE 3: Reading Data with Dynamic Offsets ============
    void example3_ReadingDataWithDynamicOffsets() {
        std::cout << "\n=== EXAMPLE 3: Reading Data with Dynamic Offsets ===" << std::endl;
        
        // Read player health using calculated address
        uintptr_t healthAddress = m_offsetManager->calculateAddress(m_playerBaseAddress, "player_health");
        
        if (m_offsetManager->validateAddress(healthAddress)) {
            float currentHealth = m_memory->read<float>(healthAddress);
            std::cout << "Current Player Health: " << currentHealth << std::endl;
        }
        
        // Read inventory items in a loop
        for (int slot = 0; slot < 10; ++slot) {
            // Update dynamic offset for current slot
            m_offsetManager->updateDynamicOffset("inventory_slot", slot * 0x20);
            
            // Calculate item address
            uintptr_t itemAddress = m_offsetManager->calculateAddress(m_inventoryBaseAddress, "inventory_slot");
            
            // Read item count at this slot
            uintptr_t itemCountAddress = itemAddress + m_offsetManager->getOffset("inventory_item_count");
            
            if (m_offsetManager->validateAddress(itemCountAddress)) {
                int itemCount = m_memory->read<int>(itemCountAddress);
                if (itemCount > 0) {
                    std::cout << "Slot " << slot << " has " << itemCount << " items" << std::endl;
                }
            }
        }
    }

    // ============ EXAMPLE 4: Multi-Level Pointer Chains ============
    void example4_PointerChains() {
        std::cout << "\n=== EXAMPLE 4: Multi-Level Pointer Chains ===" << std::endl;
        
        // Register pointer chain offsets
        m_offsetManager->registerOffset("player_pointer", 0x50, OffsetManager::OffsetType::PLAYER);
        m_offsetManager->registerOffset("stats_pointer", 0x80, OffsetManager::OffsetType::PLAYER);
        m_offsetManager->registerOffset("detailed_health", 0x10, OffsetManager::OffsetType::PLAYER);
        
        // Follow chain: GameBase -> PlayerPtr -> StatsPtr -> DetailedHealth
        std::vector<std::string> pointerChain = {"player_pointer", "stats_pointer", "detailed_health"};
        uintptr_t finalAddress = m_offsetManager->calculatePointerChain(m_gameBaseAddress, pointerChain);
        
        std::cout << "Final address through pointer chain: 0x" << std::hex << finalAddress << std::endl;
        
        // Manual equivalent of the above:
        /*
        uintptr_t step1 = m_gameBaseAddress + 0x50;           // Game base + player pointer offset
        uintptr_t playerPtr = m_memory->read<uintptr_t>(step1); // Read player pointer
        
        uintptr_t step2 = playerPtr + 0x80;                   // Player + stats pointer offset  
        uintptr_t statsPtr = m_memory->read<uintptr_t>(step2);  // Read stats pointer
        
        uintptr_t finalManual = statsPtr + 0x10;              // Stats + detailed health offset
        */
        
        std::cout << std::dec;
    }

    // ============ EXAMPLE 5: Array Element Access ============
    void example5_ArrayElementAccess() {
        std::cout << "\n=== EXAMPLE 5: Array Element Access ===" << std::endl;
        
        // Scenario: Monster array where each monster is 0x100 bytes
        uintptr_t monsterArrayBase = 0x40000000;
        size_t monsterSize = 0x100;
        
        for (int monsterIndex = 0; monsterIndex < 3; ++monsterIndex) {
            // Calculate monster base address
            uintptr_t monsterAddress = monsterArrayBase + (monsterIndex * monsterSize);
            
            // Calculate monster health address
            uintptr_t monsterHealthAddress = m_offsetManager->calculateAddress(monsterAddress, "entity_health");
            
            // Alternative using array helper
            uintptr_t monsterHealthAddress2 = m_offsetManager->calculateArrayAddress(
                monsterArrayBase, monsterIndex, monsterSize, "entity_health"
            );
            
            std::cout << "Monster " << monsterIndex << " base: 0x" << std::hex << monsterAddress << std::endl;
            std::cout << "Monster " << monsterIndex << " health: 0x" << std::hex << monsterHealthAddress << std::endl;
        }
        std::cout << std::dec;
    }

    // ============ EXAMPLE 6: Runtime Offset Adjustment ============
    void example6_RuntimeOffsetAdjustment() {
        std::cout << "\n=== EXAMPLE 6: Runtime Offset Adjustment ===" << std::endl;
        
        // Scenario: Game updates change offsets, we need to adjust at runtime
        
        std::cout << "Original player health offset: 0x" << std::hex << m_offsetManager->getOffset("player_health") << std::endl;
        
        // Simulate discovering new offset (perhaps through pattern scanning)
        uintptr_t newHealthOffset = 0x18;  // Health moved from 0x10 to 0x18
        
        // Update the offset
        m_offsetManager->removeOffset("player_health");
        m_offsetManager->registerOffset("player_health", newHealthOffset, OffsetManager::OffsetType::PLAYER);
        
        std::cout << "Updated player health offset: 0x" << std::hex << m_offsetManager->getOffset("player_health") << std::endl;
        
        // Now all future calculations will use the new offset automatically
        uintptr_t newHealthAddress = m_offsetManager->calculateAddress(m_playerBaseAddress, "player_health");
        std::cout << "New health address: 0x" << std::hex << newHealthAddress << std::endl;
        
        std::cout << std::dec;
    }

    // ============ RUN ALL EXAMPLES ============
    void runAllExamples() {
        std::cout << "DYNAMIC OFFSET IMPLEMENTATION EXAMPLES\n";
        std::cout << "======================================\n";
        
        example1_BasicOffsetCalculation();
        example2_DynamicOffsetAdjustment();
        example3_ReadingDataWithDynamicOffsets();
        example4_PointerChains();
        example5_ArrayElementAccess();
        example6_RuntimeOffsetAdjustment();
        
        std::cout << "\n=== Debug: All Registered Offsets ===" << std::endl;
        m_offsetManager->printAllOffsets();
    }
};
