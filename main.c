/*
 * Wirtten by Ho Yat Lui
 * Student Number: 7780269
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aeslib.h"

// functions declaration
void printState(u8 state[4][4], int mode);  // print state between each round
void cipher(u8 state[4][4]);
void deCipher(u8 state[4][4]);


const int numRound = 10;  // number of rounds for 128-bit 
const int numCol = 4; // number of column of state for 128-bit

int main(){
    u8 state[numCol][numCol];  // the state 
    char defaultPlain[] = "test1plaintext.txt";
    char defaultKey[] = "test1key.txt";

    initialize();    //initialize, read in sbox and generate round constant
    char pName[100];
    char kName[100];
    printf("Plaint text Filename: ");
    fgets(pName,100,stdin);  // request file name
    printf("Key Filename: ");
    fgets(kName,100,stdin);  // requet file name

    pName[strcspn(pName,"\n")] = '\0';  // remove the newline character at the end
    kName[strcspn(kName,"\n")] = '\0';  // remove the newline character at then end
    if( !pName[0]=='\0'){  // if new file name is provided, use the defualt test case
        strcpy(defaultPlain,pName);
    }
    if( !kName[0]=='\0'){  // same as above
        strcpy(defaultKey,kName);
    }

    readPlain(state,defaultPlain);   // read in platintext
    printf("Key Schedule:\n");
    keyExpansion(defaultKey);   // read in keys and print all round keys

    cipher(state);   // cipher
    deCipher(state);  // decipher

    printf("End of Processing\n");  
    exit(0);
}

/*
 * function from NISt standard, almost exactly the same as 
 * the pseudo code in page 15, figure 5
 */
void cipher(u8 state[4][4]){  
    printf("\nENCRYPTION PROCESS:\n-----------\n");
    printf("Plain Text:\n");
    printState(state,0);

    addRoundKey(state, 0,numCol-1);

    printf("Round 1\n-----------\n");
    printState(state,0); // print first round
    for(int round = 1 ; round <= numRound-1 ; round++){
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, round*numCol, ((round+1)*numCol)-1);
        printf("Round %d\n-----------\n",round+1);  // print for each round
        printState(state,0);
    }
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, numRound*numCol, ((numRound+1)*numCol)-1);

    printf("Cipher Text:\n");
    printState(state,0);  // print the cipher text
}

/*
 * function from NISt standard, almost exactly the same as 
 * the pseudo code in page 21, figure 12
 */
void deCipher(u8 state[4][4]){
    printf("\nDECRYPTION PROCESS:\n-----------\n");
    printf("Cipher Text:\n");
    printState(state,0);

    addRoundKey(state, numRound*numCol, ((numRound+1)*numCol)+1);


    for(int round = numRound-1 ; round >= 1 ; round--){
        invShiftRows(state);
        invSubBytes(state);
        printf("Round %d\n-----------\n",round); // print for each round
        printState(state,0);
        addRoundKey(state, round*numCol, ((round+1)*numCol)-1);
        invMixColumns(state);  
    }
    invShiftRows(state);
    invSubBytes(state);
    printf("Round 0\n-----------\n");
    printState(state,0); // print the last round
    addRoundKey(state, 0, numCol-1);

    printf("Plain Text:\n");
    printState(state,0);  //print plaintext
}


/*
 * print the state matrix between rounds
 * include printing in matrix form for debug purpose
 */
void printState(u8 state[4][4], int mode){
    if(mode){  //if mode == 1, print as matrix
        for(int i = 0 ; i< 4 ; i++){
            for(int j = 0 ; j< 4 ; j++){
                printf("%02x,",state[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    else{  // if mode == 0 , print as line
        for(int i = 0 ; i< 4 ; i++){
            for(int j = 0 ; j< 4 ; j++){
                printf("%02x  ",state[j][i]);
            }
            printf(" ");
        }
        printf("\n\n");
    }
}
