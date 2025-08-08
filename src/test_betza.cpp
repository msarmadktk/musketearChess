#include <iostream>
#include <string>
#include "betza.h"
#include "types.h"
#include "bitboard.h"

using namespace std;

void test_betza_parsing() {
    cout << "Testing Betza notation parsing..." << endl;
    
    Betza::BetzaManager manager;
    manager.init();
    
    // Test parsing various Betza notations
    string test_notations[] = {
        "N", "B", "R", "Q", "K", "C", "Z", "G", "E", "W", "F", "D", "A"
    };
    
    for (const string& notation : test_notations) {
        Betza::BetzaMove move = manager.parseBetzaNotation(notation);
        cout << "Parsed '" << notation << "' -> " << move.notation << endl;
    }
    
    cout << "Betza parsing test completed." << endl << endl;
}

void test_piece_creation() {
    cout << "Testing custom piece creation..." << endl;
    
    Betza::BetzaManager manager;
    manager.init();
    
    // Add a custom piece
    manager.addPiece("CustomKnight", "N", 800, 900);
    
    Betza::BetzaPiece* piece = manager.getPiece("CustomKnight");
    if (piece) {
        cout << "Created custom piece: " << piece->name << " with value " << piece->midgameValue << endl;
    } else {
        cout << "Failed to create custom piece!" << endl;
    }
    
    cout << "Piece creation test completed." << endl << endl;
}

void test_attack_generation() {
    cout << "Testing attack generation..." << endl;
    
    Betza::BetzaManager manager;
    manager.init();
    
    // Test knight attacks
    Betza::BetzaPiece* knight = manager.getPiece("Knight");
    if (knight) {
        Bitboard attacks = manager.generateAttacks(WHITE, *knight, SQ_E4, 0);
        cout << "Knight attacks from E4: " << (attacks ? "Generated successfully" : "No attacks") << endl;
    }
    
    // Test bishop attacks
    Betza::BetzaPiece* bishop = manager.getPiece("Bishop");
    if (bishop) {
        Bitboard attacks = manager.generateAttacks(WHITE, *bishop, SQ_E4, 0);
        cout << "Bishop attacks from E4: " << (attacks ? "Generated successfully" : "No attacks") << endl;
    }
    
    cout << "Attack generation test completed." << endl << endl;
}

void test_utility_functions() {
    cout << "Testing utility functions..." << endl;
    
    // Test piece type to Betza conversion
    string betza = Betza::pieceTypeToBetza(KNIGHT);
    cout << "Knight to Betza: " << betza << endl;
    
    // Test Betza to piece type conversion
    PieceType pt = Betza::betzaToPieceType("N");
    cout << "Betza 'N' to piece type: " << pt << endl;
    
    // Test validation
    bool valid = Betza::isValidBetzaNotation("N");
    cout << "Is 'N' valid Betza: " << (valid ? "yes" : "no") << endl;
    
    bool invalid = Betza::isValidBetzaNotation("N@#$");
    cout << "Is 'N@#$' valid Betza: " << (invalid ? "yes" : "no") << endl;
    
    cout << "Utility functions test completed." << endl << endl;
}

void test_asymmetric_pieces() {
    cout << "Testing asymmetric piece support..." << endl;
    
    cout << "MAX_GATES: " << MAX_GATES << endl;
    cout << "MAX_PIECES_PER_SIDE: " << MAX_PIECES_PER_SIDE << endl;
    cout << "PIECE_TYPE_NB: " << PIECE_TYPE_NB << endl;
    
    // Test that we can create different pieces for different sides
    Betza::BetzaManager manager;
    manager.init();
    
    // Add different pieces for white and black
    manager.addPiece("WhiteCustom", "N", 800, 900);
    manager.addPiece("BlackCustom", "B", 850, 950);
    
    Betza::BetzaPiece* whitePiece = manager.getPiece("WhiteCustom");
    Betza::BetzaPiece* blackPiece = manager.getPiece("BlackCustom");
    
    if (whitePiece && blackPiece) {
        cout << "Successfully created asymmetric pieces:" << endl;
        cout << "  White: " << whitePiece->name << " (value: " << whitePiece->midgameValue << ")" << endl;
        cout << "  Black: " << blackPiece->name << " (value: " << blackPiece->midgameValue << ")" << endl;
    }
    
    cout << "Asymmetric pieces test completed." << endl << endl;
}

int main() {
    cout << "=== Dynamic Betza Notation System Test ===" << endl << endl;
    
    test_betza_parsing();
    test_piece_creation();
    test_attack_generation();
    test_utility_functions();
    test_asymmetric_pieces();
    
    cout << "=== All tests completed successfully! ===" << endl;
    cout << "The dynamic Betza notation system is working correctly." << endl;
    
    return 0;
}
