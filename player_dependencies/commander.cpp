#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <set>
#include <utility>
#include <algorithm>
#include "unit.cpp"
/**
 * 2 dimensional vector of chars representing fields on the battlefield
 */
typedef std::vector<std::vector<char>> vectorMap;
/**
 * vector containing arrays of coordinates x,y describing path between two points
 */
typedef std::vector<std::array<int, 2>> path;
/**
 * Custom pair implementation specifically for use in multiset while implementing Dijkstra algorithm for finding shortest path in a graph.
 * first element is supposed to represent distance from the beginning of traversed graph to our vertex
 * second element is used as identification of traversed vertex - its coordinates
 *
 */
struct IntArrayPair
{
    int first;
    std::array<int, 2> second;

    // Custom comparison operator for sorting the pairs
    bool operator<(const IntArrayPair &other) const
    {

        return first < other.first;
    }
};

enum prices
{
    Knight = 400,
    Swordsman = 250,
    Archer = 250,
    Pikeman = 200,
    Ram = 500,
    Catapult = 800,
    Worker = 100
};

struct Mine
{
    std::array<int, 2> coordinates;
};
/**
 *Class implementing basic logic for player for StarCraft3 game that involves producing Swordman unit whenever possible.
 *
 */
class Commander
{
private:
    Unit base;

    std::vector<Unit> combatUnits;

    std::vector<Unit> workers;

    std::vector<Unit> enemies;

    std::vector<Mine> mines;

    long gold;

public:
    Commander(Unit base, std::vector<Unit> combatUnits, std::vector<Unit> workers, std::vector<Unit> enemies, long gold)
    {
        this->base = base;
        this->combatUnits = combatUnits;
        this->workers = workers;
        this->enemies = enemies;
        this->gold = gold;
    }
    Commander() {}

    void setBase(Unit base)
    {
        this->base = base;
    }
    void setMines(std::vector<Mine> mines)
    {
        this->mines = mines;
    }
    void addCombatUnit(Unit combatUnit)
    {
        this->combatUnits.push_back(combatUnit);
    }

    void addEnemy(Unit enemy)
    {
        this->enemies.push_back(enemy);
    }

    void addWorker(Unit worker)
    {
        this->workers.push_back(worker);
    }

    void setGold(long gold)
    {
        this->gold = gold;
    }

    /**
     * Method used to generate string containing commands for base.
     * This particular implementation focuses on spamming Swordman unit as it has high cost to strenght ratio.
     *
     * @param map map of the battlefield used for checking accessibility to Mines - no need to product worker if we cannot access one
     * @return returns command string with base order or empty string if base is already producing something or cannot afford a unit.
     */
    std::string orderBase(vectorMap map)
    {
        std::string returnString = std::to_string(this->base.getId()) + " B";
        path mineAccessibility = calculatePath(map, this->base);
        if (this->base.getProduction() != '0')
        {
            return "";
        }
        if ((this->enemies.size() >= this->combatUnits.size() || this->mines.size() == 0 || this->gold > 400 || mineAccessibility.size() == 0) && this->gold >= Swordsman)
        {
            // we will spam swordsman - good value

            return returnString + " S\n";
        }
        else if (this->mines.size() != 0 && this->enemies.size() < this->combatUnits.size() && this->gold >= Worker)
        {

            return returnString + " W\n";
        }
        return "";
    }
    /**
     * Method initializing our commander
     * @param map map of the battlefield
     */
    void commanderSetup(vectorMap &map)
    {
        setGoldmines(map);
        markEnemies(map);
    }
    /**
     * Method used to mark enemies as 'X' on map of the battlefield - our tactic will focus on attacking closest enemy while producing high value units
     * @param map map of the battlefield
     */
    void markEnemies(vectorMap &map)
    {
        for (Unit unit : this->enemies)
        {
            std::array<int, 2> unitCoords = unit.getCoords();
            map[unitCoords[1]][unitCoords[0]] = 'X';
        }
    }
    /**
     * Method checking for presence of goldmines on the battlefield - we need to check if we want to bother with producing workers
     * @param map map of the battlefield
     */
    void setGoldmines(vectorMap map)
    {
        std::vector<Mine> mines;
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[0].size(); j++)
            {
                if (map[i][j] == '6')
                {
                    Mine currMine = {j, i};
                    mines.push_back(currMine);
                }
            }
        }
        this->mines = mines;
    }
    /**
     * Method used for creating orders for all of our combat units - that is attack closest enemy to them
     * @param map map of the battlefield
     * @return returns string containing orders for all of our combat units
     */
    std::string orderCombatUnits(vectorMap map)
    {
        std::string returnString;
        for (Unit braveWarrior : this->combatUnits)
        {
            path enemyPath = calculatePath(map, braveWarrior);
            int remainingMoves = braveWarrior.getSpeed();
            std::array<int, 2> currentPos = enemyPath[enemyPath.size() - 1];
            int requiredMoves = 0;
            int correctMove = 0;
            // check if enemy is not in range for our attack and we need to move closer
            if (enemyPath.size() > 1 + braveWarrior.getRange())
            {
                std::string currCommand = "";

                requiredMoves = enemyPath.size() - braveWarrior.getRange() - 1;
                correctMove = enemyPath.size() - (requiredMoves > remainingMoves ? remainingMoves : requiredMoves) - 1;
                currentPos = enemyPath[correctMove];
                currCommand += std::to_string(braveWarrior.getId()) + " M " + std::to_string(currentPos[0]) + " " + std::to_string(currentPos[1]) + "\n";
                remainingMoves -= requiredMoves;

                returnString += currCommand;
            }
            // check if we are close enough to enemy to attack him and have enough move points (more than 0)
            if ((distance(currentPos, enemyPath[0]) <= braveWarrior.getRange() || enemyPath.size() == 0) && remainingMoves > 0)
            {
                std::string currCommand = "";
                currCommand += std::to_string(braveWarrior.getId()) + " A " + std::to_string(getAttackedEnemyId(enemyPath[0])) + "\n";

                returnString += currCommand;
            }
        }
        return returnString;
    }
    /**
     * Method used for generating workers orders
     * @param map map of the battlefield
     * @return returns empty sting if workers are already in the mine or have no path leading to one, or order to move possibly close to the mine
     */
    std::string orderWorkers(vectorMap map)
    {
        std::string returnString = "";

        for (Unit worker : this->workers)
        {
            bool insideMine = false;
            for (Mine mine : this->mines)
            {

                if (worker.getCoords() == mine.coordinates)
                {
                    insideMine = true;
                }
            }
            if (!insideMine)
            {

                path minePath = calculatePath(map, worker);
                // no path between current miner and a mine - we continue to check the next mine
                if (minePath.size() == 0)
                {
                    continue;
                }
                std::array<int, 2> moveCoords;
                moveCoords = (worker.getRange() >= minePath.size() - 1 ? minePath[0] : minePath[minePath.size() - worker.getRange() - 2]);
                returnString += std::to_string(worker.getId()) + " M " + std::to_string(moveCoords[0]) + " " + std::to_string(moveCoords[1]) + "\n";
            }
        }
        return returnString;
    }
    /**
     * Method generating shortest path to another point calculated by implementing djikstra algorithm - first element are enemy coordinates, last are coordinates of given unit.
     * Method treats our battlefield as unweighted graph - due to that we cannot jump over obstacles and only move over road fields ('0').
     * @param map map of the battlefield
     * @param unit unit that is looking for its target - for base ('B') and workers ('W') we check for access to mines, for combat units we check for closest enemy
     * @return returns coordinates for every step that needs to be taken in order to get to target
     */
    path calculatePath(vectorMap map, Unit unit)
    {
        // std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> enemyCoords;
        path returnPath;
        std::multiset<IntArrayPair> unvisited;
        std::vector<std::vector<bool>> unvisitedMatrix; // we will be checking if we already visited neighbouring vertices - requires more memory but should be faster than when traversing a set
        std::vector<std::vector<std::string>> previous; // we will traverse this matrix based on characters to generate shortest path
        int maxY = map.size() - 1;
        int maxX = map[0].size() - 1;
        for (int i = 0; i < map.size(); i++)
        {
            std::vector<bool> vector3(map[0].size(), false);
            std::vector<std::string> vector4(map[0].size(), "o");
            unvisitedMatrix.push_back(vector3);
            previous.push_back(vector4);
            for (int j = 0; j < map[0].size(); j++)
            {
                std::array<int, 2> unvCoord = {j, i};
                IntArrayPair vertex;
                if (unvCoord[0] == coords[0] && unvCoord[1] == coords[1])
                {
                    // We set starting point distance to 0 - we start here
                    vertex = {0, coords};
                }
                else if (map[unvCoord[1]][unvCoord[0]] != '9') // we dont want to add ground to the unvisited array - we cant visit ground
                {
                    // distance to every other vertex apart from starting one is possibly large
                    vertex = {INT32_MAX, unvCoord};
                }
                unvisited.insert(vertex);
            }
        }
        bool found = false;
        while (!found && !unvisited.empty())
        {

            const IntArrayPair &firstElement = *unvisited.begin();
            // there is no path from starting point to the vertices remaining in the unvisited array - no need to continue the algorithm
            if (firstElement.first == INT32_MAX)
            {
                break;
            }
            std::vector<std::array<int, 2>> neighbours;
            std::array<int, 2> currCorr = firstElement.second;
            if(currCorr[0] > 0) neighbours.push_back({currCorr[0] - 1, currCorr[1]});
            if(currCorr[0] < maxX)  neighbours.push_back({currCorr[0] + 1, currCorr[1]});
            if(currCorr[1] > 0)  neighbours.push_back({currCorr[0], currCorr[1] - 1});
            if(currCorr[1] < maxY)  neighbours.push_back({currCorr[0], currCorr[1] + 1});

            for (std::array<int, 2> neighbour : neighbours)
            {

                if (unvisitedMatrix[neighbour[1]][neighbour[0]])
                {

                    continue;
                }

                char currentNeighbour = map[neighbour[1]][neighbour[0]];

                if ((currentNeighbour == 'X' && unit.getType() != 'W') || (currentNeighbour == '6' && (unit.getType() == 'B' || unit.getType() == 'W')))
                {
                    if (firstElement.second[0] - neighbour[0] != 0)
                    {
                        previous.at(neighbour[1]).at(neighbour[0]) = (firstElement.second[0] - neighbour[0] == 1 ? "R" : "L");
                    }
                    else if (firstElement.second[1] - neighbour[1] != 0)
                    {
                        previous[neighbour[1]][neighbour[0]] = (firstElement.second[1] - neighbour[1] == 1 ? "D" : "U");
                    }

                    enemyCoords = neighbour;
                    found = true;
                }
                else if (currentNeighbour != '9')
                {

                    auto foundElement = std::find_if(unvisited.begin(), unvisited.end(), [&neighbour](const IntArrayPair &pair)
                                                     { return pair.second == neighbour; });

                    if (foundElement != unvisited.end())
                    {
                        const IntArrayPair &secondElement = *foundElement;
                        if (secondElement.first > firstElement.first)
                        {
                            // we cannot change the key in our pair but we need to update distance - so we erase element and reinsert it later
                            unvisited.erase(foundElement);
                            IntArrayPair currentVertex = {firstElement.first + 1, secondElement.second};
                            // we check both axes to see what was the previous element when getting to current vertex - and set correct direction to our previous matrix so we can traverse path later
                            if (firstElement.second[0] - secondElement.second[0] != 0)
                            {
                                previous.at(secondElement.second[1]).at(secondElement.second[0]) = (firstElement.second[0] - secondElement.second[0] == 1 ? "R" : "L"); // right and left
                            }
                            else if (firstElement.second[1] - secondElement.second[1] != 0)
                            {
                                previous[secondElement.second[1]][secondElement.second[0]] = (firstElement.second[1] - secondElement.second[1] == 1 ? "D" : "U"); // down and up
                            }

                            unvisited.insert(currentVertex);
                        }
                    }
                }

                unvisitedMatrix[neighbour[1]][neighbour[0]] = true;
            }
            // remove head of multiset - we already visited all its neighbours
            unvisited.erase(unvisited.begin());
        }
        bool formingResult = true;
        if (found)
        {

            returnPath.push_back(enemyCoords);
            while (formingResult)
            {
                // we check coordinates of current points to check what will be the next one based on previous array
                int currentX = returnPath[returnPath.size() - 1][0];
                int currentY = returnPath[returnPath.size() - 1][1];
                switch (previous[currentY][currentX].c_str()[0])
                {
                case ('U'):
                    returnPath.push_back({currentX, currentY - 1});
                    break;
                case ('D'):
                    returnPath.push_back({currentX, currentY + 1});
                    break;
                case ('L'):
                    returnPath.push_back({currentX - 1, currentY});
                    break;
                case ('R'):
                    returnPath.push_back({currentX + 1, currentY});
                    break;
                default:
                    formingResult = false;
                }
            }
        }
        return returnPath;
    }
    /**
     * Method responsible for invoking methods creating strings containing orders for all units (and base) and saving them to a file
     * @param orderFileName name of the file we want to save orders to
     * @param map map of the battlefield
     */
    void giveOrders(std::string orderFileName, vectorMap map)
    {
        std::string commandString = "";
        commandString += orderBase(map);

        commandString += orderCombatUnits(map);
        commandString += orderWorkers(map);
        std::ofstream ofs(orderFileName, std::ofstream::trunc);
        ofs << commandString;
        ofs.close();
    }
    /**
     * Method calculating distance between two arrays of coordinates according to the game rules
     * @param p1 first array of coordinates
     * @param p2 second array of coordinates
     * @return returns int being distance between parameters
     */
    int distance(std::array<int, 2> p1, std::array<int, 2> p2)
    {
        return abs(p1[1] - p2[1]) + abs(p1[0] - p2[0]);
    }
    /**
     * Method returning Id of enemy that we want to attack based on his coordinates
     * @param enemyCoord enemy coordinates
     * @return returns enemy id or -1 if there is no such enemy
     */
    int getAttackedEnemyId(std::array<int, 2> enemyCoord)
    {
        for (Unit enemy : this->enemies)
        {
            if (enemy.getCoords() == enemyCoord)
            {
                return enemy.getId();
            }
        }
        return -1;
    }
    /**
     * Method displaying path that unit needs to take to access its target
     * @param map map of the battlefield
     * @param unit unit that need its target
     */
    void displayPath(vectorMap map, Unit unit)
    {
        path tescik = calculatePath(map, unit);

        std::cout << "Here are the steps: " << std::endl;
        for (std::array<int, 2> coord : tescik)
        {
            std::cout << coord[0] << " " << coord[1] << std::endl;
        }
        std::cout << std::endl;
    }
};
