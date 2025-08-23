#pragma once

#include "Process.h"
#include "Memory.h"
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <cstdint>

// Forward declarations
class GameState;
class NavigationSystem;
class LootFilter;
class CombatSystem;
class EntityManager;
class Logger;
class ConfigManager;

/**
 * @brief Main bot class that orchestrates all subsystems
 */
class TorchlightBot {
public:
    enum class BotState {
        IDLE,
        FARMING,
        COMBAT,
        LOOTING,
        NAVIGATING,
        BOSS_FIGHT,
        SEASONAL_ACTIVITY,
        ERROR
    };

    enum class FarmMode {
        AGGRESSIVE,
        SAFE,
        BALANCED
    };

private:
    std::unique_ptr<Process> m_process;
    std::unique_ptr<Memory> m_memory;
    std::unique_ptr<GameState> m_gameState;
    std::unique_ptr<NavigationSystem> m_navigation;
    std::unique_ptr<LootFilter> m_lootFilter;
    std::unique_ptr<CombatSystem> m_combat;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<Logger> m_logger;
    std::unique_ptr<ConfigManager> m_config;

    std::atomic<bool> m_running{false};
    std::atomic<BotState> m_currentState{BotState::IDLE};
    std::thread m_botThread;
    
    FarmMode m_farmMode{FarmMode::BALANCED};
    std::chrono::milliseconds m_tickRate{50}; // 20 FPS

public:
    TorchlightBot();
    ~TorchlightBot();

    // Core bot control
    bool initialize();
    void start();
    void stop();
    void pause();
    void resume();

    // State management
    BotState getCurrentState() const { return m_currentState; }
    void setState(BotState state) { m_currentState = state; }
    
    // Configuration
    void setFarmMode(FarmMode mode) { m_farmMode = mode; }
    FarmMode getFarmMode() const { return m_farmMode; }
    
    // Statistics
    struct Statistics {
        uint64_t mapsCleared = 0;
        uint64_t monstersKilled = 0;
        uint64_t itemsLooted = 0;
        uint64_t bossesKilled = 0;
        std::chrono::seconds runtime{0};
    };
    
    Statistics getStatistics() const;

private:
    void botMainLoop();
    void handleFarming();
    void handleCombat();
    void handleLooting();
    void handleNavigation();
    void handleBossFight();
    void handleSeasonalActivity();
    void handleError();
    
    bool attachToGame();
    void updateGameState();
    bool isGameValid() const;
};
