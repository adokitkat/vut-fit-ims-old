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

// Argument loading
#include <getopt.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) // Windows
    #define NEWLINE "\r\n"
    #include <windows.h> 
#else
    #define NEWLINE '\n' // Avoid std::endl because of proper buffering and flushing
#endif

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
void parseArgs(int& argc, char* argv[], bool& gui, bool& show_help, bool& load_map)
{  
  const struct option long_options[] =
  {
    {"gui",     no_argument, nullptr, 'g'},
    {"help",    no_argument, nullptr, 'h'},
    {"help",    no_argument, nullptr, 'h'},
    {nullptr,   no_argument, nullptr, 0}
  };

  int arg {0};
  // Load arguments
  while (arg != -1)
  {
    arg = getopt_long(argc, argv, "gh", long_options, nullptr);
    switch (arg)
    {
      /*
      case 't':
        if (!optarg and
            argv[optind] != nullptr and
            argv[optind][0] != '-')
        { 
          token = std::string(argv[optind++]); 
        }
        else // token.empty()
        {
          std::cerr << "Token cannot be empty." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;
      */
      case 'g':
        gui = true;
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