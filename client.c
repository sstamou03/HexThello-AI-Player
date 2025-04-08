#include "global.h"
#include "board.h"
#include "move.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

/**********************************************************/
Position gamePosition;      // Position we are going to use

Move moveReceived;          // Temporary move to retrieve opponent's choice
Move myMove;                // move to save our choice and send it to the server

char myColor;               // To store our color
int mySocket;               // Our socket
char msg;                   // Used to store the received message

char * agentName = "chillGuy";  // default name.. change it! keep in mind MAX_NAME_LENGTH
//char *agentName2 = "chillGuyAB";
char * ip = "127.0.0.1";        // Default IP (local machine)

int searchDepth = 3;        // Depth of Minimax search
int useMinimax = 1;         // 0: Random player, 1: Minimax player, 2: AB player
/**********************************************************/

/**********************************************************/
// Evaluation function to assess board state
int evalfunc(const Position *pos, char player) {
    return pos->score[player] - pos->score[getOtherSide(player)];
}
/**********************************************************/

/**********************************************************/
// Minimax algorithm 
int minimax(Position *pos, int depth, char activePlayer, int isMaximizing) {
    if (depth == 0 || !canMove(pos, activePlayer)) {
        return evalfunc(pos, myColor);
    }

    int extremeValue = isMaximizing ? INT_MIN : INT_MAX;
    Position tempPos;
    Move validMoves[ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE];
    int moveCount = 0;

    for (int row = 0; row < ARRAY_BOARD_SIZE; row++) {
        for (int col = 0; col < ARRAY_BOARD_SIZE; col++) {
            Move move = { .tile = {row, col}, .color = activePlayer };
            if (isLegalMove(pos, &move)) {
                validMoves[moveCount++] = move;
            }
        }
    }

    
    for (int i = 0; i < moveCount; i++) {
        tempPos = *pos;
        doMove(&tempPos, &validMoves[i]);
        int moveEval = minimax(&tempPos, depth - 1, getOtherSide(activePlayer), !isMaximizing);
        extremeValue = isMaximizing ? (moveEval > extremeValue ? moveEval : extremeValue) 
                                    : (moveEval < extremeValue ? moveEval : extremeValue);
    }
    return extremeValue;
}

// best move using Minimax
Move findOptimalMove(Position *pos, char player) {
    Move bestMove;
    int bestValue = INT_MIN;
    Position futurePos;
    Move validMoves[ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE];
    int moveCount = 0;

    
    for (int row = 0; row < ARRAY_BOARD_SIZE; row++) {
        for (int col = 0; col < ARRAY_BOARD_SIZE; col++) {
            Move move = { .tile = {row, col}, .color = player };
            if (isLegalMove(pos, &move)) {
                validMoves[moveCount++] = move;
            }
        }
    }

    
    for (int i = 0; i < moveCount; i++) {
        futurePos = *pos;
        doMove(&futurePos, &validMoves[i]);
        int eval = minimax(&futurePos, searchDepth - 1, getOtherSide(player), 0);
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = validMoves[i];
        }
    }
    return bestMove;
}

/**********************************************************/
// AB pruning
int minimaxAB(Position *pos, int depth, char activePlayer, int isMaximizing, int alpha, int beta) {
    if (depth == 0 || !canMove(pos, activePlayer)) {
        return evalfunc(pos, myColor);
    }

    int extremeValue = isMaximizing ? INT_MIN : INT_MAX;
    Position tempPos;
    Move validMoves[ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE];
    int moveCount = 0;

    for (int row = 0; row < ARRAY_BOARD_SIZE; row++) {
        for (int col = 0; col < ARRAY_BOARD_SIZE; col++) {
            Move move = { .tile = {row, col}, .color = activePlayer };
            if (isLegalMove(pos, &move)) {
                validMoves[moveCount++] = move;
            }
        }
    }

    for (int i = 0; i < moveCount; i++) {
        tempPos = *pos;
        doMove(&tempPos, &validMoves[i]);
        int moveEval = minimaxAB(&tempPos, depth - 1, getOtherSide(activePlayer), !isMaximizing, alpha, beta);

        if (isMaximizing) {
            extremeValue = (moveEval > extremeValue) ? moveEval : extremeValue;
            alpha = (alpha > extremeValue) ? alpha : extremeValue;
        } else {
            extremeValue = (moveEval < extremeValue) ? moveEval : extremeValue;
            beta = (beta < extremeValue) ? beta : extremeValue;
        }

        if (beta <= alpha) break;  
    }
    return extremeValue;
}
// best move using AB pruning
Move findOptimalMoveAB(Position *pos, char player) {
    Move bestMove;
    int bestValue = INT_MIN;
    Position futurePos;
    Move validMoves[ARRAY_BOARD_SIZE * ARRAY_BOARD_SIZE];
    int moveCount = 0;

    for (int row = 0; row < ARRAY_BOARD_SIZE; row++) {
        for (int col = 0; col < ARRAY_BOARD_SIZE; col++) {
            Move move = { .tile = {row, col}, .color = player };
            if (isLegalMove(pos, &move)) {
                validMoves[moveCount++] = move;
            }
        }
    }

    for (int i = 0; i < moveCount; i++) {
        futurePos = *pos;
        doMove(&futurePos, &validMoves[i]);
        int eval = minimaxAB(&futurePos, searchDepth - 1, getOtherSide(player), 0, INT_MIN, INT_MAX);
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = validMoves[i];
        }
    }
    return bestMove;
}

/**********************************************************/

int main( int argc, char ** argv ) {
    int c;
    opterr = 0;

    printf("Choose player mode: 0 (Random), 1 (Minimax), 2 (Alpha-Beta): ");
    scanf("%d", &useMinimax);

    while( ( c = getopt ( argc, argv, "i:p:h" ) ) != -1 )
        switch( c ) {
            case 'h':
                printf( "[-i ip] [-p port]\n" );
                return 0;
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case '?':
                if( optopt == 'i' || optopt == 'p' )
                    printf( "Option -%c requires an argument.\n", ( char ) optopt );
                else if( isprint( optopt ) )
                    printf( "Unknown option -%c\n", ( char ) optopt );
                else
                    printf( "Unknown option character -%c\n", ( char ) optopt );
                return 1;
            default:
                return 1;
        }

/**********************************************************/
// used in random
    connectToTarget( port, ip, &mySocket );
    srand( time( NULL ) );
/**********************************************************/

    while( 1 ) 
    {

        msg = recvMsg( mySocket );
        switch ( msg ) 
        {

            case NM_REQUEST_NAME:       //server asks for our name
                sendName( agentName, mySocket );
                //sendName(agentName2, mySocket);
                break;
            
            case NM_NEW_POSITION:       //server is trying to send us a new position
                getPosition( &gamePosition, mySocket );
                printPosition( &gamePosition );
                break;
            
            case NM_COLOR_W:        //server informs us that we have WHITE color
                myColor = WHITE;
                break;
            
            case NM_COLOR_B:        //server informs us that we have BLACK color
                myColor = BLACK;
                break;
            
            case NM_PREPARE_TO_RECEIVE_MOVE:        //server informs us that he will now send us opponent's move
                getMove( &moveReceived, mySocket );
                moveReceived.color = getOtherSide( myColor );
                doMove( &gamePosition, &moveReceived );
                printPosition( &gamePosition );
                break;
            
            case NM_REQUEST_MOVE:       //server requests our move
                myMove.color = myColor;

                if( !canMove( &gamePosition, myColor ) ) {
                    myMove.tile[ 0 ] = NULL_MOVE;       // we have no move ..so send null move
                } 
                else 
                {
                    if (useMinimax == 1) 
                    {
/**********************************************************/
// minimax player
                        myMove = findOptimalMove(&gamePosition, myColor);
// end of minimax
/**********************************************************/
                    } 
                    else if (useMinimax == 2)
                    {
/**********************************************************/
// alpha-beta pruning player
                        myMove = findOptimalMoveAB(&gamePosition, myColor);
// end of alpha-beta pruning
/**********************************************************/
                    }
                    else
                    {
/**********************************************************/
// random player - not the most efficient implementation
                        while( 1 ) {
                            int i = rand() % ARRAY_BOARD_SIZE;
                            int j = rand() % ARRAY_BOARD_SIZE;
                            if( gamePosition.board[ i ][ j ] == EMPTY ) {
                                myMove.tile[ 0 ] = i;
                                myMove.tile[ 1 ] = j;
// end of random
/**********************************************************/
                                if( isLegalMove( &gamePosition, &myMove ) )
                                    break;
                            }
                        }
                    }
                }
                sendMove( &myMove, mySocket );          //send our move
                doMove( &gamePosition, &myMove );       //play our move on our position
                printPosition( &gamePosition );
                break;

            case NM_QUIT:       //server wants us to quit...we shall obey
                close( mySocket );
                return 0;
        }
    }
    return 0;
}
