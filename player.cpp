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
        std::string currentStatistic;
        std::istringstream iss(line);
        std::string s;
        std::vector<std::string> moves;
        while (getline(iss, s, ' '))
        {
            moves.push_back(s);
        }

        if (moves.size() == 6)
        {
            moves.push_back("-1");
        }
        Unit unit = Unit(moves[1].c_str()[0], stoi(moves[2]), stoi(moves[3]), stoi(moves[4]), stoi(moves[5]), moves[6].c_str()[0]);

        switch (moves[0].c_str()[0]) // Unit(char type, int id, int x, int y, int health, char production)
        {
        case 'E':
            commander.addEnemy(unit);
            break;
        case 'P':
            switch (moves[1].c_str()[0])
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
