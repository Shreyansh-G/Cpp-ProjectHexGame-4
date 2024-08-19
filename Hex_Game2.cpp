
//Homework 4, implement game of hex

#include <iostream>
#include <vector>
#include <set>

using namespace std;

enum Color{
    RED,
    BLUE,
    BLANK,
};

static const char * EnumStrings[] = {"red", "blue", "blank"};
const char * getTextForEnum(int enumVal){
    return EnumStrings[enumVal];
}

//a weighted union find data structure
class UF{
    private:
        vector<int> id;
        vector<int> sz; //size

        //non weighted version, used as help function
        void connectHelp(int p, int q){
            id[p] = q;
            sz[q] += sz[p];
        }
    public:
        UF(int usize):id(usize), sz(usize){
            //initialize
            for(int i; i < id.size(); i++){
                id[i] = i;
                sz[i] = 1; //initial size
            }
        }


        //return the component identifier, also the root
        int find(int p){
            while(id[p] != p){
                p = id[p];
            }
            return id[p];
        }
        //connect to sets
        void connect(int p, int q){
            int rootp = find(p);
            int rootq = find(q);
            if(sz[rootp] > sz[rootq]){
                connectHelp(rootq, rootp);
            }else{
                connectHelp(rootp, rootq);
            }
        }

        //whether is connected
        bool isConnected(int p, int q){
            if(find(p) == find(q)){
                return true;
            }else{
                return false;
            }
        }

};

//an abstract class which represents a game board
class Board
{
    private:
        //ids for sentinels
        int sentRed1,sentRed2, sentBlue1, sentBlue2;
    protected:

    int boardSize;
    vector<vector<Color> > board;
    UF disjointSet;

    class Node
    {
        int x;
        int y;
        Color c;
        public:
        Node(int xp, int yp, Color cp){
            x = xp;
            y = yp;
            c = cp;
        }
    };
    public:
    virtual void draw() = 0;

    //turn a (x, y) pair into a unique id, which is used by the union find algorithm
    int getId(int x, int y){
        return x * boardSize + y;
    }

    //turn a id into (x, y)
    pair<int, int> reverseId(int id){
        int x = id / boardSize;
        int y = id % boardSize;
        return pair<int, int>(x, y);
    }

    Board(int bSize):boardSize(bSize), board(boardSize, vector<Color>(boardSize)), disjointSet(boardSize*boardSize+3){
        //initialize the array representation of the hex graph

        sentBlue1 = getId(boardSize, 0);
        sentRed1 = getId(boardSize, 1);
        sentBlue2 = getId(boardSize, 2);
        sentRed2 = getId(boardSize, 3);
        for(vector<vector<Color> >::iterator i = board.begin(); i != board.end(); i++){
            for(vector<Color>::iterator j = (*i).begin(); j != (*i).end(); j++){
                *j = BLANK;
            }
        }
    }


    //return a list of (x, y) neighbours' id
    //(x0,y0) (x0, y1) (x0, y2)
    //  (x1,y0) (x, y) (x1, y2)
    //      (x2, y2) (x2, y1) (x2, y2)
    //we add 4 special ids as sentinels to represent the borders
    set<int> getNeighbours(int x, int y, Color c){
        set<int> neighbours;
        int nbx[2] = {0};
        int nby[2] = {0};
        for(int i = 0; i <= 2; i++){
            for(int j = 0; j <= 2; j++){
                nbx[i] = x + (i-1);
                nby[j] = y + (j-1);
            }
        }
        // only return the valid position
        for(int i = 0; i <= 2; i++){
            for(int j = 0; j <= 2; j++){
                //connect edges to the sentinels
                if(nby[i] < 0 && c == BLUE) neighbours.insert(sentBlue1);
                if(nbx[i] < 0 && c == RED) neighbours.insert(sentRed1);
                if(nby[i] >= boardSize && c == BLUE) neighbours.insert(sentBlue2);
                if(nbx[i] >= boardSize && c == RED) neighbours.insert(sentRed2);
                // only use six points around
                if( (i == 0 && j ==0) || (i == 2 && j== 2) || (i== 1 && j==1) ) continue;
                //only add valid points
                if(isValidPos(nbx[i], nby[j]) && board[nbx[i]][nby[j]] == c){
                    neighbours.insert(getId(nbx[i], nby[j]));
                }
            }
        }
        return neighbours;

    }


    //decide whether a position is valid or not
    bool isValidPos(int x, int y){
        if( x < 0 || x >= boardSize ) return false;
        if( y < 0 || y >= boardSize) return false;
        return true;
    }
    //decide whether a position is valid or not
    bool isValidMove(int x, int y, Color c){
        if(isValidPos(x, y)){
            if(board[x][y] == BLANK) return true; //valid only when the position is still empty
        }
        else{
            return false;
        }

    }

    bool move(int x, int y, Color c){
        if(!isValidMove(x, y, c)){
            cout << "Invalid move!!!" << endl;
            return false;
        }
        else{
            //change the color
            board[x][y] = c;
            //update the disjoint set
            set<int> nb = getNeighbours(x, y, c);
            for(set<int>::iterator i = nb.begin(); i != nb.end(); i++){
                //cout << "connecting" << getId(x, y) << " " << *i << endl;
                disjointSet.connect(getId(x, y), *i);
            }
        }
    }

    bool isConnected(int id1, int id2){
        if(disjointSet.isConnected(id1, id2)){
            return true;
        }else{
            return false;
        }
    }

    bool win(){
        if(isConnected(sentBlue1, sentBlue2)){
            cout << "Blue player wins!" << endl;
            return true;
        }else if(isConnected(sentRed1, sentRed2)){
            cout << "Red player wins!" << endl;
            return true;
        }else{
            return false;
        }
    }
};

//a board which prints itself using  ascii
class AsciiBoard:public Board
{
    public:
    AsciiBoard(int bSize):Board(bSize){}
    void draw(){
        int shiftNo = 0;
        for(int i = 0;  i < boardSize - 1 ; i++){
            shiftLine(shiftNo++);
            printLine(i);
            shiftLine(shiftNo++);
            printInline();
        }
        shiftLine(shiftNo);
        printLine(boardSize-1);
    }

    private:
    void printLine(int lineNo){
        for(int i = 0; i < boardSize-1; i++){
            cout << getSymbol(board[lineNo][i]) << " - ";
        }
        cout << getSymbol(board[lineNo][boardSize-1]) << endl;
    }
    void shiftLine(int numSpace){
        for(int i = 0; i < numSpace; i++){
            cout << " ";
        }
    }

    string getSymbol(Color c){
        switch(c){
            case BLUE:
                return "o";
                break;
            case RED:
                return "x";
                break;
            case BLANK:
                return ".";
                break;
            default:
                return ".";
        }

    }
    void printInline(){
        for(int i = 0; i < boardSize - 1; i++){
            cout << "\\ / " ;
        }
        cout << "\\" << endl;
    }
};


int main(){
    AsciiBoard ab(7);
    ab.draw();
    //driver
    Color currentPlayer = BLUE;
    int x, y;
    while(!ab.win()){
        cout << "Now is " << getTextForEnum(currentPlayer) <<" player's turn!" << endl;
        cout << "Input move (x,y)!" << endl;
        cin >> x >> y;
        while(!ab.isValidMove(x, y, currentPlayer)){//if the move is invalid
            ab.draw();
            cout << "Input a new move (x,y)!" << endl;
            cin >> x >> y;
        }
        ab.move(x, y, currentPlayer);
        ab.draw();
        //change player
        if(currentPlayer == BLUE){
            currentPlayer = RED;
        }else{
            currentPlayer = BLUE;
        }
    }
    return 0;
}