#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <set>
#include <utility>
#include "unit.cpp"
typedef std::vector<std::vector<char>> vectorMap;
typedef std::vector<std::array<int, 2>> path;
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

    void showcase()
    {
        std::array<int, 2> coord = this->base.getCoords();
        std::cout << "Base is at: " + std::to_string(coord[0]) + " X and " + std::to_string(coord[1]) + " Y" << std::endl;
    }
    std::vector<Unit> getEnemies()
    {
        return this->enemies;
    }
    std::string orderBase()
    {
        std::string returnString = std::to_string(this->base.getId()) + " B";

        if (this->base.getProduction() != '0')
        {
            return "";
        }
        if ((this->enemies.size() >= this->combatUnits.size() || this->mines.size() == 0) && this->gold >= Swordsman)
        {
            // we will spam swordsman - good value (xd)

            return returnString + " S\n";
        }
        else if (this->mines.size() != 0 && this->enemies.size() < this->combatUnits.size() && this->gold >= 100)
        {

            return returnString + " W\n";
        }
        return "";
    }
    void commanderSetup(vectorMap &map)
    {
        checkForGoldmines(map);
        markEnemies(map);
    }
    void markEnemies(vectorMap &mapCoord)
    {
        for (Unit unit : this->enemies)
        {
            std::array<int, 2> unitCoords = unit.getCoords();
            mapCoord[unitCoords[1]][unitCoords[0]] = 'X';
        }
    }
    void checkForGoldmines(vectorMap map)
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
            if (enemyPath.size() > 1 + braveWarrior.getRange())
            {
                std::string currCommand = "";

                requiredMoves = enemyPath.size() - braveWarrior.getRange() - 1;
                correctMove = enemyPath.size() - (requiredMoves > remainingMoves ? remainingMoves : requiredMoves) - 1;
                currentPos = enemyPath[correctMove];
                currCommand += std::to_string(braveWarrior.getId()) + " M " + std::to_string(currentPos[0]) + " " + std::to_string(currentPos[1]) + "\n";
                remainingMoves -= requiredMoves;

                returnString += currCommand;

                // we must get to enemy - kill all!
                // the enemy path array has coordinates of every step of the path - basically we need to choose correct coordinates from the available list
            }
            if ((distance(currentPos, enemyPath[0]) <= braveWarrior.getRange() || enemyPath.size() == 0) && remainingMoves > 0)
            {
                // we are close enough to enemy to attack him - generate attack command
                std::string currCommand = "";
                currCommand += std::to_string(braveWarrior.getId()) + " A " + std::to_string(getAttackedEnemyId(enemyPath[0])) + "\n";

                returnString += currCommand;
            }
        }
        return returnString;
    }
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

                if (!insideMine)
                {

                    path minePath = calculatePath(map, worker);
                    std::cout << minePath.size();
                    if (minePath.size() == 0)
                    {
                        continue;
                    }
                    // miner must go to the first field of the path
                    std::array<int, 2> moveCoords;
                    moveCoords = (worker.getRange() >= minePath.size() - 1 ? minePath[0] : minePath[minePath.size() - worker.getRange() - 2]);
                    returnString += std::to_string(worker.getId()) + " M " + std::to_string(moveCoords[0]) + " " + std::to_string(moveCoords[1]) + "\n";
                }
            }
        }
        return returnString;
    }
    // Shortest path to another point calculated by implementing djikstra algorithm - first element are enemy coords, last is our coords
    path calculatePath(vectorMap map, Unit unit)
    {
        // std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> enemyCoords;
        path returnPath;
        std::multiset<IntArrayPair> unvisited;
        std::multiset<IntArrayPair> visited;
        std::vector<std::vector<bool>> unvisitedMatrix;
        std::vector<std::vector<std::string>> previous;
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
                IntArrayPair p1;
                if (unvCoord[0] == coords[0] && unvCoord[1] == coords[1])
                {
                    // We set starting point distance to 0 - we start here
                    p1 = {0, coords};
                }
                else if (map[unvCoord[1]][unvCoord[0]] == '9')
                {
                    // we dont want to add ground to the unvisited array
                }
                else
                {
                    // distance to every other vertex apart from starting one is possibly large
                    p1 = {INT32_MAX, unvCoord};
                }
                unvisited.insert(p1);
            }
        }
        bool found = false;
        while (!found && !unvisited.empty())
        {

            const IntArrayPair &firstElement = *unvisited.begin();
            if (firstElement.first == INT32_MAX)
            {
                break;
            }
            std::vector<std::array<int, 2>> neighbours;
            std::array<int, 2> currCorr = firstElement.second;
            (currCorr[0] > 0 ? neighbours.push_back({currCorr[0] - 1, currCorr[1]}) : "");
            (currCorr[0] < maxX ? neighbours.push_back({currCorr[0] + 1, currCorr[1]}) : "");
            (currCorr[1] > 0 ? neighbours.push_back({currCorr[0], currCorr[1] - 1}) : "");
            (currCorr[1] < maxY ? neighbours.push_back({currCorr[0], currCorr[1] + 1}) : "");

            for (std::array<int, 2> neighbour : neighbours)
            {

                if (unvisitedMatrix[neighbour[1]][neighbour[0]])
                {

                    continue;
                }

                char currentNeighbour = map[neighbour[1]][neighbour[0]];

                if ((currentNeighbour == 'X' && unit.getType() != 'W') || (currentNeighbour == '6' && unit.getType() == 'W'))
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

                            unvisited.erase(foundElement);
                            IntArrayPair p1 = {firstElement.first + 1, secondElement.second};
                            // Up,Down,Left,Right
                            if (firstElement.second[0] - secondElement.second[0] != 0)
                            {
                                previous.at(secondElement.second[1]).at(secondElement.second[0]) = (firstElement.second[0] - secondElement.second[0] == 1 ? "R" : "L");
                            }
                            else if (firstElement.second[1] - secondElement.second[1] != 0)
                            {
                                previous[secondElement.second[1]][secondElement.second[0]] = (firstElement.second[1] - secondElement.second[1] == 1 ? "D" : "U");
                            }

                            unvisited.insert(p1);
                        }
                    }
                }

                unvisitedMatrix[neighbour[1]][neighbour[0]] = true;
            }

            visited.insert(*unvisited.begin());
            unvisited.erase(unvisited.begin());
        }
        bool formingResult = true;
        if (found)
        {

            returnPath.push_back(enemyCoords);
            while (formingResult)
            {
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
                    // returnPath.push_back({currentX, currentY});
                    formingResult = false;
                }
            }
        }
        return returnPath;
    }
    void giveOrders(std::string orderFileName, vectorMap map)
    {
        std::string commandString = "";
        commandString += orderBase();

        commandString += orderCombatUnits(map);
        commandString += orderWorkers(map);
        std::ofstream ofs(orderFileName, std::ofstream::trunc);
        ofs << commandString;
        ofs.close();
    }

    int distance(std::array<int, 2> p1, std::array<int, 2> p2)
    {
        return abs(p1[1] - p2[1]) + abs(p1[0] - p2[0]);
    }
    int getAttackedEnemyId(std::array<int, 2> enemycoord)
    {
        for (Unit enemy : this->enemies)
        {
            if (enemy.getCoords() == enemycoord)
            {
                return enemy.getId();
            }
        }
        return -1;
    }
    void testPath(vectorMap map, Unit unit)
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