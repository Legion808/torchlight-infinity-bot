#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

// Forward declarations
class GameState;
class EntityManager;
class InputManager;
class NavigationSystem;
struct Entity;

/**
 * @brief Manages combat logic, targeting, and ability usage
 */
class CombatSystem {
public:
    enum class CombatState {
        IDLE,
        ENGAGING,
        FIGHTING,
        RETREATING,
        HEALING,
        KITING,
        BOSS_FIGHT
    };

    enum class TacticsMode {
        AGGRESSIVE,     // Fight everything immediately
        DEFENSIVE,      // Careful engagement, retreat when low health
        BALANCED,       // Mix of aggressive and defensive
        BOSS_ONLY,      // Only engage bosses and elites
        KITING         // Hit and run tactics
    };

    struct AbilityInfo {
        std::string name;
        int keyBinding;            // Virtual key code
        float cooldown;            // Cooldown in seconds
        float range;               // Ability range
        float manaCost;           // Mana cost
        bool isOffensive;         // True for damage abilities
        bool isDefensive;         // True for defensive abilities
        bool isMovement;          // True for movement abilities
        int priority;             // Usage priority (higher = more important)
        std::function<bool()> condition; // When to use this ability
        
        std::chrono::steady_clock::time_point lastUsed;
        bool isOnCooldown() const {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUsed);
            return elapsed.count() < (cooldown * 1000);
        }
    };

    struct CombatTarget {
        uint64_t entityId;
        float priority;            // Target priority score
        float lastDamageTime;      // When we last damaged this target
        float estimatedTimeToKill; // Estimated time to kill
        bool isBeingAttacked;      // Currently attacking this target
    };

private:
    const GameState* m_gameState;
    const EntityManager* m_entityManager;
    InputManager* m_inputManager;
    NavigationSystem* m_navigation;
    
    CombatState m_state = CombatState::IDLE;
    TacticsMode m_tactics = TacticsMode::BALANCED;
    
    // Abilities and combat skills
    std::vector<AbilityInfo> m_abilities;
    std::unordered_map<std::string, size_t> m_abilityMap; // Name to index mapping
    
    // Targeting system
    std::vector<CombatTarget> m_targets;
    uint64_t m_primaryTarget = 0;
    uint64_t m_lastTarget = 0;
    
    // Combat parameters
    float m_engagementRange = 25.0f;
    float m_retreatHealthPercent = 0.3f;      // Retreat when health below 30%
    float m_healHealthPercent = 0.5f;         // Heal when health below 50%
    float m_kiteDistance = 15.0f;             // Distance to maintain when kiting
    float m_maxCombatTime = 30.0f;            // Max time to fight single target
    
    // Combat statistics
    uint64_t m_monstersKilled = 0;
    uint64_t m_bossesKilled = 0;
    uint64_t m_deaths = 0;
    float m_totalCombatTime = 0.0f;
    
    // Timing
    std::chrono::steady_clock::time_point m_combatStartTime;
    std::chrono::steady_clock::time_point m_lastAbilityUse;
    std::chrono::steady_clock::time_point m_lastTargetSwitch;

public:
    CombatSystem(const GameState* gameState, const EntityManager* entityManager, 
                 InputManager* inputManager, NavigationSystem* navigation);
    
    // Core combat methods
    void update();
    void startCombat();
    void stopCombat();
    void emergencyRetreat();
    
    // State management
    CombatState getState() const { return m_state; }
    void setState(CombatState state) { m_state = state; }
    void setTacticsMode(TacticsMode mode) { m_tactics = mode; }
    TacticsMode getTacticsMode() const { return m_tactics; }
    
    // Targeting
    bool selectTarget();
    bool hasValidTarget() const;
    Entity* getCurrentTarget() const;
    void clearTarget() { m_primaryTarget = 0; }
    void switchTarget();
    
    // Ability management
    void registerAbility(const AbilityInfo& ability);
    bool useAbility(const std::string& abilityName);
    bool useBestOffensiveAbility();
    bool useBestDefensiveAbility();
    bool useMovementAbility();
    AbilityInfo* getAbility(const std::string& name);
    
    // Combat actions
    bool attackTarget();
    bool defendSelf();
    bool healSelf();
    bool kiteTarget();
    bool retreatFromCombat();
    
    // Boss combat
    bool isFightingBoss() const;
    void handleBossCombat();
    void setBossTactics();
    
    // Configuration
    void setEngagementRange(float range) { m_engagementRange = range; }
    void setRetreatHealthPercent(float percent) { m_retreatHealthPercent = percent; }
    void setHealHealthPercent(float percent) { m_healHealthPercent = percent; }
    void setKiteDistance(float distance) { m_kiteDistance = distance; }
    
    // Statistics
    uint64_t getMonstersKilled() const { return m_monstersKilled; }
    uint64_t getBossesKilled() const { return m_bossesKilled; }
    uint64_t getDeaths() const { return m_deaths; }
    float getTotalCombatTime() const { return m_totalCombatTime; }
    float getAverageKillTime() const;

private:
    // Combat state handlers
    void handleIdle();
    void handleEngaging();
    void handleFighting();
    void handleRetreating();
    void handleHealing();
    void handleKiting();
    void handleBossFight();
    
    // Targeting logic
    float calculateTargetPriority(const Entity& entity) const;
    bool shouldSwitchTarget() const;
    std::vector<Entity> getValidTargets() const;
    Entity* findBestTarget() const;
    
    // Ability logic
    bool canUseAbility(const AbilityInfo& ability) const;
    AbilityInfo* selectBestAbility(bool offensive = true) const;
    void updateAbilityCooldowns();
    
    // Combat decision making
    bool shouldRetreat() const;
    bool shouldHeal() const;
    bool shouldKite() const;
    bool shouldEngageTarget(const Entity& target) const;
    bool isInDanger() const;
    
    // Utility methods
    void updateCombatStatistics();
    bool isPositionSafe(float x, float y) const;
    float getDistanceToTarget() const;
    void initializeDefaultAbilities();
};
