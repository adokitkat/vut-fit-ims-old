#include "main.hpp"

//TODO: nahodnejsia zapalnost

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
    Map::igniteCellRadius(map, mouseXPos, mouseYPos, 3);
  }

  // Update data of every cell on map
  updateCellAll(map, newMap);
  // Update status of every cell
  Map::updateMapStatus(newMap);
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
  int64_t wildfire_start_x {-1},
          wildfire_start_y {-1};
  std::string wind_dir = "";
  
  // Parses arguments, fills variables with values
  parseArgs(argc, argv, gui, show_help, load_map, wildfire_start_x, wildfire_start_y, wind_dir);

  parseWindDir(wind_dir);

  if (show_help) { showHelp(); exit(EXIT_SUCCESS); }

  srand(time(NULL)); // initialize random function
  std::vector<std::string> mapChars;
  mapChars = seed(mapChars, MAX_SIZE, '@', 40000.0f);
  mapChars = seed(mapChars, MAX_SIZE, 'Y', 52000.0f); // seed (40000,55000)
  //TODO: sem hodit po generovani nejake ohnisko v danom bode and watch the world burn

  if (wildfire_start_x == -1) { wildfire_start_x = MAX_SIZE/2; }
  if (wildfire_start_y == -1) { wildfire_start_y = MAX_SIZE/2; }

  h = mapChars.size();
  w = mapChars[0].size();
  
  // Start wildfire
  for (auto x = (wildfire_start_x - 2); x <= (wildfire_start_x + 2); ++x) 
  {
    for (auto y = (wildfire_start_y - 2); y <= (wildfire_start_y + 2); ++y)
    {
      if (x >= 0 and x < h and y >= 0 and y < w) {
        mapChars[x][y] = 'X';
      }
      else 
      {
        std::cerr << "Starting coorinates out of bounds! Select numbers in range 0-999." << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }

  
  if (logging)
  {
    map_state_log = initLog("output.txt");
    std::cout << "Log message every " << log_period << " ticks." << std::endl;  
  }

  // Load map from chars to cells
  Map::loadMap(map, mapChars, h, w);
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
      Map::printMap(map);
      // Update data of every cell on map
      updateCellAll(map, newMap);
      // Update status of every cell
      Map::updateMapStatus(newMap);
      map = newMap;
      //std::this_thread::sleep_for(1s);
    }
  }
  exit(EXIT_SUCCESS);
}