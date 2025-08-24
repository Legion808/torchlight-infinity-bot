#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class Memory;

/**
 * Dynamic Offset Management System
 * Handles both static and dynamic offset calculations
 */
class OffsetManager {
public:
    // Offset types for different data categories
    enum class OffsetType {
        PLAYER,
        INVENTORY,
        MONSTER,
        MAP,
        UI,
        CUSTOM
    };

    // Offset entry structure
    struct OffsetEntry {
        std::string name;           // Offset identifier
        uintptr_t staticValue;      // Base static offset
        uintptr_t dynamicAdjustment; // Runtime adjustment
        bool isDynamic;             // Whether this offset changes at runtime
        OffsetType type;            // Category of this offset
        
        OffsetEntry(const std::string& n, uintptr_t val, OffsetType t = OffsetType::CUSTOM, bool dynamic = false)
            : name(n), staticValue(val), dynamicAdjustment(0), isDynamic(dynamic), type(t) {}
    };

private:
    std::unordered_map<std::string, OffsetEntry> m_offsets;
    const Memory* m_memory;
    uintptr_t m_gameBaseAddress;
    
    // Initialize default offsets
    void initializeDefaultOffsets();

public:
    explicit OffsetManager(const Memory* memory);
    
    // ============ BASIC OFFSET OPERATIONS ============
    
    /**
     * Register a static offset
     * Example: registerOffset("player_health", 0x10, OffsetType::PLAYER);
     */
    void registerOffset(const std::string& name, uintptr_t offset, OffsetType type = OffsetType::CUSTOM);
    
    /**
     * Register a dynamic offset that can change at runtime
     * Example: registerDynamicOffset("inventory_slot", 0x100, OffsetType::INVENTORY);
     */
    void registerDynamicOffset(const std::string& name, uintptr_t baseOffset, OffsetType type = OffsetType::CUSTOM);
    
    /**
     * Get the final calculated offset value
     * Formula: final_offset = static_value + dynamic_adjustment
     */
    uintptr_t getOffset(const std::string& name) const;
    
    /**
     * Update dynamic adjustment for an offset
     * Example: updateDynamicOffset("inventory_slot", slotIndex * 0x8);
     */
    void updateDynamicOffset(const std::string& name, uintptr_t adjustment);
    
    // ============ ADDRESS CALCULATION HELPERS ============
    
    /**
     * Calculate final memory address
     * Formula: final_address = base_address + offset
     * Example: calculateAddress(playerBase, "player_health") -> playerBase + 0x10
     */
    uintptr_t calculateAddress(uintptr_t baseAddress, const std::string& offsetName) const;
    
    /**
     * Calculate address with manual offset
     * Formula: final_address = base_address + offset
     */
    static uintptr_t calculateAddress(uintptr_t baseAddress, uintptr_t offset);
    
    /**
     * Calculate address for array elements
     * Formula: final_address = base_address + (index * element_size) + offset
     * Example: calculateArrayAddress(inventoryBase, 5, 0x8, "item_data") for 6th item
     */
    uintptr_t calculateArrayAddress(uintptr_t baseAddress, size_t index, size_t elementSize, const std::string& offsetName) const;
    
    // ============ MULTI-LEVEL POINTER CHAINS ============
    
    /**
     * Calculate address through pointer chain with offsets
     * Example: calculatePointerChain(gameBase, {"player_ptr", "stats_ptr", "health"})
     * This does: gameBase -> [gameBase + player_ptr_offset] -> [result + stats_ptr_offset] -> [result + health_offset]
     */
    uintptr_t calculatePointerChain(uintptr_t startAddress, const std::vector<std::string>& offsetChain) const;
    
    // ============ UTILITY METHODS ============
    
    /**
     * Set the game's base address (usually the main module base)
     */
    void setGameBaseAddress(uintptr_t baseAddress) { m_gameBaseAddress = baseAddress; }
    
    /**
     * Get all offsets of a specific type
     */
    std::vector<OffsetEntry> getOffsetsByType(OffsetType type) const;
    
    /**
     * Check if an offset exists
     */
    bool hasOffset(const std::string& name) const;
    
    /**
     * Remove an offset
     */
    void removeOffset(const std::string& name);
    
    /**
     * Clear all offsets
     */
    void clearOffsets();
    
    /**
     * Load offsets from configuration file
     */
    bool loadOffsetsFromFile(const std::string& filename);
    
    /**
     * Save current offsets to configuration file
     */
    bool saveOffsetsToFile(const std::string& filename) const;
    
    // ============ DEBUG AND VALIDATION ============
    
    /**
     * Validate that a calculated address is readable
     */
    bool validateAddress(uintptr_t address) const;
    
    /**
     * Print all registered offsets (for debugging)
     */
    void printAllOffsets() const;
};
