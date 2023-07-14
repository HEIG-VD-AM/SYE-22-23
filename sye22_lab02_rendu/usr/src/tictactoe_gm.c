/**
 * @file    tictactoe_gm.c
 * @author  REDS Institute / HEIG-VD
 * @date    2021-10-04
 *
 * @brief   SYE 2021 - Lab02
 *
 * Tic-tac-toe game manager.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <syscall.h>
#include <unistd.h>

#include <tictactoe.h>

// Variable that represents a tictactoe game.
game_t game;

/**
 * @brief   Min-max algorithm.
 *
 * @param   grid    Pointer to game's grid
 * @param   player  Player number
 * @param   depth   Max depth to explore
 * @return  int     The player score for the grid
 */
static int minmax(int player, unsigned int depth) {
    int i, move, next_score, score;

    if (!depth) {
        return 0;
    }

    move  = -1;
    score = -2;   // Losing moves are preferred to no move

    // For all moves,
    for (i = 0; i < GRID_SIZE; i++) {
        // If legal
        if (game.grid[i] == EMPTY_ID) {
            // Try the move
            game.grid[i] = player;

            // Get move score
            next_score = -minmax(-player, depth - 1);

            // Pick the one that's worst for the opponent
            if (next_score > score) {
                score = next_score;
                move  = i;
            }

            // Revert move
            game.grid[i] = 0;

            if (score > 0) {
                return score;
            }
        }
    }
    if (move == -1) {
        return 0;
    }

    return score;
}

/**
 * @brief   Play computer's turn.
 *
 * @return  int     Grid index of the computer's turn
 */
static int computers_turn(void) {
    int move  = -1;
    int score = -2;
    int i;
    int depth = MAX_DEPTH;
    int tempScore;

    for (i = 0; i < GRID_SIZE; i++) {
        if (game.grid[i] == EMPTY_ID) {
            // Do move
            game.grid[i] = AI_ID;

            // Check move score
            tempScore = -minmax(AI_ID, depth - 1);

            // Store better score
            if (tempScore > score) {
                score = tempScore;
                move  = i;
            }

            // Revert move
            game.grid[i] = 0;
        }
    }

    // Return best move
    return move + 1;
}

/**
 * @brief   Draw the game's grid.
 */
static void draw_grid(void) {
    int  i, j;
    char symbol;

    for (i = 0; i < GRID_HEIGHT; i++) {
        for (j = 0; j < GRID_WIDTH; j++) {
            switch (game.grid[i * GRID_WIDTH + j]) {
                case PLAYER_ID:
                    symbol = PLAYER_SYMBOL;
                    break;

                case AI_ID:
                    symbol = AI_SYMBOL;
                    break;

                default:
                    symbol = EMPTY_SYMBOL;
                    break;
            }
            if (j > 0)
                printf("|");
            printf(" %c ", symbol);
        }
        printf(" \n");
        if (i < GRID_HEIGHT - 1) {
            for (j = 0; j < GRID_WIDTH; j++) {
                if (j > 0)
                    printf("+");
                printf("---");
            }
            printf(" \n");
        }
    }
}

/**
 * @brief   Check if the game is won by anybody, either the player or the computer. The participant who has aligned
 *          three symbols wins the game.
 *
 * @returns STATE_WIN if one of the two players has won the game, STATE_DRAW if the grid is full and there is no winner,
 *          STATE_PROGRESS otherwise.
 * 
 *  Taken from this site http://www.cprogrammingnotes.com/question/tic-tac-toe-game.html. Only used for the winning part
 */
static int win(void) {
    // TO COMPLETE

    if ((game.grid[0] == PLAYER_ID || game.grid[0] == AI_ID) && game.grid[0] == game.grid[1] && game.grid[1] == game.grid[2])
        return STATE_WIN;
        
    else if ((game.grid[3] == PLAYER_ID || game.grid[3] == AI_ID) && game.grid[3] == game.grid[4] && game.grid[4] == game.grid[5])
        return STATE_WIN;
        
    else if ((game.grid[6] == PLAYER_ID || game.grid[6] == AI_ID) && game.grid[6] == game.grid[7] && game.grid[7] == game.grid[8])
        return STATE_WIN;
        
    else if ((game.grid[0] == PLAYER_ID || game.grid[0] == AI_ID) && game.grid[0] == game.grid[3] && game.grid[3] == game.grid[6])
        return STATE_WIN;
        
    else if ((game.grid[1] == PLAYER_ID || game.grid[1] == AI_ID) && game.grid[1] == game.grid[4] && game.grid[4] == game.grid[7])
        return STATE_WIN;
        
    else if ((game.grid[2] == PLAYER_ID || game.grid[2] == AI_ID) && game.grid[2] == game.grid[5] && game.grid[5] == game.grid[8])
        return STATE_WIN;
        
    else if ((game.grid[0] == PLAYER_ID || game.grid[0] == AI_ID) && game.grid[0] == game.grid[4] && game.grid[4] == game.grid[8])
        return STATE_WIN;
        
    else if ((game.grid[2ll] == PLAYER_ID || game.grid[2] == AI_ID) && game.grid[2] == game.grid[4] && game.grid[4] == game.grid[6])
        return STATE_WIN;

    for(size_t i = 0; i < GRID_SIZE; ++i) {
        if(game.grid[i] == EMPTY_ID)
            return STATE_PROGRESS;
    }

    return STATE_DRAW;
}

/**
 * @brief   Initialize game data.
 */
static void init_game(void) {
    // TO COMPLETE
    game.over = 0;
    for(size_t i = 0; i < GRID_SIZE; ++i) {
        game.grid[i] = EMPTY_ID;
    }
    printf("Initial board:\n");
    draw_grid();
    printf("The valid moves are 1-9.\n\n");
}

/**
 * @brief   Function to display start of player's turn.
 */
static void displayBeginPlayersTurn(void) {
    printf("Begin playersTurn\n");
}

/**
 * @brief   Function to display start of computer's turn.
 */
static void displayBeginComputersTurn(void) {
    printf("Begin computersTurn\n");
}

/**
 * @brief   Function to display end of player's turn.
 */
static void displayEndPlayersTurn(void) {
    printf("End playersTurn\n");
}

/**
 * @brief   Function to display end of computer's turn.
 */
static void displayEndComputersTurn(void) {
    printf("End computersTurn\n");
}

/**
 * @brief   Ask player for a move until we get a valid one
 */
static int get_player_move(void) {
    int move, valid = 0;

    // Ask player-side for player move
    gm_send_cmd(YOU_CAN_PLAY);

    while (!valid) {
        move = gm_get_cmd();

        // If move is false, ask again. Else, return it
        if ((move < 0) || (move > GRID_SIZE-1) || (game.grid[move] != 0)) {
            gm_send_cmd(MOVE_REJECTED);
        }
        else {
            valid = 1;
        }
    }

    return move;
}

/**
 * @brief   Check the state of the game.
 */
static void check_game_state(int currentID) {
    int state;

    switch (state = win()) {
        case STATE_PROGRESS:
            break;

        case STATE_WIN:
            if (currentID == PLAYER_ID) {
                gm_send_cmd(YOU_WIN);
            }
            else {
                gm_send_cmd(YOU_LOSE);
            }
            game.over = 1;

            break;

        case STATE_DRAW:
            gm_send_cmd(NO_WINNER);
            game.over = 1;

            break;
    }
}

/**
 * @brief   Manages the game behaviour.
 */
static void game_manager(void) {
    int move;
    int player = PLAYER_ID;   // Start with the player

    // Signal player-side that the game has started
    gm_send_cmd(GAME_START);

    while (!game.over) {
        if (player == PLAYER_ID) {

            displayBeginPlayersTurn();

            // Ask for player's move
            move = get_player_move();

            // Apply move on grid
            game.grid[move] = PLAYER_ID;

            displayEndPlayersTurn();
            draw_grid();
        }
        else {
            displayBeginComputersTurn();

            // The computer plays its turn
            move            = computers_turn() - 1;
            game.grid[move] = AI_ID;

            draw_grid();
            displayEndComputersTurn();
        }

        // Check for a winner or a draw
        check_game_state(player);

        player = ((player == PLAYER_ID) ? AI_ID : PLAYER_ID);
    }

    // Signal player-side that the game has ended
    gm_send_cmd(GAME_END);

    return;
}

/**
 * @brief   Main function
 */
int main(int argc, char **argv) {

char line[MAX_BUFFER_LENGTH];

    do {
        //TO COMPLETE
        init_game();

        // Initialize Inter-Process Communication library
        ipc_init();   // DON'T REMOVE
        

        // TO COMPLETE

        int pid = sys_fork2();

        if(pid == 0) {

            char argv1[MAX_BUFFER_LENGTH];
            sprintf(argv1, "%d", ipc_player_argv1()); 
            char argv2[MAX_BUFFER_LENGTH];
            sprintf(argv2, "%d", ipc_player_argv2()); 

            execl("./tictactoe_player.elf", "./tictactoe_player.elf", argv1, argv2, (char*) NULL);
        } else {

            game_manager();
            waitpid(pid, NULL, 0);
        }
        
        ipc_close();    // DON'T REMOVE

        

        scanf("%s", line);

    } while(!strcmp(line, "Y"));
    return 0;
}
