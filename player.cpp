#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

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

std::vector<std::vector<char>> loadmap(std::string filename);

int main()
{
    std::string mapName = "mapa.txt";
    std::string state = "status.txt";
    std::string commands = "rozkazy.txt";
    auto start = std::chrono::high_resolution_clock::now();
    int timelimit = 5;
    std::vector<std::vector<char>> mapCoord = loadmap(mapName);

    std::this_thread::sleep_for(std::chrono::seconds(10));
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << ((double)duration.count() / 1000.00);
    return 0;
}

std::vector<std::vector<char>> loadmap(std::string filename)
{
    std::vector<std::vector<char>> mapa;
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