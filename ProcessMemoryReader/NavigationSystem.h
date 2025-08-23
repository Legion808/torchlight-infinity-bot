#pragma once

#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <chrono>

// Forward declarations
class GameState;
class EntityManager;
class InputManager;

/**
 * @brief Handles pathfinding and navigation throughout the game world
 */
class NavigationSystem {
public:
    struct Point {
        float x, y;
        
        Point(float x = 0, float y = 0) : x(x), y(y) {}
        
        bool operator==(const Point& other) const {
            return std::abs(x - other.x) < 0.1f && std::abs(y - other.y) < 0.1f;
        }
        
        float distanceTo(const Point& other) const {
            float dx = x - other.x;
            float dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    };
    
    struct NavigationNode {
        Point position;
        float gCost;        // Cost from start
        float hCost;        // Heuristic cost to goal
        float fCost() const { return gCost + hCost; }
        NavigationNode* parent;
        bool isWalkable;
        
        NavigationNode(Point pos = Point(), float g = 0, float h = 0, NavigationNode* p = nullptr)
            : position(pos), gCost(g), hCost(h), parent(p), isWalkable(true) {}
    };

    enum class NavigationState {
        IDLE,
        PATHFINDING,
        MOVING,
        EXPLORING,
        STUCK,
        GOAL_REACHED
    };

private:
    const GameState* m_gameState;
    const EntityManager* m_entityManager;
    InputManager* m_inputManager;
    
    NavigationState m_state = NavigationState::IDLE;
    std::vector<Point> m_currentPath;
    size_t m_currentPathIndex = 0;
    Point m_currentGoal;
    
    // Map exploration
    std::vector<std::vector<bool>> m_explorationGrid;
    std::unordered_set<uint64_t> m_exploredAreas;
    int m_gridWidth = 200;
    int m_gridHeight = 200;
    float m_gridResolution = 2.0f; // Units per grid cell
    
    // Pathfinding parameters
    float m_maxPathfindingTime = 1000.0f; // Max time in ms
    float m_nodeDistance = 2.0f;
    float m_stuckThreshold = 1.0f;
    int m_maxPathLength = 500;
    
    // Movement tracking
    Point m_lastPosition;
    std::chrono::steady_clock::time_point m_lastMovementTime;
    std::chrono::steady_clock::time_point m_stuckStartTime;
    bool m_isStuck = false;
    
    // Area exploration
    std::queue<Point> m_explorationQueue;
    std::vector<Point> m_interestingPoints; // Chests, doors, etc.

public:
    NavigationSystem(const GameState* gameState, const EntityManager* entityManager, InputManager* inputManager);
    
    // Core navigation methods
    bool navigateTo(const Point& destination);
    bool navigateToNearest(const std::vector<Point>& destinations);
    void stopNavigation();
    void update();
    
    // Pathfinding
    std::vector<Point> findPath(const Point& start, const Point& goal);
    bool isPathClear(const Point& start, const Point& end);
    bool isPositionWalkable(const Point& position);
    
    // Exploration
    void startMapExploration();
    Point getNextExplorationTarget();
    bool isAreaExplored(const Point& center, float radius = 10.0f);
    void markAreaAsExplored(const Point& center, float radius = 5.0f);
    float getExplorationProgress() const;
    
    // State queries
    NavigationState getState() const { return m_state; }
    bool isNavigating() const { return m_state == NavigationState::MOVING || m_state == NavigationState::PATHFINDING; }
    bool hasReachedGoal() const { return m_state == NavigationState::GOAL_REACHED; }
    bool isStuck() const { return m_isStuck; }
    Point getCurrentGoal() const { return m_currentGoal; }
    
    // Utility methods
    Point getPlayerPosition() const;
    float getDistanceToGoal() const;
    std::vector<Point> getVisibleEnemyPositions() const;
    std::vector<Point> getInterestingPoints() const { return m_interestingPoints; }
    
    // Configuration
    void setMaxPathfindingTime(float timeMs) { m_maxPathfindingTime = timeMs; }
    void setStuckThreshold(float threshold) { m_stuckThreshold = threshold; }
    void setGridResolution(float resolution);

private:
    // Pathfinding implementation
    std::vector<Point> aStar(const Point& start, const Point& goal);
    std::vector<NavigationNode*> getNeighbors(NavigationNode* node, const std::vector<std::vector<NavigationNode>>& grid);
    float heuristic(const Point& a, const Point& b);
    
    // Movement execution
    bool moveToNextPathPoint();
    void handleStuckState();
    void attemptUnstuck();
    
    // Grid management
    void initializeGrid();
    void updateObstacles();
    Point worldToGrid(const Point& worldPos);
    Point gridToWorld(int gridX, int gridY);
    bool isGridPositionValid(int x, int y);
    
    // Exploration helpers
    void updateExplorationGrid();
    void findInterestingPoints();
    Point findNearestUnexploredArea();
    bool shouldExplorePoint(const Point& point);
    
    // Movement validation
    bool validateMovement(const Point& from, const Point& to);
    void updateMovementTracking();
    bool detectStuck();
};
