@echo off
echo ========================================
echo Testing Musketeer-Stockfish UCI Interface
echo ========================================
echo.

echo Building the engine...
make clean
make
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Starting UCI Test
echo ========================================
echo.

echo Starting Stockfish engine...
echo uci > uci_test.txt
echo setoption name CustomPieces value "Wizard:QN,Grasshopper:Q2,Amazon:QN" >> uci_test.txt
echo setoption name UCI_Variant value musketeer >> uci_test.txt
echo ucinewgame >> uci_test.txt
echo position startpos >> uci_test.txt
echo go depth 5 >> uci_test.txt
echo quit >> uci_test.txt

echo Running UCI test commands...
echo Commands sent to engine:
type uci_test.txt
echo.
echo Engine response:
stockfish.exe < uci_test.txt

echo.
echo ========================================
echo Test completed!
echo ========================================
echo.
echo The engine should have:
echo 1. Responded to 'uci' with available options
echo 2. Added custom pieces: Wizard (QN), Grasshopper (Q2), Amazon (QN)
echo 3. Set variant to musketeer
echo 4. Started a new game and searched to depth 5
echo.
echo Check the output above for success messages.
pause
