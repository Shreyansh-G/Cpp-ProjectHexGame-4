#include <iostream>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <functional> 
#include <limits> // for std::numeric_limits

enum class Player : short { BLUE, RED, BLANK }; // Enum for players
enum class BluePath : short { LEFT_TO_RIGHT, TOP_TO_BOTTOM }; // Enum for blue path options

class Board {
private:
    int size; // Size of the board (size x size)
    std::vector<std::vector<Player>> grid; // 2D vector to represent the board grid with outer vector represents the rows and the inner vector represents the columns. 
    std::unordered_set<int> blueCoords; // store coordinates of blue player's moves with formula index = x*size+y
    std::unordered_set<int> redCoords; //store coordinates of red player's moves with formula index = x*size+y 

public:
    // Constructor initializes the board with all cells set to BLANK
    Board(int size) : size(size), grid(size, std::vector<Player>(size, Player::BLANK)) {}

    // Getter for board size
    int getSize() const {
        return size;
    }

    // Check if a move (x, y) is valid
    bool isValidMove(int x, int y) const {
        return x >= 0 && x < size && y >= 0 && y < size && grid[x][y] == Player::BLANK;
    }

    // Make a move at (x, y) for the given player after validating
    void makeMove(int x, int y, Player player) {
        if (isValidMove(x, y)) {
            grid[x][y] = player;
            // Add the move to the respective player's set of coordinates
            if (player == Player::BLUE) {
                blueCoords.emplace(x * size + y); // Convert (x, y) to single integer for set
            } else {
                redCoords.emplace(x * size + y); // Convert (x, y) to single integer for set
            }
        } else {
            std::cerr << "Invalid move. Try again." << std::endl;
        }
    }
 
    void display() const {
        // Print column coordinates
        std::cout << "  ";
        for (int col = 0; col < size; ++col) {
            std::cout << "  " << col << " ";
        }
        std::cout << std::endl;

        for (int row = 0; row < size; ++row) {
            // Print leading spaces for the staggered effect
            std::cout << std::string(row * 2, ' ');

            // Print row position
            std::cout << row << " ";

            // Print the grid contents
            for (int col = 0; col < size; ++col) {
                char ch;
                switch (grid[row][col]) {
                    case Player::BLUE: ch = 'B'; break;
                    case Player::RED: ch = 'R'; break;
                    default: ch = '.'; break;
                }
                std::cout << ch;

                // Add a dash between columns, except for the last column
                if (col < size - 1) {
                    std::cout << " - ";
                }
            }
            std::cout << std::endl;

            // Print the slashes between rows, except for the last row
            if (row < size - 1) {
                std::cout << std::string(row * 2 + 1, ' ') << " \\ /";
                for (int k = 1; k < size; ++k) {
                    std::cout << " \\ /";
                }
                std::cout << std::endl;
            }
        }
    }

    // Check if the given player has won using the specified blue path
bool hasWinner(Player player, BluePath bluePath) const {
    // Initialize visited array for each check
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    // Check winning condition for BLUE
    if (player == Player::BLUE) {
        if (bluePath == BluePath::LEFT_TO_RIGHT) {
            // Check for BLUE from left to right
            for (int i = 0; i < size; ++i) {
                visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                if (grid[i][0] == Player::BLUE && !visited[i][0]) {
                    if (dfs(i, 0, Player::BLUE, visited, bluePath)) return true;
                }
            }
        } else if (bluePath == BluePath::TOP_TO_BOTTOM) {
            // Check for BLUE from top to bottom
            for (int j = 0; j < size; ++j) {
                visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                if (grid[0][j] == Player::BLUE && !visited[0][j]) {
                    if (dfs(0, j, Player::BLUE, visited, bluePath)) return true;
                }
            }
        }
    }

    // Check winning condition for RED
    if (player == Player::RED) {
        if (bluePath == BluePath::LEFT_TO_RIGHT) {
            // Check for RED from top to bottom
            for (int j = 0; j < size; ++j) {
                visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                if (grid[0][j] == Player::RED && !visited[0][j]) {
                    if (dfs(0, j, Player::RED, visited, bluePath)) return true;
                }
            }
        } else if (bluePath == BluePath::TOP_TO_BOTTOM) {
            // Check for RED from left to right
            for (int i = 0; i < size; ++i) {
                visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                if (grid[i][0] == Player::RED && !visited[i][0]) {
                    if (dfs(i, 0, Player::RED, visited, bluePath)) return true;
                }
            }
        }
    }

    return false;
}



private:
    // Using Depth-first search algorithm to check for a path from (x, y) to an edge of the board
    bool dfs(int x, int y, Player player, std::vector<std::vector<bool>>& visited, BluePath bluePath) const {
        visited[x][y] = true;
        std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}
        };

        // Check if the current position is at the final position according to the blue path
        if (player == Player::BLUE) {
            if (bluePath == BluePath::LEFT_TO_RIGHT && y == size - 1) return true;
            if (bluePath == BluePath::TOP_TO_BOTTOM && x == size - 1) return true;
        } else if (player == Player::RED) {
            if (bluePath == BluePath::LEFT_TO_RIGHT && x == size - 1) return true;
            if (bluePath == BluePath::TOP_TO_BOTTOM && y == size - 1) return true;
        }

        // Explore neighbors recursively
        for (const auto& direction : directions) {
            int nx = x + direction.first;
            int ny = y + direction.second;
            if (nx >= 0 && nx < size && ny >= 0 && ny < size &&
                !visited[nx][ny] && grid[nx][ny] == player) {
                if (dfs(nx, ny, player, visited, bluePath)) return true;
            }
        }
        return false;
    }
};

class Game {
private:
    Board board; // The game board
    Player currentPlayer; // Current player (BLUE or RED)
    BluePath bluePath; // Chosen blue path (LEFT_TO_RIGHT or TOP_TO_BOTTOM)

public:
    // Constructor initializes the game with the specified board size
    Game(int size) : board(size), currentPlayer(Player::BLUE), bluePath(BluePath::LEFT_TO_RIGHT) {}

    // Main game loop
    void play() {
        askBluePath(); // Ask the blue player to choose their path
        int moveCount = 0;//Variable to count number of moves played 

        // Game loop
        while (true) {
            board.display(); // Display the current state of the board
            int x, y;
            bool validMove = false;

            // Keep asking for a valid move until one is made
            while (!validMove) {
                std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                          << ", enter your move (x y): ";
                std::cin >> x >> y;

                // Validate input and removed infinite loop due to invalid inputs
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "Invalid input. Please enter numeric coordinates." << std::endl;
                    continue;
                }

                // Check if the move is valid and make the move
                if (board.isValidMove(x, y)) {
                    board.makeMove(x, y, currentPlayer);
                    validMove = true;
                    moveCount++;

                    // Check if the current player has won
                    if (board.hasWinner(currentPlayer, bluePath)) {
                        board.display();
                        std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                                  << " wins!" << std::endl;
                        return;
                    }

                    // Switch to the next player
                    currentPlayer = (currentPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
                } else {
                    std::cerr << "Invalid move. The coordinate is preoccupied or out of bounds. Try again."
                              << std::endl;
                }
            }

            // Check for a draw (all cells occupied)
            if (moveCount == board.getSize() * board.getSize()) {
                std::cout << "Game over! It's a draw!" << std::endl;
                break;
            }
        }
    }

private:
    // Ask the blue player to choose their path (LEFT_TO_RIGHT or TOP_TO_BOTTOM)
    void askBluePath() {
        int choice;
        std::cout << "Player BLUE, choose your path:\n";
        std::cout << "1. Left to Right\n";
        std::cout << "2. Top to Bottom\n";
        std::cout << "Enter your choice (1 or 2): ";
        std::cin >> choice;

        // Validate and set the blue path choice
        if (choice == 1) {
            bluePath = BluePath::LEFT_TO_RIGHT;
        } else if (choice == 2) {
            bluePath = BluePath::TOP_TO_BOTTOM;
        } else {
            std::cerr << "Invalid choice. Defaulting to Left to Right." << std::endl;
            bluePath = BluePath::LEFT_TO_RIGHT;
        }
    }
};

// Main function to start the game
int main() {
    int size = 11; // Board size
    Game game(size); // Create a game instance
    game.play(); // Start the game loop

    return 0;
}

