#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <limits.h>
#include <math.h>
#include <time.h>
unsigned int butterfly[8][8][8][8];
long nodesProcessed = 0;
unsigned int butterfly_PV[8][8][8][8];
long long whiteornot;
long numNodes = 0;
int bobby = 0;
int targetedDepth = 5; // Looks this move or even more ahead
int expandedDepth = 6; // Looks this moves ahead after the intial depth but is more select
int unwantedDepth = 2; // Anything moves equal or under than this is considered not worth it to store
// Score Maps for scoring

int pawnMap[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};
int knightMap[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};
int bishopMap[64] = {

    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};
int rookMap[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0};
int queenMap[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};
int kingMap[64] = {

    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

typedef struct Possiblemove
{
    short posx;
    short posy;
    short targetx;
    short targety;
    char type;

} Possiblemove;
typedef struct nodeValue
{
    int value;
    long long node;
    int depth;
    char flag;
} nodeValue;
struct Node
{
    long long key;
    int value;
    short depth;
    short age;
    char flag;
    struct Node *left;
    struct Node *right;
    Possiblemove move;
    unsigned short height;
};

// A utility function to get the height of the tree
int height(struct Node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

int Max(int a, int b)
{
    return (a > b) ? a : b;
}

/* Helper function that allocates a new node with the given key and
    NULL left and right pointers. */
struct Node *newNode(nodeValue key, Possiblemove *move)
{
    struct Node *node = (struct Node *)
        malloc(sizeof(struct Node));
    node->key = key.node;
    node->value = key.value;
    node->depth = key.depth;
    node->flag = key.flag;
    node->move = *move;
    node->left = NULL;
    node->right = NULL;
    node->age = 0;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // new node is initially added at leaf
    return (node);
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
struct Node *rightRotate(struct Node *y)
{
    struct Node *x = y->left;
    struct Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = Max(height(y->left), height(y->right)) + 1;
    x->height = Max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
struct Node *leftRotate(struct Node *x)
{
    struct Node *y = x->right;
    struct Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = Max(height(x->left), height(x->right)) + 1;
    y->height = Max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

struct Node *search(struct Node *root, long long x)
{

    if (root == NULL) //if root->data is x then the element is found
        return root;
    if ((root->key) == x)
    {
        root->age = 0;
        return root;
    }

    root->age++;

    if (x > root->key) // x is greater, so we will search the right subtree
        return search(root->right, x);
    else //x is smaller than the data, so we will search the left subtree
        return search(root->left, x);
}

int getHeight(struct Node *left)
{
    if (left->left == NULL)
    {
        return left->height;
    }
    getHeight(left->left);
}
// Get Balance factor of node N
int getBalance(struct Node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
struct Node *insert(struct Node *node, nodeValue key, Possiblemove *move)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return (newNode(key, move));
    if (node->key == key.node && node->depth <= key.depth)
    {
        node->depth = key.depth;
        node->value = key.value;
        node->flag = key.flag;
        return node;
    }
    if (key.node < node->key)
        node->left = insert(node->left, key, move);
    else if (key.node > node->key)
        node->right = insert(node->right, key, move);
    else // Equal keys are not allowed in BST
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + Max(height(node->left),
                           height(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key.node < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key.node > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key.node > node->left->key)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key.node < node->right->key)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

// A utility function to print preorder traversal
// of the tree.
// The function also prints height of every node
void preOrder(struct Node *root)
{
    if (root != NULL)
    {
        printf("%lld ", root->key);
        preOrder(root->left);
        preOrder(root->right);
    }
}
void refreshNodes(struct Node *root)
{
    if (root != NULL)
    {

        refreshNodes(root->left);
        refreshNodes(root->right);
        free(root);
    }
}

typedef struct NodeArray
{                   // 12 bytes total not including where the pointers point to
    nodeValue *ptr; // 8 bytes
    int size;       //4 bytes
    int realSize;   // Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
    // the variable above is not really needed
} NodeArray;
//converts x and y cords to indexes and gets the positional score from a array of 64 from one of them above
int getPoints(int map[64], int x, int y, int reversed)
{
    x = min(x, 7);
    y = min(y, 7);
    if (reversed != 1)
    {
        y = 7 - x;
    }
    if ((y * 8 + x) < 64)
        return (map[y * 8 + x]);
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
typedef struct piece
{
    int posx;
    int posy;
    char type;
    char team;
} piece;

int boardHashMap[8][8];
typedef struct move
{
    int posx;
    int posy;
    int targetx;
    int targety;
    char type;
    char team;
} move;
int bestValue = -1000000000;
//List Objects / ArrayList
int NodeValueSizesize = sizeof(nodeValue);
typedef struct movesList
{ // List of Moves Meant to keep track of the History of what moves were done
    move *ptr;
    int size;     //Desired Size
    int realSize; // Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
} movesList;
typedef struct Array
{                      // 12 bytes total not including where the pointers point to
    Possiblemove *ptr; // 8 bytes
    int size;          //4 bytes
    int realSize;      // Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
    // the variable above is not really needed
} Array;

typedef struct ArrayP
{                 // Board Object / Array of Pieces
    piece *ptr;   // 8 bytes
    int size;     //4 bytes
    int realSize; // Real Size of Array in Memory  RealSize =  Pointersize / sizeOf(object)
} ArrayP;

ArrayP board;        // Array of Pieces on the Board
movesList movesDone; // List of moves Done
void changeSize(Array *target, int size)
{

    if (target->size > target->realSize)
        target->ptr = (Possiblemove *)realloc((*target).ptr, size * sizeof(Possiblemove));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
void initArr(Array *arr, int size)
{
    *arr = (Array){(Possiblemove *)calloc(size, sizeof(Possiblemove)), 0, size};
    /*
    arr->ptr =(Possiblemove*) calloc(size , sizeof(Possiblemove));
    arr->size = size;
    arr->realSize = size;*/
}
void addArr(Array *arr, Possiblemove obj)
{
    arr->size += 1;

    if (arr->realSize < arr->size)
        changeSize(arr, arr->size);

    *(arr->ptr + (arr->size - 1)) = obj;
}
Possiblemove *getArr(Array *arr, int i)
{
    return (arr->ptr + i);
}
void freeArr(Array *arr)
{
    free(arr->ptr);
}

/// <summary>
/// Resizes a array
/// </summary>
/// <param name="target"></param>
/// <param name="size"></param>
void changeSize1(ArrayP *target, int size)
{
    if (target->size > target->realSize)
        target->ptr = (piece *)realloc((*target).ptr, size * sizeof(piece));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
/// <summary>
/// Removes object in the specified position in the Array
/// </summary>
/// <param name="arr"></param>
/// <param name="pos"></param>
void removeArr1(ArrayP *arr, int pos)
{
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++)
    {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}
/// <summary>
/// Initializes a Array given the Address of an Array
/// </summary>
/// <param name="arr"></param>
/// <param name="size"></param>
void initArr1(ArrayP *arr, int size)
{
    arr->ptr = malloc(size * sizeof(piece));
    arr->size = 0;
    arr->realSize = size;
}
/// <summary>
/// Adds obj to the end of the Array
/// </summary>
/// <param name="ArrayP* array;"></param>
/// <param name="piece obj;"></param>
void addArr1(ArrayP *arr, piece obj)
{ //
    arr->size += 1;

    if (arr->realSize < arr->size)
        changeSize1(arr, arr->size);

    *(arr->ptr + (arr->size - 1)) = obj;
}
/// <summary>
/// Get Info from position i relitive to the address and memory location
/// </summary>
/// <param name="arr : Address of ArrayP Object"></param>
/// <param name="i   : Position of wanted piece"></param>
/// <returns></returns>
piece *getArr1(ArrayP *arr, int i)
{
    return (arr->ptr + i);
}
/// <summary>
/// Destroys the array
/// </summary>
/// <param name="arr"></param>
void freeArr1(ArrayP *arr)
{
    free(arr->ptr);
}

void removeArrList(movesList *arr, int pos)
{
    arr->size = arr->size - 1;
    for (int i = pos; i < arr->size; i++)
    {
        *(arr->ptr + i) = *(arr->ptr + i + 1);
    }
}

void removeFromTop(movesList *arr)
{
    arr->size = arr->size - 1;
}
void changeSizeList(movesList *target, int size)
{
    if (target->size > target->realSize)
        target->ptr = (move *)realloc((*target).ptr, size * sizeof(move));
    target->size = size;

    target->realSize = target->size > target->realSize ? target->size : target->realSize;
}
void initArrList(movesList *arr, int size)
{
    arr->ptr = malloc(size * sizeof(move));
    arr->size = size;
    arr->realSize = size;
}
void addArrList(movesList *arr, move obj)
{
    arr->size += 1;
    arr->realSize = arr->size > arr->realSize ? arr->size : arr->realSize;
    changeSizeList(arr, arr->size);
    *(arr->ptr + (arr->size - 1)) = obj;
}
move *getArrList(movesList *arr, int i)
{
    return (arr->ptr + i);
}
void freeArrList(movesList *arr)
{
    free(arr->ptr);
}

//SOMEHOW OPTIMIZE THIS   50% of all CPU goes to searching for pieces on board
int getPieceAtPos(int x, int y)
{
    for (int i = 0; i < board.size; i++)
    {
        if (((board.ptr + i))->posx == x && ((board.ptr + i))->posy == y)
        {
            return i;
        }
    }
    return -1;
}
int getPieceAtPosHash(int x, int y)
{
    return boardHashMap[x][y];
}
void initBoardHashMap()
{
    for (int i = 0; i < 8; i++)
    {
        for (int e = 0; e < 8; e++)
            boardHashMap[e][i] = -1;
    }
    for (int i = 0; i < board.size; i++)
        boardHashMap[board.ptr[i].posx][board.ptr[i].posy] = i;
}
// Moves a piece in the array 'board' in the specified location of the array to the x and y cordinates
void movePiece(int location, int x, int y)
{
    int i = getPieceAtPos(x, y);

    move temp;
    // if it found something in the loop above (a piece in the x and y cords)
    if (i != -1)
    {
        temp = (move){(board.ptr + location)->posx, (board.ptr + location)->posy, x, y, (board.ptr + i)->type, (board.ptr + i)->team};

        (board.ptr + location)->posx = x;
        (board.ptr + location)->posy = y;
        removeArr1(&board, i);
    }
    else
    {

        temp = (move){(board.ptr + location)->posx, (board.ptr + location)->posy, x, y, '-', '-'};
        (board.ptr + location)->posx = x;
        (board.ptr + location)->posy = y;
    }
    //Adds move to movelist
    addArrList(&movesDone, temp);
    initBoardHashMap();
}
// Moves a piece in the array 'board' in the specified location of the array to the x and y cordinates NO QUESTIONS ASKED
void movePieceFast(int location, int x, int y)
{
    (board.ptr + location)->posx = x;
    (board.ptr + location)->posy = y;
}

//looks in the Array movesDone and reverses a move
int takeBack()
{
    if (movesDone.size > 0)
    {
        move *lookat = (((movesDone.ptr + movesDone.size - 1)));

        int canMove = getPieceAtPos(lookat->targetx, lookat->targety);
        //move the piece back
        movePieceFast(canMove, lookat->posx, lookat->posy);
        //if there was a piece where there were add it back
        if (lookat->type != '-' && lookat->team != '-')
        {
            addArr1(&board, (piece){lookat->targetx, lookat->targety, lookat->type, lookat->team});
            movesDone.size--;
            initBoardHashMap();
            return 1;
        }
        movesDone.size--;
    }
    initBoardHashMap();
    return 0;
}
double getPieceVal(piece *obj)
{ // get Value of a piece based on it's position on the board and it's worth
    int team = 1;
    if (obj->team == 'w')
        team *= -1;

    switch (obj->type)
    {
    case 'k':
        return _king_Value + getPoints(kingMap, obj->posx, obj->posy, team);
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
    }
    return 0;
}

static struct Node *ttAble;

static struct Node *ttAble2;
static struct Node *ttAble_PV;

static struct Node *ttAble_PV2;
int checkKing()
{ // gets the number of kings on board
    int kings = 0;
    for (int i = 0; i < board.size; i++)
    {
        if (getArr1(&board, i)->type == 'k')
        {
            kings++;
            if (kings >= 2)
            {
                break;
            }
        }
    }
    return kings;
}
int addNCheck(Array *output, Possiblemove obj, int color)
{
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8)
    {
        // get the Target or -1 if there is none
        //int num = getPieceAtPos(obj.targetx, obj.targety);

        initBoardHashMap();
        int num = boardHashMap[obj.targetx][obj.targety];
        // Checks if there is nothing there
        if (num < 0)
        {
            addArr(output, obj);
            return 1;
        }
        char team = getArr1(&board, num)->team;

        if (num >= 0)
        {
            if (((team == 'w' && color != 1) || (team != 'w' && color == 1)))
                addArr(output, obj);

            return 0;
        }
    }
    return 0;
}
int addNCheck_IFTAKE(Array *output, Possiblemove obj, int color)
{
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8)
    {
        // get the Target or -1 if there is none
        int num = boardHashMap[obj.targetx][obj.targety];
        // Checks if there is nothing there
        if (num < 0)
        {
            return 1;
        }
        char team = getArr1(&board, num)->team;

        if (num >= 0 && ((team == 'w' && color != 1) || (team != 'w' && color == 1)))
        {

            addArr(output, obj);
            return 0;
        }
    }
    return 0;

} //Adds a move to the output array given that the move is possible (returns 1 if it is 0 if not)
int NCheck(Possiblemove obj, int color)
{
    //if obj is within bounds of the Chess Board
    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8)
    {
        // get the Target or -1 if there is none
        int num = boardHashMap[obj.targetx][obj.targety];
        // Checks if there is nothing there
        if (num < 0)
        {
            return 1;
        }

        char team = getArr1(&board, num)->team;
        // if there is something there and it is the enemy return 2
        if (num >= 0 && ((team == 'w' && color != 1) || (team != 'w' && color == 1)))
        {
            return 2;
        }
    }
    return 0;
}
int addNCheckCannotTake(Array *output, Possiblemove obj, int color)
{ //Adds a move to the output array given that the move is possible (returns 1 if it is 0 if not)
    // Meant for pawns as pawns can move but not take

    if (obj.targetx >= 0 && obj.targety >= 0 && obj.targetx < 8 && obj.targety < 8)
    {
        int num = boardHashMap[obj.targetx][obj.targety];

        if (num < 0)
        {
            addArr(output, obj);
            return 1;
        }
    }
    return 0;
}

long long getPieceID(piece *obj)
{
    int val = 0;
    switch (obj->type)
    {
    case 'k':
        val = 0;
        if (obj->team != 'w')
            val += 6;
        return val;
    case 'q':

        val = 1;
        if (obj->team != 'w')
            val += 6;
        return val;
    case 'r':

        val = 2;
        if (obj->team != 'w')
            val += 6;
        return val;
    case 'b':

        val = 3;
        if (obj->team != 'w')
            val += 6;
        return val;
    case 'h':

        val = 4;
        if (obj->team != 'w')
            val += 6;
        return val;
    case 'p':

        val = 5;
        if (obj->team != 'w')
            val += 6;
        return val;
    }
    return -1;
}
long long table[8][8][12];
long long table2[8][8][12];
void init_zobrist()
{
    // fill a table of random numbers/bitstrings

    for (int i = 0; i < 8; i++)
    {
        for (int e = 0; e < 8; e++)
        {
            for (int j = 0; j < 12; j++)
            {
                for (int k = 0; k < 16; k++)
                    table[i][e][j] += (rand() % 10) * pow(10, k);
            }
        }
    }
}
int compPiecePos(const void *p, const void *q)
{
    return (((piece *)p)->posx * 8 + ((piece *)p)->posy) - (((piece *)q)->posx * 8 + ((piece *)q)->posy);
}
long long hash(ArrayP *pieceMap)
{
    qsort(pieceMap->ptr, pieceMap->size, sizeof(piece), compPiecePos);
    long long h = 0;
    for (int i = 0; i < pieceMap->size; i++)
    {
        h = h ^ table[(pieceMap->ptr + i)->posx][(pieceMap->ptr + i)->posy][getPieceID((piece *)(pieceMap->ptr + i))];
    }
    return h;
}
void getPossibleMoves(Array *output, int color)
{ // Adds all possible moves to output array
    initBoardHashMap();
    for (int i = 0; i < board.size; i++)
    {                                         // iterates through all pieces on board
        piece *selPiece = getArr1(&board, i); // selected piece
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1))
        { // if the piece selected is a certain color
            switch (selPiece->type)
            {
            case 'k':
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1}, color);
                break;
            case 'q':
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'r':

                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'b':

                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'h':

                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2}, color);
                addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2}, color);
                break;
            case 'p':
                if (getPieceAtPosHash(selPiece->posx + 1, selPiece->posy + 1 * color) >= 0)
                    addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1 * color}, color);
                if (getPieceAtPosHash(selPiece->posx - 1, selPiece->posy + 1 * color) >= 0)
                    addNCheck(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1 * color}, color);
                if (
                    addNCheckCannotTake(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 * color}, color) == 1 && ((selPiece->posy == 1 && color == 1) || (selPiece->posy == 6 && color == -1)))
                    addNCheckCannotTake(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 2 * color}, color);
                break;
            default:
                printf("An Error Has Occured Type '%c' not reconized", selPiece->type);
            }
        }
    }
}

void getPossibleMovesPV(Array *output, int color)
{ // Adds all possible moves to output array
    initBoardHashMap();
    for (int i = 0; i < board.size; i++)
    {
        piece *selPiece = getArr1(&board, i);
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1))
        {
            switch (selPiece->type)
            {
            case 'k':
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1}, color);
                break;
            case 'q':
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'r':

                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'b':

                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++)
                {
                    if (addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'h':

                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2}, color);
                break;
            case 'p':

                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + color}, color);
                addNCheck_IFTAKE(output, (Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + color}, color);
                break;
            default:
                printf("An Error Has Occured Type '%c' not reconized", selPiece->type);
            }
        }
    }
}
int getNumPossibleMoves(int color)
{ // Gets number of Possible moves DO NOT USE IN EVALUATION   When used in eval it increases time complexity by 10x !
    int score = 0;
    for (int i = 0; i < board.size; i++)
    {
        piece *selPiece = getArr1(&board, i);
        int offset = 1;
        int done = 1;
        if ((selPiece->team == 'w' && color == 1) || (selPiece->team == 'b' && color == -1))
            switch (selPiece->type)
            {
            case 'k':
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 1}, color));
                break;
            case 'q':
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'r':

                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'b':

                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy + offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                for (offset = 1; offset < 8; offset++, score++)
                {
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - offset, selPiece->posy - offset}, color) != 1)
                        break;
                }
                break;
            case 'h':

                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy + 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 2, selPiece->posy - 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy + 2}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx + 1, selPiece->posy - 2}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy + 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 2, selPiece->posy - 1}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy + 2}, color));
                score += min(1, NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx - 1, selPiece->posy - 2}, color));
                break;
            case 'p':
                if (getPieceAtPosHash(selPiece->posx + 1, selPiece->posy + 1 * color) >= 0)
                    score++;
                if (getPieceAtPosHash(selPiece->posx - 1, selPiece->posy + 1 * color) >= 0)
                    score++;
                if (
                    NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 1 * color}, color) == 1)
                {
                    score++;
                    if (NCheck((Possiblemove){selPiece->posx, selPiece->posy, selPiece->posx, selPiece->posy + 2 * color}, color) == 1)
                        score++;
                }
                break;
            default:
                printf("An Error Has Occured Type '%c' not reconized", selPiece->type);
            }
    }
    return score;
}
typedef struct indexes
{
    int index1;
    int index2;
} indexes;
int compareIndexes(const void *a, const void *b)
{
    return (((indexes *)b)->index2 - ((indexes *)a)->index2);
}
void customHeuristic(Array *arr, indexes *output)
{ // It's a idea i thought based on the killer Heuristic where the moves is sorted based on simlar best moves based on simular depth

    for (int i = 0; i < arr->size; i++)
    {
        (output + i)->index1 = i;
        (output + i)->index2 = butterfly[(arr->ptr + i)->posx][(arr->ptr + i)->posy][(arr->ptr + i)->targetx][(arr->ptr + i)->targety];
    }
    qsort(output, arr->size, sizeof(indexes), compareIndexes);
}
void customHeuristicPV(Array *arr, indexes *output)
{ // It's a idea i thought based on the killer Heuristic where the moves is sorted based on simlar best moves based on simular depth

    for (int i = 0; i < arr->size; i++)
    {
        (output + i)->index1 = i;
        (output + i)->index2 = butterfly_PV[(arr->ptr + i)->posx][(arr->ptr + i)->posy][(arr->ptr + i)->targetx][(arr->ptr + i)->targety];
    }
    qsort(output, arr->size, sizeof(indexes), compareIndexes);
}
double eval(ArrayP *board)
{
    double pieceScore = 0;
    double mobilityScore = 0;
    //mobilityScore += getNumPossibleMoves(1) - getNumPossibleMoves(-1);

    for (int i = 0; i < board->size; i++)
    {
        if ((board->ptr + i)->team == 'w')
        {
            pieceScore += getPieceVal((board->ptr + i));
        }
        else
        {
            pieceScore -= getPieceVal((board->ptr + i));
        }
    }
    return pieceScore * 2 + mobilityScore;
}
double Quiesce(int depth, double alpha, double beta, int color)
{

    long long boardID = hash(&board);
    double alphaOrig = alpha;
    int stand_pat = eval(&board) * color;
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    Possiblemove localBestMove;
    struct Node *foundNodeAt;
    int val = -1000000;
    if (color == 1)
    {
        foundNodeAt = search(ttAble_PV, boardID);
    }
    else
        foundNodeAt = search(ttAble_PV2, boardID);
    if (foundNodeAt != NULL && (foundNodeAt)->depth >= depth)
    {

        if (foundNodeAt->flag == 'e')
        {

            return (foundNodeAt->value);
        }
        else if (foundNodeAt->flag == 'l')
        {
            alpha = max(alpha, (foundNodeAt->value));
        }
        else if (foundNodeAt->flag == 'u')
        {
            beta = min(beta, (foundNodeAt->value));
        }
        if (beta < alpha)
        {
            return (foundNodeAt->value);
        }
    }
    if (depth <= 0 || checkKing() < 2)
    {
        return stand_pat;
    }
    Array posMoves;
    initArr(&posMoves, 40);
    getPossibleMovesPV(&posMoves, color);
    if (posMoves.size == 0)
    {
        return stand_pat;
    }
    indexes *sortedArr = calloc(posMoves.size, sizeof(indexes));
    customHeuristicPV(&posMoves, sortedArr);

    for (int i = 0; i < posMoves.size; i++)
    {

        initBoardHashMap();
        movePiece(getPieceAtPosHash((posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy), (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety);

        initBoardHashMap();
        //double value = -negaMax(depth - 1, -beta, -alpha, -color);
        double value = -Quiesce(depth - 1, -beta, -alpha, -color);

        if (val > alpha)
            alpha = val;
        if (value > val)
        {
            localBestMove = (Possiblemove){(posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy, (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety};

            val = value;
        }

        if (takeBack() != 1 && alpha > beta)
        {
            butterfly_PV[(posMoves.ptr + (sortedArr + i)->index1)->posx][(posMoves.ptr + (sortedArr + i)->index1)->posy][(posMoves.ptr + (sortedArr + i)->index1)->targetx][(posMoves.ptr + (sortedArr + i)->index1)->targety] += depth * depth;
        }
        if (stand_pat >= beta)
        {
            free(posMoves.ptr);
            free(sortedArr);
            return beta;
        }
        if (alpha < stand_pat)
            alpha = stand_pat;
        if (alpha >= beta)
        {

            break;
        }
    }

    if (foundNodeAt == NULL)
    {
        char flag;
        if (val <= alphaOrig)
        {
            flag = 'u';
        }
        else if (val > beta)
        {
            flag = 'l';
        }
        else
        {
            flag = 'e';
        }

        if (color == 1)
        {
            ttAble_PV = insert(ttAble_PV, (nodeValue){val, boardID, depth, flag}, &localBestMove);
        }
        else
            ttAble_PV2 = insert(ttAble_PV2, (nodeValue){val, boardID, depth, flag}, &localBestMove);
    }
    else
    {
        if ((foundNodeAt)->depth < depth)
        {

            if (val <= alphaOrig)
            {
                foundNodeAt->flag = 'u';
            }
            else if (val > beta)
            {
                foundNodeAt->flag = 'l';
            }
            else
            {
                foundNodeAt->flag = 'e';
            }
            foundNodeAt->depth = depth;
            foundNodeAt->value = val;
        }
    }
    free(posMoves.ptr);
    free(sortedArr);
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
int negaMax(int depth, int alpha, int beta, int color)
{

    long long boardID = hash(&board);
    double alphaOrig = alpha;

    Possiblemove localBestMove;
    struct Node *foundNodeAt;
    int val = -1000000;
    if (color == 1)
    {
        foundNodeAt = search(ttAble, boardID);
    }
    else
        foundNodeAt = search(ttAble2, boardID);
    if (foundNodeAt != NULL && (foundNodeAt)->depth >= depth)
    {

        if (foundNodeAt->flag == 'e')
        {

            return (foundNodeAt->value);
        }
        else if (foundNodeAt->flag == 'l')
        {
            alpha = max(alpha, (foundNodeAt->value));
        }
        else if (foundNodeAt->flag == 'u')
        {
            beta = min(beta, (foundNodeAt->value));
        }
        if (beta < alpha)
        {
            return (foundNodeAt->value);
        }
    }
    if (depth <= 0 || checkKing() < 2)
    {
        return Quiesce(5, alpha, beta, color);
    }
    Array posMoves;
    initArr(&posMoves, 40);
    getPossibleMoves(&posMoves, color);
    indexes *sortedArr = calloc(posMoves.size, sizeof(indexes));
    customHeuristic(&posMoves, sortedArr);
    int b;
    for (int i = 0; i < posMoves.size; i++)
    {

        initBoardHashMap();
        movePiece(getPieceAtPosHash((posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy), (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety);

        initBoardHashMap();
        int value = -negaMax(depth - 1, -beta, -alpha, -color);
        //double value = -negaMax(depth - 1, -beta, -alpha, -color);
        if (value > val)
        {
            localBestMove = (Possiblemove){(posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy, (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety};

            val = value;
        }
        alpha = max(val, alpha);

        if (takeBack() != 1 && alpha > beta)
        {
            butterfly[(posMoves.ptr + (sortedArr + i)->index1)->posx][(posMoves.ptr + (sortedArr + i)->index1)->posy][(posMoves.ptr + (sortedArr + i)->index1)->targetx][(posMoves.ptr + (sortedArr + i)->index1)->targety] += depth * depth;
        }
        if (alpha >= beta)
        {

            break;
        }
        b = alpha + 1;
    }

    if (foundNodeAt == NULL)
    {
        char flag;
        if (val <= alphaOrig)
        {
            flag = 'u';
        }
        else if (val >= beta)
        {
            flag = 'l';
        }
        else
        {
            flag = 'e';
        }

        if (color == 1)
        {
            ttAble = insert(ttAble, (nodeValue){val, boardID, depth, flag}, &localBestMove);
        }
        else
            ttAble2 = insert(ttAble2, (nodeValue){val, boardID, depth, flag}, &localBestMove);
    }
    else
    {
        if ((foundNodeAt)->depth < depth)
        {

            if (val <= alphaOrig)
            {
                foundNodeAt->flag = 'u';
            }
            else if (val >= beta)
            {
                foundNodeAt->flag = 'l';
            }
            else
            {
                foundNodeAt->flag = 'e';
            }
            foundNodeAt->depth = depth;
            foundNodeAt->value = val;
        }
    }
    free(posMoves.ptr);
    free(sortedArr);
    return val;
} // printf("%1.d %% Done!\n", (int)((double)i / (double)posMoves.size * 100));

int globalval = -1000000;
double negaMax1(int depth, int alpha, int beta, int color)
{

    long long boardID = hash(&board);
    double alphaOrig = alpha;

    Possiblemove localBestMove;
    struct Node *foundNodeAt;
    if (color == 1)
    {
        foundNodeAt = search(ttAble, boardID);
    }
    else
        foundNodeAt = search(ttAble2, boardID);
    if (foundNodeAt != NULL && (foundNodeAt)->depth >= depth)
    {

        if (foundNodeAt->flag == 'e')
        {
            if (globalval < foundNodeAt->value)
            {
                globalval = foundNodeAt->value;
                bestMove = foundNodeAt->move;
            }

            return (foundNodeAt->value);
        }
        else if (foundNodeAt->flag == 'l')
        {
            alpha = max(alpha, (foundNodeAt->value));
        }
        else if (foundNodeAt->flag == 'u')
        {
            beta = min(beta, (foundNodeAt->value));
        }
    }
    if (depth <= 0 || checkKing() < 2)
    {
        return Quiesce(5, alpha, beta, color);
    }
    int val = -10000000;
    Array posMoves;
    initArr(&posMoves, 40);
    getPossibleMoves(&posMoves, color);
    indexes *sortedArr = calloc(posMoves.size, sizeof(indexes));
    customHeuristic(&posMoves, sortedArr);
    int b;
    for (int i = 0; i < posMoves.size; i++)
    {

        initBoardHashMap();
        movePiece(getPieceAtPosHash((posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy), (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety);

        initBoardHashMap();
        int value = -negaMax(depth - 1, -beta, -alpha, -color);
        //double value = -negaMax(depth - 1, -beta, -alpha, -color);
        if (value > globalval)
        {
            globalval = value;
            bestMove = (Possiblemove){(posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy, (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety};
        }
        val = max(val, value);
        alpha = max(val, alpha);

        if (takeBack() != 1 && alpha > beta)
        {
            butterfly[(posMoves.ptr + (sortedArr + i)->index1)->posx][(posMoves.ptr + (sortedArr + i)->index1)->posy][(posMoves.ptr + (sortedArr + i)->index1)->targetx][(posMoves.ptr + (sortedArr + i)->index1)->targety] += depth * depth;
        }
        if (alpha >= beta)
        {
            bestMove = (Possiblemove){(posMoves.ptr + (sortedArr + i)->index1)->posx, (posMoves.ptr + (sortedArr + i)->index1)->posy, (posMoves.ptr + (sortedArr + i)->index1)->targetx, (posMoves.ptr + (sortedArr + i)->index1)->targety};

            break;
        }
        b = alpha + 1;
    }
    if (depth > 2)
        if (foundNodeAt == NULL)
        {
            char flag;
            if (val <= alphaOrig)
            {
                flag = 'u';
            }
            else if (val >= beta)
            {
                flag = 'l';
            }
            else
            {
                flag = 'e';
            }

            if (color == 1)
            {
                ttAble = insert(ttAble, (nodeValue){val, boardID, depth, flag}, &localBestMove);
            }
            else
                ttAble2 = insert(ttAble2, (nodeValue){val, boardID, depth, flag}, &localBestMove);
        }
        else
        {
            if ((foundNodeAt)->depth < depth)
            {

                if (val <= alphaOrig)
                {
                    foundNodeAt->flag = 'u';
                }
                else if (val >= beta)
                {
                    foundNodeAt->flag = 'l';
                }
                else
                {
                    foundNodeAt->flag = 'e';
                }
                foundNodeAt->depth = depth;
                foundNodeAt->value = val;
            }
        }
    free(posMoves.ptr);
    free(sortedArr);
    return val;
}
int mtdf(int f, int depth, int color)
{
    int bound[2] = {-INFINITY, +INFINITY}; // lower, upper
    do
    {
        int beta = f + (f == bound[0]);
        f = negaMax1(depth, beta - 1, beta, color);
        bound[f < beta] = f;
    } while (bound[0] < bound[1]);
    return f;
}
int iterative_deepening(time_t timet, int color)
{
    time_t start = time(NULL);
    int depth = 1;
    int firstguess = 0;
    while (start + timet >= time(NULL))
    {
        printf("DEEPENING AT %d\n", depth);
        firstguess = mtdf(firstguess, depth, color);
        depth++;
    }
    return firstguess;
}
// Prints out the Board in Console
void refreshConsole(int turn)
{

    //system("CLS"); // uncomment if you want it to refresh every time
    char can;
    int e = 0;

    if (turn != 1)
    {
        for (e = 7; e >= -1; e--)
        { //Y

            if (e == -1)
            {
                printf("%s \n", "   _______________");
                printf("%s \n", "   A B C D E F G H");
            }
            else
                for (int i = 8; i >= 0; i--) //X

                    if (i == 8)
                    {
                        printf("%d| ", e + 1);
                    }
                    else
                    {

                        int j;
                        /*for (j = 0; j < board.size; j++)
                        {
                            if (((int)(getArr1(&board, j)->type)) > 64 && ((int)(getArr1(&board, j)->type)) < 123 && (e == getArr1(&board, j)->posy && i == getArr1(&board, j)->posx))
                            {
                                //printf(" (%d , %d) ", i, e);
                                if ((int)getArr1(&board, j)->team == (int)'w')
                                {
                                    printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, j)->type));
                                }
                                else
                                {
                                    printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, j)->type));
                                }

                                break;
                            }
                        }*/
                        if (boardHashMap[i][e] == -1)
                        {
                            printf("\x1b[0m\x1b[37m+ ");
                        }
                        else if ((int)getArr1(&board, boardHashMap[i][e])->team == (int)'w')
                        {
                            printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, boardHashMap[i][e])->type));
                        }
                        else
                        {
                            printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, boardHashMap[i][e])->type));
                        }

                        //if (j == board.size)
                        /*{
                            //printf(" (%d , %d) ", i, e);
                            printf("\x1b[0m\x1b[37m+ ");
                        }*/
                    }
            printf("\n");
        }
    }
    else
        for (e = 0; e <= 8; e++)
        { //Y

            if (e == 8)
            {
                printf("%s \n", "   _______________");
                printf("%s \n", "   H G F E D C B A");
            }
            else
                for (int i = -1; i < 8; i++) //X

                    if (i == -1)
                    {
                        printf("%d| ", e + 1);
                    }
                    else
                    {
                        /*
                        can = 'f';
                        for (int j = 0; j < board.size; j++)
                        {
                            if (((int)(getArr1(&board, j)->type)) > 64 && ((int)(getArr1(&board, j)->type)) < 123 && (e == getArr1(&board, j)->posy && i == getArr1(&board, j)->posx))
                            {

                                if ((int)getArr1(&board, j)->team == (int)'w')
                                {

                                    printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, j)->type));
                                }
                                else
                                {
                                    printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, j)->type));
                                }
                                can = 't';
                                break;
                            }
                        }
                        if (can != 't')
                        {
                            printf("\x1b[0m\x1b[37m+ ");
                        }*/

                        if (boardHashMap[i][e] == -1)
                        {
                            printf("\x1b[0m\x1b[37m+ ");
                        }
                        else if ((int)getArr1(&board, boardHashMap[i][e])->team == (int)'w')
                        {
                            printf("\x1b[1m\x1b[97m%c \x1b[0m", (getArr1(&board, boardHashMap[i][e])->type));
                        }
                        else
                        {
                            printf("\x1b[1m\x1b[90m%c \x1b[0m", (getArr1(&board, boardHashMap[i][e])->type));
                        }
                    }
            printf("\n");
        }
}
//useless lol
void copyArr(piece *target, piece *dest)
{

    for (int i = 0; i < 32; i++)
    {
        (*(dest + i)) = (piece){(*(target + i)).posx, (*(target + i)).posy, (*(target + i)).type, (*(target + i)).team};
    }
}
int main()
{
    srand((unsigned)time(0));

    //initNodeArrMod(&nodeArray,10000000);
    // initialize the variable sizes
    initArr1(&board, 32);
    init_zobrist();
    initArrList(&movesDone, 0);
    board.size = 32;
    //adds the pieces

    for (int i = 0; i < 8; i++)
        (*(board.ptr + i)) = (piece){
            i, 6, 'p', 'b'};
    for (int i = 8; i < 16; i++)
        (*(board.ptr + i)) = (piece){
            i - 8, 1, 'p', 'w'};
    (*(board.ptr + 17)) = (piece){
        1, 0, 'h', 'w'};
    (*(board.ptr + 18)) = (piece){
        6, 0, 'h', 'w'};
    (*(board.ptr + 19)) = (piece){
        0, 0, 'r', 'w'};
    (*(board.ptr + 20)) = (piece){
        2, 0, 'b', 'w'};
    (*(board.ptr + 21)) = (piece){
        3, 0, 'k', 'w'};
    (*(board.ptr + 22)) = (piece){
        4, 0, 'q', 'w'};
    (*(board.ptr + 23)) = (piece){
        5, 0, 'b', 'w'};
    (*(board.ptr + 24)) = (piece){
        7, 0, 'r', 'w'};

    (*(board.ptr + 25)) = (piece){
        0, 7, 'r', 'b'};

    (*(board.ptr + 26)) = (piece){
        1, 7, 'h', 'b'};
    (*(board.ptr + 27)) = (piece){
        2, 7, 'b', 'b'};
    (*(board.ptr + 28)) = (piece){
        3, 7, 'k', 'b'};
    (*(board.ptr + 29)) = (piece){
        4, 7, 'q', 'b'};
    (*(board.ptr + 30)) = (piece){
        5, 7, 'b', 'b'};
    (*(board.ptr + 31)) = (piece){
        6, 7, 'h', 'b'};
    (*(board.ptr + 16)) = (piece){
        7, 7, 'r', 'b'};

    initBoardHashMap();
    // ttAble = insert(ttAble, (nodeValue){0, hash(&board), 0, 'e'}, 0);
    // prints current state of board
    refreshConsole(-1);

    int turn = 1; // Who's turn to move. 1 for white -1 for black
    char cat[5];  // input from console
    while (1 == 1)
    {
        // gets input into cat
        scanf("%s", &cat);

        //if user puts a word with ev at start it will print out evaluations of the board state
        //if user puts a word with ex at start it will exit
        if (cat[0] == 'e')
            if (cat[1] == 'v')
            {
                targetDepth = 14;
                printf("eval : %f\n ID : %lld \n", eval(&board), hash(&board));
            }
            else if (cat[1] == 'x')
                return 0;

        // if user puts word bo at start then it will let the bot make it's move
        if (cat[0] == 'p' && cat[1] == 'm')
        {

            int pickedPosX = 7 - ((int)cat[2] - 65) % 32;
            int pickedPosY = (int)cat[3] - 49;

            Array possMoves;
            possMoves.ptr = malloc(0);
            possMoves.size = 0;

            possMoves.realSize = 0;
            getPossibleMoves(&possMoves, turn);
            int i;

            for (i = 0; i < possMoves.size; i++)
            { // searches for the move specified by the Human and is in the list of possible moves
                Possiblemove *point = getArr(&possMoves, i);
                if (point->posx == pickedPosX && point->posy == pickedPosY)
                {

                    printf("%c , %c to %c , %d\n", cat[2], cat[3], (7 - point->targetx) + 65, point->targety + 1);
                }
            }
        }
        else if (cat[0] == 'b' && cat[1] == 'o')
        {
            for (int from = 0; from < 8; from++)
                for (int to = 0; to < 8; to++)
                    for (int from1 = 0; from1 < 8; from1++)
                        for (int to1 = 0; to1 < 8; to1++)
                            butterfly[from][to][from1][to1] = 0;

            for (int from = 0; from < 8; from++)
                for (int to = 0; to < 8; to++)
                    for (int from1 = 0; from1 < 8; from1++)
                        for (int to1 = 0; to1 < 8; to1++)
                            butterfly_PV[from][to][from1][to1] = 0;
            bestValue = -10000000;
            long seconds = time(NULL);
            // gets the value of the board and best move is put in the Variable bestMove
            double val = iterative_deepening(1, turn);

            printf("Took %ld seconds\n%dNodes Recorded\nBest Move is %c%d  to %c%d \n ", time(0) - seconds, 0, ((7 - bestMove.posx) + 65), bestMove.posy + 1, (7 - bestMove.targetx) + 65, bestMove.targety + 1);
            printf("Bot confidence : %f\n", val);

            Array possMoves;
            possMoves.ptr = malloc(0);
            possMoves.size = 0;

            possMoves.realSize = 0;
            getPossibleMoves(&possMoves, turn);
            int i;

            for (i = 0; i < possMoves.size; i++)
            { // searches for the move specified by the Human and is in the list of possible moves
                Possiblemove *point = getArr(&possMoves, i);
                if (point->posx == bestMove.posx && point->posy == bestMove.posy && point->targetx == bestMove.targetx && point->targety == bestMove.targety)
                {

                    break;
                }
            }

            if (i < possMoves.size)
            { // if  the move is specified by the Human and is in the list of possible moves

                movePiece(getPieceAtPosHash(bestMove.posx, bestMove.posy), bestMove.targetx, bestMove.targety);
                printf("\nTurn: %d\n", movesDone.size);
                turn *= -1;
            }
            else
                printf("\nInvalid Move Try Again\n");

            freeArr(&possMoves);
            initBoardHashMap();
            refreshConsole(-turn);
        }
        else // reverses a move
            if (cat[0] == 'b' && cat[1] == 'a')
        {
            takeBack();
            refreshConsole(turn);
            turn *= -1;
        }
        else
        { // HUMAN MOVES
            // converts letters into array indexes
            int pickedPosX = 7 - ((int)cat[0] - 65) % 32;
            int pickedPosY = (int)cat[1] - 49;
            int canMove = getPieceAtPos(pickedPosX, pickedPosY);
            if (canMove >= 0 && ((getArr1(&board, canMove)->team == 'w' && turn > 0) || (getArr1(&board, canMove)->team != 'w' && turn < 0)))
            { // if they can move there
                Array possMoves;
                possMoves.ptr = malloc(0);
                possMoves.size = 0;

                possMoves.realSize = 0;
                getPossibleMoves(&possMoves, turn);
                int i;

                for (i = 0; i < possMoves.size; i++)
                { // searches for the move specified by the Human and is in the list of possible moves
                    Possiblemove *point = getArr(&possMoves, i);
                    if (point->posx == pickedPosX && point->posy == pickedPosY && point->targetx == (7 - ((int)cat[2] - 65) % 32) && point->targety == ((int)cat[3] - 49))
                    {

                        break;
                    }
                }

                if (i < possMoves.size)
                { // if  the move is specified by the Human and is in the list of possible moves

                    movePiece(canMove, 7 - (((int)cat[2] - 65) % 32), (int)cat[3] - 49);

                    printf("%c, %d  to %c , %d   Real : %d , %d\n", 7 - pickedPosX + 65, pickedPosY + 1, cat[2], (int)cat[3] - 49, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posx, getArr1(&board, getPieceAtPos((((int)cat[2] - 65) % 32), (int)cat[3] - 49))->posy);

                    printf("\nTurn: %d\n", movesDone.size);
                    turn *= -1;
                    refreshConsole(-turn);
                }
                else
                    printf("\nInvalid Move Try Again\n");

                freeArr(&possMoves);
            }
            else
            {
                printf("\nInvalid Move Try Again\n");
            }
        } /*
            printf("Looking at %d\n" , movesDone.size-1);
            for (int i = 0; i < movesDone.size; i++) {
                move* look = (movesDone.ptr + i);
                printf(" %d : %d %d %d %d\n",i, look->posx, look->posx, look->targetx, look->targety);
            }*/
    }
    return 0;
}
