#include <GL/glut.h>
//#include "cell_blob.hpp"
#include "map.hpp"
#include <execution>

using namespace std::chrono_literals;

//TODO: zapalnost sa znizuje vzhladom na vzdialenost, nahodnejsia zapalnost, stav dohorene

// Global variabiable map for callback GLUT function
std::vector<std::vector<Cell>>  map,
                                newMap;
int64_t MAX_SIZE {1000},
        h {0},
        w {0},
        radius_global {2}, // Radius of spread
        k_global {1}, // Wind speed
        big_wind_global {3}, // Wind speed treshold
        tree_burning_time {162},
        brush_burning_time {100};
Wind wind { Wind::None };
int64_t tick {0};

// Mouse events
bool  leftMouseButtonDown { false },
      rightMouseButtonDown { false };
int64_t mouseXPos {0},
        mouseYPos {0};

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
        //if(newMap[x][y].flammability > 0.3) { std::cout << map[x][y].flammability << " " << newMap[x][y].flammability << NEWLINE; }
      }
    }
  }
  return 0;
}

void updateCellFunc(std::vector<std::vector<Cell>>&map, std::vector<std::vector<Cell>>& newMap)
{
  //#pragma omp parallel for
  for (int64_t i = 0; i < h; ++i) {
    for (int64_t j = 0; j < w; ++j) {
      updateCell(map, newMap, i, j, wind, radius_global, k_global, big_wind_global);
    }
  }

  //std::cout << std::endl;
  
/*
  std::vector<std::string> foo;
  std::for_each(
    std::execution::par_unseq,
    map.begin(), map.end(),
    [](auto&& item)
    {
        //do stuff with item
    });
*/
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
      switch(map[i][j].status) {
        case Status::BURNING:
          if (map[i][j].type == CellType::Tree)
          {
            if (map[i][j].burning_tick >= tree_burning_time)
              { map[i][j].status = Status::BURNED; }
          }
          else if (map[i][j].type == CellType::Brush)
          {
            if (map[i][j].burning_tick >= brush_burning_time)
              { map[i][j].status = Status::BURNED; }
          }

          ++map[i][j].burning_tick;
          break;

        case Status::NOT_BURNING:
          rand = rand_mod_100();
          if (map[i][j].flammability*100 > rand)//* 100 >= rand_mod_100())
          {
            map[i][j].status = Status::BURNING;
          }
          break;

        default:
          break;
      }
      map[i][j].flammability = 0.0;
    }
  }
  ++tick;
  if (tick % 100 == 0) std::cout << tick << std::endl;
  return 0;
}

void igniteCell(std::vector<std::vector<Cell>>& map, int64_t i, int64_t j) {
  
  for (auto x = (i - 3); x <= (i + 3); ++x) 
  {
    for (auto y = (j - 3); y <= (j + 3); ++y)
    {
      if (x == i and y == j) { continue; }

      if (x >= 0 and x < h and y >= 0 and y < w)
      {
        if (map[x][y].status == Status::NOT_BURNING)
          { map[x][y].status = Status::BURNING; }
      }
    }
  }

}

void mouse(int button, int state, int x, int y)
{
    // Save the button state
    if (button == GLUT_LEFT_BUTTON)
      { leftMouseButtonDown = (state == GLUT_DOWN); }

    else if (button == GLUT_RIGHT_BUTTON)
      { rightMouseButtonDown = (state == GLUT_DOWN); }

    // Save the mouse position
    mouseXPos = (int64_t)y;
    mouseYPos = (int64_t)x;
}

void display()
{
  GLfloat minSize = 60.0f/map.size();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 60, 60, 0.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, MAX_SIZE, MAX_SIZE);

  if (leftMouseButtonDown) {
    igniteCell(map, mouseXPos, mouseYPos);
  }

  // Update data of every cell on map
  updateCellFunc(map, newMap);
  // Update status of every cell
  updateMapStatus(newMap);
  map = newMap;

  for (auto [i, line] : enumerate(map))
  {
    for (auto [j, c]: enumerate(line))
    {
      switch(c.status) {
        case Status::NOT_BURNING:
          if (c.type == CellType::Tree) {
            // light green
            glColor3f(0.0f, 0.8f, 0.0f);
          }
          else if (c.type == CellType::Brush) {
            // dark green 
            glColor3f(0.4f, 0.8f, 0.4f);
          }
          else if (c.type == CellType::Dirt) {
            // brown
            glColor3f(0.7f, 0.6f, 0.4f);
          }
          
          break;

        case Status::BURNING:
          // red
          glColor3f(0.8f, 0.0f, 0.0f);
          break;

        case Status::BURNED:
          // gray
          glColor3f(0.1f, 0.1f, 0.1f);
          break;
        
        default:
          break;
      }

      glBegin(GL_QUADS); // 2x2 pixels
        glVertex2f(0.0f+minSize*j,     0.0f+minSize*i);
        glVertex2f(0.0f+minSize*(j+1), 0.0f+minSize*i);
        glVertex2f(0.0f+minSize*(j+1), 0.0f+minSize*(i+1));
        glVertex2f(0.0f+minSize*j,     0.0f+minSize*(i+1));
      glEnd();
    }
  }

  glutSwapBuffers();
  glutPostRedisplay();
  //std::this_thread::sleep_for(1ms);
}

int main (int argc, char *argv[])
{

  // Variables init
  bool  gui {false},
        show_help {false},
        load_map {false};
  
  // Parses arguments, fills variables with values
  parseArgs(argc, argv, gui, show_help, load_map);

  if (show_help) { showHelp(); exit(EXIT_SUCCESS); }

  srand(time(NULL)); // initialize random function
  std::vector<std::string> mapChars;
  mapChars = seed(mapChars, MAX_SIZE, '@', 40000.0f);
  mapChars = seed(mapChars, MAX_SIZE, 'Y', 52000.0f); // seed (40000,55000)
  //TODO: sem hodit po generovani nejake ohnisko v danom bode and watch the world burn
  for (auto x = (MAX_SIZE/2 - 2); x <= (MAX_SIZE/2 + 2); ++x) 
  {
    for (auto y = (MAX_SIZE/2 - 2); y <= (MAX_SIZE/2 + 2); ++y)
    {
      mapChars[x][y] = 'X';
    }
  }

  std::tie(h, w) = loadMap(map, mapChars);
  newMap = map;

  if (gui) {
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(MAX_SIZE, MAX_SIZE);
    glutCreateWindow("IMS - Wildfire simulation - xmlkvy00, xmudry01");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
  }
  else {
    //bool running {false};
    while (true)
    {
      // Print the map
      printMap(map);
      // Update data of every cell on map
      updateCellFunc(map, newMap);
      // Update status of every cell
      updateMapStatus(newMap);
      map = newMap;
      //std::this_thread::sleep_for(1s);
    }
  }
  exit(EXIT_SUCCESS);
}