#include <iostream>
#include <string>

#include <GL/glut.h>
#include "cell_blob.hpp"
#include <vector>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) // Windows
    #define NEWLINE "\r\n"
    #include <windows.h> 
#else
    #define NEWLINE '\n' // Avoid std::endl because of proper buffering and flushing
#endif

#define MAX_SIZE 800

/*
void init()
{   
    //glViewport(0, 0, MAX_SIZE, MAX_SIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}
*/

float pseudo_rand()
{
  return (float)rand()/13376.9f;  
}


/*
void seed(float _seed = 0.05f, CellBlob::CellBlob cell_blob)
{
  for(int y = 0; y < MAX_SIZE; y++)
  {
    for(int x = 0; x < MAX_SIZE; x++)
    {
      if( pseudo_rand() < _seed) cell_blob.fill_cell_blob();
    }
  }
}
*/

/*
void seed(float _seed = 0.05f, CellBlob::CellBlob cell_blob)
{
  for(int i = 0; i < cell_blob.cells.size(); x++)
  {
    if( pseudo_rand() < _seed) cell_blob.cells[i].active = true;
  }
}
*/

  /**
   * @brief:  function for generating cell of given type into map or the map itself; 
   *          to generate empty map --> _seed = 0.0f or type = default);
   *          to empty already generated map of given size --> _seed = 999999.0f and type = deafault;
   * 
   * @size: sizeXsize map matrix to generate; needs declaration of size even if its generated; can enlarge the map (not shrink);
   * @type: which seed to "plant" 
   * @_seed: 10000 <= _seed: less dense; 100000 >= _seed: more dense;
   * 
   * 
   */
  //TODO: FOR NOW CELLS ARE TYPE OF CHAR!!! THEY NEED THE TYPE OF CELL AND SOME GOOD ATRIBUTE FILLING BOI!!!
std::vector<std::string> seed(std::vector<std::string> mapChars, int size = MAX_SIZE, char type = '.', float _seed = 40000.0f)
{
  for(int y = 0; y < size; y++)
  {
    std::string row;
    //std::cout << " halo:" << mapChars.size() << "\n";
    if(y < mapChars.size()){
      row = mapChars[y];
    }
    for(int x = 0; x < size; x++)
    {
      std::cout << pseudo_rand() << " seed:" << _seed << "\n";
      if( pseudo_rand() < _seed)
      {
        if(x < row.size())
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
        if(x < row.size())
        {
          continue;
        }
        else
        {
          row.push_back('.'); //TODO: i am inactive cell of this type... just set me to active = false
        }
      }
    }
    if(y < mapChars.size()){
      mapChars[y] = row;
    }
    else
    {
      mapChars.push_back(row);
    }
  }
  return mapChars;
}

void display()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
    
  // Draw a Red 1x1 Square centered at origin
  glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex2f(-0.5f, -0.5f);    // x, y
    glVertex2f( 0.5f, -0.5f);
    glVertex2f( 0.5f,  0.5f);
    glVertex2f(-0.5f,  0.5f);
  glEnd();
    
  glFlush();  // Render now
}

int getNeighbor(const std::vector<std::vector<Cell>>& map, std::vector<std::vector<Cell>>& newMap, int64_t i, int64_t j)
{
  int64_t h = map.size(),
          w = map[0].size();
  
  for (auto x = (i - 1); x <= (i + 1); ++x) 
  {
    for (auto y = (j - 1); y <= (j + 1); ++y) 
    {
      if (x == i and y == j) { continue; }

      if (x >= 0 and x < h and y >= 0 and y < w)
      {
        switch (map[x][y].status)
        {
          case Status::NOT_BURNING:
            //map[i][j] ; // Current Cell
            break;
          
          case Status::BURNING:
            if (map[i][j].type == CellType::Tree) {
              newMap[i][j].status = Status::BURNING;
            }
            
            break;

          case Status::BURNED:
            break;
          
          default:
            break;
        }        
      }
    }
  }
}

int main (int argc, char *argv[])
{
  /*
  // init(); // no need cause GLUT is black magic
  glutInit(&argc, argv);
  //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(MAX_SIZE, MAX_SIZE);
  glutCreateWindow("IMS");
  glutDisplayFunc(display);
  glutMainLoop();
  */

  std::vector<std::string> mapChars;
  
  mapChars = seed(mapChars, 100, 'Y', 40000.0f);
  mapChars = seed(mapChars, 100, 'O', 5000.0f); // TODO: ja nevjem... random neimplementovany zaciatocny stav
  //TODO: sem hodit po generovani nejake ohnisko v danom bode and watch the world burn
/*  mapChars = { "..YYY..Y.."
              ,"..YY..YYY."
              ,".YY.X.Y..."
              ,"....YY...Y"
              ,".Y....Y..."
              };
*/

  int64_t h = mapChars.size();
  int64_t w = mapChars[0].size();

  std::vector<std::vector<Cell>> map, newMap;
  
  for (int64_t i = 0; i < h; ++i){
    std::vector<Cell> vec {};
    for (int64_t j = 0; j < w; ++j) {
      vec.push_back(Cell(i, j));
      switch (mapChars[i][j])
      {
        case 'X':
          vec[j].status = Status::BURNING;
          vec[j].type = CellType::Tree;
          break;

        case 'Y':
          vec[j].status = Status::NOT_BURNING;
          vec[j].type = CellType::Tree;
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

  newMap = map;

  bool running {false};

  while (true)
  {
    // Print the map
    for (auto& line : map) {
      for (auto& c : line) {
        std::cout << c.printCell();
      }
      std::cout << NEWLINE; 
    }
    std::cout << std::endl;

    for (int64_t i = 0; i < h; ++i)
    {
      for (int64_t j = 0; j < w; ++j)
      {
        
        getNeighbor(map, newMap, i, j);

      }
    }

    map = newMap;
    std::this_thread::sleep_for(1s);
  }

  exit(EXIT_SUCCESS);
}