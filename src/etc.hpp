#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <array>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <sstream>

// Argument loading
#include <getopt.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) // Windows
    #define NEWLINE "\r\n"
    #include <windows.h> 
#else
    #define NEWLINE '\n' // Avoid std::endl because of proper buffering and flushing
#endif

enum struct Wind
{
  None,
  N, NW, W, SW, S, SE, E, NE
};

// Global variabiable map for callback GLUT function
int64_t MAX_SIZE {1000},
        h {0},
        w {0},
        radius_global {2}, // Radius of spread
        k_global {1}, // Wind speed
        big_wind_global {3}, // Wind speed treshold
        tree_burning_time {162},
        brush_burning_time {100};
Wind wind { Wind::None };

// Mouse events
bool  leftMouseButtonDown { false },
      rightMouseButtonDown { false };
int64_t mouseXPos {0},
        mouseYPos {0};

// Logging
bool logging {false};
std::ofstream map_state_log;
int64_t log_period {25},
        tick {0},
        active {0},
        tree {0},
        brush {0},
        
        not_burning {0},
        burning {0},
        burned {0},

        not_burning_tree {0},
        burning_tree {0},
        burned_tree {0},

        not_burning_brush {0},
        burning_brush {0},
        burned_brush {0};

// Enumerate implementation
template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable)
{
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

// Function for help text
void showHelp()
{
  std::cout
    << "Usage:" << NEWLINE
    << "  ./ims [-h|--help] [-g|--gui]" << NEWLINE
    << NEWLINE
    << "Flags:" << NEWLINE
    << "  -h, --help\t" << "Show help" << NEWLINE
    << "  -g, --gui\t"  << "Verbose output switch" << std::endl
  ;
}

// Argument parsing
void parseArgs(int& argc, char* argv[], bool& gui, bool& show_help, bool& load_map, int64_t& x, int64_t& y, std::string& wind_dir)
{  
  const struct option long_options[] =
  {
    {"gui",       no_argument,       nullptr, 'g'},
    {"wind",      no_argument,       nullptr, 'w'},
    {"intensity", no_argument,       nullptr, 'i'},
    {"log",       no_argument,       nullptr, 'l'},
    {"help",      no_argument,       nullptr, 'h'},
    {nullptr,     no_argument,       nullptr, 0}
  };

  int arg {0};
  // Load arguments
  while (arg != -1)
  {
    arg = getopt_long(argc, argv, "xyghwil", long_options, nullptr);
    switch (arg)
    {
      case 'x':

        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
            { x = val; }
        }
        else
        {
          std::cerr << "Please specify x coordinate." << std::endl;
          std::exit(EXIT_FAILURE);
        }

        break;

      case 'y':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
            { y = val; }
        }
        else
        {
          std::cerr << "Please specify y coordinate." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

      case 'w':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          std::string val;

          if (iss >> val)
            { wind_dir = val; }
        }
        else
        {
          std::cerr << "Please specify wind direction." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

       case 'i':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
            { k_global = val; }
        }

      case 'g':
        gui = true;
        break;

      case 'l':
        logging = true;
        break;

      case 'h':
      case '?':
        show_help = true;
        break;

      default:
        break;
    }
  }
}

void parseWindDir(std::string wind_dir)
{

  if (wind_dir != "")
  {
    if      (wind_dir == "N"  or wind_dir == "n")  { wind = Wind::N;}
    else if (wind_dir == "NW" or wind_dir == "nw") { wind = Wind::NW;}
    else if (wind_dir == "W"  or wind_dir == "w")  { wind = Wind::W;}
    else if (wind_dir == "SW" or wind_dir == "sw") { wind = Wind::SW;}
    else if (wind_dir == "S"  or wind_dir == "s")  { wind = Wind::S;}
    else if (wind_dir == "SE" or wind_dir == "se") { wind = Wind::SE;}
    else if (wind_dir == "E"  or wind_dir == "e")  { wind = Wind::E;}
    else if (wind_dir == "NE" or wind_dir == "ne") { wind = Wind::NE;}

  }
}