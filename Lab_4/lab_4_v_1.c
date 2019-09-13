#include "eyebot.h"

#define LINE_MAX 255
#define DEPTH_LIMIT 4
#define MAX_QUADS 256

void read_pbm_header(FILE *file, int *width, int *height) {
	char line[LINE_MAX];
	char word[LINE_MAX];
	char *next;
	int read;
	int step = 0;
	while (1) {
		if (!fgets(line, LINE_MAX, file)) {
			fprintf(stderr, "Error: End of file\n");
			exit(EXIT_FAILURE);
		}
		next = line;
		if (next[0] == '#') continue;
		if (step == 0) {
			int count = sscanf(next, "%s%n", word, &read);
			if (count == EOF) continue;
			next += read;
			if (strcmp(word, "P1") != 0 && strcmp(word, "p1") != 0) {
				fprintf(stderr, "Error: Bad magic number\n");
				exit(EXIT_FAILURE);
			}
			step = 1;
		}
		if (step == 1) {
			int count = sscanf(next, "%d%n", width, &read);
			if (count == EOF) continue;
			next += read;
			step = 2;
		}
		if (step == 2) {
			int count = sscanf(next, "%d%n", height, &read);
			if (count == EOF) continue;
			next += read;
			return;
		}
	}
}

void read_pbm_data(FILE *file, int width, int height, BYTE *img) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = ' ';
            while(c == ' ' || c == '\n' || c == '\r')
                c = fgetc(file);

            if(c != '0' && c != '1'){
                fprintf(stderr, "Bad character: %c\n", c);
                exit(0);
            }
            *img = c - '0';
            img++;
        }
    }
}

void read_pbm(char *filename, BYTE **img) {
	int width;
	int height;
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Error: Cannot open the input file\n");
		exit(EXIT_FAILURE);
	}
	read_pbm_header(file, &width, &height);
	*img = (BYTE*) malloc(width * height * sizeof(BYTE));
	read_pbm_data(file, width, height, *img);
	fclose(file);
}

typedef struct Quadbox {
    int xcoord;
    int ycoord;
    int width;
} Quadbox;

Quadbox blocked_quads[MAX_QUADS];
Quadbox free_quads[MAX_QUADS];



int main(){
    char* filename = "corner.pbm";
    BYTE* img;
    read_pbm(filename, &img);
    printf("%d\n", img[0]);

    LCDImageStart(0, 0, 128, 128);
    LCDImageBinary(img);

    for (int t = 0; t < 100; t++){
        usleep(100000);
        printf("%d\n", t);
    }
    return 0;
}
