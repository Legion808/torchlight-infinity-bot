#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <limits>
#include <string>

// Forward declarations
class Memory;
class GameState;

/**
 * @brief Manages entity detection and tracking in the game world
 */
class EntityManager {
public:
    enum class EntityType {
        UNKNOWN,
        PLAYER,
        MONSTER,
        BOSS,
        NPC,
        ITEM,
        CHEST,
        PORTAL,
        WAYPOINT,
        SEASONAL_OBJECT
    };

    struct Entity {
        uint64_t id;                   // Unique entity identifier
        EntityType type;               // Type of entity
        float x, y, z;                 // World position
        float health, maxHealth;       // Health values (for living entities)
        bool isAlive;                  // Alive status
        bool isTargetable;             // Can be targeted
        bool isVisible;                // Visible on screen
        std::string name;              // Entity name/identifier
        int level;                     // Entity level (for monsters/NPCs)
        float threatLevel;             // Calculated threat (for monsters)
        std::chrono::time_point<std::chrono::steady_clock> lastSeen; // Last detection time
        
        // Type-specific data
        union {
            struct { // For monsters
                bool isElite;
                bool isBoss;
                float attackRange;
                float movementSpeed;
            } monster;
            
            struct { // For items
                int rarity;            // Item rarity level
                std::string itemType;  // Type of item
                bool isFiltered;       // Matches loot filter
            } item;
            
            struct { // For seasonal objects
                std::string eventType;
                bool isInteractable;
                float interactionRange;
            } seasonal;
        } data;
    };

private:
    const Memory* m_memory;
    const GameState* m_gameState;
    
    std::unordered_map<uint64_t, Entity> m_entities;
    std::vector<uint64_t> m_recentlyRemoved; // Recently removed entities
    
    // Entity list scanning
    uintptr_t m_entityListBase = 0;
    uintptr_t m_entityListSize = 0;
    
    // Entity structure offsets (need to be found via reverse engineering)
    struct EntityOffsets {
        uintptr_t id = 0x8;
        uintptr_t type = 0x10;
        uintptr_t position = 0x20;     // X, Y, Z
        uintptr_t health = 0x40;
        uintptr_t maxHealth = 0x44;
        uintptr_t isAlive = 0x48;
        uintptr_t isTargetable = 0x4C;
        uintptr_t name = 0x50;
        uintptr_t level = 0x60;
    } m_offsets;
    
    // Filtering functions
    std::function<bool(const Entity&)> m_monsterFilter;
    std::function<bool(const Entity&)> m_itemFilter;
    std::function<bool(const Entity&)> m_interactableFilter;

public:
    EntityManager(const Memory* memory, const GameState* gameState);
    
    // Core update method
    bool update();
    
    // Entity retrieval
    std::vector<Entity> getAllEntities() const;
    std::vector<Entity> getEntitiesByType(EntityType type) const;
    std::vector<Entity> getNearbyEntities(float x, float y, float radius) const;
    std::vector<Entity> getTargetableEnemies() const;
    std::vector<Entity> getLootableItems() const;
    std::vector<Entity> getInteractableObjects() const;
    
    // Specific entity searches
    Entity* findNearestEnemy(float playerX, float playerY);
    Entity* findNearestItem(float playerX, float playerY);
    Entity* findBoss();
    std::vector<Entity> findSeasonalObjects();
    
    // Entity state queries
    bool hasNearbyEnemies(float x, float y, float radius = 20.0f) const;
    bool hasLootableItems(float x, float y, float radius = 10.0f) const;
    int getEnemyCount() const;
    int getAliveEnemyCount() const;
    
    // Filtering configuration
    void setMonsterFilter(std::function<bool(const Entity&)> filter);
    void setItemFilter(std::function<bool(const Entity&)> filter);
    void setInteractableFilter(std::function<bool(const Entity&)> filter);
    
    // Memory management
    bool findEntityList();
    void clearEntities();
    void removeStaleEntities();

private:
    bool scanEntityList();
    bool parseEntity(uintptr_t entityAddress, Entity& entity);
    EntityType determineEntityType(uintptr_t typeValue) const;
    float calculateThreatLevel(const Entity& entity) const;
    bool isEntityValid(const Entity& entity) const;
    void updateEntityVisibility(Entity& entity);
    std::string readEntityName(uintptr_t nameAddress) const;
};
