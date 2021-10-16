#include<stdio.h>
#include<string.h>
#include <windows.h>
#include <math.h>

// Score Maps for scoring
int  pawnMap[64] = {
 0,  0,  0,  0,  0,  0,  0,  0,
50, 50, 50, 50, 50, 50, 50, 50,
10, 10, 20, 30, 30, 20, 10, 10,
 5,  5, 10, 25, 25, 10,  5,  5,
 0,  0,  0, 20, 20,  0,  0,  0,
 5, -5,-10,  0,  0,-10, -5,  5,
 5, 10, 10,-20,-20, 10, 10,  5,
 0,  0,  0,  0,  0,  0,  0,  0 };
int knightMap[64]={
    -50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50
};
int bishopMap[64] = {

-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20
};
int rookMap[64] = {
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0 };
int queenMap[64] = {
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20 };
int kingMap[64] = {

-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20
};
//converts x and y cords to indexes and gets the positional score from a array of 64 from one of them above
int getPoints(int map[64] ,int x, int y, int reversed) {
    if (reversed != 1) {
        y = 7 - x;

    }
    return (map[  y*8 +  x ]);
}
// score given per piece
double _king_Value = 20000;
double _queen_Value = 900;
double _rook_Value = 500;
double _bishop_Value = 330;
double _knight_Value = 320;
double _pawn_Value = 100;


// Objects
typedef struct piece { 
    int posx ;
    int posy ;
    char type;
    char team;
}piece;
typedef struct move { 
    int posx;
    int posy;
    int targetx;
    int targety;
    char type;
    char team;
}move;
typedef struct Possiblemove { 
    int posx;
    int posy;
    int targetx;
    int targety;

}Possiblemove;

//List Objects / ArrayList

typedef struct  movesList { // List of Moves Meant to keep track of the History of what moves were done
    move* ptr; 
    int size;//Desired Size
    int realSize; // Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
}movesList;
typedef struct  Array { // 12 bytes total not including where the pointers point to
    Possiblemove* ptr; // 8 bytes
    int size;//4 bytes
    int realSize;// Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
    // the variable above is not really needed
}Array;

typedef struct  ArrayP { // Board Object / Array of Pieces
    piece* ptr; // 8 bytes
    int size;//4 bytes
    int realSize;// Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
}ArrayP;


//Initialize Variables here

ArrayP board; // Array of Pieces on the Board
movesList movesDone;// List of moves Done




// ARRAYS Methods
void removeArr(Array* arr, int pos) {
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++) {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}
void changeSize(Array* target, int size) {

    if (target->size > target->realSize)
        target->ptr = (Possiblemove*)realloc((*target).ptr, size * sizeof(Possiblemove));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
void initArr(Array* arr, int size) {
    arr->ptr =(Possiblemove*) malloc(0 );
    arr->size = 0;
    arr->realSize = size;
}
void addArr(Array* arr, Possiblemove obj) {
    arr->size += 1;

    if (arr->realSize < arr->size)
        changeSize(arr, arr->size);

    *(arr->ptr + (arr->size - 1)) = obj;
}
Possiblemove* getArr(Array* arr, int i) {
    return (arr->ptr + i);
}
void freeArr(Array* arr) {
    free(arr->ptr);
}









/// <summary>
/// Resizes a array
/// </summary>
/// <param name="target"></param>
/// <param name="size"></param>
void changeSize1(ArrayP* target, int size) {
    if(target->size > target->realSize)
        target->ptr = (piece*)realloc((*target).ptr, size * sizeof(piece));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
/// <summary>
/// Removes object in the specified position in the Array
/// </summary>
/// <param name="arr"></param>
/// <param name="pos"></param>
void removeArr1(ArrayP* arr, int pos) {
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++) {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}
/// <summary>
/// Initializes a Array given the Address of an Array
/// </summary>
/// <param name="arr"></param>
/// <param name="size"></param>
void initArr1(ArrayP* arr, int size) {
    arr->ptr = malloc(size * sizeof(piece));
    arr->size = size;
    arr->realSize = size;
}
/// <summary>
/// Adds obj to the end of the Array
/// </summary>
/// <param name="ArrayP* array;"></param>
/// <param name="piece obj;"></param>
void addArr1(ArrayP* arr, piece obj) {// 
    arr->size += 1;

    if( arr->realSize< arr->size)
        changeSize1(arr, arr->size);

    *(arr->ptr + (arr->size -1)) = obj;
}
/// <summary>
/// Get Info from position i relitive to the address and memory location
/// </summary>
/// <param name="arr : Address of ArrayP Object"></param>
/// <param name="i   : Position of wanted piece"></param>
/// <returns></returns>
piece* getArr1(ArrayP* arr, int i) {
    return (arr->ptr + i);
}
/// <summary>
/// Destroys the array 
/// </summary>
/// <param name="arr"></param>
void freeArr1(ArrayP* arr) {
    free(arr->ptr);
}


void removeArrList(movesList* arr, int pos) {
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++) {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}

void removeFromTop(movesList* arr) {
    arr->size = arr->size - 1;
    
}
void changeSizeList(movesList* target, int size) {
    if (target->size > target->realSize)
        target->ptr = (move*)realloc((*target).ptr, size * sizeof(move));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
void initArrList(movesList* arr, int size) {
    arr->ptr = malloc(size * sizeof(move));
    arr->size = size;
    arr->realSize = size;
}
void addArrList(movesList* arr, move obj) {
    arr->size += 1;
    arr->realSize = arr->size > arr->realSize ? arr->size : arr->realSize;
    changeSizeList(arr, arr->size);
    *(arr->ptr + (arr->size-1 )) = obj;
}
move* getArrList(movesList* arr, int i) {
    return (arr->ptr + i);
}
void freeArrList(movesList* arr) {
    free(arr->ptr);
}


//SOMEHOW OPTIMIZE THIS   50% of all CPU goes to searching for pieces on board
int getPieceAtPos(int x, int y) {
    for (int i = 0; i < board.size; i++) {
        if (((board.ptr + i))->posx == x && ((board.ptr + i))->posy == y) {
            return i;
        }
    }return -1;
}
// Moves a piece in the array 'board' in the specified location of the array to the x and y cordinates
void movePiece(int location, int x, int y) {
    int i=0;
    // checks for piece in x and y cords
    for (i = 0; i < board.size; i++) {

        if ((board.ptr + i)->posx == x && (board.ptr + i)->posy == y) break;
    }move temp;
    // if it found something in the loop above (a piece in the x and y cords)
    if (i < board.size) {
        temp = (move){ (board.ptr + location)->posx ,(board.ptr + location)->posy ,x,y,(board.ptr+i)->type , (board.ptr + i)->team };

        (board.ptr + location)->posx = x;
        (board.ptr + location)->posy = y;
        removeArr1(&board, i);
    }
    else {
        
        temp = (move){ (board.ptr + location)->posx ,(board.ptr + location)->posy ,x,y,'-', '-'};
       (board.ptr + location)->posx = x;
       (board.ptr + location)->posy = y;
    }
    //Adds move to movelist
    addArrList(&movesDone, temp);
}

//looks in the Array movesDone and reverses a move
void takeBack() {
    if (movesDone.size > 0) {
        move* lookat = (((movesDone.ptr + movesDone.size - 1)));
        
        int canMove = getPieceAtPos(lookat->targetx, lookat->targety);
        //move the piece back
        movePiece(canMove, lookat->posx, lookat->posy);
        //if there was a piece where there were add it back
        if (lookat->type != '-' && lookat->team != '-') {
            addArr1(&board, (piece) { lookat->targetx, lookat->targety, lookat->type, lookat->team });


        }movesDone.size-=2;
    }
}
double getPieceVal(piece* obj) { // get Value of a piece based on it's position on the board and it's worth
    int team = 1;
    if (obj->team == 'w')team *= -1;

    switch (obj->type) {
    case 'k':
        return _king_Value + getPoints(kingMap, obj->posx, obj->posy , team);
    case 'q':
        return _queen_Value + getPoints(queenMap, obj->posx, obj->posy, team);
    case 'r':
        return _rook_Value + getPoints(rookMap, obj->posx, obj->posy, team);
    case 'b':
        return _bishop_Value + getPoints(bishopMap, obj->posx, obj->posy, team);
    case 'h':
        return _knight_Value + getPoints(knightMap, obj->posx, obj->posy, team);
    case 'p':
        return _pawn_Value + getPoints(pawnMap, obj->posx, obj->posy, team);

    }return 0;
}

int checkKing() {// gets the number of kings on board 
    int kings = 0;
    for (int i = 0; i < board.size; i++) {
        if (getArr1(&board, i)->type == 'k') {
            kings++;
            if (kings >= 2) {
                break;
            }
        }
    }
    return kings;
}
int addNCheck(Array* output, Possiblemove obj, int color) {
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8) {
        // get the Target or -1 if there is none
        int num = getPieceAtPos(obj.targetx, obj.targety);
        // Checks if there is nothing there
        if (num < 0) {
            addArr(output, obj);
            return 1;
        }
        char team = getArr1(&board, num)->team;

        if (num >= 0 && ((team == 'w' && color != 1) || (team != 'w' && color == 1))) {

            addArr(output, obj);
            return 0;
        }
    }return 0;

}//Adds a move to the output array given that the move is possible (returns 1 if it is 0 if not)
int NCheck(Possiblemove obj, int color) {
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8) {
        // get the Target or -1 if there is none
        int num = getPieceAtPos(obj.targetx, obj.targety);
        // Checks if there is nothing there
        if (num < 0) {
            return 1;
        }
        
        char team = getArr1(&board, num)->team;
        // if there is something there and it is the enemy return 2
        if (num >= 0 && ((team == 'w' && color != 1) || (team != 'w' && color == 1))) {
            return 2;
        }
    }return 0;
}
int addNCheckCannotTake(Array* output, Possiblemove obj, int color) {//Adds a move to the output array given that the move is possible (returns 1 if it is 0 if not)   
    // Meant for pawns as pawns can move but not take 


    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8) {
        int num = getPieceAtPos(obj.targetx, obj.targety);
        char team = getArr1(&board, num)->team;//

        if (num < 0) {
            addArr(output, obj);
            return 1;

        }
    }return 0;

}

void getPossibleMoves(Array* output, int color) {// Adds all possible moves to output array
    int score = 0;
    for (int i = 0; i < board.size; i++) {
        piece* selPiece = getArr1(&board, i);
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1)) {
            switch (selPiece->type) {
            case 'k':
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1 }, color);
                break;
            case 'q':

                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset }, color) == 1)
                    offset++;

                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                break;
            case 'r':

                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset }, color);
                    offset++;
                }
                break;
            case 'b':

                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                break;
            case 'h':

                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2 }, color);
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2 }, color);
                break;
            case 'p':
                if (getPieceAtPos(selPiece->posx + 1, selPiece->posy + 1 * color) >=0)
                    addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 * color }, color);
                if (getPieceAtPos(selPiece->posx - 1, selPiece->posy + 1 * color) >= 0)
                addNCheck(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 * color }, color);
                if (
                    addNCheckCannotTake(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 * color }, color) == 1 && ((selPiece->posy==1 && color ==1 )|| (selPiece->posy == 6 && color == -1) ) )
                    addNCheckCannotTake(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 2 * color }, color);
                break;
            default:
                printf("An Error Has Occured Type '%c' not reconized", selPiece->type);
            }
        }
    }
}
int getNumPossibleMoves( int color) {// Gets number of Possible moves DO NOT USE IN EVALUATION   When used in eval it increases time complexity by 10x !
    int score=0;
    for (int i = 0; i < board.size; i++) {
        piece* selPiece = getArr1(&board, i);
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1)) 
            switch (selPiece->type) {
            case 'k':
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1 }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1 }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy }, color));
               score += min(1,NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1 }, color));
                break;
            case 'q':
                done = 1;
                offset = 1;
                while ( done== 1) {
                    done = NCheck( (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx , selPiece->posy +offset}, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx , selPiece->posy-offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color);
                    score++;
                    offset++;
                }
                break;
            case 'r':

                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset }, color);
                    score++;
                    offset++;
                }
                break;
            case 'b':

                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy+offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color);
                    score++;
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color);
                    score++;
                    offset++;
                }
                break;
            case 'h':

                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2 }, color));
                score += min(1,NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2 }, color));
                break;
            case 'p':
                if (getPieceAtPos(selPiece->posx + 1, selPiece->posy + 1 * color) >= 0)score++;
                if (getPieceAtPos(selPiece->posx - 1, selPiece->posy + 1 * color) >= 0)score++;
                if (
                    NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 * color }, color) == 1) {
                    score++; if (NCheck((Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 2 * color }, color) == 1)score++;
                }
                break;
            default:
                printf("An Error Has Occured Type '%c' not reconized", selPiece->type);
            
        }
    }return score;
}
double eval(ArrayP* board) {
    double pieceScore = 0;
    double mobilityScore = 0;
    double positionalScore = 0;
    //mobilityScore += getNumPossibleMoves(1) - getNumPossibleMoves(-1); 
    
    for (int i = 0; i < board->size; i++) {
        if ((board->ptr + i)->team == 'w') { 
            pieceScore +=getPieceVal((board->ptr + i));
        }
        else {
            pieceScore -= getPieceVal((board->ptr + i));
        }
    }
    return pieceScore+ mobilityScore;
}double Quiesce(double alpha, double beta, int color) {
    double stand_pat = color * eval(&board);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    Array posMoves;
    initArr(&posMoves, 0);
    getPossibleMoves(&posMoves, color);
    for(int i = 0; i < posMoves.size; i++)
    {
        int posPiece = getPieceAtPos((posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);
        if (posPiece != -1 &&((color ==1 &&getArr1(&board , i)->team=='b')||((color != 1 && getArr1(&board, i)->team == 'w')))) { // if the move takes and the taken piece is a enemy
            movePiece(getPieceAtPos((posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy), (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);
            double score = -Quiesce(-beta, -alpha,-color);
            

            takeBack();
            if (score >= beta) {
                freeArr(&posMoves);
                return beta;
            }
            if (score > alpha)
                alpha = score;
        }
    }
    freeArr(&posMoves);
    return alpha;
}

int targetDepth;
Possiblemove bestMove;

/* Concept of the negaMax function
int alphaBeta(int alpha, int beta, int depthleft, int color) {
    if (depthleft == 0) return Evaluate() * color;
    /*for (all moves) {
        score = -alphaBeta(-beta, -alpha, depthleft - 1 , -color);
        if (score >= beta)
            return beta;   //  fail hard beta-cutoff
        if (score > alpha)
            alpha = score; // alpha acts like max in MiniMax
    }
    return alpha;
}*/
double negaMax(int depth, int alpha , int beta, int color) {

    if (depth <= 0 || checkKing() < 2) {
        return eval(&board) *color;
    }
    double bestScore = -100000000000;
    Array posMoves; 
    initArr(&posMoves,0);
    getPossibleMoves(&posMoves, color);


    for (int i = 0; i < posMoves.size; i++) {

        movePiece(getPieceAtPos((posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy), (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);
        double value  = -negaMax(depth - 1,-beta,-alpha, -color);
        
        if (value > bestScore)bestScore = value;
        if (value > alpha)alpha = value;
        takeBack();
        if (alpha >= beta) {
            return alpha; 
        }
        
    }
    return bestScore;
    
}
double negaMax1(int depth, int alpha, int beta, int color) {
    if (depth == 0 || checkKing() < 2) {
        return eval(&board) * color;
    }
    double bestScore = -100000000000;
    Array posMoves;
    initArr(&posMoves, 0);
    getPossibleMoves(&posMoves, color);


    for (int i = 0; i < posMoves.size; i++) {
        printf("\r%1.d %% Done\r", (int)((double)i / (double)posMoves.size * 100));
        movePiece(getPieceAtPos((posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy), (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);
        double value = -negaMax(depth - 1, -beta, -alpha, -color);

        if (value > bestScore) { 
            bestScore = value; 
            bestMove = (Possiblemove){ (posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy , (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety };
        }
        if (value > alpha)alpha = value;
        takeBack();
        //printf("\nMove : (%d, %d) to (%d, %d)\nValue : %f", (posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy, (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety, value);
        if (alpha >= beta) {
            bestMove = (Possiblemove){ (posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy , (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety };

            free(posMoves.ptr);
            return alpha;
        }

    }//printf("\rDone Calculating\n");
    free(posMoves.ptr);
    return bestScore;

}


// Prints out the Board in Console
void refreshConsole(int turn) {
    //system("CLS"); // uncomment if you want it to refresh every time
    char can;
    int e = 0;

    if (turn != 1) {
        for (e = 7; e >= -1; e--) {//Y

            if (e == -1) {
                printf("%s \n", "   _______________");
                printf("%s \n", "   A B C D E F G H");
            }
            else
                for (int i = 8; i >=0; i--)//X  

                    if (i == 8) {
                        printf("%d| ", e + 1);
                    }
                    else {

                        int j;
                        for (j=0 ; j < board.size; j++) {
                            if (((int)(getArr1(&board, j)->type)) > 64 && ((int)(getArr1(&board, j)->type)) < 123 && (e == getArr1(&board, j)->posy && i == getArr1(&board, j)->posx)) {
                                //printf(" (%d , %d) ", i, e);
                                if ((int)getArr1(&board, j)->team == (int)'w') {
                                    printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, j)->type));
                                }
                                else {
                                    printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, j)->type));
                                }
                                
                                break;
                            }
                        }
                        if (j== board.size) {
                            //printf(" (%d , %d) ", i, e);
                            printf("\x1b[0m\x1b[37m+ ");
                        }

                    }
            printf("\n");
        }
    }else 
    for (e = 0; e <= 8; e++) {//Y

        if (e == 8) {
            printf("%s \n", "   _______________");
            printf("%s \n", "   H G F E D C B A");
        }else
        for(int i = -1 ; i < 8 ; i ++)//X  

            if (i == -1) {
                printf("%d| ", e+1);
            }
            else {
                can = 'f';
                for (int j = 0; j < board.size; j++) {
                    if (((int)(getArr1(&board, j)->type)) > 64 &&((int)(getArr1(&board,j)->type)) <123&& (e == getArr1(&board, j)->posy&&i == getArr1(&board, j)->posx)) {

                        if ((int)getArr1(&board, j)->team == (int)'w') {

                            printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, j)->type));
                        }
                        else {
                            printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, j)->type));
                        }
                        can = 't';
                        break;
                    }
                }
                if (can != 't') {
                    printf("\x1b[0m\x1b[37m+ ");
                }

            }
        printf("\n");
    }



}
//useless lol
void copyArr(piece* target, piece* dest) {

    for (int i = 0; i < 32; i++)
    {
        (*(dest + i)) = (piece){ (*(target + i)).posx, (*(target + i)).posy,(*(target + i)).type, (*(target + i)).team };
        
    }
}
int main() {
    // initialize the variable sizes
    initArr1(&board, 32);

    initArrList(&movesDone, 0);
    //adds the pieces
    for (int i = 0; i < 8; i++)(*(board.ptr + i)) = (piece){
             i , 6, 'p', 'b'
    };
    for (int i = 8; i < 16; i++)(*(board.ptr + i)) = (piece){
             i - 8, 1, 'p', 'w'
    };
    (*(board.ptr + 17)) = (piece){
                 1, 0, 'h', 'w'
    };
    (*(board.ptr + 18)) = (piece){
                 6, 0, 'h', 'w'
    };
    (*(board.ptr + 19)) = (piece){
                 0, 0, 'r'  , 'w' 
        };
    (*(board.ptr + 20)) = (piece){
                 2, 0, 'b', 'w' 
        }; 
    (*(board.ptr + 21)) = (piece){
                 3, 0, 'k', 'w' 
        }; 
    (*(board.ptr + 22)) = (piece){
                 4, 0, 'q', 'w' 
        }; 
    (*(board.ptr + 23)) = (piece){
                 5, 0, 'b', 'w' 
        }; 
    (*(board.ptr + 24)) = (piece){
                 7, 0, 'r', 'w' 
        }; 
       
    (*(board.ptr + 25)) = (piece){
                     0, 7, 'r'  , 'b' 
        };
        
    (*(board.ptr + 26)) = (piece){
                 1, 7, 'h', 'b' 
        }; 
    (*(board.ptr + 27)) = (piece){
                 2, 7, 'b', 'b' 
        }; 
    (*(board.ptr + 28)) = (piece){
                 3, 7, 'k', 'b' 
        }; 
    (*(board.ptr + 29)) = (piece){
                 4, 7, 'q', 'b' 
        }; 
    (*(board.ptr + 30)) = (piece){
                 5, 7, 'b', 'b' 
        }; 
    (*(board.ptr + 31)) = (piece){
                 6, 7, 'h', 'b' 
        }; 
    (*(board.ptr + 16)) = (piece){
                 7, 7, 'r', 'b' 
        }; 
       
    // prints current state of board
    refreshConsole(-1);

    int turn = 1;// Who's turn to move. 1 for white -1 for black
    char cat[5];// input from console
    while (1==1) {
        // gets input into cat
        scanf("%s", &cat);

        //if user puts a word with ev at start it will print out evaluations of the board state
        //if user puts a word with ex at start it will exit
        if (cat[0] == 'e')
            if (cat[1] == 'v') {
                targetDepth = 14;
                printf("eval : %f\n NegaMax : %f \n" , eval(&board), negaMax1(6, -100000000000, 100000000000, turn));
                
            }else if (cat[1] == 'x') 
                return 0;

        // if user puts word bo at start then it will let the bot make it's move
        
        if (cat[0] == 'b' && cat[1] == 'o') {
                // gets the value of the board and best move is put in the Variable bestMove
                double val = negaMax1(6, -100000000000, 100000000000   ,turn);
                
                printf("Best Move is %d , %d  to %d, %d \n ", bestMove.posx, bestMove.posy, bestMove.targetx, bestMove.targety);
                printf("Bot confidence : %f\n", val);
                movePiece(getPieceAtPos(bestMove.posx, bestMove.posy), bestMove.targetx, bestMove.targety);

                printf("\nTurn: %d\n", movesDone.size);
                turn *= -1;
                refreshConsole(-turn);


            }else// reverses a move
            if (cat[0] == 'b' && cat[1] == 'a') {
                takeBack();
                refreshConsole(turn);
                turn *= -1;
            }else{// HUMAN MOVES
                // converts letters into array indexes 
        int pickedPosX =7- ((int)cat[0]-65)%32;
        int pickedPosY = (int)cat[1]-49;
        int canMove = getPieceAtPos(pickedPosX, pickedPosY);
        if (canMove >= 0 && ((getArr1(&board, canMove)->team == 'w' && turn > 0) || (getArr1(&board, canMove)->team != 'w' && turn < 0))) {// if they can move there 
            Array possMoves;
            possMoves.ptr = malloc(0);
            possMoves.size = 0;

            getPossibleMoves(&possMoves, turn);
            int i;

            for (i = 0; i < possMoves.size; i++) {// searches for the move specified by the Human and is in the list of possible moves
                Possiblemove* point = getArr(&possMoves, i);
                if (point->posx == pickedPosX && point->posy == pickedPosY && point->targetx == (7 - ((int)cat[2] - 65) % 32) && point->targety == ((int)cat[3] - 49)) {
                   
                    break;
                }
            }

            if (i < possMoves.size) {// if  the move is specified by the Human and is in the list of possible moves

                movePiece(canMove, 7 - (((int)cat[2] - 65) % 32), (int)cat[3] - 49);

                printf("%d, %d  to %d , %d   Real : %d , %d\n", pickedPosX, pickedPosY, (7 - ((int)cat[2] - 65) % 32), (int)cat[3] - 49, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posx, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posy);

                printf("\nTurn: %d\n", movesDone.size);
                turn *= -1;
                refreshConsole(-turn);

            }else 
                printf("\nInvalid Move Try Again\n");


        
        }
        else {
            printf("\nInvalid Move Try Again\n");
        }
            }/*
            printf("Looking at %d\n" , movesDone.size-1);
            for (int i = 0; i < movesDone.size; i++) {
                move* look = (movesDone.ptr + i);
                printf(" %d : %d %d %d %d\n",i, look->posx, look->posx, look->targetx, look->targety);
            }*/
    }
    return 0;
}
