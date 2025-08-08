#!/bin/bash

echo "========================================"
echo "Testing Musketeer-Stockfish UCI Interface"
echo "========================================"
echo

echo "Building the engine..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Error: Build failed!"
    exit 1
fi

echo
echo "========================================"
echo "Starting UCI Test"
echo "========================================"
echo

echo "Starting Stockfish engine..."
cat > uci_test.txt << EOF
uci
setoption name CustomPieces value "Wizard:QN,Grasshopper:Q2,Amazon:QN"
setoption name UCI_Variant value musketeer
ucinewgame
position startpos
go depth 5
quit
EOF

echo "Running UCI test commands..."
echo "Commands sent to engine:"
cat uci_test.txt
echo
echo "Engine response:"
./stockfish < uci_test.txt

echo
echo "========================================"
echo "Test completed!"
echo "========================================"
echo
echo "The engine should have:"
echo "1. Responded to 'uci' with available options"
echo "2. Added custom pieces: Wizard (QN), Grasshopper (Q2), Amazon (QN)"
echo "3. Set variant to musketeer"
echo "4. Started a new game and searched to depth 5"
echo
echo "Check the output above for success messages."
