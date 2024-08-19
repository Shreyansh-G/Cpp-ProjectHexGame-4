#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <chrono>
#include <cmath>

enum class Player {
    BLUE,
    RED
};

enum class BluePath {
    LEFT_TO_RIGHT,
    TOP_TO_BOTTOM
};

class Board {
private:
    int size;
    std::vector<std::vector<int>> grid;

public:
    Board(int size) : size(size), grid(size, std::vector<int>(size, 0)) {}

    bool isValidMove(int x, int y) const {
        return x >= 0 && x < size && y >= 0 && y < size && grid[x][y] == 0;
    }

    void makeMove(int x, int y, Player player) {
        grid[x][y] = (player == Player::BLUE) ? 1 : 2;
    }

    bool isTerminal() const {
        // Check if the board is fully occupied
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool hasWinner(Player player, BluePath bluePath) const {
        // Implement your winning condition based on the game rules
        return false;
    }

    std::vector<std::pair<int, int>> getLegalMoves() const {
        std::vector<std::pair<int, int>> legalMoves;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == 0) {
                    legalMoves.push_back({i, j});
                }
            }
        }
        return legalMoves;
    }

    void display() const {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (grid[i][j] == 0) {
                    std::cout << "- ";
                } else if (grid[i][j] == 1) {
                    std::cout << "B ";
                } else {
                    std::cout << "R ";
                }
            }
            std::cout << std::endl;
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
        Node* rootNode = new Node(board, bluePath, {-1, -1});
        const int simulations = 1000;

        for (int sim = 0; sim < simulations; ++sim) {
            Node* selectedNode = selectNode(rootNode);
            expandNode(selectedNode);
            bool result = simulateGame(selectedNode);
            backpropagate(selectedNode, result);
        }

        double bestUCB1Value = -std::numeric_limits<double>::infinity();
        Node* bestChild = nullptr;

        for (Node* child : rootNode->children) {
            double UCB1Value = child->UCB1Value();
            if (UCB1Value > bestUCB1Value) {
                bestUCB1Value = UCB1Value;
                bestChild = child;
            }
        }

        std::pair<int, int> bestMove = bestChild->action;
        delete rootNode;
        return bestMove;
    }

private:
    struct Node {
        Board board;
        BluePath bluePath;
        std::pair<int, int> action;
        int N;
        double Q;
        std::vector<Node*> children;
        Node* parent;

        Node(Board board, BluePath bluePath, std::pair<int, int> action)
            : board(board), bluePath(bluePath), action(action), N(0), Q(0.0), parent(nullptr) {}

        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }

        double UCB1Value() const {
            const double C = 1.0;
            return Q / static_cast<double>(N) + C * std::sqrt(std::log(parent->N) / static_cast<double>(N));
        }
    };

    Node* selectNode(Node* node) {
        while (!node->board.isTerminal()) {
            if (node->children.empty()) {
                return node;
            } else {
                node = UCB1Select(node);
            }
        }
        return node;
    }

    Node* UCB1Select(Node* node) {
        double bestUCB1Value = -std::numeric_limits<double>::infinity();
        Node* bestChild = nullptr;

        for (Node* child : node->children) {
            double UCB1Value = child->UCB1Value();
            if (UCB1Value > bestUCB1Value) {
                bestUCB1Value = UCB1Value;
                bestChild = child;
            }
        }

        return bestChild;
    }

    void expandNode(Node* node) {
        std::vector<std::pair<int, int>> legalMoves = node->board.getLegalMoves();
        for (const auto& move : legalMoves) {
            Board newBoard = node->board;
            newBoard.makeMove(move.first, move.second, player);
            Node* newNode = new Node(newBoard, node->bluePath, move);
            newNode->parent = node;
            node->children.push_back(newNode);
        }
    }

    bool simulateGame(Node* node) {
        Board board = node->board;
        Player currentPlayer = player;

        while (!board.isTerminal()) {
            std::vector<std::pair<int, int>> legalMoves = board.getLegalMoves();
            std::uniform_int_distribution<int> dist(0, legalMoves.size() - 1);
            std::pair<int, int> randomMove = legalMoves[dist(rng)];
            board.makeMove(randomMove.first, randomMove.second, currentPlayer);
            currentPlayer = (currentPlayer == player) ? opponent : player;
        }

        return board.hasWinner(player, node->bluePath);
    }

    void backpropagate(Node* node, bool result) {
        while (node != nullptr) {
            node->N++;
            if (result) {
                node->Q++;
            }
            node = node->parent;
        }
    }
};

class AIGame : public Board {
private:
    AIPlayer aiPlayer;
    Player currentPlayer;
    BluePath bluePath;

public:
    AIGame(int size, Player userPlayer) : Board(size), aiPlayer(userPlayer == Player::BLUE ? Player::RED : Player::BLUE) {
        currentPlayer = Player::BLUE;
        if (userPlayer == Player::RED) {
            askBluePath();
        }
    }

    void play() {
        int moveCount = 0;
        Player userPlayer = aiPlayer.getPlayer() == Player::BLUE ? Player::RED : Player::BLUE;

        while (true) {
            display();
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

                    if (isValidMove(x, y)) {
                        makeMove(x, y, currentPlayer);
                        validMove = true;
                        moveCount++;

                        if (hasWinner(currentPlayer, bluePath)) {
                            display();
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
                std::pair<int, int> bestMove = aiPlayer.getBestMove(*this, bluePath);
                makeMove(bestMove.first, bestMove.second, currentPlayer);
                moveCount++;
                std::cout << "AI Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                          << " makes move (" << bestMove.first << ", " << bestMove.second << ")" << std::endl;

                if (hasWinner(currentPlayer, bluePath)) {
                    display();
                    std::cout << "Player " << (currentPlayer == Player::BLUE ? "BLUE" : "RED")
                              << " wins!" << std::endl;
                    return;
                }

                currentPlayer = (currentPlayer == Player::BLUE) ? Player::RED : Player::BLUE;
            }

            if (moveCount == size * size) {
                display();
                std::cout << "It's a draw!" << std::endl;
                return;
            }
        }
    }

private:
    void askBluePath() {
        int choice;
        std::cout << "Choose Blue's winning path:" << std::endl;
        std::cout << "1. Left to Right" << std::endl;
        std::cout << "2. Top to Bottom" << std::endl;
        std::cin >> choice;

        switch (choice) {
            case 1:
                bluePath = BluePath::LEFT_TO_RIGHT;
                break;
            case 2:
                bluePath = BluePath::TOP_TO_BOTTOM;
                break;
            default:
                std::cerr << "Invalid choice. Defaulting to Left to Right." << std::endl;
                bluePath = BluePath::LEFT_TO_RIGHT;
                break;
        }
    }
};

int main() {
    int boardSize = 3; // Example board size
    Player userPlayer = Player::RED; // Example: User plays as RED

    AIGame game(boardSize, userPlayer);
    game.play();

    return 0;
}
