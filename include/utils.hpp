#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <tuple>
#include <string>

std::vector<std::tuple<int, int, bool, int>> next_generation(const std::vector<std::tuple<int, int, bool, int>> &current_status, int rows, int columns);
std::vector<std::tuple<int, int, bool, int>> start(std::string filename, int rows, int columns);
void grid_gen(int x, int y);
#endif
