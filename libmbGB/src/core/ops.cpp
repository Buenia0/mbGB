// This file is part of libmbGB.
// Copyright (C) 2021 Buenia.
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

#include "../../include/libmbGB/cpu.h"
using namespace gb;

namespace gb
{
    int CPU::executenextopcode(uint8_t opcode)
    {
	hardwaretick(4);
	int cycles = 0;

	switch (opcode)
	{
	    case 0x00: cycles = 4; break;
	    case 0x01: bc.setreg(getimmword()); cycles = 12; break;
	    case 0x02: load8intomem(bc.getreg(), af.hi); cycles = 8; break;
	    case 0x03: bc.setreg(increg(bc.getreg())); cycles = 8; break;
	    case 0x04: bc.hi = (incregbyte(bc.hi)); cycles = 4; break;
	    case 0x05: bc.hi = (decregbyte(bc.hi)); cycles = 4; break;
	    case 0x06: bc.hi = (getimmbyte()); cycles = 8; break;
	    case 0x07: af.hi = (rlc(af.hi)); setzero(false); cycles = 4; break;
	    case 0x08: load16intomem(getimmword(), sp); cycles = 20; break;
	    case 0x09: hl.setreg(add16reg(hl.getreg(), bc.getreg())); cycles = 8; break;
	    case 0x0A: af.hi = (store8frommem(bc.getreg())); cycles = 8; break;
	    case 0x0B: bc.setreg(decreg(bc.getreg())); cycles = 8; break;
	    case 0x0C: bc.lo = (incregbyte(bc.lo)); cycles = 4; break;
	    case 0x0D: bc.lo = (decregbyte(bc.lo)); cycles = 4; break;
	    case 0x0E: bc.lo = (getimmbyte()); cycles = 8; break;
	    case 0x0F: af.hi = (rrc(af.hi)); setzero(false); cycles = 4; break;
	    case 0x10:
	    {	
		if (mem.isgbcmode())
		{
		    doubleexec();
		}
		else
		{
		    stop();
		}

		cycles = 4;
	    }
	    break;
	    case 0x11: de.setreg(getimmword()); cycles = 12; break;
	    case 0x12: load8intomem(de.getreg(), af.hi); cycles = 8; break;
	    case 0x13: de.setreg(increg(de.getreg())); cycles = 8; break;
	    case 0x14: de.hi = (incregbyte(de.hi)); cycles = 4; break;
	    case 0x15: de.hi = (decregbyte(de.hi)); cycles = 4; break;
	    case 0x16: de.hi = (getimmbyte()); cycles = 8; break;
	    case 0x17: af.hi = (rl(af.hi)); setzero(false); cycles = 4; break;
	    case 0x18: reljump(getimmsignedbyte()); cycles = 12; break;
	    case 0x19: hl.setreg(add16reg(hl.getreg(), de.getreg())); cycles = 8; break;
	    case 0x1A: af.hi = (store8frommem(de.getreg())); cycles = 8; break;
	    case 0x1B: de.setreg(decreg(de.getreg())); cycles = 8; break;
	    case 0x1C: de.lo = (incregbyte(de.lo)); cycles = 4; break;
	    case 0x1D: de.lo = (decregbyte(de.lo)); cycles = 4; break;
	    case 0x1E: de.lo = (getimmbyte()); cycles = 8; break;
	    case 0x1F: af.hi = (rr(af.hi)); setzero(false); cycles = 4; break;
	    case 0x20: cycles = reljumpcond(getimmsignedbyte(), !iszero()); break;
	    case 0x21: hl.setreg(getimmword()); cycles = 12; break;
	    case 0x22: load8intomem(hl.getreg(), af.hi); hl.setreg(hl.getreg() + 1); cycles = 8; break;
	    case 0x23: hl.setreg(increg(hl.getreg())); cycles = 8; break;
	    case 0x24: hl.hi = (incregbyte(hl.hi)); cycles = 4; break;
	    case 0x25: hl.hi = (decregbyte(hl.hi)); cycles = 4; break;
	    case 0x26: hl.hi = (getimmbyte()); cycles = 8; break;
	    case 0x27: daa(); cycles = 4; break;
	    case 0x28: cycles = reljumpcond(getimmsignedbyte(), iszero()); break;
	    case 0x29: hl.setreg(add16reg(hl.getreg(), hl.getreg())); cycles = 8; break;
	    case 0x2A: af.hi = (store8frommem(hl.getreg())); hl.setreg(hl.getreg() + 1); cycles = 8; break;
	    case 0x2B: hl.setreg(decreg(hl.getreg())); cycles = 8; break;
	    case 0x2C: hl.lo = (incregbyte(hl.lo)); cycles = 4; break;
	    case 0x2D: hl.lo = (decregbyte(hl.lo)); cycles = 4; break;
	    case 0x2E: hl.lo = (getimmbyte()); cycles = 8; break;
	    case 0x2F: compareg(); cycles = 4; break;
	    case 0x30: cycles = reljumpcond(getimmsignedbyte(), !iscarry()); break;
	    case 0x31: sp = getimmword(); cycles = 12; break;
	    case 0x32: load8intomem(hl.getreg(), af.hi); hl.setreg(hl.getreg() - 1); cycles = 8; break;
	    case 0x33: sp = increg(sp); cycles = 8; break;
	    case 0x34: load8intomem(hl.getreg(), incregbyte(store8frommem(hl.getreg()))); cycles = 12; break;
	    case 0x35: load8intomem(hl.getreg(), decregbyte(store8frommem(hl.getreg()))); cycles = 12; break;
	    case 0x36: load8intomem(hl.getreg(), getimmbyte()); cycles = 12; break;
	    case 0x37: setcarryflag(); cycles = 4; break;
	    case 0x38: cycles = reljumpcond(getimmsignedbyte(), iscarry()); break;
	    case 0x39: hl.setreg(add16reg(hl.getreg(), sp)); cycles = 8; break;
	    case 0x3A: af.hi = (store8frommem(hl.getreg())); hl.setreg(hl.getreg() - 1); cycles = 8; break;
	    case 0x3B: sp = decreg(sp); cycles = 8; break;
	    case 0x3C: af.hi = (incregbyte(af.hi)); cycles = 4; break;
	    case 0x3D: af.hi = (decregbyte(af.hi)); cycles = 4; break;
	    case 0x3E: af.hi = (getimmbyte()); cycles = 8; break;
	    case 0x3F: compcarryflag(); cycles = 4; break;
	    case 0x40: bc.hi = (bc.hi); cycles = 4; break;
	    case 0x41: bc.hi = (bc.lo); cycles = 4; break;
	    case 0x42: bc.hi = (de.hi); cycles = 4; break;
	    case 0x43: bc.hi = (de.lo); cycles = 4; break;
	    case 0x44: bc.hi = (hl.hi); cycles = 4; break;
	    case 0x45: bc.hi = (hl.lo); cycles = 4; break;
	    case 0x46: bc.hi = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x47: bc.hi = (af.hi); cycles = 4; break;
	    case 0x48: bc.lo = (bc.hi); cycles = 4; break;
	    case 0x49: bc.lo = (bc.lo); cycles = 4; break;
	    case 0x4A: bc.lo = (de.hi); cycles = 4; break;
	    case 0x4B: bc.lo = (de.lo); cycles = 4; break;
	    case 0x4C: bc.lo = (hl.hi); cycles = 4; break;
	    case 0x4D: bc.lo = (hl.lo); cycles = 4; break;
	    case 0x4E: bc.lo = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x4F: bc.lo = (af.hi); cycles = 4; break;
	    case 0x50: de.hi = (bc.hi); cycles = 4; break;
	    case 0x51: de.hi = (bc.lo); cycles = 4; break;
	    case 0x52: de.hi = (de.hi); cycles = 4; break;
	    case 0x53: de.hi = (de.lo); cycles = 4; break;
	    case 0x54: de.hi = (hl.hi); cycles = 4; break;
	    case 0x55: de.hi = (hl.lo); cycles = 4; break;
	    case 0x56: de.hi = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x57: de.hi = (af.hi); cycles = 4; break;
	    case 0x58: de.lo = (bc.hi); cycles = 4; break;
	    case 0x59: de.lo = (bc.lo); cycles = 4; break;
	    case 0x5A: de.lo = (de.hi); cycles = 4; break;
	    case 0x5B: de.lo = (de.lo); cycles = 4; break;
	    case 0x5C: de.lo = (hl.hi); cycles = 4; break;
	    case 0x5D: de.lo = (hl.lo); cycles = 4; break;
	    case 0x5E: de.lo = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x5F: de.lo = (af.hi); cycles = 4; break;
	    case 0x60: hl.hi = (bc.hi); cycles = 4; break;
	    case 0x61: hl.hi = (bc.lo); cycles = 4; break;
	    case 0x62: hl.hi = (de.hi); cycles = 4; break;
	    case 0x63: hl.hi = (de.lo); cycles = 4; break;
	    case 0x64: hl.hi = (hl.hi); cycles = 4; break;
	    case 0x65: hl.hi = (hl.lo); cycles = 4; break;
	    case 0x66: hl.hi = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x67: hl.hi = (af.hi); cycles = 4; break;
	    case 0x68: hl.lo = (bc.hi); cycles = 4; break;
	    case 0x69: hl.lo = (bc.lo); cycles = 4; break;
	    case 0x6A: hl.lo = (de.hi); cycles = 4; break;
	    case 0x6B: hl.lo = (de.lo); cycles = 4; break;
	    case 0x6C: hl.lo = (hl.hi); cycles = 4; break;
	    case 0x6D: hl.lo = (hl.lo); cycles = 4; break;
	    case 0x6E: hl.lo = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x6F: hl.lo = (af.hi); cycles = 4; break;
	    case 0x70: load8intomem(hl.getreg(), bc.hi); cycles = 8; break;
	    case 0x71: load8intomem(hl.getreg(), bc.lo); cycles = 8; break;
	    case 0x72: load8intomem(hl.getreg(), de.hi); cycles = 8; break;
	    case 0x73: load8intomem(hl.getreg(), de.lo); cycles = 8; break;
	    case 0x74: load8intomem(hl.getreg(), hl.hi); cycles = 8; break;
	    case 0x75: load8intomem(hl.getreg(), hl.lo); cycles = 8; break;
	    case 0x76: halt(); cycles = 4; break;
	    case 0x77: load8intomem(hl.getreg(), af.hi); cycles = 8; break;
	    case 0x78: af.hi = (bc.hi); cycles = 4; break;
	    case 0x79: af.hi = (bc.lo); cycles = 4; break;
	    case 0x7A: af.hi = (de.hi); cycles = 4; break;
	    case 0x7B: af.hi = (de.lo); cycles = 4; break;
	    case 0x7C: af.hi = (hl.hi); cycles = 4; break;
	    case 0x7D: af.hi = (hl.lo); cycles = 4; break;
	    case 0x7E: af.hi = (store8frommem(hl.getreg())); cycles = 8; break;
	    case 0x7F: af.hi = (af.hi); cycles = 4; break;
	    case 0x80: af.hi = (addreg(bc.hi)); cycles = 4; break;
	    case 0x81: af.hi = (addreg(bc.lo)); cycles = 4; break;
	    case 0x82: af.hi = (addreg(de.hi)); cycles = 4; break;
	    case 0x83: af.hi = (addreg(de.lo)); cycles = 4; break;
	    case 0x84: af.hi = (addreg(hl.hi)); cycles = 4; break;
	    case 0x85: af.hi = (addreg(hl.lo)); cycles = 4; break;
	    case 0x86: af.hi = (addreg(store8frommem(hl.getreg()))); cycles = 8; break;
	    case 0x87: af.hi = (addreg(af.hi)); cycles = 4; break;
	    case 0x88: af.hi = (adcreg(bc.hi)); cycles = 4; break;
	    case 0x89: af.hi = (adcreg(bc.lo)); cycles = 4; break;
	    case 0x8A: af.hi = (adcreg(de.hi)); cycles = 4; break;
	    case 0x8B: af.hi = (adcreg(de.lo)); cycles = 4; break;
	    case 0x8C: af.hi = (adcreg(hl.hi)); cycles = 4; break;
	    case 0x8D: af.hi = (adcreg(hl.lo)); cycles = 4; break;
	    case 0x8E: af.hi = (adcreg(store8frommem(hl.getreg()))); cycles = 8; break;
	    case 0x8F: af.hi = (adcreg(af.hi)); cycles = 4; break;
	    case 0x90: af.hi = (subreg(bc.hi)); cycles = 4; break;
	    case 0x91: af.hi = (subreg(bc.lo)); cycles = 4; break;
	    case 0x92: af.hi = (subreg(de.hi)); cycles = 4; break;
	    case 0x93: af.hi = (subreg(de.lo)); cycles = 4; break;
	    case 0x94: af.hi = (subreg(hl.hi)); cycles = 4; break;
	    case 0x95: af.hi = (subreg(hl.lo)); cycles = 4; break;
	    case 0x96: af.hi = (subreg(store8frommem(hl.getreg()))); cycles = 8; break;
	    case 0x97: af.hi = (subreg(af.hi)); cycles = 4; break;
	    case 0x98: af.hi = (sbcreg(bc.hi)); cycles = 4; break;
	    case 0x99: af.hi = (sbcreg(bc.lo)); cycles = 4; break;
	    case 0x9A: af.hi = (sbcreg(de.hi)); cycles = 4; break;
	    case 0x9B: af.hi = (sbcreg(de.lo)); cycles = 4; break;
	    case 0x9C: af.hi = (sbcreg(hl.hi)); cycles = 4; break;
	    case 0x9D: af.hi = (sbcreg(hl.lo)); cycles = 4; break;
	    case 0x9E: af.hi = (sbcreg(store8frommem(hl.getreg()))); cycles = 8; break;
	    case 0x9F: af.hi = (sbcreg(af.hi)); cycles = 4; break;
	    case 0xA0: andreg(bc.hi); cycles = 4; break;
	    case 0xA1: andreg(bc.lo); cycles = 4; break;
	    case 0xA2: andreg(de.hi); cycles = 4; break;
	    case 0xA3: andreg(de.lo); cycles = 4; break;
	    case 0xA4: andreg(hl.hi); cycles = 4; break;
	    case 0xA5: andreg(hl.lo); cycles = 4; break;
	    case 0xA6: andreg(store8frommem(hl.getreg())); cycles = 8; break;
	    case 0xA7: andreg(af.hi); cycles = 4; break;
	    case 0xA8: xorreg(bc.hi); cycles = 4; break;
	    case 0xA9: xorreg(bc.lo); cycles = 4; break;
	    case 0xAA: xorreg(de.hi); cycles = 4; break;
	    case 0xAB: xorreg(de.lo); cycles = 4; break;
	    case 0xAC: xorreg(hl.hi); cycles = 4; break;
	    case 0xAD: xorreg(hl.lo); cycles = 4; break;
	    case 0xAE: xorreg(store8frommem(hl.getreg())); cycles = 8; break;
	    case 0xAF: xorreg(af.hi); cycles = 4; break;
	    case 0xB0: orreg(bc.hi); cycles = 4; break;
	    case 0xB1: orreg(bc.lo); cycles = 4; break;
	    case 0xB2: orreg(de.hi); cycles = 4; break;
	    case 0xB3: orreg(de.lo); cycles = 4; break;
	    case 0xB4: orreg(hl.hi); cycles = 4; break;
	    case 0xB5: orreg(hl.lo); cycles = 4; break;
	    case 0xB6: orreg(store8frommem(hl.getreg())); cycles = 8; break;
	    case 0xB7: orreg(af.hi); cycles = 4; break;
	    case 0xB8: cmpreg(bc.hi); cycles = 4; break;
	    case 0xB9: cmpreg(bc.lo); cycles = 4; break;
	    case 0xBA: cmpreg(de.hi); cycles = 4; break;
	    case 0xBB: cmpreg(de.lo); cycles = 4; break;
	    case 0xBC: cmpreg(hl.hi); cycles = 4; break;
	    case 0xBD: cmpreg(hl.lo); cycles = 4; break;
	    case 0xBE: cmpreg(store8frommem(hl.getreg())); cycles = 8; break;
	    case 0xBF: cmpreg(af.hi); cycles = 4; break;
	    case 0xC0: cycles = retcond(!iszero()); break;
	    case 0xC1: bc.setreg(pop()); cycles = 12; break;
	    case 0xC2: cycles = jumpcond(getimmword(), !iszero()); break;
	    case 0xC3: jump(getimmword()); cycles = 16; break;
	    case 0xC4: cycles = callcond(getimmword(), !iszero()); break;
	    case 0xC5: push(bc.getreg()); cycles = 16; break;
	    case 0xC6: af.hi = (addreg(getimmbyte())); cycles = 8; break;
	    case 0xC7: call(0x00); cycles = 16; break;
	    case 0xC8: cycles = retcond(iszero()); break;
	    case 0xC9: ret(); cycles = 16; break;
	    case 0xCA: cycles = jumpcond(getimmword(), iszero()); break;
	    case 0xCB: cycles = executenextcbopcode(getimmbyte()); break;
	    case 0xCC: cycles = callcond(getimmword(), iszero()); break;
	    case 0xCD: call(getimmword()); cycles = 24; break;
	    case 0xCE: af.hi = (adcreg(getimmbyte())); cycles = 8; break;
	    case 0xCF: call(0x08); cycles = 16; break;
	    case 0xD0: cycles = retcond(!iscarry()); break;
	    case 0xD1: de.setreg(pop()); cycles = 12; break;
	    case 0xD2: cycles = jumpcond(getimmword(), !iscarry()); break;
	    case 0xD4: cycles = callcond(getimmword(), !iscarry()); break;
	    case 0xD5: push(de.getreg()); cycles = 16; break;
	    case 0xD6: af.hi = (subreg(getimmbyte())); cycles = 8; break;
	    case 0xD7: call(0x10); cycles = 16; break;
	    case 0xD8: cycles = retcond(iscarry()); break;
	    case 0xD9: ret(); interruptmasterenable = true; cycles = 16; break;
	    case 0xDA: cycles = jumpcond(getimmword(), iscarry()); break;
	    case 0xDC: cycles = callcond(getimmword(), iscarry()); break;
	    case 0xDE: af.hi = (sbcreg(getimmbyte())); cycles = 8; break;
	    case 0xDF: call(0x18); cycles = 16; break;
	    case 0xE0: load8intomem((0xFF00 + getimmbyte()), af.hi); cycles = 12; break;
	    case 0xE1: hl.setreg(pop()); cycles = 12; break;
	    case 0xE2: load8intomem((0xFF00 + bc.lo), af.hi); cycles = 8; break;
	    case 0xE5: push(hl.getreg()); cycles = 16; break;
	    case 0xE6: andreg(getimmbyte()); cycles = 8; break;
	    case 0xE7: call(0x20); cycles = 16; break;
	    case 0xE8: addsp(getimmsignedbyte()); cycles = 16; break;
	    case 0xE9: jumptohl(); cycles = 4; break;
	    case 0xEA: load8intomem(getimmword(), af.hi); cycles = 16; break;
	    case 0xEE: xorreg(getimmbyte()); cycles = 8; break;
	    case 0xEF: call(0x28); cycles = 16; break;
	    case 0xF0: af.hi = (store8frommem((0xFF00 + getimmbyte()))); cycles = 12; break;
	    case 0xF1: af.setreg(pop()); af.lo = ((af.lo & 0xF0)); cycles = 12; break;
	    case 0xF2: af.hi = (store8frommem((0xFF00 + bc.lo))); cycles = 8; break;
	    case 0xF3: interruptmasterenable = false; cycles = 4; break;
	    case 0xF5: push(af.getreg()); cycles = 16; break;
	    case 0xF6: orreg(getimmbyte()); cycles = 8; break;
	    case 0xF7: call(0x30); cycles = 16; break;
	    case 0xF8: hl.setreg(loadspn(getimmsignedbyte())); cycles = 12; break;
	    case 0xF9: loadhltosp(); cycles = 8; break;
	    case 0xFA: af.hi = (store8frommem(getimmword())); cycles = 16; break;
	    case 0xFB: enableinterruptsdelayed = true; cycles = 4; break;
	    case 0xFE: cmpreg(getimmbyte()); cycles = 8; break;
	    case 0xFF: call(0x38); cycles = 16; break;
	    default: cout << "Unrecognized opcode at 0x" << uppercase << hex << (int)(opcode) << endl; printregs(); exit(1); break;
	}

	return cycles;
    }

    int CPU::executenextcbopcode(uint8_t opcode)
    {
	int cycles = 0;	

	switch (opcode)
	{
	    case 0x00: bc.hi = (rlc(bc.hi)); cycles = 8; break;
	    case 0x01: bc.lo = (rlc(bc.lo)); cycles = 8; break;
	    case 0x02: de.hi = (rlc(de.hi)); cycles = 8; break;
	    case 0x03: de.lo = (rlc(de.lo)); cycles = 8; break;
	    case 0x04: hl.hi = (rlc(hl.hi)); cycles = 8; break;
	    case 0x05: hl.lo = (rlc(hl.lo)); cycles = 8; break;
	    case 0x06: load8intomem(hl.getreg(), rlc(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x07: af.hi = (rlc(af.hi)); cycles = 8; break;
	    case 0x08: bc.hi = (rrc(bc.hi)); cycles = 8; break;
	    case 0x09: bc.lo = (rrc(bc.lo)); cycles = 8; break;
	    case 0x0A: de.hi = (rrc(de.hi)); cycles = 8; break;
	    case 0x0B: de.lo = (rrc(de.lo)); cycles = 8; break;
	    case 0x0C: hl.hi = (rrc(hl.hi)); cycles = 8; break;
	    case 0x0D: hl.lo = (rrc(hl.lo)); cycles = 8; break;
	    case 0x0E: load8intomem(hl.getreg(), rrc(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x0F: af.hi = (rrc(af.hi)); cycles = 8; break;
	    case 0x10: bc.hi = (rl(bc.hi)); cycles = 8; break;
	    case 0x11: bc.lo = (rl(bc.lo)); cycles = 8; break;
	    case 0x12: de.hi = (rl(de.hi)); cycles = 8; break;
	    case 0x13: de.lo = (rl(de.lo)); cycles = 8; break;
	    case 0x14: hl.hi = (rl(hl.hi)); cycles = 8; break;
	    case 0x15: hl.lo = (rl(hl.lo)); cycles = 8; break;
	    case 0x16: load8intomem(hl.getreg(), rl(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x17: af.hi = (rl(af.hi)); cycles = 8; break;
	    case 0x18: bc.hi = (rr(bc.hi)); cycles = 8; break;
	    case 0x19: bc.lo = (rr(bc.lo)); cycles = 8; break;
	    case 0x1A: de.hi = (rr(de.hi)); cycles = 8; break;
	    case 0x1B: de.lo = (rr(de.lo)); cycles = 8; break;
	    case 0x1C: hl.hi = (rr(hl.hi)); cycles = 8; break;
	    case 0x1D: hl.lo = (rr(hl.lo)); cycles = 8; break;
	    case 0x1E: load8intomem(hl.getreg(), rr(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x1F: af.hi = (rr(af.hi)); cycles = 8; break;
	    case 0x20: bc.hi = (sla(bc.hi)); cycles = 8; break;
	    case 0x21: bc.lo = (sla(bc.lo)); cycles = 8; break;
	    case 0x22: de.hi = (sla(de.hi)); cycles = 8; break;
	    case 0x23: de.lo = (sla(de.lo)); cycles = 8; break;
	    case 0x24: hl.hi = (sla(hl.hi)); cycles = 8; break;
	    case 0x25: hl.lo = (sla(hl.lo)); cycles = 8; break;
	    case 0x26: load8intomem(hl.getreg(), sla(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x27: af.hi = (sla(af.hi)); cycles = 8; break;
	    case 0x28: bc.hi = (sra(bc.hi)); cycles = 8; break;
	    case 0x29: bc.lo = (sra(bc.lo)); cycles = 8; break;
	    case 0x2A: de.hi = (sra(de.hi)); cycles = 8; break;
	    case 0x2B: de.lo = (sra(de.lo)); cycles = 8; break;
	    case 0x2C: hl.hi = (sra(hl.hi)); cycles = 8; break;
	    case 0x2D: hl.lo = (sra(hl.lo)); cycles = 8; break;
	    case 0x2E: load8intomem(hl.getreg(), sra(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x2F: af.hi = (sra(af.hi)); cycles = 8; break;
	    case 0x30: bc.hi = (swap(bc.hi)); cycles = 8; break;
	    case 0x31: bc.lo = (swap(bc.lo)); cycles = 8; break;
	    case 0x32: de.hi = (swap(de.hi)); cycles = 8; break;
	    case 0x33: de.lo = (swap(de.lo)); cycles = 8; break;
	    case 0x34: hl.hi = (swap(hl.hi)); cycles = 8; break;
	    case 0x35: hl.lo = (swap(hl.lo)); cycles = 8; break;
	    case 0x36: load8intomem(hl.getreg(), swap(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x37: af.hi = (swap(af.hi)); cycles = 8; break;
	    case 0x38: bc.hi = (srl(bc.hi)); cycles = 8; break;
	    case 0x39: bc.lo = (srl(bc.lo)); cycles = 8; break;
	    case 0x3A: de.hi = (srl(de.hi)); cycles = 8; break;
	    case 0x3B: de.lo = (srl(de.lo)); cycles = 8; break;
	    case 0x3C: hl.hi = (srl(hl.hi)); cycles = 8; break;
	    case 0x3D: hl.lo = (srl(hl.lo)); cycles = 8; break;
	    case 0x3E: load8intomem(hl.getreg(), srl(store8frommem(hl.getreg()))); cycles = 16; break;
	    case 0x3F: af.hi = (srl(af.hi)); cycles = 8; break;
	    case 0x40: bit(bc.hi, 0); cycles = 8; break;
	    case 0x41: bit(bc.lo, 0); cycles = 8; break;
	    case 0x42: bit(de.hi, 0); cycles = 8; break;
	    case 0x43: bit(de.lo, 0); cycles = 8; break;
	    case 0x44: bit(hl.hi, 0); cycles = 8; break;
	    case 0x45: bit(hl.lo, 0); cycles = 8; break;
	    case 0x46: bit(store8frommem(hl.getreg()), 0); cycles = 12; break;
	    case 0x47: bit(af.hi, 0); cycles = 8; break;
	    case 0x48: bit(bc.hi, 1); cycles = 8; break;
	    case 0x49: bit(bc.lo, 1); cycles = 8; break;
	    case 0x4A: bit(de.hi, 1); cycles = 8; break;
	    case 0x4B: bit(de.lo, 1); cycles = 8; break;
	    case 0x4C: bit(hl.hi, 1); cycles = 8; break;
	    case 0x4D: bit(hl.lo, 1); cycles = 8; break;
	    case 0x4E: bit(store8frommem(hl.getreg()), 1); cycles = 12; break;
	    case 0x4F: bit(af.hi, 1); cycles = 8; break;
	    case 0x50: bit(bc.hi, 2); cycles = 8; break;
	    case 0x51: bit(bc.lo, 2); cycles = 8; break;
	    case 0x52: bit(de.hi, 2); cycles = 8; break;
	    case 0x53: bit(de.lo, 2); cycles = 8; break;
	    case 0x54: bit(hl.hi, 2); cycles = 8; break;
	    case 0x55: bit(hl.lo, 2); cycles = 8; break;
	    case 0x56: bit(store8frommem(hl.getreg()), 2); cycles = 12; break;
	    case 0x57: bit(af.hi, 2); cycles = 8; break;
	    case 0x58: bit(bc.hi, 3); cycles = 8; break;
	    case 0x59: bit(bc.lo, 3); cycles = 8; break;
	    case 0x5A: bit(de.hi, 3); cycles = 8; break;
	    case 0x5B: bit(de.lo, 3); cycles = 8; break;
	    case 0x5C: bit(hl.hi, 3); cycles = 8; break;
	    case 0x5D: bit(hl.lo, 3); cycles = 8; break;
	    case 0x5E: bit(store8frommem(hl.getreg()), 3); cycles = 12; break;
	    case 0x5F: bit(af.hi, 3); cycles = 8; break;
	    case 0x60: bit(bc.hi, 4); cycles = 8; break;
	    case 0x61: bit(bc.lo, 4); cycles = 8; break;
	    case 0x62: bit(de.hi, 4); cycles = 8; break;
	    case 0x63: bit(de.lo, 4); cycles = 8; break;
	    case 0x64: bit(hl.hi, 4); cycles = 8; break;
	    case 0x65: bit(hl.lo, 4); cycles = 8; break;
	    case 0x66: bit(store8frommem(hl.getreg()), 4); cycles = 12; break;
	    case 0x67: bit(af.hi, 4); cycles = 8; break;
	    case 0x68: bit(bc.hi, 5); cycles = 8; break;
	    case 0x69: bit(bc.lo, 5); cycles = 8; break;
	    case 0x6A: bit(de.hi, 5); cycles = 8; break;
	    case 0x6B: bit(de.lo, 5); cycles = 8; break;
	    case 0x6C: bit(hl.hi, 5); cycles = 8; break;
	    case 0x6D: bit(hl.lo, 5); cycles = 8; break;
	    case 0x6E: bit(store8frommem(hl.getreg()), 5); cycles = 12; break;
	    case 0x6F: bit(af.hi, 5); cycles = 8; break;
	    case 0x70: bit(bc.hi, 6); cycles = 8; break;
	    case 0x71: bit(bc.lo, 6); cycles = 8; break;
	    case 0x72: bit(de.hi, 6); cycles = 8; break;
	    case 0x73: bit(de.lo, 6); cycles = 8; break;
	    case 0x74: bit(hl.hi, 6); cycles = 8; break;
	    case 0x75: bit(hl.lo, 6); cycles = 8; break;
	    case 0x76: bit(store8frommem(hl.getreg()), 6); cycles = 12; break;
	    case 0x77: bit(af.hi, 6); cycles = 8; break;
	    case 0x78: bit(bc.hi, 7); cycles = 8; break;
	    case 0x79: bit(bc.lo, 7); cycles = 8; break;
	    case 0x7A: bit(de.hi, 7); cycles = 8; break;
	    case 0x7B: bit(de.lo, 7); cycles = 8; break;
	    case 0x7C: bit(hl.hi, 7); cycles = 8; break;
	    case 0x7D: bit(hl.lo, 7); cycles = 8; break;
	    case 0x7E: bit(store8frommem(hl.getreg()), 7); cycles = 12; break;
	    case 0x7F: bit(af.hi, 7); cycles = 8; break;
	    case 0x80: bc.hi = (res(bc.hi, 0)); cycles = 8; break;
	    case 0x81: bc.lo = (res(bc.lo, 0)); cycles = 8; break;
	    case 0x82: de.hi = (res(de.hi, 0)); cycles = 8; break;
	    case 0x83: de.lo = (res(de.lo, 0)); cycles = 8; break;
	    case 0x84: hl.hi = (res(hl.hi, 0)); cycles = 8; break;
	    case 0x85: hl.lo = (res(hl.lo, 0)); cycles = 8; break;
	    case 0x86: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 0)); cycles = 16; break;
	    case 0x87: af.hi = (res(af.hi, 0)); cycles = 8; break;
	    case 0x88: bc.hi = (res(bc.hi, 1)); cycles = 8; break;
	    case 0x89: bc.lo = (res(bc.lo, 1)); cycles = 8; break;
	    case 0x8A: de.hi = (res(de.hi, 1)); cycles = 8; break;
	    case 0x8B: de.lo = (res(de.lo, 1)); cycles = 8; break;
	    case 0x8C: hl.hi = (res(hl.hi, 1)); cycles = 8; break;
	    case 0x8D: hl.lo = (res(hl.lo, 1)); cycles = 8; break;
	    case 0x8E: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 1)); cycles = 16; break;
	    case 0x8F: af.hi = (res(af.hi, 1)); cycles = 8; break;
	    case 0x90: bc.hi = (res(bc.hi, 2)); cycles = 8; break;
	    case 0x91: bc.lo = (res(bc.lo, 2)); cycles = 8; break;
	    case 0x92: de.hi = (res(de.hi, 2)); cycles = 8; break;
	    case 0x93: de.lo = (res(de.lo, 2)); cycles = 8; break;
	    case 0x94: hl.hi = (res(hl.hi, 2)); cycles = 8; break;
	    case 0x95: hl.lo = (res(hl.lo, 2)); cycles = 8; break;
	    case 0x96: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 2)); cycles = 16; break;
	    case 0x97: af.hi = (res(af.hi, 2)); cycles = 8; break;
	    case 0x98: bc.hi = (res(bc.hi, 3)); cycles = 8; break;
	    case 0x99: bc.lo = (res(bc.lo, 3)); cycles = 8; break;
	    case 0x9A: de.hi = (res(de.hi, 3)); cycles = 8; break;
	    case 0x9B: de.lo = (res(de.lo, 3)); cycles = 8; break;
	    case 0x9C: hl.hi = (res(hl.hi, 3)); cycles = 8; break;
	    case 0x9D: hl.lo = (res(hl.lo, 3)); cycles = 8; break;
	    case 0x9E: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 3)); cycles = 16; break;
	    case 0x9F: af.hi = (res(af.hi, 3)); cycles = 8; break;
	    case 0xA0: bc.hi = (res(bc.hi, 4)); cycles = 8; break;
	    case 0xA1: bc.lo = (res(bc.lo, 4)); cycles = 8; break;
	    case 0xA2: de.hi = (res(de.hi, 4)); cycles = 8; break;
	    case 0xA3: de.lo = (res(de.lo, 4)); cycles = 8; break;
	    case 0xA4: hl.hi = (res(hl.hi, 4)); cycles = 8; break;
	    case 0xA5: hl.lo = (res(hl.lo, 4)); cycles = 8; break;
	    case 0xA6: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 4)); cycles = 16; break;
	    case 0xA7: af.hi = (res(af.hi, 4)); cycles = 8; break;
	    case 0xA8: bc.hi = (res(bc.hi, 5)); cycles = 8; break;
	    case 0xA9: bc.lo = (res(bc.lo, 5)); cycles = 8; break;
	    case 0xAA: de.hi = (res(de.hi, 5)); cycles = 8; break;
	    case 0xAB: de.lo = (res(de.lo, 5)); cycles = 8; break;
	    case 0xAC: hl.hi = (res(hl.hi, 5)); cycles = 8; break;
	    case 0xAD: hl.lo = (res(hl.lo, 5)); cycles = 8; break;
	    case 0xAE: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 5)); cycles = 16; break;
	    case 0xAF: af.hi = (res(af.hi, 5)); cycles = 8; break;
	    case 0xB0: bc.hi = (res(bc.hi, 6)); cycles = 8; break;
	    case 0xB1: bc.lo = (res(bc.lo, 6)); cycles = 8; break;
	    case 0xB2: de.hi = (res(de.hi, 6)); cycles = 8; break;
	    case 0xB3: de.lo = (res(de.lo, 6)); cycles = 8; break;
	    case 0xB4: hl.hi = (res(hl.hi, 6)); cycles = 8; break;
	    case 0xB5: hl.lo = (res(hl.lo, 6)); cycles = 8; break;
	    case 0xB6: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 6)); cycles = 16; break;
	    case 0xB7: af.hi = (res(af.hi, 6)); cycles = 8; break;
	    case 0xB8: bc.hi = (res(bc.hi, 7)); cycles = 8; break;
	    case 0xB9: bc.lo = (res(bc.lo, 7)); cycles = 8; break;
	    case 0xBA: de.hi = (res(de.hi, 7)); cycles = 8; break;
	    case 0xBB: de.lo = (res(de.lo, 7)); cycles = 8; break;
	    case 0xBC: hl.hi = (res(hl.hi, 7)); cycles = 8; break;
	    case 0xBD: hl.lo = (res(hl.lo, 7)); cycles = 8; break;
	    case 0xBE: load8intomem(hl.getreg(), res(store8frommem(hl.getreg()), 7)); cycles = 16; break;
	    case 0xBF: af.hi = (res(af.hi, 7)); cycles = 8; break;
	    case 0xC0: bc.hi = (set(bc.hi, 0)); cycles = 8; break;
	    case 0xC1: bc.lo = (set(bc.lo, 0)); cycles = 8; break;
	    case 0xC2: de.hi = (set(de.hi, 0)); cycles = 8; break;
	    case 0xC3: de.lo = (set(de.lo, 0)); cycles = 8; break;
	    case 0xC4: hl.hi = (set(hl.hi, 0)); cycles = 8; break;
	    case 0xC5: hl.lo = (set(hl.lo, 0)); cycles = 8; break;
	    case 0xC6: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 0)); cycles = 16; break;
	    case 0xC7: af.hi = (set(af.hi, 0)); cycles = 8; break;
	    case 0xC8: bc.hi = (set(bc.hi, 1)); cycles = 8; break;
	    case 0xC9: bc.lo = (set(bc.lo, 1)); cycles = 8; break;
	    case 0xCA: de.hi = (set(de.hi, 1)); cycles = 8; break;
	    case 0xCB: de.lo = (set(de.lo, 1)); cycles = 8; break;
	    case 0xCC: hl.hi = (set(hl.hi, 1)); cycles = 8; break;
	    case 0xCD: hl.lo = (set(hl.lo, 1)); cycles = 8; break;
	    case 0xCE: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 1)); cycles = 16; break;
	    case 0xCF: af.hi = (set(af.hi, 1)); cycles = 8; break;
	    case 0xD0: bc.hi = (set(bc.hi, 2)); cycles = 8; break;
	    case 0xD1: bc.lo = (set(bc.lo, 2)); cycles = 8; break;
	    case 0xD2: de.hi = (set(de.hi, 2)); cycles = 8; break;
	    case 0xD3: de.lo = (set(de.lo, 2)); cycles = 8; break;
	    case 0xD4: hl.hi = (set(hl.hi, 2)); cycles = 8; break;
	    case 0xD5: hl.lo = (set(hl.lo, 2)); cycles = 8; break;
	    case 0xD6: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 2)); cycles = 16; break;
	    case 0xD7: af.hi = (set(af.hi, 2)); cycles = 8; break;
	    case 0xD8: bc.hi = (set(bc.hi, 3)); cycles = 8; break;
	    case 0xD9: bc.lo = (set(bc.lo, 3)); cycles = 8; break;
	    case 0xDA: de.hi = (set(de.hi, 3)); cycles = 8; break;
	    case 0xDB: de.lo = (set(de.lo, 3)); cycles = 8; break;
	    case 0xDC: hl.hi = (set(hl.hi, 3)); cycles = 8; break;
	    case 0xDD: hl.lo = (set(hl.lo, 3)); cycles = 8; break;
	    case 0xDE: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 3)); cycles = 16; break;
	    case 0xDF: af.hi = (set(af.hi, 3)); cycles = 8; break;
	    case 0xE0: bc.hi = (set(bc.hi, 4)); cycles = 8; break;
	    case 0xE1: bc.lo = (set(bc.lo, 4)); cycles = 8; break;
	    case 0xE2: de.hi = (set(de.hi, 4)); cycles = 8; break;
	    case 0xE3: de.lo = (set(de.lo, 4)); cycles = 8; break;
	    case 0xE4: hl.hi = (set(hl.hi, 4)); cycles = 8; break;
	    case 0xE5: hl.lo = (set(hl.lo, 4)); cycles = 8; break;
	    case 0xE6: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 4)); cycles = 16; break;
	    case 0xE7: af.hi = (set(af.hi, 4)); cycles = 8; break;
	    case 0xE8: bc.hi = (set(bc.hi, 5)); cycles = 8; break;
	    case 0xE9: bc.lo = (set(bc.lo, 5)); cycles = 8; break;
	    case 0xEA: de.hi = (set(de.hi, 5)); cycles = 8; break;
	    case 0xEB: de.lo = (set(de.lo, 5)); cycles = 8; break;
	    case 0xEC: hl.hi = (set(hl.hi, 5)); cycles = 8; break;
	    case 0xED: hl.lo = (set(hl.lo, 5)); cycles = 8; break;
	    case 0xEE: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 5)); cycles = 16; break;
	    case 0xEF: af.hi = (set(af.hi, 5)); cycles = 8; break;
	    case 0xF0: bc.hi = (set(bc.hi, 6)); cycles = 8; break;
	    case 0xF1: bc.lo = (set(bc.lo, 6)); cycles = 8; break;
	    case 0xF2: de.hi = (set(de.hi, 6)); cycles = 8; break;
	    case 0xF3: de.lo = (set(de.lo, 6)); cycles = 8; break;
	    case 0xF4: hl.hi = (set(hl.hi, 6)); cycles = 8; break;
	    case 0xF5: hl.lo = (set(hl.lo, 6)); cycles = 8; break;
	    case 0xF6: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 6)); cycles = 16; break;
	    case 0xF7: af.hi = (set(af.hi, 6)); cycles = 8; break;
	    case 0xF8: bc.hi = (set(bc.hi, 7)); cycles = 8; break;
	    case 0xF9: bc.lo = (set(bc.lo, 7)); cycles = 8; break;
	    case 0xFA: de.hi = (set(de.hi, 7)); cycles = 8; break;
	    case 0xFB: de.lo = (set(de.lo, 7)); cycles = 8; break;
	    case 0xFC: hl.hi = (set(hl.hi, 7)); cycles = 8; break;
	    case 0xFD: hl.lo = (set(hl.lo, 7)); cycles = 8; break;
	    case 0xFE: load8intomem(hl.getreg(), set(store8frommem(hl.getreg()), 7)); cycles = 16; break;
	    case 0xFF: af.hi = (set(af.hi, 7)); cycles = 8; break;
	    default: cout << "Unrecognized extended opcode at 0xCB" << uppercase << hex << (int)(opcode) << endl; printregs(); exit(1); break;
	}

	return cycles;
    }
}
