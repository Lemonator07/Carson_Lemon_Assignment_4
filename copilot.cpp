#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class SudokuPuzzle
{
private:
    char board[9][9];
    int solutionCount;

public:
    SudokuPuzzle()
    {
        solutionCount = 0;
    }

    bool loadPuzzle(string fileName)
    {
        ifstream fin(fileName);

        if (!fin)
            return false;

        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                fin >> board[row][col];
            }
        }

        fin.close();
        return true;
    }

    void printPuzzle()
    {
        for (int row = 0; row < 9; row++)
        {
            for (int col = 0; col < 9; col++)
            {
                cout << board[row][col] << " ";
            }

            cout << endl;
        }
    }

    bool validMove(int row, int col, char value)
    {
        // Check row
        for (int c = 0; c < 9; c++)
        {
            if (board[row][c] == value)
                return false;
        }

        // Check column
        for (int r = 0; r < 9; r++)
        {
            if (board[r][col] == value)
                return false;
        }

        // Check 3x3 block
        int startRow = (row / 3) * 3;
        int startCol = (col / 3) * 3;

        for (int r = startRow; r < startRow + 3; r++)
        {
            for (int c = startCol; c < startCol + 3; c++)
            {
                if (board[r][c] == value)
                    return false;
            }
        }

        return true;
    }

    bool findBlank(int &row, int &col)
    {
        for (row = 0; row < 9; row++)
        {
            for (col = 0; col < 9; col++)
            {
                if (board[row][col] == '_')
                    return true;
            }
        }

        return false;
    }

    void solve()
    {
        int row;
        int col;

        if (!findBlank(row, col))
        {
            solutionCount++;

            cout << "\nSolution " << solutionCount << endl;
            printPuzzle();
            cout << endl;

            return;
        }

        for (char digit = '1'; digit <= '9'; digit++)
        {
            if (validMove(row, col, digit))
            {
                board[row][col] = digit;

                solve();     // Recursive call

                board[row][col] = '_';  // Backtracking
            }
        }
    }

    void startSolving()
    {
        solutionCount = 0;

        solve();

        if (solutionCount == 0)
        {
            cout << "\nNo solution found.\n";
        }
    }
};

class PuzzleDriver
{
public:
    void run()
    {
        string files[5] =
        {
            "puzzle1.txt",
            "puzzle2.txt",
            "puzzle3.txt",
            "puzzle4.txt",
            "puzzle5.txt"
        };

        for (int i = 0; i < 5; i++)
        {
            SudokuPuzzle sudoku;

            cout << "\n=================================\n";
            cout << "File: " << files[i] << endl;
            cout << "=================================\n";

            if (!sudoku.loadPuzzle(files[i]))
            {
                cout << "Unable to open file.\n";
                continue;
            }

            cout << "\nOriginal Puzzle:\n";
            sudoku.printPuzzle();

            cout << "\nSolutions:\n";
            sudoku.startSolving();
        }
    }
};

int main()
{
    PuzzleDriver program;
    program.run();

    return 0;
}