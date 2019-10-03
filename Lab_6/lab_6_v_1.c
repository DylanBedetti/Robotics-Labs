#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFSIZE 1024
#define MAXLEN 50
#define MY_FILE "branch.txt" //can also change to grid.txt

typedef struct {
    double x;
    double y;
} Node;

double matrix[MAXLEN][MAXLEN]; //for storing paths between nodes
Node nodes[MAXLEN];
int maxnodes = 0;

void init() {
    for (int i = 0; i < MAXLEN; i++) {
        for (int j = 0; j < MAXLEN; j++) {
            matrix[i][j] = -1;
        }

    Node new_node = { .x = 0, .y = 0};
    nodes[i] = new_node;
    }
}

void readFile() {
    FILE* file = fopen(MY_FILE, "r");
    char line[BUFFSIZE];
    
    while (fgets(line, sizeof(line), file)) {//for each line
        char *token;
        token = strtok(line, " ");
        while (token != NULL) {//for each token
                token = strtok(NULL, " ");
        }
    }
    fclose(file);
}

int main(){


    return 0;
}
