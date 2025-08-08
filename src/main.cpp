/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2018 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>

#include "bitboard.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"
#include "syzygy/tbprobe.h"

namespace PSQT {
  void init();
}

// Debug static initialization
struct StaticInitDebug {
  StaticInitDebug() {
    std::cout << "DEBUG: Static initialization starting" << std::endl;
  }
};
StaticInitDebug static_debug;

int main(int argc, char* argv[]) {

  std::cout << "DEBUG: Starting main()" << std::endl;

  std::cout << "DEBUG: About to call UCI::init(Options)" << std::endl;
  UCI::init(Options);
  std::cout << "DEBUG: UCI::init(Options) completed" << std::endl;
  
  std::cout << "DEBUG: About to call engine_info()" << std::endl;
  std::cout << engine_info() << std::endl;
  std::cout << "DEBUG: engine_info() completed" << std::endl;

  std::cout << "DEBUG: About to call PSQT::init()" << std::endl;
  PSQT::init();
  std::cout << "DEBUG: PSQT::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Bitboards::init()" << std::endl;
  Bitboards::init();
  std::cout << "DEBUG: Bitboards::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Position::init()" << std::endl;
  Position::init();
  std::cout << "DEBUG: Position::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Bitbases::init()" << std::endl;
  Bitbases::init();
  std::cout << "DEBUG: Bitbases::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Search::init()" << std::endl;
  Search::init();
  std::cout << "DEBUG: Search::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Pawns::init()" << std::endl;
  Pawns::init();
  std::cout << "DEBUG: Pawns::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Tablebases::init()" << std::endl;
  Tablebases::init(Options["SyzygyPath"]); // After Bitboards are set
  std::cout << "DEBUG: Tablebases::init() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Threads.set()" << std::endl;
  Threads.set(Options["Threads"]);
  std::cout << "DEBUG: Threads.set() completed" << std::endl;
  
  std::cout << "DEBUG: About to call Search::clear()" << std::endl;
  Search::clear(); // After threads are up
  std::cout << "DEBUG: Search::clear() completed" << std::endl;

  std::cout << "DEBUG: About to call UCI::loop()" << std::endl;
  UCI::loop(argc, argv);

  Threads.set(0);
  return 0;
}
