#pragma once

#include "etc.hpp"

#define WATER -7.0
#define DIRT 0.0
#define GRASS 0.5
#define TREE 0.9

enum struct Status 
{
  NOT_BURNING,
  BURNING,
  BURNED
};

enum struct CellType 
{
  Unknown,
  Dirt,
  Tree,
  Brush
};

class Cell {
private:
  /* data */
public:
  Cell();
  Cell(int64_t, int64_t);
  Cell(int64_t, int64_t, bool, double, double, double, int64_t, int64_t, Status, CellType);
  ~Cell();

  int64_t x = 0;
  int64_t y = 0;
  bool    active       = false; // empty space or filled with fuel
  double  neigh_prob   = 0.0;
  double  height       = 0.0; // height of the cell imported from altitude map
  double  flammability = 0.0;

  int64_t fuel_amount  = 0;
  int64_t burning_tick = 0;

  Status status = Status::NOT_BURNING;
  CellType type = CellType::Unknown;

  char cellStatusChar();

  void igniteCellRadius(std::vector<std::vector<Cell>>&, int64_t, int64_t, int64_t);
};

Cell::~Cell() {}

Cell::Cell(int64_t x, int64_t y)
{
  this->x = x;
  this->y = y;
}

Cell::Cell(int64_t x, int64_t y, bool active, double neigh_prob, double height, double flammability, int64_t fuel_amount, int64_t burning_tick, Status status, CellType type)
{
  this->x = x;
  this->y = y;
  this->active = active;
  this->neigh_prob = neigh_prob;
  this->height = height;
  this->flammability = flammability;
  this->fuel_amount = fuel_amount;
  this->burning_tick = burning_tick;
  this->status = status;
  this->type = type;
}



char Cell::cellStatusChar() {
  switch (this->type)
  {
    case CellType::Tree:
      if (this->status == Status::NOT_BURNING) { return 'Y';}
      else if (this->status == Status::BURNING) { return 'X';}
      break;
    
    case CellType::Brush:
      if (this->status == Status::NOT_BURNING) { return '@';}
      else if (this->status == Status::BURNING) { return 'X';}
      break;

    case CellType::Unknown:
      if (this->status == Status::NOT_BURNING) { return 'U';}
      else if (this->status == Status::BURNING) { return 'X';}
      break;

    case CellType::Dirt:
      return '.';
      break;
    
    default:
      return 'Z';
      break;
  }
  return 'E';
}
