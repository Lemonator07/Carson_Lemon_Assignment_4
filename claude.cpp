// Sudoku solver: recursive depth-first search with backtracking (all solutions).
// Build: g++ -o claude claude.cpp
// Usage: ./claude puzzle1.txt puzzle2.txt puzzle3.txt puzzle4.txt puzzle5.txt
#include <array>    
#include <fstream>   
#include <iostream>  
#include <string>    
#include <vector>    

// A 9x9 board. 0 represents a blank ('_' in the files).
class Board { 
public:  
    using Cells = std::array<std::array<int, 9>, 9>;  // a 9x9 grid of ints

    Board() { cells_.fill({}); }  // fills every row with zeros so all cells start blank

    // Returns false if the file can't be opened or isn't a 9x9 puzzle.
    bool load(const std::string& path) {  // Reads a puzzle from the file at 'path'
        std::ifstream in(path);  // Opens the file for reading
        if (!in) return false;  // If the file failed to open, report failure
        std::string tok;  // Holds one whitespace-separated token read from the file
        int count = 0;  // Counts how many cells have been read so far
        while (in >> tok && count < 81) {  // Keep reading tokens until the file ends or 81 cells are read
            cells_[count / 9][count % 9] = (tok == "_") ? 0 : std::stoi(tok);  // Row = count/9, col = count%9; "_" becomes 0, otherwise convert text to int
            ++count;  // Move on to the next cell
        }  
        return count == 81;  // Success only if exactly 81 cells were read
    } 

    int  get(int r, int c) const { return cells_[r][c]; }  // Returns the value stored at row r, column c
    void set(int r, int c, int v) { cells_[r][c] = v; }  // Stores value v at row r, column c

    // Digits that may legally go in cell (r, c), given the rest of the board.
    std::vector<int> candidates(int r, int c) const { 
        bool used[10] = {false};  // used[d] is true if digit d already appears in a related cell (index 0 covers blanks)
        for (int i = 0; i < 9; ++i) {  // Loop over the 9 positions in the row and column
            used[cells_[r][i]] = true;  // row
            used[cells_[i][c]] = true;  // column
        }  // End of the row/column loop
        int br = 3 * (r / 3), bc = 3 * (c / 3);  // Compute the top-left corner of the 3x3 box containing (r, c)
        for (int i = br; i < br + 3; ++i)  // Loop over the three rows of the box
            for (int j = bc; j < bc + 3; ++j)  // Loop over the three columns of the box
                used[cells_[i][j]] = true;  // Mark the digit found in the same 3x3 box
        std::vector<int> out;  // Will hold the digits that are still allowed
        for (int d = 1; d <= 9; ++d)  // Try each digit from 1 to 9
            if (!used[d]) out.push_back(d);  // If the digit isn't used by any neighbor, it's a candidate
        return out;  
    }  

    // True if the given clues don't already conflict with each other.
    bool isConsistent() {  
        for (int r = 0; r < 9; ++r)  // row
            for (int c = 0; c < 9; ++c) {  // column
                int v = cells_[r][c];  // Remember the value in this cell
                if (v == 0) continue;  // Skip blank cells; only clues need checking
                cells_[r][c] = 0;  // Temporarily blank the cell so it doesn't conflict with itself
                bool ok = false;  // Tracks whether v is still a legal value here
                for (int d : candidates(r, c)) if (d == v) ok = true;  // If v appears among the candidates, it's legal
                cells_[r][c] = v;  // Restore the original clue
                if (!ok) return false; 
            } 
        return true;  
    }  

    // Finds the empty cell with the fewest candidates. Returns false if full.
    bool findEmpty(int& row, int& col, std::vector<int>& cands) const {  
        bool found = false;  // Becomes true once any blank cell has been seen
        for (int r = 0; r < 9; ++r)  // row
            for (int c = 0; c < 9; ++c) {  // column
                if (cells_[r][c] != 0) continue;  // Skip cells that are already filled
                std::vector<int> cs = candidates(r, c);  // Get the legal digits for this blank cell
                if (!found || cs.size() < cands.size()) {  // Take this cell if it's the first blank or has fewer candidates than the best so far
                    found = true;  // Record that a blank cell exists
                    row = r; col = c; cands = cs;  // Save this cell's position and its candidate list as the best so far
                    if (cands.size() <= 1) return true;  // 0 or 1 candidates can't be beaten, so stop searching early
                }  
            }  
        return found;  // True if a blank cell was found, false if the board is full
    }  

    void print(std::ostream& os) const {  // Writes the board to the given output stream
        for (int r = 0; r < 9; ++r) {  // row
            for (int c = 0; c < 9; ++c) {  // column
                if (c) os << ' ';  // Put a space between values (not before the first one)
                if (cells_[r][c]) os << cells_[r][c]; else os << '_';  // Print the digit, or '_' for a blank
            }  
            os << '\n'; 
        }  
    } 

private: 
    Cells cells_;  // The 9x9 grid of values (0 means blank)
};

// Depth-first search with backtracking; collects every solution.
class Solver {  // Declares the Solver class, which finds all solutions to a puzzle
public:  
    explicit Solver(const Board& puzzle) : board_(puzzle) {}  // makes a working copy of the puzzle

    const std::vector<Board>& solveAll() {  // Runs the search and returns all solutions found
        solutions_.clear();  // Discard any solutions from a previous run
        if (board_.isConsistent()) search();  // Only search if the starting clues don't conflict
        return solutions_; 
    } 

private:  
    void search() {  // Recursive backtracking routine
        int r = 0, c = 0;  // Will hold the row and column of the chosen blank cell
        std::vector<int> cands;  // Will hold the candidate digits for that cell
        if (!board_.findEmpty(r, c, cands)) {   // k == 81: solved  // No blank cells left means the board is completely filled
            solutions_.push_back(board_);  // Save a copy of the finished board as a solution
            return;  
        }  
        for (int d : cands) {                   // empty list -> dead end 
            board_.set(r, c, d);  // Place the candidate in the cell
            search();                           // recurse with k+1 cells filled 
            board_.set(r, c, 0);                // backtrack  
        } 
    }  

    Board board_;  // The working board that is modified during the search
    std::vector<Board> solutions_;  // Every complete solution found so far
}; 

// Runs the solver over a list of puzzle files and prints the results.
class Application {  
public:
    Application(int argc, char** argv) {  // builds the list of files to process from the command line
        for (int i = 1; i < argc; ++i) files_.push_back(argv[i]);  // Add each command-line argument (skipping the program name) as a file name
        if (files_.empty())  // If no file names were given on the command line
            for (int i = 1; i <= 5; ++i)  // Loop through the default puzzle numbers 1 to 5
                files_.push_back("puzzle" + std::to_string(i) + ".txt");  // Add "puzzle1.txt" through "puzzle5.txt"
    }  

    int run() {  
        for (const std::string& name : files_) {  // Loop over each file name without copying it
            std::cout << name << '\n';  // Print the file name as a heading
            Board puzzle;  // Create an empty board to load into
            if (!puzzle.load(name)) {  // Try to load the file; enter this block on failure
                std::cout << "Could not read puzzle file\n\n";  // Tell the user the file couldn't be read
                continue;  // Skip to the next file
            }  
            puzzle.print(std::cout);  // Show the original puzzle
            std::cout << '\n';  // Print a blank line after the puzzle

            Solver solver(puzzle);  // Create a solver working on a copy of the puzzle
            const std::vector<Board>& sols = solver.solveAll();  // Run the search and keep a reference to the solutions
            if (sols.empty()) std::cout << "No solution found\n";  // Report when no solutions exist
            for (size_t i = 0; i < sols.size(); ++i) {  // Loop over every solution found
                if (sols.size() > 1)  // Only label solutions when there is more than one
                    std::cout << "Solution " << i + 1 << " of " << sols.size() << ":\n";  // Print a label like "Solution 2 of 3:"
                sols[i].print(std::cout);  // Print this solution's board
                std::cout << '\n';  
            }  
            std::cout << "--------------------\n";  
        }  
        return 0; 
    }  

private: 
    std::vector<std::string> files_;  // The list of puzzle file names to process
};  

int main(int argc, char** argv) { 
    return Application(argc, argv).run();
} 