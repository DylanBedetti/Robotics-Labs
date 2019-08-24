#include <stdio.h>

float pos[4][2] = {
    {0, 0}, // P1 -> Start Point (x,y)
    {400, 1000}, // P2 -> End Point (x,y)
    {0, 0}, // T1 -> Start Tangent (x,y)
    {0, 0} // T2 -> End Tangent (x,y)
} ;

void print_array(){
    for (int k = 0; k < 4; k++){
            for (int j = 0; j < 2; j++){
                printf("%.0f\t", pos[k][j]);
            }
            printf("\n");
        }
}

int main() {
    FILE* file = fopen("way.txt", "r");
    int points[20][2];
    int i = 0;
    while (fscanf(file,"%d %d", &points[i][0], &points[i][1]) == 2) {
        printf("\nPoint: %d\n", i);
        if (i == 0){
            // printf("x:%d y:%d\n", points[i][0], points[i][1]);
            pos[1][0] = points[i][0];
            pos[1][1] = points[i][1];
            print_array();
            
        } else{
    	    // printf("x:%d y:%d\n", points[i][0] - points[i-1][0], points[i][1]-points[i-1][1]);
            pos[0][0] = points[i-1][0];
            pos[0][1] = points[i-1][1];
            
            pos[1][0] = points[i][0];
            pos[1][1] = points[i][1];

            print_array();
        }
	i++;
    }
    pos[0][0] = pos[1][0];
    pos[0][1] = pos[1][1];

    pos[1][0] = 0;
    pos[1][1] = 0;
    
    printf("\nPoint: %d\n", i);
    print_array();


    fclose(file);
}




