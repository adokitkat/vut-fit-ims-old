#pragma once

#include "cell.hpp"

class CellBlob 
{
private:
  /* data */
public:
  CellBlob(std::vector<Cell>, float);
  ~CellBlob();

  //bool crown; // if blob fuel is tree set true
  //bool shrub; // if blob fuel is shrub set true

  //int burning_speed;
  //int fuel_amount;

  float density;
  std::vector<Cell> cells; // blob surface of active and inactive cells
  void fill_cell_blob(Cell cell);
};

CellBlob::CellBlob(std::vector<Cell> cells, float density)
{
  this->cells = cells;
  this->density = density;
}

CellBlob::~CellBlob()
{

}

void CellBlob::fill_cell_blob(Cell cell)
{
  this->cells.push_back(cell);
}