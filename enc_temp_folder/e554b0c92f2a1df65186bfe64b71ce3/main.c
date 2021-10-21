#include<stdio.h>
#include <windows.h>
<<<<<<< HEAD
#include <limits.h>
#include <time.h>
 unsigned int butterfly[8][8][8][8];
=======

>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
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

typedef struct nodeValue { 
    int value;
    long node ;
    int depth;
    char flag;
}nodeValue;

typedef struct  NodeArray { // 12 bytes total not including where the pointers point to
    nodeValue* ptr; // 8 bytes
    int size;//4 bytes
    int realSize;// Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
    // the variable above is not really needed
}NodeArray;
void removeNodeArr(NodeArray* arr, int pos) {
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++) {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}
void changeNodeSize(NodeArray* target, int size) {

    if (target->size > target->realSize)
        target->ptr = (nodeValue*)realloc((*target).ptr, size * sizeof(nodeValue));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
void initNodeArr(NodeArray* arr, int size) {
    *arr = (NodeArray){ (nodeValue*)calloc(size , sizeof(nodeValue)) , 0,size};
    /*
    arr->ptr =(Possiblemove*) calloc(size , sizeof(Possiblemove));
    arr->size = size;
    arr->realSize = size;*/
}int compareNode(const void *p, const void *q) 
{
    return ((long)((struct nodeValue *)p)->node - (long)((struct nodeValue *)q)->node);
}

int binarySearch(nodeValue*arr, int l, int r, long x) // low high target
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
 
        // If the element is present at the middle
        // itself
        if ((arr+mid)->node == x)
            return mid;
 
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if ((arr+mid)->node > x)
            return binarySearch(arr, l, mid - 1, x);
 
        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
 
    // We reach here when element is not
    // present in array
    return -1;
}
 
int searchNode(NodeArray* arr, long value){
    /*qsort(arr->ptr,arr->size,sizeof(nodeValue),compareNode);
            return (binarySearch(arr->ptr,0,arr->size,value));
    return -1;*/
    for(int i = 0 ; i < arr->size ; i ++){
        if((arr->ptr+i)->node== value)return i;
    }return -1;
}
void addNodeArr(NodeArray* arr, nodeValue obj) {
    arr->size += 1;

    if (arr->realSize < arr->size)
        changeNodeSize(arr, arr->size);

    *(arr->ptr + (arr->size - 1)) = obj;
}
void storeNode(NodeArray* arr, int pos,nodeValue obj) {
    if(pos==-1){
        arr->size += 1;

        if (arr->realSize < arr->size)
            changeNodeSize(arr, arr->size);

        *(arr->ptr + (arr->size - 1)) = obj;
    }else{
         *(arr->ptr+pos)= obj ;
    }

}
nodeValue* getNodeArr(NodeArray* arr, int i) {
    return (arr->ptr + i);
}
void freeNodeArr(NodeArray* arr) {
    free(arr->ptr);
}
//converts x and y cords to indexes and gets the positional score from a array of 64 from one of them above
int getPoints(int map[64] ,int x, int y, int reversed) {
    x = min(x, 7);
    y = min(y, 7);
    if (reversed != 1) {
        y = 7 - x;

    }
    if( (y * 8 + x)< 64)
        return (map[  y*8 +  x ]);
    return 0;
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
int bestValue =-1000000000; 
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
NodeArray nodeArray;



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
<<<<<<< HEAD
    *arr = (Array){ (Possiblemove*)calloc(size , sizeof(Possiblemove)) , 0,size};
=======
    *arr = (Array){ (Possiblemove*)calloc(size , sizeof(Possiblemove)) , size,size};
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
    /*
    arr->ptr =(Possiblemove*) calloc(size , sizeof(Possiblemove));
    arr->size = size;
    arr->realSize = size;*/
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
    arr->size = 0;
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
// Moves a piece in the array 'board' in the specified location of the array to the x and y cordinates NO QUESTIONS ASKED
void movePieceFast(int location, int x, int y) {
       (board.ptr + location)->posx = x;
       (board.ptr + location)->posy = y;
}

//looks in the Array movesDone and reverses a move
int takeBack() {
    if (movesDone.size > 0) {
        move* lookat = (((movesDone.ptr + movesDone.size - 1)));
        
        int canMove = getPieceAtPos(lookat->targetx, lookat->targety);
        //move the piece back
        movePieceFast(canMove, lookat->posx, lookat->posy);
        //if there was a piece where there were add it back
        if (lookat->type != '-' && lookat->team != '-') {
            addArr1(&board, (piece) { lookat->targetx, lookat->targety, lookat->type, lookat->team });
        movesDone.size--;
            return 1;
        }movesDone.size--;
    }return 0;
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

}
int addNCheck_IFTAKE(Array* output, Possiblemove obj, int color) {
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8) {
        // get the Target or -1 if there is none
        int num = getPieceAtPos(obj.targetx, obj.targety);
        // Checks if there is nothing there
        if (num < 0) {
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

long getPieceID(piece* obj){
    int val =0;
    switch(obj->type){
        case 'k':
            val =0;
            if(obj->team!='w')val +=6;
            return val;
        case 'q':
            
            val =1;
            if(obj->team!='w')val +=6;
            return val;
        case 'r':
            
            val =2;
            if(obj->team!='w')val +=6;
            return val;
        case 'b':
            
            val =3;
            if(obj->team!='w')val +=6;
            return val;
        case 'h':
                
            val =4;
            if(obj->team!='w')val +=6;
            return val;
        case 'p':
            
            val =5;
            if(obj->team!='w')val +=6;
            return val;
        }
}long table[8][8][12];
void init_zobrist(){
    // fill a table of random numbers/bitstrings
    
    for (int i = 0; i < 8; i++)
    {
        for (int e = 0; e < 8; e++)
        {
            for (int j = 0; j < 12; j++)
            {
                
                table[i][e][j] = ((long)((rand())*LONG_MAX));
            }
            
        }
        
    }
    
            
}
int compPiecePos(const void *p, const void *q){
    return (((piece*)p )->posx*8 +((piece*)p )->posy) -(((piece*)q )->posx*8 +((piece*)q )->posy);
}
long hash(ArrayP* pieceMap){
    qsort(pieceMap->ptr, pieceMap->size, sizeof(piece),compPiecePos);
    long h = 0;
    for( int i = 0 ; i< pieceMap->size ; i++ ){
        h=  h ^ table[(pieceMap->ptr+i)->posx][(pieceMap->ptr+i)->posy][getPieceID((piece*)(pieceMap->ptr+i))];
    }
    return h;
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

void getPossibleMovesPV(Array* output, int color) {// Adds all possible moves to output array
    int score = 0;
    for (int i = 0; i < board.size; i++) {
        piece* selPiece = getArr1(&board, i);
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1)) {
            switch (selPiece->type) {
            case 'k':
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1 }, color);
                break;
            case 'q':

                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset }, color) == 1)
                    offset++;

                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                break;
            case 'r':

                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset }, color);
                    offset++;
                }
                done = 1;
                offset = 1;
                while (done == 1) {
                    done = addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset }, color);
                    offset++;
                }
                break;
            case 'b':

                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                done = 1;
                offset = 1;
                while (addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset }, color) == 1) {
                    offset++;
                }
                break;
            case 'h':

                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2 }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2 }, color);
                break;
            case 'p':
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 * color }, color);
                addNCheck_IFTAKE(output, (Possiblemove) { selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 * color }, color);
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
typedef struct indexes{
    int index1;
    int index2;
}indexes;
int compareIndexes(const void * a, const void * b) {
   return (   ((indexes*)b)->index2 -((indexes*)a)->index2);
}
indexes* customHeuristic( Array* arr,indexes* output) {// It's a idea i thought based on the killer Heuristic where the moves is sorted based on simlar best moves based on simular depth

    for( int i = 0 ;i <arr->size; i ++){
        (output+i)->index1 = i;
        (output+i)->index2 =butterfly[(arr->ptr+i)->posx][(arr->ptr+i)->posy][(arr->ptr+i)->targetx ][(arr->ptr+i)->targety];

    }
    qsort(output,arr->size ,sizeof(indexes),compareIndexes);
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
}double Quiesce(int depth,double alpha, double beta, int color) {
    
    double stand_pat = color * eval(&board);
    if (depth <= 0 || checkKing() < 2) {
        return stand_pat;
    }
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    Array posMoves =  (Array){ (Possiblemove*)calloc(1 , sizeof(Possiblemove)) , 1,1};
    //initArr(&posMoves, 0);
<<<<<<< HEAD
    getPossibleMovesPV(&posMoves, color);
    indexes* sortedArr =calloc(posMoves.size, sizeof(indexes));
    customHeuristic( &posMoves,sortedArr);
    int toggle =0;
    for(int i = 0; i < posMoves.size; i++)
    {
           movePiece(getPieceAtPos((posMoves.ptr + (sortedArr+i)->index1)->posx, (posMoves.ptr + (sortedArr+i)->index1)->posy), (posMoves.ptr + (sortedArr+i)->index1)->targetx, (posMoves.ptr + (sortedArr+i)->index1)->targety);
        
            double score = -Quiesce(depth-1,-beta, -alpha,-color);
=======
    getPossibleMoves(&posMoves, color);
    int toggle =0;
    for(int i = 0; i < posMoves.size; i++)
    {
        int posPiece = getPieceAtPos((posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);// returns -1 if there is no piece
        if (posPiece != -1 &&((color ==1 &&getArr1(&board , i)->team=='b')||((color != 1 && getArr1(&board, i)->team == 'w')))) { // if the move takes and the taken piece is a enemy
            movePiece(getPieceAtPos((posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy), (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety);
            double score = -Quiesce(depth-1,-beta, -alpha,-color);
            toggle ++;
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f

            takeBack();

            if (score > alpha)
                alpha = score;
            if (score >= beta) {
               // butterfly[(posMoves.ptr + i)->posx][(posMoves.ptr + i)->posy][(posMoves.ptr + i)->targetx][(posMoves.ptr + i)->targety]=depth;
                freeArr(&posMoves);
                return beta;
            }
<<<<<<< HEAD
        
    }
    freeArr(&posMoves);
    if(posMoves.size ==0){
        return stand_pat;
    }
=======
        }
    }
    freeArr(&posMoves);
    if(toggle ==0)return stand_pat;
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
    return alpha;
}

int targetDepth;
Possiblemove bestMove;

/* Concept of the negaMax functionfunction negamax(node, depth, α, β, color) is
    alphaOrig := α

    (* Transposition Table Lookup; node is the lookup key for ttEntry *)
    ttEntry := transpositionTableLookup(node)
    if ttEntry is valid and ttEntry.depth ≥ depth then
        if ttEntry.flag = EXACT then
            return ttEntry.value
        else if ttEntry.flag = LOWERBOUND then
            α := max(α, ttEntry.value)
        else if ttEntry.flag = UPPERBOUND then
            β := min(β, ttEntry.value)

        if α ≥ β then
            return ttEntry.value

    if depth = 0 or node is a terminal node then
        return color × the heuristic value of node

    childNodes := generateMoves(node)
    childNodes := orderMoves(childNodes)
    value := −∞
    for each child in childNodes do
        value := max(value, −negamax(child, depth − 1, −β, −α, −color))
        α := max(α, value)
        if α ≥ β then
            break

    (* Transposition Table Store; node is the lookup key for ttEntry *)
    ttEntry.value := value
    if value ≤ alphaOrig then
        ttEntry.flag := UPPERBOUND
    else if value ≥ β then
        ttEntry.flag := LOWERBOUND
    else
        ttEntry.flag := EXACT
    ttEntry.depth := depth	
    transpositionTableStore(node, ttEntry)

    return value
}*/
double negaMax(int depth, int alpha , int beta, int color) {
    /*
    double alphaOrig = alpha;
    int ttEntryPos = searchNode(&nodeArray,hash(&board) );
    nodeValue ttEntry ;
    if(ttEntryPos !=-1 &&(nodeArray.ptr+ttEntryPos)->depth >=depth){
        ttEntry = (nodeValue){(nodeArray.ptr+ttEntryPos)->value ,(nodeArray.ptr+ttEntryPos)->node,(nodeArray.ptr+ttEntryPos)->depth,(nodeArray.ptr+ttEntryPos)->flag};
        if(ttEntry.flag=='e'){
            return (nodeArray.ptr+ttEntryPos)->value;
        }else if(ttEntry.flag=='l'){
            alpha= max(alpha,(nodeArray.ptr+ttEntryPos)->value);
        }else if(ttEntry.flag=='u'){
            beta= min(beta, (nodeArray.ptr+ttEntryPos)->value);
        }
        if(beta < alpha){
            return (nodeArray.ptr+ttEntryPos)->value;
        }
    }*/
    if (depth <= 0 || checkKing() < 2) {
        return
            Quiesce(6,alpha, beta, color);
    }
    Array posMoves; 
    initArr(&posMoves,40);
    getPossibleMoves(&posMoves, color);
    indexes* sortedArr =calloc(posMoves.size, sizeof(indexes));
    customHeuristic( &posMoves,sortedArr);
    int val = -1000000;
    for (int i = 0; i < posMoves.size; i++) {

        movePiece(getPieceAtPos((posMoves.ptr + (sortedArr+i)->index1)->posx, (posMoves.ptr + (sortedArr+i)->index1)->posy), (posMoves.ptr + (sortedArr+i)->index1)->targetx, (posMoves.ptr + (sortedArr+i)->index1)->targety);
        
        double value  = -negaMax(depth - 1,-beta,-alpha, -color);
        if( value > val){
            val =value;
        }
                
        if (val > alpha)alpha = val;
        
        if(takeBack()!=1&&alpha > beta) {
            butterfly[(posMoves.ptr + (sortedArr+i)->index1)->posx][ (posMoves.ptr + (sortedArr+i)->index1)->posy] [(posMoves.ptr + (sortedArr+i)->index1)->targetx][ (posMoves.ptr + (sortedArr+i)->index1)->targety]+= depth *depth;
        }
        if (alpha > beta) {

            break;
        
        }

    }
/*
    ttEntry.value =val;
    if(val <=alphaOrig){
        ttEntry.flag='u';
    }else if(val >beta){
        ttEntry.flag='l';
    }else{
        ttEntry.flag='e';
    }ttEntry.node = hash(&board);
    ttEntry.depth = depth;
    storeNode(&nodeArray,ttEntryPos,ttEntry);*/


    free(posMoves.ptr);free(sortedArr);
    return alpha;
    
}
double negaMax1(int depth, int alpha, int beta, int color) {
    if (depth == 0 || checkKing() < 2) {
        return
            Quiesce(4,alpha, beta, color);
    }
    Array posMoves;
    initArr(&posMoves, 0);
    getPossibleMoves(&posMoves, color);
    indexes* sortedArr =calloc(posMoves.size, sizeof(indexes));
    customHeuristic( &posMoves,sortedArr);

    for (int i = 0; i < posMoves.size; i++) {
        printf("\r%1.d %% Done!\r", (int)((double)i / (double)posMoves.size * 100));
        movePiece(getPieceAtPos((posMoves.ptr + (sortedArr+i)->index1)->posx, (posMoves.ptr + (sortedArr+i)->index1)->posy), (posMoves.ptr + (sortedArr+i)->index1)->targetx, (posMoves.ptr + (sortedArr+i)->index1)->targety);
        
<<<<<<< HEAD
        double value = -negaMax(depth - 1, -beta, -alpha, -color);

        if(takeBack()!=1&&value > beta) {
            butterfly[(posMoves.ptr + (sortedArr+i)->index1)->posx][ (posMoves.ptr + (sortedArr+i)->index1)->posy] [(posMoves.ptr + (sortedArr+i)->index1)->targetx][ (posMoves.ptr + (sortedArr+i)->index1)->targety]+= depth *depth;
        }
                
        if(bestValue< value){
            bestMove = (Possiblemove){ (posMoves.ptr + (sortedArr+i)->index1)->posx, (posMoves.ptr + (sortedArr+i)->index1)->posy, (posMoves.ptr + (sortedArr+i)->index1)->targetx, (posMoves.ptr + (sortedArr+i)->index1)->targety}; 
                    bestValue=value;
        }

        if (value > alpha) {
            alpha = value; 
        }
        if (alpha > beta) {
=======
if(bestValue< value){
    bestMove = (Possiblemove){ (posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy , (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety }; 
            bestValue=value;
}

        if (value > beta) {
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
            //bestMove = (Possiblemove){ (posMoves.ptr + i)->posx, (posMoves.ptr + i)->posy , (posMoves.ptr + i)->targetx, (posMoves.ptr + i)->targety };
            free(posMoves.ptr);
            free(sortedArr);
            return beta;
        
        }
<<<<<<< HEAD
=======
        if (value > alpha) {
            alpha = value; 
        }
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
    }//printf("\rDone Calculating\n");
            free(sortedArr);
    free(posMoves.ptr);
    return alpha;

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
    init_zobrist();
    initNodeArr(&nodeArray ,100);
    initArrList(&movesDone, 0);
    board.size=32;
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
<<<<<<< HEAD
                printf("eval : %f\n ID : %ld \n" , eval(&board),hash(&board));
=======
                printf("eval : %f\n NegaMax : %f \n" , eval(&board), negaMax1(6, -1000000000, 1000000000, turn));
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
                                                                                    
            }else if (cat[1] == 'x') 
                return 0;

        // if user puts word bo at start then it will let the bot make it's move
        
        if (cat[0] == 'b' && cat[1] == 'o') {
<<<<<<< HEAD
            long seconds = time(NULL);
            for(int from = 0 ; from < 8;from++)
                for(int to = 0 ; to < 8;to++)
                for(int from1 = 0 ; from1 < 8;from1++)
                for(int to1 = 0 ; to1 < 8;to1++)
                butterfly[from][to][from1][to1]=0;
=======
            
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
            bestValue = -10000000;
                // gets the value of the board and best move is put in the Variable bestMove
                double val = negaMax1(5, -1000000000, 1000000000,turn);
                
<<<<<<< HEAD
                printf("Took %ld seconds\nBest Move is %c%d  to %c%d \n ",time(0)-seconds , ((7-bestMove.posx)+65), bestMove.posy+1, (7-bestMove.targetx)+65, bestMove.targety+1);
=======
                printf("Best Move is %c%d  to %c%d \n ", ((7-bestMove.posx)+65), bestMove.posy+1, (7-bestMove.targetx)+65, bestMove.targety+1);
>>>>>>> afa2099f1046fffa734dd289bc6d36eeddbf0c2f
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

            possMoves.realSize = 0;
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

                printf("%c, %d  to %c , %d   Real : %d , %d\n", 7-pickedPosX+65, pickedPosY+1, cat[2], (int)cat[3] - 49, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posx, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posy);

                printf("\nTurn: %d\n", movesDone.size);
                turn *= -1;
                refreshConsole(-turn);

            }else 
                printf("\nInvalid Move Try Again\n");


            freeArr(&possMoves);
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
