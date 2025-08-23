#include "EntityManager.h"
#include "Memory.h"
#include "GameState.h"

EntityManager::EntityManager(const Memory* memory, const GameState* gameState) 
    : m_memory(memory), m_gameState(gameState) {
}

bool EntityManager::update() {
    if (!m_memory || !m_gameState) {
        return false;
    }
    
    // Clear stale entities
    removeStaleEntities();
    
    // Scan for new entities
    return scanEntityList();
}

std::vector<EntityManager::Entity> EntityManager::getAllEntities() const {
    std::vector<Entity> entities;
    entities.reserve(m_entities.size());
    
    for (const auto& [id, entity] : m_entities) {
        entities.push_back(entity);
    }
    
    return entities;
}

std::vector<EntityManager::Entity> EntityManager::getEntitiesByType(EntityType type) const {
    std::vector<Entity> entities;
    
    for (const auto& [id, entity] : m_entities) {
        if (entity.type == type) {
            entities.push_back(entity);
        }
    }
    
    return entities;
}

std::vector<EntityManager::Entity> EntityManager::getNearbyEntities(float x, float y, float radius) const {
    std::vector<Entity> entities;
    float radiusSquared = radius * radius;
    
    for (const auto& [id, entity] : m_entities) {
        float dx = entity.x - x;
        float dy = entity.y - y;
        float distanceSquared = dx * dx + dy * dy;
        
        if (distanceSquared <= radiusSquared) {
            entities.push_back(entity);
        }
    }
    
    return entities;
}

std::vector<EntityManager::Entity> EntityManager::getTargetableEnemies() const {
    std::vector<Entity> enemies;
    
    for (const auto& [id, entity] : m_entities) {
        if ((entity.type == EntityType::MONSTER || entity.type == EntityType::BOSS) &&
            entity.isAlive && entity.isTargetable) {
            enemies.push_back(entity);
        }
    }
    
    return enemies;
}

std::vector<EntityManager::Entity> EntityManager::getLootableItems() const {
    return getEntitiesByType(EntityType::ITEM);
}

std::vector<EntityManager::Entity> EntityManager::getInteractableObjects() const {
    std::vector<Entity> objects;
    
    for (const auto& [id, entity] : m_entities) {
        if (entity.type == EntityType::CHEST || 
            entity.type == EntityType::PORTAL ||
            entity.type == EntityType::WAYPOINT ||
            entity.type == EntityType::SEASONAL_OBJECT) {
            objects.push_back(entity);
        }
    }
    
    return objects;
}

EntityManager::Entity* EntityManager::findNearestEnemy(float playerX, float playerY) {
    Entity* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();
    
    for (auto& [id, entity] : m_entities) {
        if ((entity.type == EntityType::MONSTER || entity.type == EntityType::BOSS) &&
            entity.isAlive && entity.isTargetable) {
            
            float dx = entity.x - playerX;
            float dy = entity.y - playerY;
            float distance = dx * dx + dy * dy;
            
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearest = &entity;
            }
        }
    }
    
    return nearest;
}

EntityManager::Entity* EntityManager::findNearestItem(float playerX, float playerY) {
    Entity* nearest = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();
    
    for (auto& [id, entity] : m_entities) {
        if (entity.type == EntityType::ITEM) {
            float dx = entity.x - playerX;
            float dy = entity.y - playerY;
            float distance = dx * dx + dy * dy;
            
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearest = &entity;
            }
        }
    }
    
    return nearest;
}

EntityManager::Entity* EntityManager::findBoss() {
    for (auto& [id, entity] : m_entities) {
        if (entity.type == EntityType::BOSS && entity.isAlive) {
            return &entity;
        }
    }
    
    return nullptr;
}

std::vector<EntityManager::Entity> EntityManager::findSeasonalObjects() {
    return getEntitiesByType(EntityType::SEASONAL_OBJECT);
}

bool EntityManager::hasNearbyEnemies(float x, float y, float radius) const {
    return !getNearbyEntities(x, y, radius).empty();
}

bool EntityManager::hasLootableItems(float x, float y, float radius) const {
    auto nearby = getNearbyEntities(x, y, radius);
    
    for (const auto& entity : nearby) {
        if (entity.type == EntityType::ITEM) {
            return true;
        }
    }
    
    return false;
}

int EntityManager::getEnemyCount() const {
    int count = 0;
    
    for (const auto& [id, entity] : m_entities) {
        if (entity.type == EntityType::MONSTER || entity.type == EntityType::BOSS) {
            count++;
        }
    }
    
    return count;
}

int EntityManager::getAliveEnemyCount() const {
    int count = 0;
    
    for (const auto& [id, entity] : m_entities) {
        if ((entity.type == EntityType::MONSTER || entity.type == EntityType::BOSS) && 
            entity.isAlive) {
            count++;
        }
    }
    
    return count;
}

void EntityManager::setMonsterFilter(std::function<bool(const Entity&)> filter) {
    m_monsterFilter = filter;
}

void EntityManager::setItemFilter(std::function<bool(const Entity&)> filter) {
    m_itemFilter = filter;
}

void EntityManager::setInteractableFilter(std::function<bool(const Entity&)> filter) {
    m_interactableFilter = filter;
}

bool EntityManager::findEntityList() {
    // Placeholder for finding entity list in memory
    // This would require reverse engineering the game
    m_entityListBase = 0x4000000; // Placeholder address
    m_entityListSize = 0x1000;    // Placeholder size
    
    return m_entityListBase != 0;
}

void EntityManager::clearEntities() {
    m_entities.clear();
    m_recentlyRemoved.clear();
}

void EntityManager::removeStaleEntities() {
    auto now = std::chrono::steady_clock::now();
    auto staleThreshold = std::chrono::seconds(5);
    
    auto it = m_entities.begin();
    while (it != m_entities.end()) {
        if (now - it->second.lastSeen > staleThreshold) {
            m_recentlyRemoved.push_back(it->first);
            it = m_entities.erase(it);
        } else {
            ++it;
        }
    }
    
    // Keep recently removed list small
    if (m_recentlyRemoved.size() > 100) {
        m_recentlyRemoved.erase(m_recentlyRemoved.begin(), 
                               m_recentlyRemoved.begin() + 50);
    }
}

bool EntityManager::scanEntityList() {
    // Placeholder for entity list scanning
    // This would involve reading from the game's entity list in memory
    
    // For now, return true to indicate scanning was attempted
    return true;
}

bool EntityManager::parseEntity(uintptr_t entityAddress, Entity& entity) {
    if (!m_memory || !m_memory->isValidAddress(entityAddress)) {
        return false;
    }
    
    try {
        // Read entity data from memory
        entity.id = m_memory->read<uint64_t>(entityAddress + m_offsets.id);
        
        // Read position
        auto position = m_memory->readArray<float>(entityAddress + m_offsets.position, 3);
        entity.x = position[0];
        entity.y = position[1];
        entity.z = position[2];
        
        // Read other properties
        entity.health = m_memory->read<float>(entityAddress + m_offsets.health);
        entity.maxHealth = m_memory->read<float>(entityAddress + m_offsets.maxHealth);
        entity.isAlive = m_memory->read<bool>(entityAddress + m_offsets.isAlive);
        entity.isTargetable = m_memory->read<bool>(entityAddress + m_offsets.isTargetable);
        entity.level = m_memory->read<int>(entityAddress + m_offsets.level);
        
        // Determine entity type
        uint32_t typeValue = m_memory->read<uint32_t>(entityAddress + m_offsets.type);
        entity.type = determineEntityType(typeValue);
        
        // Read name
        entity.name = readEntityName(entityAddress + m_offsets.name);
        
        // Update timing
        entity.lastSeen = std::chrono::steady_clock::now();
        
        // Calculate threat level for monsters
        if (entity.type == EntityType::MONSTER || entity.type == EntityType::BOSS) {
            entity.threatLevel = calculateThreatLevel(entity);
        }
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

EntityManager::EntityType EntityManager::determineEntityType(uintptr_t typeValue) const {
    // Placeholder type determination
    // This would need to be reverse engineered from the game
    
    switch (typeValue) {
        case 1: return EntityType::MONSTER;
        case 2: return EntityType::BOSS;
        case 3: return EntityType::ITEM;
        case 4: return EntityType::CHEST;
        case 5: return EntityType::PORTAL;
        default: return EntityType::UNKNOWN;
    }
}

float EntityManager::calculateThreatLevel(const Entity& entity) const {
    float threat = 1.0f;
    
    // Base threat on level difference
    if (m_gameState) {
        int levelDiff = entity.level - m_gameState->getPlayer().level;
        threat += levelDiff * 0.1f;
    }
    
    // Increase threat for bosses
    if (entity.type == EntityType::BOSS) {
        threat *= 3.0f;
    }
    
    // Increase threat for elites
    if (entity.type == EntityType::MONSTER && entity.data.monster.isElite) {
        threat *= 2.0f;
    }
    
    return std::max(0.1f, threat);
}

bool EntityManager::isEntityValid(const Entity& entity) const {
    // Basic validation
    return entity.id != 0 && 
           entity.type != EntityType::UNKNOWN &&
           entity.x >= -10000 && entity.x <= 10000 &&
           entity.y >= -10000 && entity.y <= 10000;
}

void EntityManager::updateEntityVisibility(Entity& entity) {
    // Placeholder for visibility checking
    // Would need screen projection calculations
    entity.isVisible = true;
}

std::string EntityManager::readEntityName(uintptr_t nameAddress) const {
    if (!m_memory || !m_memory->isValidAddress(nameAddress)) {
        return "Unknown";
    }
    
    try {
        return m_memory->readString(nameAddress, 64);
    }
    catch (const std::exception&) {
        return "Unknown";
    }
}
