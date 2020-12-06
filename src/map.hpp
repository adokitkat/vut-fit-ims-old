#pragma once

#include "cell.hpp"

enum struct Wind
{
  None,
  N, NW, W, SW, S, SE, E, NE
};

std::tuple<int64_t, int64_t> loadMap(std::vector<std::vector<Cell>>& map, const std::vector<std::string>& mapChars)
{
  int64_t h = mapChars.size();
  int64_t w = mapChars[0].size();

  for (int64_t i = 0; i < h; ++i){
    std::vector<Cell> vec {};
    for (int64_t j = 0; j < w; ++j) {
      vec.push_back(Cell(i, j));
      switch (mapChars[i][j])
      {
        case 'X':
          vec[j].status = Status::BURNING;
          vec[j].type = CellType::Tree; // unknown ??
          break;

        case 'Y':
          vec[j].status = Status::NOT_BURNING;
          vec[j].type = CellType::Tree;
          break;
        
        case '@':
          vec[j].status = Status::NOT_BURNING;
          vec[j].type = CellType::Brush;
          break;

        case '.':
          vec[j].status = Status::NOT_BURNING;
          vec[j].type = CellType::Dirt;
          break;

        default:
          break;
      }
    }
    map.push_back(vec);
  }
  return {h, w};
}

void printMap(std::vector<std::vector<Cell>>& map) {
  for (auto& line : map)
  {
    for (auto& c : line)
    {
      std::cout << c.printCell();
    }
    std::cout << NEWLINE; 
  }
  std::cout << std::endl;
}