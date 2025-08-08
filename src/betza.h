#ifndef BETZA_H_INCLUDED
#define BETZA_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "types.h"

// Forward declare Betza namespace types (only what's needed)
namespace Betza {
    struct BetzaPiece;
    struct BetzaMove;
    class BetzaManager;
    extern BetzaManager betzaManager;
}

namespace Betza {

// Betza notation movement components
struct BetzaMove {
    bool isLeaper = false;
    bool isSlider = false;
    bool isRider = false;
    bool isHook = false;
    bool isLion = false;
    bool isLocust = false;
    bool isMao = false;
    bool isMoa = false;
    bool isCamel = false;
    bool isZebra = false;
    bool isGiraffe = false;
    bool isElephant = false;
    bool isWazir = false;
    bool isFerz = false;
    bool isDabbaba = false;
    bool isAlfil = false;
    bool isKnight = false;
    bool isBishop = false;
    bool isRook = false;
    bool isQueen = false;
    bool isKing = false;
    bool isPawn = false;
    
    // Direction modifiers
    bool isForward = false;
    bool isBackward = false;
    bool isSideways = false;
    bool isDiagonal = false;
    bool isOrthogonal = false;
    bool isOmni = false;
    
    // Distance modifiers
    int minDistance = 1;
    int maxDistance = 1;
    bool isUnlimited = false;
    
    // Special modifiers
    bool isInitial = false;
    bool isCapture = false;
    bool isNonCapture = false;
    bool isPromotion = false;
    bool isCastling = false;
    bool isEnPassant = false;
    
    // Custom directions
    std::vector<Direction> customDirections;
    
    // Betza notation string
    std::string notation;
};

// Piece definition with multiple movement patterns
struct BetzaPiece {
    std::string name;
    std::string symbol;
    std::vector<BetzaMove> moves;
    Value midgameValue;
    Value endgameValue;
    bool isCustom = false;
};

// Dynamic piece manager
class BetzaManager {
private:
    std::unordered_map<std::string, BetzaPiece> pieces;
    std::unordered_map<PieceType, BetzaPiece> pieceTypes;
    std::vector<Direction> allDirections;
    
public:
    BetzaManager();
    ~BetzaManager() = default;
    
    // Initialize with default pieces
    void init();
    
    // Parse Betza notation
    BetzaMove parseBetzaNotation(const std::string& notation);
    
                   // Add custom piece
               void addPiece(const std::string& name, const std::string& betzaNotation,
                             Value mgValue = PawnValueMg, Value egValue = PawnValueEg);
    
    // Get piece by name
    BetzaPiece* getPiece(const std::string& name);
    
    // Get piece by type
    BetzaPiece* getPiece(PieceType pt);
    
    // Generate attack patterns for a piece
    Bitboard generateAttacks(Color c, const BetzaPiece& piece, Square s, Bitboard occupied);
    
    // Generate attack patterns for a specific move
    Bitboard generateMoveAttacks(const BetzaMove& move, Square s, Bitboard occupied);
    
    // Get all registered pieces
    const std::unordered_map<std::string, BetzaPiece>& getAllPieces() const { return pieces; }
    
    // Clear all custom pieces
    void clearCustomPieces();
    
    // Load pieces from string (for FEN-like notation)
    void loadPiecesFromString(const std::string& pieceString);
    
    // Get piece string representation
    std::string getPieceString() const;
    
private:
    // Helper functions for parsing
    Direction parseDirection(char c);
    std::vector<Direction> parseDirections(const std::string& dirs);
    int parseNumber(const std::string& num);
    bool parseModifier(char c, BetzaMove& move);
    
    // Helper functions for attack generation
    Bitboard generateLeaperAttacks(const BetzaMove& move, Square s);
    Bitboard generateSliderAttacks(const BetzaMove& move, Square s, Bitboard occupied);
    Bitboard generateRiderAttacks(const BetzaMove& move, Square s, Bitboard occupied);
};

// Global instance
extern BetzaManager betzaManager;

// Utility functions
std::string pieceTypeToBetza(PieceType pt);
PieceType betzaToPieceType(const std::string& betza);
bool isValidBetzaNotation(const std::string& notation);

} // namespace Betza

#endif // BETZA_H_INCLUDED
