#include "TorchlightBot.h"
#include "GameState.h"
#include "NavigationSystem.h"
#include "LootFilter.h"
#include "CombatSystem.h"
#include "EntityManager.h"
#include "Logger.h"
#include "ConfigManager.h"
#include "InputManager.h"
#include <iostream>
#include <thread>

TorchlightBot::TorchlightBot() {
    // Initialize all subsystems
    m_logger = std::make_unique<Logger>();
    m_config = std::make_unique<ConfigManager>();
    m_process = std::make_unique<Process>();
    
    m_logger->info("TorchlightBot initialized");
}

TorchlightBot::~TorchlightBot() {
    stop();
    m_logger->info("TorchlightBot destroyed");
}

bool TorchlightBot::initialize() {
    m_logger->info("Initializing TorchlightBot...");
    
    // Load configuration
    if (!m_config->loadConfig()) {
        m_logger->warning("Could not load config, using defaults");
    }
    
    // Attach to game process
    if (!attachToGame()) {
        m_logger->error("Failed to attach to Torchlight Infinity process");
        return false;
    }
    
    // Initialize memory reader
    m_memory = std::make_unique<Memory>(m_process.get());
    
    // Initialize game state
    m_gameState = std::make_unique<GameState>(m_memory.get());
    if (!m_gameState->findGameAddresses()) {
        m_logger->error("Failed to find game memory addresses");
        return false;
    }
    
    // Initialize entity manager
    m_entityManager = std::make_unique<EntityManager>(m_memory.get(), m_gameState.get());
    if (!m_entityManager->findEntityList()) {
        m_logger->warning("Entity list not found, will retry during runtime");
    }
    
    // Initialize input manager
    auto inputManager = std::make_unique<InputManager>();
    if (!inputManager->initialize()) {
        m_logger->error("Failed to initialize input manager");
        return false;
    }
    
    // Store the input manager pointer for other systems
    InputManager* inputPtr = inputManager.get();
    
    // Initialize navigation system
    m_navigation = std::make_unique<NavigationSystem>(
        m_gameState.get(), m_entityManager.get(), inputPtr);
    
    // Initialize combat system
    m_combat = std::make_unique<CombatSystem>(
        m_gameState.get(), m_entityManager.get(), inputPtr, m_navigation.get());
    
    // Initialize loot filter
    m_lootFilter = std::make_unique<LootFilter>();
    
    // Apply configuration
    const auto& config = m_config->getConfig();
    m_tickRate = std::chrono::milliseconds(config.tickRate);
    setFarmMode(config.farmMode == "aggressive" ? FarmMode::AGGRESSIVE :
                config.farmMode == "safe" ? FarmMode::SAFE : FarmMode::BALANCED);
    
    m_logger->info("TorchlightBot initialization complete");
    return true;
}

void TorchlightBot::start() {
    if (m_running) {
        m_logger->warning("Bot is already running");
        return;
    }
    
    m_logger->info("Starting TorchlightBot");
    m_running = true;
    m_currentState = BotState::FARMING;
    
    // Start main bot thread
    m_botThread = std::thread(&TorchlightBot::botMainLoop, this);
}

void TorchlightBot::stop() {
    if (!m_running) {
        return;
    }
    
    m_logger->info("Stopping TorchlightBot");
    m_running = false;
    
    if (m_botThread.joinable()) {
        m_botThread.join();
    }
    
    m_currentState = BotState::IDLE;
    m_logger->info("TorchlightBot stopped");
}

void TorchlightBot::pause() {
    if (m_currentState != BotState::IDLE) {
        m_logger->info("Pausing bot");
        setState(BotState::IDLE);
    }
}

void TorchlightBot::resume() {
    if (m_currentState == BotState::IDLE && m_running) {
        m_logger->info("Resuming bot");
        setState(BotState::FARMING);
    }
}

TorchlightBot::Statistics TorchlightBot::getStatistics() const {
    Statistics stats;
    
    if (m_combat) {
        stats.monstersKilled = m_combat->getMonstersKilled();
        stats.bossesKilled = m_combat->getBossesKilled();
    }
    
    if (m_lootFilter) {
        stats.itemsLooted = m_lootFilter->getItemsLooted();
    }
    
    // Calculate runtime
    static auto startTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    stats.runtime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    
    return stats;
}

void TorchlightBot::botMainLoop() {
    m_logger->info("Bot main loop started");
    
    while (m_running) {
        try {
            // Validate game state
            if (!isGameValid()) {
                m_logger->error("Game validation failed");
                setState(BotState::ERROR);
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
            
            // Update all subsystems
            updateGameState();
            
            // Handle current state
            switch (m_currentState) {
                case BotState::IDLE:
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    break;
                    
                case BotState::FARMING:
                    handleFarming();
                    break;
                    
                case BotState::COMBAT:
                    handleCombat();
                    break;
                    
                case BotState::LOOTING:
                    handleLooting();
                    break;
                    
                case BotState::NAVIGATING:
                    handleNavigation();
                    break;
                    
                case BotState::BOSS_FIGHT:
                    handleBossFight();
                    break;
                    
                case BotState::SEASONAL_ACTIVITY:
                    handleSeasonalActivity();
                    break;
                    
                case BotState::ERROR:
                    handleError();
                    break;
            }
            
            std::this_thread::sleep_for(m_tickRate);
        }
        catch (const std::exception& e) {
            m_logger->error("Exception in bot main loop: %s", e.what());
            setState(BotState::ERROR);
        }
    }
    
    m_logger->info("Bot main loop ended");
}

void TorchlightBot::handleFarming() {
    // Check if player is alive
    if (!m_gameState->isPlayerAlive()) {
        m_logger->warning("Player is dead, waiting for respawn");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return;
    }
    
    // Check for nearby enemies
    if (m_entityManager->hasNearbyEnemies(
            m_gameState->getPlayer().x, 
            m_gameState->getPlayer().y, 
            m_combat->getEngagementRange())) {
        setState(BotState::COMBAT);
        return;
    }
    
    // Check for nearby loot
    if (m_entityManager->hasLootableItems(
            m_gameState->getPlayer().x, 
            m_gameState->getPlayer().y)) {
        setState(BotState::LOOTING);
        return;
    }
    
    // Check for seasonal activities
    if (m_gameState->hasActiveSeason()) {
        auto seasonalObjects = m_entityManager->findSeasonalObjects();
        if (!seasonalObjects.empty()) {
            setState(BotState::SEASONAL_ACTIVITY);
            return;
        }
    }
    
    // Check if map is completed
    if (m_gameState->isMapCompleted()) {
        m_logger->info("Map completed, looking for portal or new map");
        // TODO: Implement map completion logic
    }
    
    // Default: continue exploring
    if (!m_navigation->isNavigating()) {
        m_navigation->startMapExploration();
        setState(BotState::NAVIGATING);
    }
}

void TorchlightBot::handleCombat() {
    m_combat->update();
    
    // Check if combat is finished
    if (m_combat->getState() == CombatSystem::CombatState::IDLE) {
        setState(BotState::FARMING);
        return;
    }
    
    // Check if we need to retreat
    if (m_combat->getState() == CombatSystem::CombatState::RETREATING) {
        m_logger->info("Retreating from combat");
        // Combat system handles the retreat
    }
}

void TorchlightBot::handleLooting() {
    auto nearbyItems = m_entityManager->getLootableItems();
    
    if (nearbyItems.empty()) {
        setState(BotState::FARMING);
        return;
    }
    
        // Convert entities to loot filter items
        std::vector<LootFilter::ItemInfo> items;
        for (const auto& entity : nearbyItems) {
            LootFilter::ItemInfo item;
            item.name = entity.name;
            item.type = static_cast<LootFilter::ItemType>(entity.data.item.itemType == "weapon" ? 1 : 2);
            item.rarity = static_cast<LootFilter::ItemRarity>(entity.data.item.rarity);
            item.x = entity.x;
            item.y = entity.y;
            item.z = entity.z;
            items.push_back(item);
        }    // Filter and prioritize items
    auto filteredItems = m_lootFilter->filterItems(items);
    auto prioritizedItems = m_lootFilter->prioritizeItems(filteredItems);
    
    // Pick up items in priority order
    for (const auto& item : prioritizedItems) {
        if (item.shouldLoot) {
            m_logger->info("Looting item: %s", item.name.c_str());
            // TODO: Implement actual item pickup via InputManager
        }
    }
    
    setState(BotState::FARMING);
}

void TorchlightBot::handleNavigation() {
    m_navigation->update();
    
    if (m_navigation->hasReachedGoal() || !m_navigation->isNavigating()) {
        setState(BotState::FARMING);
    }
}

void TorchlightBot::handleBossFight() {
    m_combat->handleBossCombat();
    
    if (!m_combat->isFightingBoss()) {
        setState(BotState::FARMING);
    }
}

void TorchlightBot::handleSeasonalActivity() {
    auto seasonalObjects = m_entityManager->findSeasonalObjects();
    
    if (seasonalObjects.empty()) {
        setState(BotState::FARMING);
        return;
    }
    
    // Navigate to nearest seasonal object
    auto nearestObject = seasonalObjects[0];
    m_navigation->navigateTo({nearestObject.x, nearestObject.y});
    
    setState(BotState::NAVIGATING);
}

void TorchlightBot::handleError() {
    m_logger->error("Bot is in error state, attempting recovery");
    
    // Try to re-attach to game
    if (!isGameValid()) {
        if (attachToGame()) {
            m_logger->info("Successfully re-attached to game");
            setState(BotState::FARMING);
        } else {
            m_logger->error("Failed to re-attach to game, retrying in 10 seconds");
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    } else {
        setState(BotState::FARMING);
    }
}

bool TorchlightBot::attachToGame() {
    // Try common Torchlight Infinity process names
    std::vector<std::string> processNames = {
        "Torchlight3.exe",
        "TorchlightInfinity.exe", 
        "TL3.exe",
        "Game.exe"
    };
    
    for (const auto& processName : processNames) {
        if (m_process->attachToProcess(processName)) {
            m_logger->info("Successfully attached to process: %s", processName.c_str());
            return true;
        }
    }
    
    m_logger->error("Could not find Torchlight Infinity process");
    return false;
}

void TorchlightBot::updateGameState() {
    if (m_gameState) {
        m_gameState->update();
    }
    
    if (m_entityManager) {
        m_entityManager->update();
    }
}

bool TorchlightBot::isGameValid() const {
    return m_process && m_process->isAttached() && 
           m_gameState && m_gameState->isPlayerAlive();
}
