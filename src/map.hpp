#pragma once

#include "cell.hpp"

// Map global variables
std::vector<std::vector<Cell>>  map,
                                newMap;

float pseudo_rand()
{
  return (float)rand()/13376.9f;  
}

float rand_mod_100()
{
  return static_cast<int>((float)rand()/4857.0f) % 100;
}

void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nWidth; x++)
			for (int y = 0; y < nHeight; y++)
			{				
				float fNoise = 0.0f;
				float fScaleAcc = 0.0f;
				float fScale = 1.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;
					
					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;					
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
					float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}

				// Scale to seed range
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}
	
	}

/**
 * @brief:  function for generating cell of given type into map or the map itself; 
 *          to generate empty map --> _seed = 0.0f or type = default);
 *          to empty already generated map of given size --> _seed = 999999.0f and type = deafault;
 * 
 * @size: sizeXsize map matrix to generate; needs declaration of size even if its generated; can enlarge the map (not shrink);
 * @type: which seed to "plant" 
 * @_seed: 10000 <= _seed: less dense; 100000 >= _seed: more dense;
 * 
 */
//TODO: FOR NOW CELLS ARE TYPE OF CHAR!!! THEY NEED THE TYPE OF CELL AND SOME GOOD ATRIBUTE FILLING BOI!!!
std::vector<std::string> seed(std::vector<std::string> mapChars, int64_t size = MAX_SIZE, char type = '.', float _seed = 40000.0f)
{
  float *noise_seed = new float[size*size];
  float *out_seed   = new float[size*size];

  for (int i = 0; i < size * size; i++)
    { noise_seed[i] = pseudo_rand(); }
  PerlinNoise2D(size, size, noise_seed, 6, 0.4f, out_seed);
  
  //for (auto x : out_seed) {}
  for(auto y = 0; y < size; y++)
  {
    std::string row;
    if(y < (int64_t)mapChars.size()) { row = mapChars[y]; }
    for(auto x = 0; x < size; x++)
    {
      //std::cout << pseudo_rand() << " seed:" << _seed << "\n";
      //std::cout << (int)(out_seed[y * size + x] * 0.6f) << "\n";
      if( (int)(out_seed[y * size + x] * 0.5f) < _seed && pseudo_rand() < _seed + 10000.0f)
      {
        if(x < (int64_t)row.size())
        {
          row[x] = type; //TODO: i am active cell of this type... fill my atributes daddy
        }
        else
        {
          row.push_back(type); //TODO: i am active cell of this type... fill my atributes daddy
        }
      }
      else
      {
        if(x < (int64_t)row.size())
        {
          continue;
        }
        else
        {
          row.push_back('.'); //TODO: i am inactive cell of this type... just set me to active = false
        }
      }
    }
    if(y < (int64_t)mapChars.size()){
      mapChars[y] = row;
    }
    else
    {
      mapChars.push_back(row);
    }
  }
  delete(noise_seed);
  delete(out_seed);
  return mapChars;
}

 std::string getWindDirection()
  {
    switch (wind)
    {
      case Wind::None:
        return "None";
        break;
      
      case Wind::N:
        return "N";
        break;
      
      case Wind::NW:
        return "NW";
        break;

      case Wind::W:
        return "W";
        break;

      case Wind::SW:
        return "SW";
        break;

      case Wind::S:
        return "S";
        break;
      
      case Wind::SE:
        return "SE";
        break;

      case Wind::E:
        return "E";
        break;
      
      case Wind::NE:
        return "NE";
        break;

      default:
        return "";
        break;
    }
    return "";
  }

namespace Map {

  void loadMap(std::vector<std::vector<Cell>>& map, const std::vector<std::string>& mapChars, int64_t h, int64_t w)
  {
    for (int64_t i = 0; i < h; ++i){
      std::vector<Cell> vec {};
      for (int64_t j = 0; j < w; ++j) {
        vec.push_back(Cell(i, j));
        switch (mapChars[i][j])
        {
          case 'X':
            vec[j].status = Status::BURNING;
            vec[j].type = CellType::Tree; // unknown ??
            vec[j].active = true;
            break;

          case 'Y':
            vec[j].status = Status::NOT_BURNING;
            vec[j].type = CellType::Tree;
            vec[j].active = true;
            break;
          
          case '@':
            vec[j].status = Status::NOT_BURNING;
            vec[j].type = CellType::Brush;
            vec[j].active = true;
            break;

          case '.':
            vec[j].status = Status::NOT_BURNING;
            vec[j].type = CellType::Dirt;
            vec[j].active = false;
            break;

          default:
            break;
        }
      }
      map.push_back(vec);
    }
  }

  void printMap(std::vector<std::vector<Cell>>& map) {
    for (auto& line : map)
    {
      for (auto& c : line)
      {
        std::cout << c.cellStatusChar();
      }
      std::cout << NEWLINE; 
    }
    std::cout << std::endl;
  }

  int updateMapStatus(std::vector<std::vector<Cell>>& map)
  {
    int64_t h = map.size(),
            w = map[0].size();
    float rand;
    
    for (int64_t i = 0; i < h; ++i)
    {
      for (int64_t j = 0; j < w; ++j)
      {
        if (map[i][j].active)
        {
          switch(map[i][j].status) {
            case Status::BURNING:
              if (map[i][j].type == CellType::Tree)
              {
                if (map[i][j].burning_tick >= tree_burning_time)
                {
                  map[i][j].status = Status::BURNED;
                  ++burned;
                  ++burned_tree;
                }
                else
                {
                  ++burning;
                  ++burning_tree;
                }
              }
              else if (map[i][j].type == CellType::Brush)
              {
                if (map[i][j].burning_tick >= brush_burning_time)
                {
                  map[i][j].status = Status::BURNED;
                  ++burned;  
                  ++burned_brush;
                }
                else
                {
                  ++burning;
                  ++burning_brush;
                }
                
              }

              ++map[i][j].burning_tick;
              break;

            case Status::NOT_BURNING:
              rand = rand_mod_100();
              if (map[i][j].flammability*100 > rand)//* 100 >= rand_mod_100())
              {
                map[i][j].status = Status::BURNING;
                ++burning;

                if (map[i][j].type == CellType::Tree)
                  { ++burning_tree; }
                else if (map[i][j].type == CellType::Brush)
                  { ++burning_brush; }

              } else
              {
                ++not_burning;

                if (map[i][j].type == CellType::Tree)
                  { ++not_burning_tree; }
                else if (map[i][j].type == CellType::Brush)
                  { ++not_burning_brush; }
              }
              
              break;

            case Status::BURNED:
              ++burned;

              if (map[i][j].type == CellType::Tree)
                { ++burned_tree; }
              else if (map[i][j].type == CellType::Brush)
                { ++burned_brush; }
              break;

            default:
              break;
          }
        }
        map[i][j].flammability = 0.0;
      }
    }
    
     
    if (tick % log_period == 0) 
    {
      std::cout << "Tick: " << tick << "\t\tWind direction: " << getWindDirection() << "\tWind intensity: " << k_global << "\tActive: " << not_burning+burning+burned << NEWLINE
        << " Not burning (All):\t" << not_burning << "\tBurning (All):\t\t" << burning << "\tBurned (All):\t\t" << burned << "\tAffected (All):\t\t" << burning+burned << NEWLINE
        << " Not burning (Tree):\t" << not_burning_tree << "\tBurning (Tree):\t\t" << burning_tree << "\tBurned (Tree):\t\t" << burned_tree << "\tAffected (Tree):\t" << burning_tree+burned_tree << NEWLINE
        << " Not burning (Brush):\t" << not_burning_brush << "\tBurning (Brush):\t" << burning_brush << "\tBurned (Brush):\t\t" << burned_brush << "\tAffected (Brush):\t" << burning_brush+burned_brush << NEWLINE
        << std::endl;
      
      if (logging)
      {
        map_state_log
          << tick << " " << getWindDirection() << " " << not_burning+burning+burned << " " << not_burning << " " << burning << " " << burned << " " << burning+burned  
          << " " << not_burning_tree << " " << burning_tree << " " << burned_tree << " " << burning_tree+burned_tree
          << " " << not_burning_brush << " " << burning_brush << " " << burned_brush << " " << burning_brush+burned_brush
          << " " << k_global << std::endl;
      }
    }
    ++tick;

    burning = 0;
    burned = 0;
    not_burning = 0;

    not_burning_tree = 0;
    burning_tree = 0;
    burned_tree = 0;

    not_burning_brush = 0;
    burning_brush = 0;
    burned_brush = 0;

    return 0;
  }

  void igniteCellRadius(std::vector<std::vector<Cell>>& map, int64_t i, int64_t j, int64_t radius = 3) {
    
    for (auto x = (i - radius); x <= (i + radius); ++x) 
    {
      for (auto y = (j - radius); y <= (j + radius); ++y)
      {
        if (x >= 0 and x < h and y >= 0 and y < w)
        {
          if (map[x][y].status == Status::NOT_BURNING)
            { map[x][y].status = Status::BURNING; }
        }
      }
    }

  }

}
