// This file is part of libmbGB.
// Copyright (C) 2019 Buenia.
//
// libmbGB is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbGB is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbGB.  If not, see <https://www.gnu.org/licenses/>.

#ifndef LIBMBGB_MMU
#define LIBMBGB_MMU

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include "enums.h"
#include "libmbgb_api.h"
using namespace std;

namespace gb
{
    inline bool TestBit(uint32_t reg, int bit)
    {
	return (reg & (1 << bit)) ? true : false;
    }

    inline uint8_t BitSet(uint32_t reg, int bit)
    {
	return (reg | (1 << bit));
    }

    inline uint8_t BitReset(uint32_t reg, int bit)
    {
	return (reg & ~(1 << bit));
    }

    inline int BitGetVal(uint32_t reg, int bit)
    {
	return (reg & (1 << bit)) ? 1 : 0;
    }

    using poweronfunc = function<void(bool)>;
    using joypadfunc = function<void()>;

    class LIBMBGB_API MMU
    {
	public:
	    MMU();
	    ~MMU();

	    Console gameboy = Console::Default;
	    Mode gbmode;
	    MBCType gbmbc;

	    void init();
	    void shutdown();

	    vector<uint8_t> rom;
	    vector<uint8_t> vram;
	    vector<uint8_t> sram;
	    vector<uint8_t> wram;
	    vector<uint8_t> oam;
	    vector<uint8_t> hram;
	    vector<uint8_t> cartmem;
	    vector<uint8_t> bios;

	    bool externalrampres = false;
	    bool biosload = false;
	    int ramsize = 0;

	    void determinembctype(vector<uint8_t>& rom);
	    void determineramsize(vector<uint8_t>& rom);
	    string determinegametitle(vector<uint8_t>& rom);
	    string mbctype;

	    bool loadBIOS(string filename);
	    bool loadROM(string filename);

	    uint8_t readByte(uint16_t addr);
	    void writeByte(uint16_t addr, uint8_t value);
	    uint16_t readWord(uint16_t addr);
	    void writeWord(uint16_t addr, uint16_t value);

	    uint8_t readIO(uint16_t addr);
	    void writeIO(uint16_t addr, uint8_t value);

	    poweronfunc poweron;
	    joypadfunc updatep1;

	    inline void dodmatransfer(uint8_t value)
	    {		
		uint16_t addr = (value << 8);
		for (int i = 0; i < 0xA0; i++)
		{
		    writeByte((0xFE00 + i), readByte(addr + i));
		}
	    }

	    inline bool islcdenabled()
	    {
		return TestBit(lcdc, 7);
	    }

	    inline bool iswinenabled()
	    {
		return TestBit(lcdc, 5);
	    }

	    inline bool isobjenabled()
	    {
		return TestBit(lcdc, 1);
	    }

	    inline bool isbgenabled()
	    {
		return TestBit(lcdc, 0);
	    }

	    inline void setstatmode(int mode)
	    {
		stat = ((stat & 0xFC) | mode);
	    }

	    inline void writejoypad(uint8_t value)
	    {
		joypad = ((joypad & 0x0F) | (value & 0x30));
		updatep1();
	    }

	    inline void writelcdc(uint8_t value)
	    {
		bool lcdwasenabled = islcdenabled();

		lcdc = value;

		poweron(lcdwasenabled);
	    }

	    inline void writediv()
	    {
		divider = 0x0000;
	    }

	    void setpoweroncallback(poweronfunc cb)
	    {
		poweron = cb;
	    }

	    void setjoypadcallback(joypadfunc cb)
	    {
		updatep1 = cb;
	    }

	    inline void exitbios()
	    {
		biosload = false;
		cout << "MMU::Exiting BIOS..." << endl;
	    }

	    inline void requestinterrupt(int id)
	    {
		if (!ifwrittenthiscycle)
		{
		    interruptflags = BitSet(interruptflags, id);
		}
	    }

	    inline void clearinterrupt(int id)
	    {
		if (!ifwrittenthiscycle)
		{
		    interruptflags = BitReset(interruptflags, id);
		}
	    }

	    bool ispending(int id)
	    {
		uint8_t temp = (interruptflags & interruptenabled);
		return TestBit(temp, id);
	    }

	    bool requestedenabledinterrupts()
	    {
		return (interruptflags & interruptenabled);
	    }

	    inline void writesc(uint8_t value)
	    {
		sc = value;
	    }

	    inline void writeif(uint8_t value)
	    {
		interruptflags = (value & 0x1F);
		ifwrittenthiscycle = true;
	    }

	    inline void writestat(uint8_t value)
	    {
		int temp = (stat & 0x07);	
		stat = ((value & 0x78) | temp);
	    }

	    inline uint16_t readdiv()
	    {
		return divider;
	    }

	    inline void incdiv(int cycles)
	    {
		divider += cycles;
	    }

	    inline void setlycompare(bool cond)
	    {
		if (cond)
		{
		    stat = BitSet(stat, 2);
		}
		else
		{
		    stat = BitReset(stat, 2);
		}
	    }

	    inline void setstatsignal()
	    {
		statinterruptsignal = true;
	    }

	    bool ifwrittenthiscycle = false;
	

     	    bool statinterruptsignal = false;
	    bool previnterruptsignal = false;
	    bool bgpchanged = false;

	    uint8_t joypad = 0x00;
	    uint8_t sb = 0x00;
	    uint8_t sc = 0x00;
	    uint16_t divider = 0x0000;
	    uint8_t timercounter = 0x00;
	    uint8_t timermodulo = 0x00;
	    uint8_t timercontrol = 0x00;
	    uint8_t interruptflags = 0xE1;
	    uint8_t lcdc = 0x91;
	    uint8_t stat = 0x01;
	    uint8_t scrolly = 0x00;
	    uint8_t scrollx = 0x00;
	    uint8_t windowy = 0x00;
	    uint8_t windowx = 0x00;
	    uint8_t ly = 0x00;
	    uint8_t lyc = 0x00;
	    uint8_t bgpalette = 0xFC;
	    uint8_t objpalette0 = 0xFF;
	    uint8_t objpalette1 = 0xFF;
	    uint8_t lastbgp = 0x00;
	    uint8_t interruptenabled = 0x00;

	    uint8_t lylastcycle = 0xFF;
    };
};

#endif // LIBMBGB_MMU
