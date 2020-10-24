#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 8
#define EMPTY ' '
#define PLAYER 'X'
#define COMPUTER 'O'
#define DEPTH 2

struct Node{
    char board[N][N];
    int heuristic;
    int children_count;
    struct Node** children;
};

struct Node* createRoot() {
    struct Node* root = malloc(sizeof(struct Node));

    for(int row = 0; row < N; row++) {
        for(int column = 0; column < N; column++) {
            root->board[row][column] = EMPTY;
        }
    }
    return root;
}

int freeColumns(struct Node* node) {
    int count = 0;

    for(int column = 0; column < N; column++) {
        char square = node->board[0][column];
        if(square == EMPTY) {
            count++;
        }
    }
    return count;
}

int playColumn(struct Node* node, int column, char player) {
    for(int row = N-1; row >= 0; row--) {
        char* square = &node->board[row][column];
        if(*square == EMPTY) {
            *square = player;
            return 1;
        }
    }
    return 0;
}

int heuristic(struct Node* node) {
    int value = 0;
    //////////////////////
    // Horizontal check //
    //////////////////////
    for(int row = 0; row < N; row++) {
        for(int column = 0; column < N; column++) {
            int count = 0;
            while(1) {
                char square = node->board[row][column++];
                if(square == PLAYER) {
                    count++;
                } else {
                    break;
                }
            }
            if(count == 2) {
                value -= 500;
            } else if(count == 3) {
                value -= 2500;
            } else if(count == 4) {
                value -= 12500;
            }
        }
    }
    ////////////////////
    // Vertical check //
    ////////////////////
    for(int column = 0; column < N; column++) {
        for(int row = 0; row < N; row++) {
            int count = 0;
            while(1) {
                char square = node->board[row++][column];
                if(square == PLAYER) {
                    count++;
                } else {
                    break;
                }
            }
            if(count == 2) {
                value -= 500;
            } else if(count == 3) {
                value -= 2500;
            } else if(count == 4) {
                value -= 12500;
            }
        }
    }
    return value;
}

void copyBoard(struct Node* parent, struct Node* child) {
    for(int row = 0; row < N; row++) {
        for(int column = 0; column < N; column++) {
            child->board[row][column] = parent->board[row][column];
        }
    }
}

void performComputer(struct Node* node, int level) {
    ////////////////////////
    // Maximum DEPTH halt //
    ////////////////////////
    if(level == DEPTH) {
        node->heuristic = heuristic(node);
        return;
    }

    ////////////////////
    // Children array //
    ////////////////////
    node->children_count = freeColumns(node);
    node->children = malloc(sizeof(struct Node**)*node->children_count);

    if(node->children_count > 0) {
        /////////////////////////////////////
        // Initialize and perform children //
        /////////////////////////////////////
        int index = 0;

        for(int column = 0; column < N; column++) {
            char square = node->board[0][column];
            if(square != EMPTY) {
                continue;
            }
            struct Node* child = malloc(sizeof(struct Node));

            copyBoard(node, child);

            char identity = (level % 2) ? PLAYER : COMPUTER;

            playColumn(child, column, identity);

            performComputer(child, level + 1);

            node->children[index] = child;

            index++;
        }

        ///////////////////////////////
        // Infer of heuristic values //
        ///////////////////////////////
        int maximum = INT_MIN;
        int minimum = INT_MAX;

        struct Node* max_child;
        struct Node* min_child;

        for(int index = 0; index < node->children_count; index++) {
            struct Node* child = node->children[index];
            int value = child->heuristic;
            if(maximum < value) {
                maximum = value;
                max_child = child;
            }
            if(minimum > value) {
                minimum = value;
                min_child = child;
            }
        }
        if((level % 2) == 0) {
            node->heuristic = max_child->heuristic;
        } else {
            node->heuristic = min_child->heuristic;
        }

        /////////////////////////
        // Root level specific //
        /////////////////////////
        if(level == 0) {
            copyBoard(max_child, node);
        }
    }

    ///////////////////
    // Final cleanup //
    ///////////////////
    for(int index = 0; index < node->children_count; index++) {
        free(node->children[index]);
    }
    free(node->children);
}

void showBoard(struct Node* node) {
    for(int column = 0; column < N; column++) {
        printf("| %i ", column + 1);
    }
    printf("|\n");
    for(int row = 0; row < N; row++) {
        for(int column = 0; column < N; column++) {
            char square = node->board[row][column];
            printf("| %c ", square);
        }
        printf("|\n");
    }
}

int main() {
    struct Node* root = createRoot();

    while(1) {
        showBoard(root);

        while(1) {
            printf("Enter a column [1-%i]: ", N);

            int column = 0;

            scanf("%i", &column);

            if((column < 1) || (column > N)) {
                continue;
            }
            if(!playColumn(root, --column, PLAYER)) {
                continue;
            }
            break;
        }
        performComputer(root, 0);

        printf("\n");
    }

    return 0;
}
