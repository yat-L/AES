/*
 * Wirtten by Ho Yat Lui
 * Student Number: 7780269
 */

#include "aeslib.h"
#include <stdlib.h>

// theres a typedef in the headre file
// u32 is a 32bit unsinged integer type uint32_t
// u8 is a 8bit unsinged integer type uint8_t


// static function, private
static u8 asciiToHex( char * s);
static void fillBox(char* file, u8 box[16][16]);
static void fillRcon();
static u8 subSbox(u8 byte, u8 box[16][16]);
static void printKeys();
static u32 subWord(u32 word);
static u8 xtime( u8 x);
static u8 gfMulti(u8 a, u8 b);
static int mod(int a, int b);
static u32 rotWord(u32 word);


// gloabl variable
u8 sbox[16][16]; // hold the sbox
u8 invSbox[16][16];  // hold the inverse sbox
u32 Rcon[10];   // hold a list of round constant
u32  roundKey[4*(10+1)];  // hold the list of round keys

/**
 * initialize() will fill sbox and inverse sbox, and also generate all round constant
 * called at the start of main
 **/
void initialize(){
    fillBox("aes_sbox.txt",sbox);
    fillBox("aes_inv_sbox.txt",invSbox);
    fillRcon();
}


/**
 * change string in ascii into hex number
 * used in reading in files
 */
static u8 asciiToHex( char * s){
    u8 upper = (s[0] <= '9') ? (s[0] - '0') : (s[0] - 'a' + 10);
    u8 lower = (s[1] <= '9') ? (s[1] - '0') : (s[1] - 'a' + 10);
    lower = (upper << 4) | lower;
    return lower;
}

/**
 * this function will receive file name of the sbox, and the global variable.
 * It will fill up the sbox and invSbox, called in initialize
 */
static void fillBox(char* fName, u8 box [16][16]){
    char str[100];
    FILE *file = fopen(fName,"r");
    int row = 0;
    while( fgets(str,100,file)){
        char* noNewline = strtok(str,"\n");  // remove the last newline charater
        char* byte = strtok(noNewline," ");  // tokenize string
        int col = 0;
        while( byte != NULL){
            box[row][col] = asciiToHex(byte);  // convert to hex value before stroing
            byte = strtok(NULL," ");
            col++;
        }
        row++;
    }
}

/**
 * the function is the NIST standard, it will replace every byte with the sbox byte
 */
void subBytes(u8 state[4][4]){
    for(int i = 0 ; i < 4 ; i++){  // loop through the state
        for(int j = 0 ; j < 4 ; j++){
            state[i][j] = subSbox(state[i][j],sbox);  //substitude with a function
        }
    }
}

/**
 * the function is the NIST standard, it will replace every byte with the invsbox byte
 */
void invSubBytes(u8 state[4][4]){
    for(int i = 0 ; i < 4 ; i++){ // loop through the state
        for(int j = 0 ; j < 4 ; j++){
            state[i][j] = subSbox(state[i][j],invSbox);  // substitude
        }
    }
}

/**
 * function in NISt standard, it will shift the rows
 */
void shiftRows(u8 state[4][4]){
    u8 copy[4][4];  // a temporary deep copy of state
    memcpy(copy,state,sizeof(u8)*4*4);  // copy it with memcpy
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            state[i][j] = copy[i][(j+i)%4];   // shift rows
        }
    }
}

/**
 * for invShiftRows, since % operator is the remainder, not modulo cannot be used in negative
 */
static int mod(int a, int b){  
    int r = a % b;
    return (r<0) ? (r+b) : r;
}

/**
 * function in NISt standard, it will shift the rows in reverse 
 */
void invShiftRows(u8 state[4][4]){
    u8 copy[4][4];   // a temporary deep copy of state
    memcpy(copy,state,sizeof(u8)*4*4);
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            state[i][j] = copy[i][mod((j-i),4)];  // shift rows
        }
    }
}

/**
 * function in NISt standard, xtime if GF(2^8)
 */
static u8 xtime( u8 x){
    u8 ifMod = (x >> 7) & 1;  //check if first bit is 1
    if( ifMod){
        return (x << 1) ^ 0x11b;
    }
    else{
        return (x << 1);
    }
}

/**
 * gf multiply, used in mixColumns and invMixColumns
 */
static u8 gfMulti(u8 a, u8 b){
    u8 result = 0;
    for(int i = 0 ; i < 8 ; i++){
        if((b & 1)){
            result = result ^ a;
        }
        a = xtime(a);
        b = b >> 1;
    }
    return result;
}


/**
 * this function generate all the round constant with xtime()
 */
static void fillRcon(){
    u8 start = 0x01;
    for(int i = 0 ; i < 10 ; i++){
        Rcon[i] = start;
        start = xtime(start);
        Rcon[i] = Rcon[i] * 0x1000000;
    }
}

/**
 * This function generate all round keys from the master key and store them in global array
 */
void keyExpansion(char* fName){
    u32 temp;
    u8 key[16];
    FILE *file = fopen(fName,"r");
    if( file == NULL){  // if filename is wrong
        printf("Cannot Open key File, Exiting.....\n");
        exit(1);
    }
    char line[100];
    while( fgets(line,100,file)){  // read in master key
        char* noNewLine = strtok(line,"\n");  // remove newline
        char* byte = strtok(noNewLine," ");
        int index = 0;
        while( byte != NULL){
            key[index] = asciiToHex(byte);  // convert master key to hex
            byte = strtok(NULL," ");
            index++;
        }
    }
    int i = 0; 
    while( i < 4){  // combine round keys into 32 bit integer
        roundKey[i] = (key[4*i] << 24) | (key[4*i+1] << 16) | (key[4*i+2] << 8) | (key[4*i+3]);
        i++;
    }

    i = 4;
    while( i < 4*(10+1)){  //generate round keys
        temp = roundKey[i-1];
        if( i % 4 == 0){
            u32 afterRot = rotWord(temp);
            u32 afterSub = subWord(afterRot);
            u32 afterX = afterSub ^ Rcon[(i/4)-1];
            temp = afterX;
        }
        roundKey[i] = roundKey[i-4] ^ temp;
        i++;
    }
    printKeys();
}

/**
 * called in keyExpansion(), print all the generated round keys
 */
static void printKeys(){
    for(int i=0 ; i < 44 ; i++){
        if( (i+1) % 4 == 0){
            printf("%08x\n",roundKey[i]);
        }
        else{
            printf("%08x, ",roundKey[i]);
        }
    }
}

/**
 * this function will substibue 1 byte from 1 byte in sbox , called in subBytes()
 * , invSubBytes() and subWord() from keyExpansion()
 */
static u8 subSbox(u8 byte, u8 box[16][16]){
    u8 upper = byte >> 4;
    u8 lowerInt = byte << 4;
    u8 lower = lowerInt >> 4;
    return box[upper][lower];
}

/**
 * substidue whole word from sbox, used in keyExpansion()
 */
static u32 subWord(u32 word){
    u8 temp[4]; 
    for(int i =0 ; i< 4 ; i++){
        u8 currentByte = word >> (24-(i*8));
        temp[i] = subSbox(currentByte,sbox);  // sub for 1 byte
        word = word ^ ( currentByte << (24-(i*8)));
    }
    return  (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | (temp[3]);  // combine result
}

/**
 * function in NISt standard, for cyclic permutation of 32 bit
 */
static u32 rotWord(u32 word){
    u32 upper = word << 8;
    u32 lower = word >> (32-8);
    return upper | lower;
}

/**
 * read plaintext file into the state matrix
 */
void readPlain(u8 p[4][4],char* fName){
    FILE *file = fopen(fName,"r");
    if( file == NULL){  // if wrong plaintext file
        printf("Cannot Open Plaintext File, Exiting.....\n");
        exit(1);
    }
    char line[100];
    while( fgets(line,100,file)){
        char* noNewLine = strtok(line,"\n");  // remove newline
        char* byte = strtok(noNewLine," ");   // tokenize input
        int i = 0;
        int j = 0;
        while( byte != NULL){
            p[i%4][j] = asciiToHex(byte);   // store in state
            byte = strtok(NULL," ");
            i++;
            if(i % 4 == 0){
                j++;
            }
        }
    }
}

/**
 * function in NISt standard, adding roundkeys to the state column
 */
void addRoundKey(u8 state[4][4], int start, int end){
    for(int i = 0 ; i < 4 ; i++){
        u32 stateCol = (state[0][i] << 24) | (state[1][i]<< 16) | (state[2][i] << 8) | (state[3][i]);    // convert a column into 32bit word
        u32 result = stateCol ^ roundKey[start+i];  // XOR with roundkeys

        for(int j =0 ; j< 4 ; j++){   // separate the word and store back to state
            u8 currentByte = result >> (24-(j*8));
            state[j][i] = currentByte;
            result = result ^ ( currentByte << (24-(j*8)));
        }
    }
}

/**
 * function in NISt standard, mixing up the column by a matrix multiplication with GF(2^8)
 * for cipher
 */
void mixColumns(u8 state[4][4]){
    for(int i = 0; i < 4 ; i++){
        u8 col[4] = {state[0][i], state[1][i], state[2][i], state[3][i]};    
        state[0][i] = gfMulti(0x02,col[0]) ^ gfMulti(0x03,col[1]) ^ col[2] ^ col[3];
        state[1][i] = col[0] ^ gfMulti(0x02,col[1]) ^ gfMulti(0x03,col[2]) ^ col[3];
        state[2][i] = col[0] ^ col[1] ^ gfMulti(0x02,col[2]) ^ gfMulti(0x03,col[3]);
        state[3][i] = gfMulti(0x03,col[0]) ^ col[1] ^ col[2] ^ gfMulti(0x02,col[3]);
    }
}

/**
 * function in NISt standard, mixing up the column by a matrix multiplication with GF(2^8)
 * for cipher
 */
void invMixColumns(u8 state[4][4]){
    for(int i = 0; i < 4 ; i++){
        u8 col[4] = {state[0][i], state[1][i], state[2][i], state[3][i]};    
        state[0][i] = gfMulti(0x0e,col[0]) ^ gfMulti(0x0b,col[1]) 
                        ^ gfMulti(0x0d,col[2]) ^ gfMulti(0x09,col[3]);
        state[1][i] = gfMulti(0x09,col[0]) ^ gfMulti(0x0e,col[1]) 
                        ^ gfMulti(0x0b,col[2]) ^ gfMulti(0x0d,col[3]);
        state[2][i] = gfMulti(0x0d,col[0]) ^ gfMulti(0x09,col[1]) 
                        ^ gfMulti(0x0e,col[2]) ^ gfMulti(0x0b,col[3]);
        state[3][i] = gfMulti(0x0b,col[0]) ^ gfMulti(0x0d,col[1]) 
                        ^ gfMulti(0x09,col[2]) ^ gfMulti(0x0e,col[3]);
    }
}



