#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// function prototypes
char** init_board(int size);
void print_board(char **board, int size);
int check_move(char **board, int size, int p_row, int p_col, int t_row, int t_col);
void move_piece(char **board, int size, int p_row, int p_col, int t_row, int t_col , int *counts1, int *counts2, int player_number);
void update_points(char **board, int size, int *points, int *counts , int player_number);
int are_moves_available(char **board, int size);
void save_board(char **board, int size, int whos_turn, int *counts1, int *counts2, int points1, int points2);
char** load_board(int *size, int *whos_turn, int *counts1, int *counts2, int *points1, int *points2);
void computer_move(char **board, int size, int *counts2, int *points2, int *whos_turn);

int main() {
    int size;
    char **board;

    int p_row, p_col;
    int t_row, t_col;

    int whos_turn = 0;
    char choice;

    int counts1[5] = {0};
    int points1 = 0;

    int counts2[5] = {0};
    int points2 = 0;

    int play_vs_computer = 0;

    printf("Welcome to the Skippity game!\n");
    printf("Do you want to start a new game or load a saved game? (n/l): ");
    scanf(" %c", &choice);

    if (choice == 'n') {
        printf("Enter the length of the board (board size is length x length): ");
        scanf("%d", &size);

        if (size < 4 || size > 20) {
            printf("Board size is not valid!\n");
            printf("Please enter a size between 4 and 20\n");
            return 1;
        }

        printf("Do you want to play against the computer? (1 for yes, 0 for no): ");
        scanf("%d", &play_vs_computer);

        board = init_board(size);
    } else if (choice == 'l') {
        board = load_board(&size, &whos_turn, counts1, counts2, &points1, &points2);
        if (board == NULL) {
            printf("Failed to load game. Starting a new game instead.\n");
            printf("Enter the length of the board (board size is length x length): ");
            scanf("%d", &size);

            if (size < 4 || size > 20) {
                printf("Board size is not valid!\n");
                printf("Please enter a size between 4 and 20\n");
                return 1;
            }

            printf("Do you want to play against the computer? (1 for yes, 0 for no): ");
            scanf("%d", &play_vs_computer);

            board = init_board(size);
        }else{
            printf("Do you want to play against the computer? (1 for yes, 0 for no): ");
            scanf("%d", &play_vs_computer);
        }
    } else {
        printf("Invalid choice!\n");
        return 1;
    }

    while (are_moves_available(board, size)) {
        print_board(board, size);
        printf("Player %d\n", whos_turn % 2 + 1);

        if (play_vs_computer && whos_turn % 2 == 1) {
            computer_move(board, size, counts2, &points2, &whos_turn);
        } else {
            printf("Enter the row and column of the piece you want to move (row col): ");
            scanf("%d %d", &p_row, &p_col);

            printf("Enter the row and column of the destination (row col): ");
            scanf("%d %d", &t_row, &t_col);

            // Adjust coordinates to be zero-based
            p_row--;
            p_col--;
            t_row--;
            t_col--;

            if (!check_move(board, size, p_row, p_col, t_row, t_col)) {
                printf("Please enter a valid move\n");
                continue;
            }

            printf("Do you want to move the piece at (%d, %d) to (%d, %d)? (y/n/r): ", p_row + 1, p_col + 1, t_row + 1, t_col + 1);
            printf("Enter y for yes, r for redo: ");
            scanf(" %c", &choice);

            if (choice == 'r') {
                continue;
            }

            move_piece(board, size, p_row, p_col, t_row, t_col, counts1, counts2, whos_turn % 2 + 1); // Move the piece

            save_board(board, size, whos_turn % 2 + 1, counts1, counts2, points1, points2); // Save the game state

            if (whos_turn % 2 + 1 == 1) { // Update points
                update_points(board, size, &points1, counts1, whos_turn % 2 + 1);
            } else {
                update_points(board, size, &points2, counts2, whos_turn % 2 + 1);
            }

            whos_turn++; // Change the turn
        }
    }

    print_board(board, size);

    printf("Player 1 Points: %d\n", points1);
    printf("Player 2 Points: %d\n", points2);

    if (points1 > points2) {
        printf("Player 1 wins!\n");
    } else if (points1 < points2) {
        printf("Player 2 wins!\n");
    } else {
        printf("It's a tie!\n");
    }

    return 0;
}

// The rest of the functions remain unchanged, but are included for completeness

char** init_board(int size) {
    srand(time(NULL));
    char **board = (char**)malloc(size * sizeof(char*));
    for(int i = 0; i < size; i++) {
        board[i] = (char*)malloc(size * sizeof(char));
    }

    int total_cells = size * size;
    int each_char_count = total_cells / 5;
    int remainder = total_cells % 5;

    // Create an array to hold the characters and their counts
    char chars[5] = {'A', 'B', 'C', 'D', 'E'};
    int counts[5] = {each_char_count, each_char_count, each_char_count, each_char_count, each_char_count};

    // Distribute the remainder among the characters
    for(int i = 0; i < remainder; i++) {
        counts[i]++;
    }

    // Fill the board with the characters
    int index = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++){
            while(counts[index] == 0) {
                index++;
            }
            board[i][j] = chars[index];
            counts[index]--;
        }
    }

    // Randomize the board to mix up the characters
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int r = rand() % size;
            int c = rand() % size;
            char temp = board[i][j];
            board[i][j] = board[r][c];
            board[r][c] = temp;
        }
    }

    // Set the center of the board to be empty
    board[size / 2][size / 2] = '.';

    return board;
}

void print_board(char **board, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

int check_move(char **board, int size, int p_row, int p_col, int t_row, int t_col){
    // check if its out of bounds
    if(p_row < 0 || p_row >= size || p_col < 0 || p_col >= size || t_row < 0 || t_row >= size || t_col < 0 || t_col >= size){
        return 0;
    }

    // check if the piece is not in the same position as the target
    if(p_row == t_row && p_col == t_col){
        return 0;
    }

    // check if the piece is not empty and the target is empty
    if(board[p_row][p_col] == '.' || board[t_row][t_col] != '.'){
        return 0;
    }

    // check if the piece is not moving diagonally
    if(p_row != t_row && p_col != t_col){
        return 0;
    }

    // check if the piece is not moving more than 2 steps
    if(abs(p_row - t_row) > 2 || abs(p_col - t_col) > 2){
        return 0;
    }

    // check if the piece is moving by 2 steps, the jumped piece is not empty
    if(abs(p_row - t_row) == 2 || abs(p_col - t_col) == 2){
        if(board[(p_row + t_row) / 2][(p_col + t_col) / 2] == '.'){
            return 0;
        }
        return 1;
    }

    return 0;
}

void move_piece(char **board, int size, int p_row, int p_col, int t_row, int t_col, int *counts1, int *counts2, int player_number){
    // check if it's a jumping move
    if(abs(p_row - t_row) == 2 || abs(p_col - t_col) == 2){
        if(player_number == 1){
            // increment the count of the character that was jumped over
            counts1[(int)(board[(p_row + t_row) / 2][(p_col + t_col) / 2] - 65)]++;
        }else if(player_number == 2){
            // increment the count of the character that was jumped over
            counts2[(int)(board[(p_row + t_row) / 2][(p_col + t_col) / 2] - 65)]++;
        }

        // remove the piece that was jumped over
        board[(p_row + t_row) / 2][(p_col + t_col) / 2] = '.';
    }

    // move the piece to the target
    board[t_row][t_col] = board[p_row][p_col];
    board[p_row][p_col] = '.';
}

void update_points(char **board, int size, int *points, int *counts, int player_number){
    int smallest = counts[0];

    printf("P%d Counts: \n", player_number);

    // update the points of the player
    for(int i = 0; i < 5; i++){
        if(counts[i] < smallest){
            smallest = counts[i];
        }
        printf("%c: %d ", (char)(i + 65), counts[i]);
    }

    printf("\n");

    *points = smallest;
    printf("P%d Points: %d\n",player_number, *points);
}

int are_moves_available(char **board, int size) {
    // Iterate through the board to find pieces
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] != '.') {
                // Check all possible moves (up, down, left, right)
                if (check_move(board, size, i, j, i-2, j)) return 1;
                if (check_move(board, size, i, j, i+2, j)) return 1;
                if (check_move(board, size, i, j, i, j-2)) return 1;
                if (check_move(board, size, i, j, i, j+2)) return 1;
            }
        }
    }
    return 0;
}

void save_board(char **board, int size, int whos_turn, int *counts1, int *counts2, int points1, int points2) {
    FILE *file = fopen("savefile.txt", "w");
    if (file == NULL) {
        printf("Error opening file for saving.\n");
        return;
    }

    // Save board size and current turn
    fprintf(file, "%d %d\n", size, whos_turn);

    // Save points and counts
    fprintf(file, "%d %d\n", points1, points2);
    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d ", counts1[i]);
    }
    fprintf(file, "\n");
    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d ", counts2[i]);
    }
    fprintf(file, "\n");

    // Save the board state
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%c ", board[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Game saved successfully!\n");
}

char** load_board(int *size, int *whos_turn, int *counts1, int *counts2, int *points1, int *points2) {
    FILE *file = fopen("savefile.txt", "r");
    if (file == NULL) {
        printf("Error opening file for loading.\n");
        return NULL;
    }

    // Read board size and current turn
    fscanf(file, "%d %d", size, whos_turn);

    // Read points and counts
    fscanf(file, "%d %d", points1, points2);
    for (int i = 0; i < 5; i++) {
        fscanf(file, "%d", &counts1[i]);
    }
    for (int i = 0; i < 5; i++) {
        fscanf(file, "%d", &counts2[i]);
    }

    // Allocate memory for the board
    char **board = (char**)malloc(*size * sizeof(char*));
    for (int i = 0; i < *size; i++) {
        board[i] = (char*)malloc(*size * sizeof(char));
    }

    // Read the board state
    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            fscanf(file, " %c", &board[i][j]);
        }
    }

    fclose(file);
    printf("Game loaded successfully!\n");
    return board;
}

void computer_move(char **board, int size, int *counts2, int *points2, int *whos_turn) {
    int p_row, p_col, t_row, t_col;
    char moved = 0;

    // Find a valid move
    for (p_row = 0; p_row < size && !moved; p_row++) {
        for (p_col = 0; p_col < size && !moved; p_col++) {
            if (board[p_row][p_col] != '.') {
                if (check_move(board, size, p_row, p_col, p_row - 2, p_col)) {
                    t_row = p_row - 2;
                    t_col = p_col;
                    moved = 1;
                } else if (check_move(board, size, p_row, p_col, p_row + 2, p_col)) {
                    t_row = p_row + 2;
                    t_col = p_col;
                    moved = 1;
                } else if (check_move(board, size, p_row, p_col, p_row, p_col - 2)) {
                    t_row = p_row;
                    t_col = p_col - 2;
                    moved = 1;
                } else if (check_move(board, size, p_row, p_col, p_row, p_col + 2)) {
                    t_row = p_row;
                    t_col = p_col + 2;
                    moved = 1;
                }
            }
        }
    }

    // Perform the move
    if (moved) {
        printf("Computer moves piece at (%d, %d) to (%d, %d)\n", p_row, p_col, t_row + 1, t_col + 1);
        move_piece(board, size, p_row - 1, p_col - 1, t_row, t_col, counts2, counts2, 2); // Move the piece
        save_board(board, size, *whos_turn % 2 + 1, counts2, counts2, *points2, *points2); // Save the game state
        update_points(board, size, points2, counts2, 2); // Update points
        (*whos_turn)++; // Change the turn
    }
}
