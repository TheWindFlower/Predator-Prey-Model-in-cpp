#include <vector>
#include <tuple>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <unordered_map>
#include "../include/utils.hpp"

using std::tuple;
using std::vector;

// game rule cap
const int UNDER_POPULATION_CAP = 2;        // below that -> die DEFAULT = 2
const int OVER_POPULATION_CAP = 3;         // over that -> die or reproduce DEFAULT = 3
const int REPRODUCTION_POPULATION_CAP = 3; // eproduce cap  DEFAULT = 3
/*
notable exemple(U=UNDER_POPULATION_CAP, O=OVER_POPULATION_CAP, R=REPRODUCTION_POPULATION_CAP):
U=1
O=4
R=3
lineare pattern

U=4
O=9
R=5
cave pattern
*/

// determine the integer the most frequent in a vector
int mostFrequent(vector<int> &vec)
{
    // some chatGPT black magic
    std::unordered_map<int, int> freq;
    int max_freq = 0, res = vec[0];

    for (int num : vec)
    {
        freq[num]++;
        if (freq[num] > max_freq)
        {
            max_freq = freq[num];
            res = num;
        }
    }
    return res;
}

// return the next board of the game given the current one
vector<tuple<int, int, bool, int>> next_generation(const vector<tuple<int, int, bool, int>> &current_status, int rows, int columns)
{
    vector<tuple<int, int, bool, int>> next_status;
    // for each cell count the friendly || enemy neighbors
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            int alive_neighbors = 0;
            int friendly_neighbors = 0;
            int enemy_neighbors = 0;
            int cells_type = 0;                                                      // default = dead
            bool current_cell_status = std::get<2>(current_status[i * columns + j]); // dead or alive
            if (std::get<2>(current_status[i * columns + j]) == true)                // if the cell is alive then get the species
            {
                cells_type = std::get<3>(current_status[i * columns + j]); // get the species
            }
            else
            {
                cells_type = 0; // dead cell
            }

            // Check neighbors (for each cell check the neighbors cells)
            vector<int> enemy_neighbors_vect; // vector of enemy species
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {

                    int neighbor_row = i + x;
                    int neighbor_column = j + y;
                    // Check if neighbor is within bounds
                    if (neighbor_row >= 0 && neighbor_row < rows && neighbor_column >= 0 && neighbor_column < columns)
                    {
                        // Ignore current cell
                        if (x == 0 && y == 0)
                        {
                            continue;
                        }
                        // Count alive neighbors
                        if (std::get<2>(current_status[neighbor_row * columns + neighbor_column]))
                        {
                            alive_neighbors++;
                            if (std::get<3>(current_status[neighbor_row * columns + neighbor_column]) == cells_type) // if the cells is dead(species=0) then their is no friendly
                            {
                                friendly_neighbors++;
                            }
                            else
                            {
                                enemy_neighbors_vect.push_back(std::get<3>(current_status[neighbor_row * columns + neighbor_column]));
                                enemy_neighbors++;
                            }
                        }
                    }
                }
            }

            // Apply Game of Life rules
            if (current_cell_status && alive_neighbors < UNDER_POPULATION_CAP)
            {
                // underpopulation
                next_status.emplace_back(i, j, false, cells_type); // die
            }
            else if (current_cell_status && (alive_neighbors == UNDER_POPULATION_CAP || alive_neighbors == OVER_POPULATION_CAP))
            {
                // Survival
                next_status.emplace_back(i, j, true, cells_type); // stay alive
            }
            else if (current_cell_status && alive_neighbors > OVER_POPULATION_CAP)
            {
                // Overpopulation
                next_status.emplace_back(i, j, false, cells_type); // die
            }
            else if (!current_cell_status && alive_neighbors == REPRODUCTION_POPULATION_CAP)
            {
                // Reproduction
                // need to check witch neighbors is the more present
                next_status.emplace_back(i, j, true, mostFrequent(enemy_neighbors_vect));
            }
            else
            {
                // Stasis
                next_status.emplace_back(i, j, current_cell_status, cells_type);
            }
        }
    }
    return (next_status);
}

vector<tuple<int, int, bool, int>> start(std::string filename, int rows, int columns)
{
    vector<tuple<int, int, bool, int>> coordinates;

    // Initialize all coordinates to false (dead cells so no species)
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            coordinates.push_back(std::make_tuple(i, j, false, 0));
        }
    }

    // Open file and set bool to true (1) for specified coordinates and set
    std::ifstream inputFile(filename);
    int x, y, kind;
    while (inputFile >> x >> y >> kind)
    {
        std::cout << kind << std::endl;
        int index = x * columns + y;
        coordinates[index] = std::make_tuple(x, y, true, kind);
    }
    inputFile.close();

    return coordinates;
}

void grid_gen(int x, int y)
{
    std::srand(time(NULL)); // seed the random number generator

    vector<std::string> end;
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            if ((double)std::rand() / RAND_MAX < 0.5)
            {
                if (i >= int(x / 2) && j >= int(y / 2)) // top right
                {
                    end.push_back(std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(1)); // blue
                }
                else if (i <= int(x / 2) && j <= int(y / 2)) // top left
                {
                    end.push_back(std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(3)); // green
                }
                else if (i >= int(x / 2) && j <= int(y / 2)) // bottom right
                {
                    end.push_back(std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(2)); // red
                }
                else // bottom left
                {
                    end.push_back(std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(4)); // pink
                }
            }
        }
    }

    std::ofstream file("../data/board.brd");
    file.clear(); // clear the file
    for (const std::string &cell : end)
    {
        file << cell << std::endl;
    }
    file.close();
}
