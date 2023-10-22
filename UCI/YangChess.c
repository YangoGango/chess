#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#define MAX_MOVES 256
const char* ENGINE_NAME = "YangChess Engine";
const char* AUTHOR = "Yi Yang Hsu";



typedef unsigned long long bitboard;

#define BIT(sq) (1L << (sq))
#define RF(rank, file) ((rank) * 8 + (file))

#define LSB(x) (__builtin_ctzll(x))
#define MSB(x) (__builtin_clzll(x))
#define BITS(x) (__builtin_popcountll(x))

#define POP_LSB(b, x) b = LSB(x); x &= ~BIT(b);
#define POP_MSB(b, x) b = MSB(x); x &= ~BIT(b);
#define PIECE(x) ((x) & 0x0f) 
#define COLOR(x) ((x) & 0x10)

#define PAWN 0x01
#define KNIGHT 0x02
#define BISHOP 0x03
#define ROOK 0x04
#define QUEEN 0x05
#define KING 0x06

#define RANK_1 0x00000000000000ffL
#define RANK_2 0x000000000000ff00L
#define RANK_3 0x0000000000ff0000L
#define RANK_4 0x00000000ff000000L
#define RANK_5 0x000000ff00000000L
#define RANK_6 0x0000ff0000000000L
#define RANK_7 0x00ff000000000000L
#define RANK_8 0xff00000000000000L

#define FILE_A 0x0101010101010101L
#define FILE_B 0x0202020202020202L
#define FILE_C 0x0404040404040404L
#define FILE_D 0x0808080808080808L
#define FILE_E 0x1010101010101010L
#define FILE_F 0x2020202020202020L
#define FILE_G 0x4040404040404040L
#define FILE_H 0x8080808080808080L

#define BOARD_SIZE 0x00
#define WHITE 0x00
#define BLACK 0x10
#define EMPTY 0x00
#define WHITE_PAWN 0x01
#define WHITE_KNIGHT 0x02
#define WHITE_BISHOP 0x03
#define WHITE_ROOK 0x04
#define WHITE_QUEEN 0x05
#define WHITE_KING 0x06

#define BLACK_PAWN 0x11
#define BLACK_KNIGHT 0x12
#define BLACK_BISHOP 0x13
#define BLACK_ROOK 0x14
#define BLACK_QUEEN 0x15
#define BLACK_KING 0x16


// Precalculated attack squares
bitboard bbKnight[64];
bitboard bbKing[64];


int debug = 0;

typedef struct Move
{
    unsigned char from;
    unsigned char to;
    unsigned char promote;

} Move;


typedef struct Board
{
    int moveNum ;
    Move moves[6000];
    int tile[64];
    bitboard all;
    bitboard white;
    bitboard black;
    bitboard white_pawns;
    bitboard black_pawns;
    bitboard white_knights;
    bitboard black_knights;
    bitboard white_bishops;
    bitboard black_bishops;
    bitboard white_rooks;
    bitboard black_rooks;
    bitboard white_queens;
    bitboard black_queens;
    bitboard white_kings;
    bitboard black_kings;
    bitboard hash;
    bitboard pawn_hash;
    int white_material;
    int black_material;
    int castle;
} Board;

//Initialize BB Values

bitboard bb_knight(int row , int file){
    const int offsets[8][2] = {
        {2,1},
        {-2,1},
        {-2,-1},
        {2,-1}, 
         
        {1,2},
        {-1,2},
        {1,-2}, 
        {-1,-2}
    };
    bitboard mask= 0;
    for(int i = 0 ; i < 8; i++){
        if( row + offsets[i][0]>= 0 && file+ offsets[i][1] >=0 && row + offsets[i][0]<8 && file+ offsets[i][1] <8 ){
            mask |= BIT(RF(row + offsets[i][0], file+ offsets[i][1])) ;
     
        }
    }
    return mask;
}
bitboard bb_king(int row , int file){
    const int offsets[8][2] = {
        {1,1},
        {0,1},
        {1,0},
        {-1,-1},
        {0,-1},
        {-1,0},
        {1,-1},
        {-1,1},
    };
    bitboard mask= 0;
    for(int i = 0 ; i < 8; i++){
        if( row + offsets[i][0]>= 0 && file+ offsets[i][1] >=0 && row + offsets[i][0]<8 && file+ offsets[i][1] <8 ){
            mask |= BIT(RF(row + offsets[i][0], file+ offsets[i][1])) ;
     
        }
    }
    
    
    return mask;
}
bitboard bb_rook(int row , int file){
    const int offsets[8][2] = {
        {-1,0},
        {0,1},
        {1,0},
        {0,-1},
        
    };
    bitboard mask= 0;
    for(int i = 0 ; i < 8; i++){
        if( row + offsets[i][0]>= 0 && file+ offsets[i][1] >=0 && row + offsets[i][0]<8 && file+ offsets[i][1] <8 ){
            mask |= BIT(RF(row + offsets[i][0], file+ offsets[i][1])) ;
     
        }
    }
    return mask;
}
void bb_init(){
    for (int file = 0; file < 8; file++)
    {
        for(int row = 0; row < 8; row++){
            bbKnight[RF(row,file)]= bb_knight(row,file);
        }
    }
    for (int file = 0; file < 8; file++)
    {
        for(int row = 0; row < 8; row++){
            bbKing[RF(row,file)]= bb_king(row,file);
        }
    }
    
}





void board_clear(Board *board) {
    memset(board, 0, sizeof(Board));
    board->castle = 0;
}









void setBoardPos(Board* board, int pos , int piece){
    int previous = board->tile[pos];
    board->tile[pos] = piece;
    if(previous){
        bitboard mask = ~BIT(pos); // set mask to everything not in the square
        board->all &=  mask; // rid of everything not in mask
        if(COLOR(previous)){// IF PREVIOUS IS BLACK
            
            board->black &= mask;// Rid of Piece from Black board
            //Update values
            switch(PIECE(previous)){
                case PAWN:
                    board->black_pawns &= mask;
                    board->black_material -=100;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case BISHOP:
                    board->black_bishops &= mask;
                    board->black_material -=350;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case ROOK:
                    board->black_rooks &= mask;
                    board->black_material -=500;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KNIGHT:
                    board->black_knights &= mask;
                    board->black_material -=300;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case QUEEN:
                    board->black_pawns &= mask;
                    board->black_material -=800;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KING:
                    board->black_pawns &= mask;
                    board->black_material -=2000;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
            }
        }else{
            board->white &= mask;// Rid of Piece from white board
            //Update values
            switch(PIECE(previous)){
                case PAWN:
                    board->white_pawns &= mask;
                    board->white_material -=100;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case BISHOP:
                    board->white_bishops &= mask;
                    board->white_material -=350;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case ROOK:
                    board->white_rooks &= mask;
                    board->white_material -=500;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KNIGHT:
                    board->white_knights &= mask;
                    board->white_material -=300;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case QUEEN:
                    board->white_pawns &= mask;
                    board->white_material -=800;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KING:
                    board->white_pawns &= mask;
                    board->white_material -=2000;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
            }
        }
    }
    if(piece){
        
        bitboard mask = BIT(pos); // set mask to the square to place piece in 
        board->all |=  mask; // add piece to board
        if(COLOR(piece)){// IF PIECE IS BLACK
            
            board->black |= mask;// Rid of Piece from Black board
            //Update values
            switch(PIECE(previous)){
                case PAWN:
                    board->black_pawns |= mask;
                    board->black_material +=100;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case BISHOP:
                    board->black_bishops |= mask;
                    board->black_material +=350;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case ROOK:
                    board->black_rooks |= mask;
                    board->black_material +=500;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KNIGHT:
                    board->black_knights |= mask;
                    board->black_material +=300;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case QUEEN:
                    board->black_pawns |= mask;
                    board->black_material +=800;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KING:
                    board->black_pawns |= mask;
                    board->black_material +=2000;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
            }
        }else{
            board->white |= mask;// Rid of Piece from white board
            //Update values
            switch(PIECE(previous)){
                case PAWN:
                    board->white_pawns |= mask;
                    board->white_material +=100;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case BISHOP:
                    board->white_bishops |= mask;
                    board->white_material +=350;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case ROOK:
                    board->white_rooks |= mask;
                    board->white_material +=500;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KNIGHT:
                    board->white_knights |= mask;
                    board->white_material +=300;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case QUEEN:
                    board->white_pawns |= mask;
                    board->white_material +=800;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
                case KING:
                    board->white_pawns |= mask;
                    board->white_material +=2000;
                    //TODO : Add butterfly maps and zobrist hashing
                    break;
            }
        }
    }
}

void initBoard(Board *board) {
    board_clear(board);
    for (int file = 0; file < 8; file++) {
        setBoardPos(board, RF(1, file), WHITE_PAWN);
        setBoardPos(board, RF(6, file), BLACK_PAWN);
    }
    setBoardPos(board, RF(0, 0), WHITE_ROOK);
    setBoardPos(board, RF(0, 1), WHITE_KNIGHT);
    setBoardPos(board, RF(0, 2), WHITE_BISHOP);
    setBoardPos(board, RF(0, 3), WHITE_QUEEN);
    setBoardPos(board, RF(0, 4), WHITE_KING);
    setBoardPos(board, RF(0, 5), WHITE_BISHOP);
    setBoardPos(board, RF(0, 6), WHITE_KNIGHT);
    setBoardPos(board, RF(0, 7), WHITE_ROOK);
    setBoardPos(board, RF(7, 0), BLACK_ROOK);
    setBoardPos(board, RF(7, 1), BLACK_KNIGHT);
    setBoardPos(board, RF(7, 2), BLACK_BISHOP);
    setBoardPos(board, RF(7, 3), BLACK_QUEEN);
    setBoardPos(board, RF(7, 4), BLACK_KING);
    setBoardPos(board, RF(7, 5), BLACK_BISHOP);
    setBoardPos(board, RF(7, 6), BLACK_KNIGHT);
    setBoardPos(board, RF(7, 7), BLACK_ROOK);
}
void printBoard(Board *board) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            char c;
            int piece = board->tile[RF(rank, file)];
            switch (PIECE(piece)) {
                case EMPTY:  c = '.'; break;
                case PAWN:   c = 'P'; break;
                case KNIGHT: c = 'N'; break;
                case BISHOP: c = 'B'; break;
                case ROOK:   c = 'R'; break;
                case QUEEN:  c = 'Q'; break;
                case KING:   c = 'K'; break;
            };
            if (COLOR(piece)) {
                c |= 0x20;
            }
            putchar(c);
        }
        putchar('\n');
    }
    putchar('\n');
}
int addMove(Move* src , unsigned char from, unsigned char to , unsigned char promote){
    src->from = from;
    src->to = to;
    src->promote = promote;
    return 1;// One for how many possible moves you just added to the array.... which is one
}


// MOVE GENERATION

int genMoves(){

}















//String TOOLS
int stringEquals(char* str , char* arr){
    int length = min(strlen(str) , strlen(arr) );
    for(int index = 0 ; index < length ; index++ ){
        if(*(arr+index) != *(str+index))
            return 0;
    }
    return 1;
}
char *strip(char *str) {
    while (isspace(*str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }
    *(end + 1) = 0;
    return str;
}

int sendUCIDebugString(char* str){
    if(!str)
        return 0;
    
    printf("info string %s\n", str);
}










int freeMemory(){

}

int main (){

    char cNL[1024];
    Board* board ;
    initBoard(board);
    setbuf(stdout, NULL);
    while(fgets( cNL , 1024 , stdin)){
        char* line =strip(cNL);
        if(strcmp(line, "uci") == 0){
            printf("id name %s\nid author %s\nuciok\n",ENGINE_NAME,AUTHOR);
        }
        if(strcmp(line, "printBoard") == 0){
            printBoard(board);
        }
        if(strcmp(line, "quit") == 0){
            
            return 1;
        }
        if(strcmp(line, "isready") == 0){
            printf("readyok\n");
        }
    }
    
    return 1;
}

