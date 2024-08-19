# Cpp-ProjectHexGame-4
## Major Task 1:
Your program should use a graph representation and treat the game as a path finding problem. Each internal node (hexagon) has six neighbors – so each would have 6 edges. The corners and the edges are special. A corner has either two or three neighbors and a non-corner edge has 4 neighbors.

The player should be able to interact with the program and choose its “color” with blue going first. The program should have a convenient interface for entering a move, displaying the board, and then making its own move. The program should determine when the game is over and announce the winner.

A simple strategy could be to extend your current longest path or to block your opponent’s longest path. A very dumb strategy would be to play on an empty hexagon at random.

A simple board display would be to have an 11 x 11 printout with B, R, or a blank in each position. A simple way to input a move would be to have the player enter an (i,j) coordinate corresponding to a currently empty hexagon and have the program check that this is legal and if not ask for another choice.

### Task 1 expectations:
1.Be able to draw the board using ASCII symbols and a given size, such as 7 by 7 or 11 by 11.
2.Input a move and determine if a move is legal.
3.Determine who won.
Some suggestions:
     -The board can be drawn in ASCII as follows: (7 x 7 board)
4.Here X indicates a move in the corner. The dots are empty cells. A  O indicates the second player.

## Major Task 2:
Your program should use your Task 1 work or an improvement on it.

The player should be able to interact with the program and choose its “color” with blue (or X) going first and red (or O) going second. The program should have a convenient interface for entering a move, displaying the board, and then making its own move. The program should determine when the game is over and announce the winner. 

This program will evaluate a position using a Monte Carlo selection of moves until the board is filled up. Then using work of Homework 4 you determine who won.  The program takes turns. It inputs the human (or machine opponent if playing against another program) move. When it is the “AI”’s turn, it is to evaluate all legal available next moves and select a “best” move.  Each legal move will be evaluated using  ~1000 or more trials. Each trial winds the game forward by randomly selecting successive moves until there is a winner. The trial is counted as a win or loss. The ratio: wins/trials are the AI’s metric for picking which next move to make.

A simple board display would be to have an 11 x 11 printout with B, R, or a blank (or X, O, b) in each position. A simple way to input a move would be to have the player enter an (i,j) coordinate corresponding to a currently empty hexagon and have the program check that this is legal, and if not, ask for another choice.

You may want to combine this approach with the min-max algorithm (or the more efficient alpha-beta), as described in the videos.

### Task 2 expectations:

1.The computer should be able to play Hex intelligently against a human on an 11 by 11 board.
2.Efficiently determine within no more than 2 minutes what the computer's move is.
Some suggestions:
 -Use a specialized algorithm to determine who won (as opposed to Dijkstra).
 -Try your algorithm on a 5 x 5 board first to make sure it works.
 -Be careful about computational cost – do not regenerate boards in making next move calculations.
 -To evaluate a specific trial, fill up the remaining empty squares on the board and evaluate the 
   filled up board for who won. This avoids applying the algorithm for who wins each time a move 
   is made. You should understand why this works.
 -If available, use the new C++11 <random> library to get experience with it.
 -Resources for computer hex: http://webdocs.cs.ualberta.ca/~hayward/hex/.

### More Advanced:

If you have the interest, time and energy, some videos talk about using Minimax evaluation of moves (with Alpha/Beta pruning) as well as Monte Carlo. The two approaches can be combined.
