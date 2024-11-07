struct collidable_system 
{
    collision_result checkCollision(
        int src_x, int src_y, int src_width, int src_height, 
        int tgt_x, int tgt_y, int tgt_width, int tgt_height
    ) {
        collision_result result = {false, false, false};

        // Calculate the difference in positions
        int src_right = src_x + src_width;
        int src_bottom = src_y + src_height;
        int tgt_right = tgt_x + tgt_width;
        int tgt_bottom = tgt_y + tgt_height;

        // Check if the rectangles overlap
        bool overlap_x = src_x < tgt_right && src_right > tgt_x;
        bool overlap_y = src_y < tgt_bottom && src_bottom > tgt_y;

        // Set collision flags if there is an overlap on each axis
        result.collided_x = overlap_x;
        result.collided_y = overlap_y;

        // The overall collision occurs only if both axes overlap
        result.collided = overlap_x && overlap_y;

        return result;
    }
};

/**
* A* pathfinding function
* Uses the grid that was created on instantiation of the NPC as well as the source and target coords to do this
*/
std::vector<Node*> getPathToTarget(std::vector<std::vector<Node>>& grid, int startX, int startY, int targetX, int targetY, Background* background) {
    int numRows = grid.size();
    int numCols = grid[0].size();

    int centreCharX = startX + width / 2;
    int centreCharY = startY + height / 2;
    int centreTargetX = targetX + width / 2;
    int centreTargetY = targetY + height / 2;

    GridLocator startGridLocation = getGridLocation(startX, startY, background);
    GridLocator targetGridLocation = getGridLocation(centreTargetX, centreTargetY, background);

    // std::cout << "Grid Rows: " << numRows << "Grid Cols: " << numCols << " ---- ";
    // std::cout << "\nStart X: " << startGridLocation.gridX << " Start Y: " << startGridLocation.gridY << " ---- \n";
    // std::cout << "\nTarget X: " << targetGridLocation.gridX << " Target Y: " << targetGridLocation.gridY << " ---- \n";

    // Check if the start and target positions are valid
    if (startGridLocation.gridX < 0 || startGridLocation.gridY < 0 || startGridLocation.gridX >= numCols || startGridLocation.gridY >= numRows ||
        targetGridLocation.gridX < 0 || targetGridLocation.gridY < 0 || targetGridLocation.gridX >= numCols || targetGridLocation.gridY >= numRows) {
        std::cout << "Positions not valid";
        return std::vector<Node*>();
    }

    struct NodeComparator {
        bool operator()(Node* a, Node* b) const {
            return a->f() > b->f();
        }
    };

    // Declare the priority queue using the comparator
    std::priority_queue<Node*, std::vector<Node*>, NodeComparator> openSet;

    // Initialize start node
    Node* startNode = &grid[startGridLocation.gridY][startGridLocation.gridX];
    startNode->cost = 0;
    startNode->heuristic = calculateHeuristic(startGridLocation.gridX, startGridLocation.gridY, targetGridLocation.gridX, targetGridLocation.gridY);
    
    // To avoid it selecting itself as first path
    startNode->parent = startNode;
    openSet.push(startNode);

    while (!openSet.empty()) {
        // Get the node with the lowest f value from the open set
        Node* currentNode = openSet.top();
        openSet.pop();

        // Check if we've reached the target node
        if (currentNode->gridX == targetGridLocation.gridX && currentNode->gridY == targetGridLocation.gridY) {
            // Reconstruct the path from the target node to the start node
            std::vector<Node*> path;
            Node* current = currentNode;
            // std::cout << "\nLAST NODE: " << current->gridX << ", " << current->gridY << " LAST NODE PARENT: " << current->parent->gridX << ", " << current->parent->gridY << " START NODE: " << startNode->gridX << ", " << startNode->gridY << "\n";
            while (current != nullptr) {
                if (current->gridX == startNode->gridX && current->gridY == startNode->gridY) {
                    break;
                }
                // std::cout << "NTH ARRAY GRID POSITION: " << current->gridX << ", " << current->gridY << " START POSITION: " <<  startNode->gridX << ", " << startNode->gridY << "\n";
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        double tentativeCost = currentNode->cost + 1; // Assuming each step has a cost of 1

        // Generate neighboring nodes. Path finding with diagonals
        int minX = std::max(0, currentNode->gridX - 1);
        int maxX = std::min(numCols, currentNode->gridX + 1);
        int minY = std::max(0, currentNode->gridY - 1);
        int maxY = std::min(numRows, currentNode->gridY + 1);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                // Get rid of neighboring diagonals
                // if (x != currentNode->gridX && y != currentNode->gridY) {
                //     continue;
                // }

                if (currentNode->blocker) {
                    continue;
                }

                int nextX = x;
                int nextY = y;

                if (nextX >= 0 && nextX < numCols && nextY >= 0 && nextY < numRows) {
                    // carry on
                } else {
                    continue;
                }
                
                Node* neighbor = &grid[nextY][nextX];


                // std::cout << "\nXY: " << nextX << ", " << nextY << " ---- tent G: " << tentativeCost << " neighbour G: " << neighbor->cost << "\n";

                // Check if the neighbor node is not in the closed set or has a lower g value
                if (tentativeCost < neighbor->cost || !neighbor->parent) {    
                    neighbor->cost = tentativeCost;
                    neighbor->heuristic = calculateHeuristic(nextX, nextY, targetGridLocation.gridX, targetGridLocation.gridY);
                    neighbor->parent = currentNode;

                    // Add the neighbor to the open set
                    openSet.push(neighbor);
                }
            }
        }
    }

    // No path found
    return std::vector<Node*>();
}
};






//  ANOTHER ONE

#include <queue>
#include <unordered_map>
#include <vector>

struct Node {
    int grid_x;
    int grid_y;
    int g_cost;
    int h_cost;
    Node* parent;

    int f_cost() const { return g_cost + h_cost; }
};

// Heuristic function for A*
int manhattan_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Comparison operator for the priority queue
struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        return a->f_cost() > b->f_cost();
    }
};

// A* algorithm
std::vector<Node> find_path(entt::registry& registry, int start_x, int start_y, int target_x, int target_y) {
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> open_set;
    std::unordered_map<int, Node> all_nodes;
    std::unordered_map<int, bool> closed_set;

    auto get_index = [](int x, int y) { return y * 100 + x; };  // Unique index for grid position

    // Initialize start node
    Node* start_node = &all_nodes[get_index(start_x, start_y)];
    start_node->grid_x = start_x;
    start_node->grid_y = start_y;
    start_node->g_cost = 0;
    start_node->h_cost = manhattan_distance(start_x, start_y, target_x, target_y);
    start_node->parent = nullptr;
    open_set.push(start_node);

    // A* loop
    while (!open_set.empty()) {
        Node* current = open_set.top();
        open_set.pop();

        // Check if reached the target and return reversed path back if we have
        if (current->grid_x == target_x && current->grid_y == target_y) {
            std::vector<Node> path;
            while (current != nullptr) {
                path.push_back(*current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closed_set[get_index(current->grid_x, current->grid_y)] = true;

        // Explore neighbors (does not do diagonals)
        for (const auto& [dx, dy] : std::vector<std::pair<int, int>>{{0, 1}, {1, 0}, {0, -1}, {-1, 0}}) {
            int neighbor_x = current->grid_x + dx;
            int neighbor_y = current->grid_y + dy;

            int neighbor_index = get_index(neighbor_x, neighbor_y);

            // Check if neighbor is within bounds or already in closed set
            if (neighbor_x < 0 || neighbor_y < 0 || neighbor_x >= 30 || neighbor_y >= 20 || closed_set[neighbor_index]) {
                continue;
            }

            // Check for collision and add collidables to closed set immediately
            auto view = registry.view<transform_component, collidable_component>();
            bool collision_detected = false;
            view.each([&](auto entity, transform_component& transform, collidable_component& collidable) {
                if (transform.grid_x == neighbor_x && transform.grid_y == neighbor_y) {
                    collision_detected = true;
                    return;
                }
            });

            if (collision_detected) {
                closed_set[neighbor_index] = true;  // Mark collidable as visited
                continue;
            }

            Node* neighbor = &all_nodes[neighbor_index];
            int tentative_g_cost = current->g_cost + 1;

            if (neighbor->g_cost == 0 || tentative_g_cost < neighbor->g_cost) {
                neighbor->grid_x = neighbor_x;
                neighbor->grid_y = neighbor_y;
                neighbor->g_cost = tentative_g_cost;
                neighbor->h_cost = manhattan_distance(neighbor_x, neighbor_y, target_x, target_y);
                neighbor->parent = current;
                open_set.push(neighbor);
            }
        }
    }

    // Return an empty path if no path is found
    return {};
}
