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

#include <algorithm>
#include <cassert>
#include <ostream>
#include <iostream>
#include <sstream>

#include "misc.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"
#include "xboard.h"
#include "syzygy/tbprobe.h"
#include "betza.h"

using std::string;

UCI::OptionsMap Options; // Global object

namespace UCI {

/// 'On change' actions, triggered by an option's value change
void on_clear_hash(const Option&) { Search::clear(); }
void on_hash_size(const Option& o) { TT.resize(o); }
void on_logger(const Option& o) { start_logger(o); }
void on_threads(const Option& o) { Threads.set(o); }
void on_tb_path(const Option& o) { Tablebases::init(o); }
void on_variant(const Option& o) {
    if (Options["Protocol"] == "xboard")
    {
        // Send setup command
        sync_cout << "setup (PNBRQ.E....C.AF.MH.SU........D............LKpnbrq.e....c.af.mh.su........d............lk) "
                  << "8x10+0_seirawan"
                  << " " << XBoard::StartFEN
                  << sync_endl;
        
        // Initialize Betza manager
        Betza::betzaManager.init();
        
        // Send piece commands with dynamic Betza notation
        // https://www.gnu.org/software/xboard/Betza.html
        const auto& pieces = Betza::betzaManager.getAllPieces();
        for (const auto& pair : pieces) {
            const auto& piece = pair.second;
            if (!piece.moves.empty()) {
                sync_cout << "piece " << piece.symbol << "& " << piece.moves[0].notation << sync_endl;
            }
        }
    }
    else
        sync_cout << "info string variant " << (std::string)o
                  << " files " << 8
                  << " ranks " << 10
                  << " pocket " << 0
                  << " template " << "seirawan"
                  << " startpos " << XBoard::StartFEN
                  << sync_endl;
}

void on_custom_pieces(const Option& o) {
    // Clear existing custom pieces
    Betza::betzaManager.clearCustomPieces();
    
    std::string pieceString = (std::string)o;
    if (pieceString.empty() || pieceString == "<empty>") {
        sync_cout << "info string Custom pieces cleared" << sync_endl;
        return;
    }
    
    // Parse custom piece definitions
    // Format: "Name1:Betza1,Name2:Betza2,..."
    std::istringstream iss(pieceString);
    std::string pieceDef;
    int pieceCount = 0;
    
    while (std::getline(iss, pieceDef, ',')) {
        size_t colonPos = pieceDef.find(':');
        if (colonPos != std::string::npos) {
            std::string name = pieceDef.substr(0, colonPos);
            std::string betza = pieceDef.substr(colonPos + 1);
            
            // Trim whitespace
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            betza.erase(0, betza.find_first_not_of(" \t"));
            betza.erase(betza.find_last_not_of(" \t") + 1);
            
            if (!name.empty() && !betza.empty()) {
                Betza::betzaManager.addPiece(name, betza);
                pieceCount++;
                sync_cout << "info string Added custom piece: " << name << " (" << betza << ")" << sync_endl;
            }
        }
    }
    
    if (pieceCount > 0) {
        sync_cout << "info string Loaded " << pieceCount << " custom pieces" << sync_endl;
    }
}


/// Our case insensitive less() function as required by UCI protocol
bool CaseInsensitiveLess::operator() (const string& s1, const string& s2) const {

  return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(),
         [](char c1, char c2) { return tolower(c1) < tolower(c2); });
}


/// init() initializes the UCI options to their hard-coded default values

void init(OptionsMap& o) {

  // at most 2^32 clusters.
  constexpr int MaxHashMB = Is64Bit ? 131072 : 2048;

  o["Protocol"]              << Option("uci", {"uci", "xboard"});
  o["Debug Log File"]        << Option("", on_logger);
  o["Contempt"]              << Option(21, -100, 100);
  o["Analysis Contempt"]     << Option("Both", {"Both", "Off", "White", "Black"});
  o["Threads"]               << Option(1, 1, 512, on_threads);
  o["Hash"]                  << Option(16, 1, MaxHashMB, on_hash_size);
  o["Clear Hash"]            << Option(on_clear_hash);
  o["Ponder"]                << Option(false);
  o["MultiPV"]               << Option(1, 1, 500);
  o["Skill Level"]           << Option(20, 0, 20);
  o["Move Overhead"]         << Option(30, 0, 5000);
  o["Minimum Thinking Time"] << Option(20, 0, 5000);
  o["Slow Mover"]            << Option(84, 10, 1000);
  o["nodestime"]             << Option(0, 0, 10000);
  o["UCI_Variant"]           << Option("musketeer", {"musketeer"}, on_variant);
  o["UCI_Chess960"]          << Option(false);
  o["UCI_AnalyseMode"]       << Option(false);
  o["CustomPieces"]          << Option("<empty>", on_custom_pieces);
  o["SyzygyPath"]            << Option("<empty>", on_tb_path);
  o["SyzygyProbeDepth"]      << Option(1, 1, 100);
  o["Syzygy50MoveRule"]      << Option(true);
  o["SyzygyProbeLimit"]      << Option(6, 0, 6);
}


/// operator<<() is used to print all the options default values in chronological
/// insertion order (the idx field) and in the format defined by the UCI protocol.

std::ostream& operator<<(std::ostream& os, const OptionsMap& om) {

  if (Options["Protocol"] == "xboard")
  {
      for (size_t idx = 0; idx < om.size(); ++idx)
          for (const auto& it : om)
              if (it.second.idx == idx && it.first != "Protocol")
              {
                  const Option& o = it.second;
                  os << "\nfeature option=\"" << it.first << " -" << o.type;

                  if (o.type == "string" || o.type == "combo")
                      os << " " << o.defaultValue;
                  else if (o.type == "check")
                      os << " " << int(o.defaultValue == "true");

                  if (o.type == "combo")
                      for (string value : o.comboValues)
                          if (value != o.defaultValue)
                              os << " /// " << value;

                  if (o.type == "spin")
                      os << " " << int(stof(o.defaultValue))
                         << " " << o.min
                         << " " << o.max;

                  os << "\"";

                  break;
              }
  }
  else

  for (size_t idx = 0; idx < om.size(); ++idx)
      for (const auto& it : om)
          if (it.second.idx == idx && it.first != "Protocol")
          {
              const Option& o = it.second;
              os << "\noption name " << it.first << " type " << o.type;

              if (o.type == "string" || o.type == "check" || o.type == "combo")
                  os << " default " << o.defaultValue;

              if (o.type == "combo")
                  for (string value : o.comboValues)
                      os << " var " << value;

              if (o.type == "spin")
                  os << " default " << int(stof(o.defaultValue))
                     << " min "     << o.min
                     << " max "     << o.max;

              break;
          }

  return os;
}


/// Option class constructors and conversion operators

Option::Option(const char* v, OnChange f) : type("string"), min(0), max(0), on_change(f)
{ defaultValue = currentValue = v; }

Option::Option(const char* v, const std::vector<std::string>& combo, OnChange f) : type("combo"), min(0), max(0), comboValues(combo), on_change(f)
{ defaultValue = currentValue = v; }

Option::Option(bool v, OnChange f) : type("check"), min(0), max(0), on_change(f)
{ defaultValue = currentValue = (v ? "true" : "false"); }

Option::Option(OnChange f) : type("button"), min(0), max(0), on_change(f)
{}

Option::Option(double v, int minv, int maxv, OnChange f) : type("spin"), min(minv), max(maxv), on_change(f)
{ defaultValue = currentValue = std::to_string(v); }

Option::operator double() const {
  assert(type == "check" || type == "spin");
  return (type == "spin" ? stof(currentValue) : currentValue == "true");
}

Option::operator std::string() const {
  assert(type == "string" || type == "combo");
  return currentValue;
}

bool Option::operator==(const char* s) {
  assert(type == "combo");
  return    !CaseInsensitiveLess()(currentValue, s)
         && !CaseInsensitiveLess()(s, currentValue);
}


/// operator<<() inits options and assigns idx in the correct printing order

void Option::operator<<(const Option& o) {

  static size_t insert_order = 0;

  *this = o;
  idx = insert_order++;
}


/// operator=() updates currentValue and triggers on_change() action. It's up to
/// the GUI to check for option's limits, but we could receive the new value from
/// the user by console window, so let's check the bounds anyway.

Option& Option::operator=(const string& v) {

  assert(!type.empty());

  if (   (type != "button" && v.empty())
      || (type == "check" && v != "true" && v != "false")
      || (type == "combo" && (std::find(comboValues.begin(), comboValues.end(), v) == comboValues.end()))
      || (type == "spin" && (stof(v) < min || stof(v) > max)))
      return *this;

  if (type != "button")
      currentValue = v;

  if (on_change)
      on_change(*this);

  return *this;
}

const std::string Option::get_type() const {
    return type;
}

} // namespace UCI
