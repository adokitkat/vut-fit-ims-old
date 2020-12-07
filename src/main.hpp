#include <GL/glut.h>
//#include "cell_blob.hpp"
#include "map.hpp"

using namespace std::chrono_literals;

int updateCell(const std::vector<std::vector<Cell>>& map, std::vector<std::vector<Cell>>& newMap, int64_t i, int64_t j, Wind wDirection = Wind::None, int64_t radius = 2, int64_t k = 1, int64_t big_wind = 3)
{
  int64_t h = map.size(),
          w = map[0].size();
  
  int64_t a {-1*radius}, b {1*radius},
          c {-1*radius}, d {1*radius};

  switch(wDirection) {
    case Wind::N:
      //if (k != 1)
      //  { a += 1; } // 1*a-(k-a)
      a += 1; b += 1*k;
      c += 0; d += 0;
      if (k >= big_wind) { a = radius; }
      break;

    case Wind::NW:
      a += 1;  b += 1*k;
      c += 1;  d += 1*k;
      if (k >= big_wind) { a = radius; c = radius; }
      break;

    case Wind::W:
      a += 0;  b += 0;
      c += 1;  d += 1*k;
      if (k >= big_wind) { c = radius; }
      break;

    case Wind::SW:
      a += -1*k; b += -1;
      c += 1;    d += 1*k;
      if (k >= big_wind) { b = -radius; c = radius; }
      break;
    
    case Wind::S:
      a += -1*k; b += -1;
      c += 0;    d += 0;
      if (k >= big_wind) { b = -radius; }
      break;

    case Wind::SE:
      a += -1*k; b += -1;
      c += -1*k; d += -1;
      if (k >= big_wind) { b = -radius; d = -radius; }
      break;

    case Wind::E:
      a += 0;    b += 0;
      c += -1*k; d += -1;
      if (k >= big_wind) { d = -radius; }
      break;

    case Wind::NE:
      a += 1;    b += 1*k;
      c += -1*k; d += -1;
      if (k >= big_wind) { a = radius; d = -radius; }
      break;
    
    default:
      break;
  }

  int64_t distance {1};
  int64_t abs_x {0},
          abs_y {0};

  for (auto x = (i + a); x <= (i + b); ++x) 
  {
    for (auto y = (j + c); y <= (j + d); ++y)
    {
      if (x == i and y == j) { continue; }

      if (x >= 0 and x < h and y >= 0 and y < w)
      {
        if (map[i][j].active)
        {
          distance = 1;
          abs_x = abs(i-x);
          abs_y = abs(j-y);
          if (abs_x > radius/2 or abs_y > radius/2) {
            if (abs_x > abs_y) {
              distance = abs_x;
            } else {
              distance = abs_y;
            }
          }

          switch (map[x][y].status) // Neighbor status
          {
            case Status::NOT_BURNING:
              //map[i][j] ; // Current Cell
              break;
            
            case Status::BURNING:
              
              switch(map[i][j].type) { // My type
                case CellType::Tree:  // My type == Tree
              
                  if (newMap[i][j].status == Status::NOT_BURNING) // My status
                  {
                    switch (map[x][y].type) // Neighbor type
                    {
                      case CellType::Tree:
                        newMap[i][j].flammability += std::pow(0.1, distance); // TODO: range
                        break;
                      
                      case CellType::Brush:
                        newMap[i][j].flammability += std::pow(0.08, distance); // TODO: range
                        break;
                      
                      default:
                        break;
                    }
                  } 
                  break;

                case CellType::Brush: // My type == Brush
                  if (newMap[i][j].status == Status::NOT_BURNING) // My status
                  {
                    switch (map[x][y].type) // Neighbor type
                    {
                      case CellType::Tree:
                        newMap[i][j].flammability += std::pow(0.1, distance); // TODO: range
                        break;
                        
                      case CellType::Brush:
                        newMap[i][j].flammability += std::pow(0.12, distance); // TODO: range
                        break;
                        
                      default:
                        break;
                    }
                  }
                  break;

                  default:
                    break;
              }
              
              // 

              break;

            case Status::BURNED:
              break;
            
            default:
              break;
          }
        }
        //if(newMap[x][y].flammability > 0.3) { std::cout << map[x][y].flammability << " " << newMap[x][y].flammability << NEWLINE; }
      }
    }
  }
  return 0;
}


void updateCellAll(std::vector<std::vector<Cell>>&map, std::vector<std::vector<Cell>>& newMap)
{
  //#pragma omp parallel for
  for (int64_t i = 0; i < h; ++i) {
    for (int64_t j = 0; j < w; ++j) {
      updateCell(map, newMap, i, j, wind, radius_global, k_global, big_wind_global);
    }
  }
}

std::ofstream initLog(std::string filename)
{
  std::ofstream outfile;
  outfile.open(filename, std::ofstream::out | std::ios_base::trunc); // append instead of overwrite
  outfile << "";
  return outfile;
}

void logAdd(std::ofstream outfile, std::string out)
{
  if (outfile.is_open())
  {
    outfile << out;
  }
}