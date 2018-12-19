#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>
using namespace std;

bool TestBit(uint8_t reg, int bit);

uint8_t BitSet(uint8_t reg, int bit);

uint8_t BitReset(uint8_t reg, int bit);

int BitGetVal(uint8_t reg, int bit);

uint8_t BitSetTo(uint8_t reg, int bit, bool set);

string tohexstring(uint32_t val);
string getmnemonic(uint8_t opcode, uint16_t nn, uint8_t n);
string getextmnemonic(uint8_t opcode);


#endif // UTIL_H
