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
    int count_t = 0;
    int count_l = 0;
    FILE* file = fopen(MY_FILE, "r");
    char line[BUFFSIZE];

    while (fgets(line, sizeof(line), file)) {//for each line
        char *token;
        // printf("%s", line);

        token = strtok(line, " ");
        count_t = 0;
        while (token != NULL) {//for each token
            // printf("%s\n",token);
            if (count_t == 0){
                nodes[count_l].x = atoi(token); printf("line: %d, token: %d, x: %s\n",count_l, count_t, token);
            }
            if (count_t == 1){
                nodes[count_l].y = atoi(token); printf("line: %d, token: %d, y: %s\n",count_l, count_t, token);
            }

            count_t++;
            token = strtok(NULL, " ");
        }
        count_l++;
    }
    fclose(file);
}

int main(){

    printf("starting Lab 6!\n");
    // init();

    readFile();

    return 0;
}
