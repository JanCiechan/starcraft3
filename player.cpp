#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <array>
#include "./player_dependencies/commander.cpp"
#include <sstream>
typedef std::vector<std::vector<char>> vectorMap;

vectorMap loadmap(std::string filename);
void showMap(vectorMap mapCoord);
void createCommander(Commander &commander, std::string stateFileName);

int main()
{

    auto start = std::chrono::high_resolution_clock::now();
    std::string mapName = "mapa.txt";
    std::string state = "status.txt";
    std::string commands = "rozkazy.txt";
    std::string testCommands = "test.txt";
    int timelimit = 5;

    vectorMap mapCoord = loadmap(mapName);

    Commander commander;
    createCommander(commander, state);

    commander.commanderSetup(mapCoord);
    commander.giveOrders(testCommands, mapCoord);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << ((double)duration.count() / 1000.00) << " seconds";
    return 0;
}

vectorMap loadmap(std::string filename)
{
    vectorMap mapa;
    std::string line;
    std::ifstream infile(filename);
    while (std::getline(infile, line))
    {
        std::vector<char> row;

        for (char &c : line)
        {
            if (c != '\n')
            {
                row.push_back(c);
            }
        }

        mapa.push_back(row);
    }

    infile.close();
    return mapa;
}

void createCommander(Commander &commander, std::string stateFileName)
{
    std::string line;
    std::ifstream infile(stateFileName);
    std::getline(infile, line);
    commander.setGold(stoul(line));

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string s;
        std::vector<std::string> currentUnit;
        // we need to iterate over line by using spaces as some data might be longer than one character
        while (getline(iss, s, ' '))
        {
            currentUnit.push_back(s);
        }

        if (currentUnit.size() == 6) // this means current unit is not a base - we dont have/need production data so we place -1 there
        {
            currentUnit.push_back("-1");
        }
        Unit unit = Unit(currentUnit[1].c_str()[0], stoi(currentUnit[2]), stoi(currentUnit[3]), stoi(currentUnit[4]), stoi(currentUnit[5]), currentUnit[6].c_str()[0]);
        // where to insert current unit - either as enemy or as some kind of player unit
        switch (currentUnit[0].c_str()[0])
        {
        case 'E':
            commander.addEnemy(unit);
            break;
        case 'P':
            switch (currentUnit[1].c_str()[0])
            {
            case 'B':
                commander.setBase(unit);
                break;
            case 'W':
                commander.addWorker(unit);
                break;
            default:
                commander.addCombatUnit(unit);
            }
            break;
        default:;
        }
    }
}

void showMap(vectorMap mapCoord)
{
    for (std::vector<char> row : mapCoord)
    {
        for (char field : row)
        {
            std::cout << field;
        }
        std::cout << std::endl;
    }
}
