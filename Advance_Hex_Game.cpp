#include <iostream>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <functional>
#include <limits>
#include <random>
#include <ctime>
#include <chrono>

enum class Player : short { BLUE, RED, BLANK };
enum class BluePath : short { LEFT_TO_RIGHT, TOP_TO_BOTTOM };

class Board {
private:
    int size; // Size of the board (size x size)
    std::vector<std::vector<Player>> grid; // 2D vector to represent the board grid with outer vector represents the rows and the inner vector represents the columns. 
    std::unordered_set<int> blueCoords; // store coordinates of blue player's moves with formula index = x*size+y
    std::unordered_set<int> redCoords; //store coordinates of red player's moves with formula index = x*size+y 

public:
    Board(int size) : size(size), grid(size, std::vector<Player>(size, Player::BLANK)) {}

    int getSize() const {
        return size;
    }

    // Check if a move (x, y) is valid
    bool isValidMove(int x, int y) const {
        return x >= 0 && x < size && y >= 0 && y < size && grid[x][y] == Player::BLANK;
    }

    void makeMove(int x, int y, Player player) {
        if (isValidMove(x, y)) {
            grid[x][y] = player;
            if (player == Player::BLUE) {
                blueCoords.emplace(x * size + y);
            } else {
                redCoords.emplace(x * size + y);
            }
        } else {
            std::cerr << "Invalid move. Try again." << std::endl;
        }
    }

    Player getPlayerAt(int x, int y) const {
        return grid[x][y];
    }
    //Displaying Staggered Effect Visual 
    void display() const {
        std::cout << "  ";
        for (int col = 0; col < size; ++col) {
            std::cout << "  " << col << " ";
        }
        std::cout << std::endl;

        for (int row = 0; row < size; ++row) {
            std::cout << std::string(row * 2, ' ');
            std::cout << row << " ";
            for (int col = 0; col < size; ++col) {
                char ch;
                switch (grid[row][col]) {
                    case Player::BLUE: ch = 'B'; break;
                    case Player::RED: ch = 'R'; break;
                    default: ch = '.'; break;
                }
                std::cout << ch;
                if (col < size - 1) {
                    std::cout << " - ";
                }
            }
            std::cout << std::endl;

            if (row < size - 1) {
                std::cout << std::string(row * 2 + 1, ' ') << " \\ /";
                for (int k = 1; k < size; ++k) {
                    std::cout << " \\ /";
                }
                std::cout << std::endl;
            }
        }
    }
  //Heart of Code for Checking Winner for Blue and Red Simultaneoously
    bool hasWinner(Player player, BluePath bluePath) const {
        std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
        if (player == Player::BLUE) {
            if (bluePath == BluePath::LEFT_TO_RIGHT) {
                for (int i = 0; i < size; ++i) {
                    visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                    if (grid[i][0] == Player::BLUE && !visited[i][0]) {
                        if (dfs(i, 0, Player::BLUE, visited, bluePath)) return true;
                    }
                }
            } else if (bluePath == BluePath::TOP_TO_BOTTOM) {
                for (int j = 0; j < size; ++j) {
                    visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                    if (grid[0][j] == Player::BLUE && !visited[0][j]) {
                        if (dfs(0, j, Player::BLUE, visited, bluePath)) return true;
                    }
                }
            }
        }

        if (player == Player::RED) {
            if (bluePath == BluePath::LEFT_TO_RIGHT) {
                for (int j = 0; j < size; ++j) {
                    visited = std::vector<std::vector<bool>>(size, std::vector<bool>(size, false));
                    if (grid[0][j] == Player::RED && !visited[0][j]) {
                        if (dfs(0, j, Player::RED, visited, bluePath)) return true;
                    }
                }
            } else if (bluePath == BluePath::TOP_TO_BOTTOM) {
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
        //Relative Neighbour Position
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
protected:
    Board board;
    Player currentPlayer;
    BluePath bluePath;

public:
    Game(int size) : board(size), currentPlayer(Player::BLUE), bluePath(BluePath::LEFT_TO_RIGHT) {}

    virtual void play() = 0;//Function for Calling Method Of Play

protected:
    void askBluePath() {
        int choice;
        std::cout << "Player BLUE, choose your path:\n";
        std::cout << "1. Left to Right\n";
        std::cout << "2. Top to Bottom\n";
        std::cout << "Enter your choice (1 or 2): ";
        std::cin >> choice;

        if (choice == 1) {
            bluePath = BluePath::LEFT_TO_RIGHT;
        } else if (choice == 2) {
            bluePath = BluePath::TOP_TO_BOTTOM;
        } else {
            std::cerr << "Invalid choice. Defaulting to Left to Right." << std::endl;
            bluePath = BluePath::LEFT_TO_RIGHT;
        }
    }

    bool checkDraw(int moveCount) const {
        return moveCount == board.getSize() * board.getSize();
    }
};

class ManualGame : public Game {
public:
    ManualGame(int size) : Game(size) {}
    //Game loop
    void play() override {
        askBluePath();
        int moveCount = 0;

        while (true) {
            board.display();
            int x, y;
            bool validMove = false;

            while (!validMove) {
                std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                          << ", enter your move (x y): ";
                std::cin >> x >> y;

                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "Invalid input. Please enter numeric coordinates." << std::endl;
                    continue;
                }

                if (board.isValidMove(x, y)) {
                    board.makeMove(x, y, currentPlayer);
                    validMove = true;
                    moveCount++;

                    if (board.hasWinner(currentPlayer, bluePath)) {
                        board.display();
                        std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                                  << " wins!" << std::endl;
                        return;
                    }

                    currentPlayer = (currentPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
                } else {
                    std::cerr << "Invalid move. The coordinate is preoccupied or out of bounds. Try again."
                              << std::endl;
                }
            }

            if (checkDraw(moveCount)) {
                std::cout << "Game over! It's a draw!" << std::endl;
                break;
            }
        }
    }
};

class AIPlayer {
private:
    Player player;
    Player opponent;
    std::mt19937 rng;

public:
    AIPlayer(Player player) : player(player), opponent(player == Player::BLUE ? Player::RED : Player::BLUE) {
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

    Player getPlayer() const {
        return player;
    }

    std::pair<int, int> getBestMove(Board board, BluePath bluePath) {
        int size = board.getSize();
        std::vector<std::pair<int, int>> validMoves;

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board.isValidMove(i, j)) {
                    validMoves.emplace_back(i, j);
                }
            }
        }

        int bestMoveIndex = 0;
        double bestWinRate = -1.0;
        const int simulations = 1000;//Number of Simulation For Slow Performance Change it to 100

        for (size_t i = 0; i < validMoves.size(); ++i) {
            int wins = 0;
            for (int sim = 0; sim < simulations; ++sim) {
                Board simBoard = board;
                simBoard.makeMove(validMoves[i].first, validMoves[i].second, player);
                if (simulateRandomGame(simBoard, bluePath)) {
                    wins++;
                }
            }
            double winRate = static_cast<double>(wins) / simulations;
            if (winRate > bestWinRate) {
                bestWinRate = winRate;
                bestMoveIndex = i;
            }
        }

        return validMoves[bestMoveIndex];
    }

private:
    bool simulateRandomGame(Board board, BluePath bluePath) {
        std::vector<std::pair<int, int>> moves;
        int size = board.getSize();

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board.isValidMove(i, j)) {
                    moves.emplace_back(i, j);
                }
            }
        }

        Player currentSimPlayer = opponent;
        std::shuffle(moves.begin(), moves.end(), rng);

        for (const auto& move : moves) {
            board.makeMove(move.first, move.second, currentSimPlayer);
            if (board.hasWinner(currentSimPlayer, bluePath)) {
                return currentSimPlayer == player;
            }
            currentSimPlayer = (currentSimPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
        }

        return false;
    }
};
//AI Agent which uses MCS algorithm for Efficient Computation of Trials
class AIGame : public Game {
private:
    AIPlayer aiPlayer;

public:
    AIGame(int size, Player userPlayer) : Game(size), aiPlayer(userPlayer == Player::BLUE ? Player::RED : Player::BLUE) {
        currentPlayer = Player::BLUE;
        if (userPlayer == Player::RED) {
            askBluePath();
        } else {
            askUserPath();
        }
    }
    //Game loop
    void play() override {
        int moveCount = 0;
        Player userPlayer = aiPlayer.getPlayer() == Player::BLUE ? Player::RED : Player::BLUE;

        while (true) {
            board.display();
            if (currentPlayer == userPlayer) {
                int x, y;
                bool validMove = false;

                while (!validMove) {
                    std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                              << ", enter your move (x y): ";
                    std::cin >> x >> y;

                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cerr << "Invalid input. Please enter numeric coordinates." << std::endl;
                        continue;
                    }

                    if (board.isValidMove(x, y)) {
                        board.makeMove(x, y, currentPlayer);
                        validMove = true;
                        moveCount++;

                        if (board.hasWinner(currentPlayer, bluePath)) {
                            board.display();
                            std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                                      << " wins!" << std::endl;
                            return;
                        }

                        currentPlayer = (currentPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
                    } else {
                        std::cerr << "Invalid move. The coordinate is preoccupied or out of bounds. Try again."
                                  << std::endl;
                    }
                }
            } else {
                std::pair<int, int> bestMove = aiPlayer.getBestMove(board, bluePath);
                board.makeMove(bestMove.first, bestMove.second, currentPlayer);
                moveCount++;
                std::cout << "AI Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                          << " makes move (" << bestMove.first << ", " << bestMove.second << ")" << std::endl;

                if (board.hasWinner(currentPlayer, bluePath)) {
                    board.display();
                    std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                              << " wins!" << std::endl;
                    return;
                }

                currentPlayer = (currentPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
            }

            if (checkDraw(moveCount)) {
                std::cout << "Game over! It's a draw!" << std::endl;
                break;
            }
        }
    }

private:
    void askUserPath() {
        int choice;
        std::cout << "Player RED, choose your path:\n";
        std::cout << "1. Left to Right\n";
        std::cout << "2. Top to Bottom\n";
        std::cout << "Enter your choice (1 or 2): ";
        std::cin >> choice;

        if (choice == 1) {
            bluePath = BluePath::TOP_TO_BOTTOM;
        } else if (choice == 2) {
            bluePath = BluePath::LEFT_TO_RIGHT;
        } else {
            std::cerr << "Invalid choice. Defaulting to Top to Bottom." << std::endl;
            bluePath = BluePath::TOP_TO_BOTTOM;
        }
    }
};

int main() {
    int size = 11;//User can change size of board  
    char gameType;
    std::cout << "Choose game type:\n";
    std::cout << "1. Manual Game\n";
    std::cout << "2. AI Game\n";
    std::cout << "Enter your choice (1 or 2): ";
    std::cin >> gameType;

    if (gameType == '1') {
        ManualGame game(size);
        game.play();
    } else if (gameType == '2') {
        char userColor;
        std::cout << "Choose your color (B for Blue, R for Red): ";
        std::cin >> userColor;
        Player userPlayer = (userColor == 'B' || userColor == 'b') ? Player::BLUE : Player::RED;
        AIGame game(size, userPlayer);
        game.play();
    } else {
        std::cerr << "Invalid choice." << std::endl;
    }

    return 0;
}