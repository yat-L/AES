/*
 * Wirtten by Ho Yat Lui
 * Student Number: 7780269
 */
#ifndef AES_HEADER
#define AES_HEADER

#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint8_t u8;

void initialize();

void subBytes(u8 state[4][4]);

void invSubBytes(u8 state[4][4]);

void shiftRows(u8 state[4][4]);

void invShiftRows(u8 state[4][4]);

void keyExpansion();

void readPlain(u8 p[4][4],char* fName);

void addRoundKey(u8 state[4][4], int start, int end);

void mixColumns(u8 state[4][4]);

void invMixColumns(u8 state[4][4]);

#endif
