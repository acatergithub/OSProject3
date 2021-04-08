#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
int main(){
/*
    int *nums = (int *) malloc(10);

    for (int i = 0; i < 10; i++) {
        nums[i] = i;
    }
    int *temp = *nums[0]+6;
    printf(temp);
*/
    unsigned int myaddress = 4026544704;
    char *bitmap = myaddress;
    for (int i = 0; i < 8; i++) {
        printf(bitmap & 1);
        bitmap<<1;
    }
}