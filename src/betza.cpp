#include "betza.h"
#include "bitboard.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

namespace Betza {

// Global instance
BetzaManager betzaManager;

BetzaManager::BetzaManager() {
    // Initialize all possible directions
    allDirections = {
        NORTH, SOUTH, EAST, WEST,
        NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST
    };
}

void BetzaManager::init() {
    // Clear existing pieces
    pieces.clear();
    pieceTypes.clear();
    
    // Add standard chess pieces
    addPiece("Pawn", "mWfceFifmnD", Value(171), Value(240));
    addPiece("Knight", "N", Value(764), Value(848));
    addPiece("Bishop", "B", Value(826), Value(891));
    addPiece("Rook", "R", Value(1282), Value(1373));
    addPiece("Queen", "Q", Value(2500), Value(2670));
    addPiece("King", "KisO2", Value(0), Value(0));
    
    // Add Musketeer pieces with their Betza notation
    addPiece("Cannon", "llNrrNDK", Value(1710), Value(2239));
    addPiece("Leopard", "NB2", Value(1648), Value(2014));
    addPiece("Archbishop", "BN", Value(2036), Value(2202));
    addPiece("Chancellor", "RN", Value(2251), Value(2344));
    addPiece("Spider", "B2DN", Value(2321), Value(2718));
    addPiece("Dragon", "QN", Value(3280), Value(2769));
    addPiece("Unicorn", "CN", Value(1584), Value(1772));
    addPiece("Hawk", "DHAG", Value(1537), Value(1561));
    addPiece("Elephant", "KDA", Value(1770), Value(2000));
    addPiece("Fortress", "B3DfNbN", Value(1956), Value(2100));
    
    // Map to piece types
    pieceTypes[PAWN] = pieces["Pawn"];
    pieceTypes[KNIGHT] = pieces["Knight"];
    pieceTypes[BISHOP] = pieces["Bishop"];
    pieceTypes[ROOK] = pieces["Rook"];
    pieceTypes[QUEEN] = pieces["Queen"];
    pieceTypes[KING] = pieces["King"];
    pieceTypes[CANNON] = pieces["Cannon"];
    pieceTypes[LEOPARD] = pieces["Leopard"];
    pieceTypes[ARCHBISHOP] = pieces["Archbishop"];
    pieceTypes[CHANCELLOR] = pieces["Chancellor"];
    pieceTypes[SPIDER] = pieces["Spider"];
    pieceTypes[DRAGON] = pieces["Dragon"];
    pieceTypes[UNICORN] = pieces["Unicorn"];
    pieceTypes[HAWK] = pieces["Hawk"];
    pieceTypes[ELEPHANT] = pieces["Elephant"];
    pieceTypes[FORTRESS] = pieces["Fortress"];
}

BetzaMove BetzaManager::parseBetzaNotation(const std::string& notation) {
    BetzaMove move;
    move.notation = notation;
    
    if (notation.empty()) return move;
    
    size_t pos = 0;
    while (pos < notation.length()) {
        char c = notation[pos];
        
        // Parse basic piece types
        switch (c) {
            case 'N': move.isKnight = true; break;
            case 'B': move.isBishop = true; break;
            case 'R': move.isRook = true; break;
            case 'Q': move.isQueen = true; break;
            case 'K': move.isKing = true; break;
            case 'P': move.isPawn = true; break;
            case 'C': move.isCamel = true; break;
            case 'Z': move.isZebra = true; break;
            case 'G': move.isGiraffe = true; break;
            case 'E': move.isElephant = true; break;
            case 'W': move.isWazir = true; break;
            case 'F': move.isFerz = true; break;
            case 'D': move.isDabbaba = true; break;
            case 'A': move.isAlfil = true; break;
            case 'L': move.isLion = true; break;
            case 'H': move.isHook = true; break;
            case 'M': 
                if (pos + 1 < notation.length() && notation[pos + 1] == 'a') {
                    move.isMao = true;
                    pos++;
                } else if (pos + 1 < notation.length() && notation[pos + 1] == 'o') {
                    move.isMoa = true;
                    pos++;
                }
                break;
            default:
                // Parse modifiers
                if (!parseModifier(c, move)) {
                    // Try to parse as number (distance)
                    if (isdigit(c)) {
                        std::string num;
                        while (pos < notation.length() && isdigit(notation[pos])) {
                            num += notation[pos++];
                        }
                        pos--; // Back up one position
                        move.maxDistance = parseNumber(num);
                        if (move.minDistance == 1) move.minDistance = move.maxDistance;
                    }
                }
                break;
        }
        pos++;
    }
    
    // Set default movement type
    if (!move.isLeaper && !move.isSlider && !move.isRider) {
        if (move.maxDistance == 1) move.isLeaper = true;
        else move.isSlider = true;
    }
    
    return move;
}

void BetzaManager::addPiece(const std::string& name, const std::string& betzaNotation, 
                           Value mgValue, Value egValue) {
    BetzaPiece piece;
    piece.name = name;
    piece.symbol = name.substr(0, 1);
    piece.midgameValue = mgValue;
    piece.endgameValue = egValue;
    piece.isCustom = true;
    
    // Parse the Betza notation into moves
    piece.moves.push_back(parseBetzaNotation(betzaNotation));
    
    pieces[name] = piece;
}

BetzaPiece* BetzaManager::getPiece(const std::string& name) {
    auto it = pieces.find(name);
    return it != pieces.end() ? &it->second : nullptr;
}

BetzaPiece* BetzaManager::getPiece(PieceType pt) {
    auto it = pieceTypes.find(pt);
    return it != pieceTypes.end() ? &it->second : nullptr;
}

Bitboard BetzaManager::generateAttacks(Color c, const BetzaPiece& piece, Square s, Bitboard occupied) {
    Bitboard attacks = 0;
    
    for (const auto& move : piece.moves) {
        attacks |= generateMoveAttacks(move, s, occupied);
    }
    
    return attacks;
}

Bitboard BetzaManager::generateMoveAttacks(const BetzaMove& move, Square s, Bitboard occupied) {
    if (move.isLeaper) {
        return generateLeaperAttacks(move, s);
    } else if (move.isSlider) {
        return generateSliderAttacks(move, s, occupied);
    } else if (move.isRider) {
        return generateRiderAttacks(move, s, occupied);
    }
    
    return 0;
}

Bitboard BetzaManager::generateLeaperAttacks(const BetzaMove& move, Square s) {
    Bitboard attacks = 0;
    std::vector<Direction> directions;
    
    // Determine directions based on piece type
    if (move.isKnight) {
        directions = {Direction(-17), Direction(-15), Direction(-10), Direction(-6), 
                     Direction(6), Direction(10), Direction(15), Direction(17)};
    } else if (move.isBishop) {
        directions = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    } else if (move.isRook) {
        directions = {NORTH, EAST, SOUTH, WEST};
    } else if (move.isQueen) {
        directions = {NORTH, EAST, SOUTH, WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    } else if (move.isKing) {
        directions = {Direction(-9), Direction(-8), Direction(-7), Direction(-1), 
                     Direction(1), Direction(7), Direction(8), Direction(9)};
    } else if (move.isCamel) {
        directions = {Direction(-17), Direction(-15), Direction(-10), Direction(-6), 
                     Direction(6), Direction(10), Direction(15), Direction(17)};
    } else if (move.isZebra) {
        directions = {Direction(-25), Direction(-23), Direction(-17), Direction(-15), 
                     Direction(-11), Direction(-10), Direction(-6), Direction(-5),
                     Direction(25), Direction(23), Direction(17), Direction(15), 
                     Direction(11), Direction(10), Direction(6), Direction(5)};
    } else if (move.isGiraffe) {
        directions = {Direction(-27), Direction(-24), Direction(-21), Direction(-18), 
                     Direction(-16), Direction(-14), Direction(-3), Direction(-2),
                     Direction(27), Direction(24), Direction(21), Direction(18), 
                     Direction(16), Direction(14), Direction(3), Direction(2)};
    } else if (move.isElephant) {
        directions = {Direction(-18), Direction(-16), Direction(-14), Direction(-9), 
                     Direction(-8), Direction(-7), Direction(-2), Direction(-1),
                     Direction(18), Direction(16), Direction(14), Direction(9), 
                     Direction(8), Direction(7), Direction(2), Direction(1)};
    } else if (move.isWazir) {
        directions = {NORTH, EAST, SOUTH, WEST};
    } else if (move.isFerz) {
        directions = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    } else if (move.isDabbaba) {
        directions = {Direction(-16), Direction(-4), Direction(4), Direction(16)};
    } else if (move.isAlfil) {
        directions = {Direction(-18), Direction(-14), Direction(14), Direction(18)};
    } else if (!move.customDirections.empty()) {
        directions = move.customDirections;
    }
    
    // Generate attacks for each direction
    for (Direction dir : directions) {
        Square to = s + dir;
        if (is_ok(to) && distance(s, to) <= move.maxDistance) {
            attacks |= to;
        }
    }
    
    return attacks;
}

Bitboard BetzaManager::generateSliderAttacks(const BetzaMove& move, Square s, Bitboard occupied) {
    Bitboard attacks = 0;
    std::vector<Direction> directions;
    
    // Determine directions based on piece type
    if (move.isBishop) {
        directions = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    } else if (move.isRook) {
        directions = {NORTH, EAST, SOUTH, WEST};
    } else if (move.isQueen) {
        directions = {NORTH, EAST, SOUTH, WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    }
    
    // Generate sliding attacks
    for (Direction dir : directions) {
        Square to = s;
        for (int dist = 1; dist <= move.maxDistance; dist++) {
            to += dir;
            if (!is_ok(to)) break;
            
            attacks |= to;
            if (occupied & to) break; // Blocked by piece
        }
    }
    
    return attacks;
}

Bitboard BetzaManager::generateRiderAttacks(const BetzaMove& move, Square s, Bitboard occupied) {
    // Similar to slider but with unlimited distance
    return generateSliderAttacks(move, s, occupied);
}

Direction BetzaManager::parseDirection(char c) {
    switch (c) {
        case 'N': return NORTH;
        case 'S': return SOUTH;
        case 'E': return EAST;
        case 'W': return WEST;
        case 'n': return NORTH_EAST;
        case 's': return SOUTH_WEST;
        case 'e': return SOUTH_EAST;
        case 'w': return NORTH_WEST;
        default: return NORTH;
    }
}

std::vector<Direction> BetzaManager::parseDirections(const std::string& dirs) {
    std::vector<Direction> directions;
    for (char c : dirs) {
        directions.push_back(parseDirection(c));
    }
    return directions;
}

int BetzaManager::parseNumber(const std::string& num) {
    return std::stoi(num);
}

bool BetzaManager::parseModifier(char c, BetzaMove& move) {
    switch (c) {
        case 'f': move.isForward = true; return true;
        case 'b': move.isBackward = true; return true;
        case 's': move.isSideways = true; return true;
        case 'd': move.isDiagonal = true; return true;
        case 'o': move.isOrthogonal = true; return true;
        case 'm': move.isOmni = true; return true;
        case 'i': move.isInitial = true; return true;
        case 'c': move.isCapture = true; return true;
        case 'n': move.isNonCapture = true; return true;
        case 'p': move.isPromotion = true; return true;
        case 'k': move.isCastling = true; return true;
        case 'e': move.isEnPassant = true; return true;
        case 'l': move.isLeaper = true; return true;
        case 'r': move.isRider = true; return true;
        case 'h': move.isHook = true; return true;
        default: return false;
    }
}

void BetzaManager::clearCustomPieces() {
    pieces.clear();
    pieceTypes.clear();
    init(); // Reinitialize with default pieces
}

void BetzaManager::loadPiecesFromString(const std::string& pieceString) {
    (void)pieceString; // Suppress unused parameter warning
    // Parse piece string like "PNBRQCLAMSDUHEFK"
    // This would be used to load custom piece combinations
    // Implementation depends on the specific format needed
}

std::string BetzaManager::getPieceString() const {
    std::string result;
    for (const auto& pair : pieces) {
        if (!pair.second.isCustom) {
            result += pair.second.symbol;
        }
    }
    return result;
}

// Utility functions
std::string pieceTypeToBetza(PieceType pt) {
    switch (pt) {
        case PAWN: return "mWfceFifmnD";
        case KNIGHT: return "N";
        case BISHOP: return "B";
        case ROOK: return "R";
        case QUEEN: return "Q";
        case KING: return "KisO2";
        case CANNON: return "llNrrNDK";
        case LEOPARD: return "NB2";
        case ARCHBISHOP: return "BN";
        case CHANCELLOR: return "RN";
        case SPIDER: return "B2DN";
        case DRAGON: return "QN";
        case UNICORN: return "CN";
        case HAWK: return "DHAG";
        case ELEPHANT: return "KDA";
        case FORTRESS: return "B3DfNbN";
        default: return "";
    }
}

PieceType betzaToPieceType(const std::string& betza) {
    if (betza == "mWfceFifmnD") return PAWN;
    if (betza == "N") return KNIGHT;
    if (betza == "B") return BISHOP;
    if (betza == "R") return ROOK;
    if (betza == "Q") return QUEEN;
    if (betza == "KisO2") return KING;
    if (betza == "llNrrNDK") return CANNON;
    if (betza == "NB2") return LEOPARD;
    if (betza == "BN") return ARCHBISHOP;
    if (betza == "RN") return CHANCELLOR;
    if (betza == "B2DN") return SPIDER;
    if (betza == "QN") return DRAGON;
    if (betza == "CN") return UNICORN;
    if (betza == "DHAG") return HAWK;
    if (betza == "KDA") return ELEPHANT;
    if (betza == "B3DfNbN") return FORTRESS;
    return NO_PIECE_TYPE;
}

bool isValidBetzaNotation(const std::string& notation) {
    // Basic validation - check for valid characters
    for (char c : notation) {
        if (!isalpha(c) && !isdigit(c) && c != '&' && c != '|' && c != '(' && c != ')') {
            return false;
        }
    }
    return true;
}

} // namespace Betza
