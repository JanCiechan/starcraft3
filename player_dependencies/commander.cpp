#include <iostream>
#include <vector>
#include <array>
#include <string>
#include "unit.cpp"
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
};