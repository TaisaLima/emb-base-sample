#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    
    char plate[8];
    char alphabet[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

    for (int j = 0; j < 3; j++){
        int index = rand() % 26; 
        plate[j] = alphabet[index];
    }

    int num1 = rand() % 10; 
    plate[3] = num1 + '0'; 

    int index4 = rand() % 26;
    plate[4] = alphabet[index4];
    
    int num5 = rand() % 10;
    plate[5] = num5 + '0'; 
    
    int num6 = rand() % 10;
    plate[6] = num6 + '0';
    
    plate[7] = '\0'; 


        /*
        
        printf("Placa Mercosul Gerada (LLLNLNN): ");
         printf("%c%c%c %c%c%c%c\n", 
           plate[0], plate[1], plate[2], 
           plate[3], plate[4], plate[5], plate[6]);
        */

    return 0;
}