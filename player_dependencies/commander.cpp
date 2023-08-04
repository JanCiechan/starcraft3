#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <set>
#include <utility>
#include "unit.cpp"
typedef std::vector<std::vector<char>> vectorMap;
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
class Commander
{
private:
    Unit base;

    std::vector<Unit> combatUnits;

    std::vector<Unit> workers;

    std::vector<Unit> enemies;

    int gold;

public:
    Commander(Unit base, std::vector<Unit> combatUnits, std::vector<Unit> workers, std::vector<Unit> enemies, int gold)
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

    void setGold(int gold)
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
        return "";
    }
    std::string orderCombatUnits()
    {
        return "";
    }
    std::string orderWorkers()
    {
        return "";
    }
    // Shortest path to another point calculated by implementing djikstra algorithm
    std::vector<std::array<int, 2>> calculatePath(vectorMap map, Unit unit)
    {
        // std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> coords = unit.getCoords();
        std::array<int, 2> enemyCoords;
        std::vector<std::array<int, 2>> returnPath;
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
        while (!found)
        {

            const IntArrayPair &firstElement = *unvisited.begin();

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

                if (currentNeighbour == 'X')
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

            std::cout << visited.size() << " " << unvisited.size() << std::endl;
            // found = true;
        }

        bool formingResult = true;
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
                returnPath.push_back({currentX, currentY});
                formingResult = false;
                ;
            }
        }
        // std::cout << firstElement.first;
        return returnPath;
    }
    void giveOrders(std::string orderFileName, vectorMap map)
    {
        calculatePath(map, this->base);
    }

    int distance(std::array<int, 2> p1, std::array<int, 2> p2)
    {
        return abs(p1[1] - p2[1]) + abs(p1[0] - p2[0]);
    }
};