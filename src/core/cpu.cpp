#include "../../include/libmbGB/cpu.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace gb;

namespace gb
{
	CPU::CPU()
	{
	    reset();
	}

	CPU::~CPU()
	{
	    cout << "CPU::Shutting down..." << endl;
	}

    bool CPU::loadcpu(string filename)
    {
        fstream file(filename.c_str(), ios::in | ios::binary);
        
        if (!file.is_open())
        {
            cout << "Error opening save state" << endl;
            return false;
        }
        
        file.read((char*)&af.hi, sizeof(af.hi));
        file.read((char*)&af.lo, sizeof(af.lo));
        file.read((char*)&bc.hi, sizeof(bc.hi));
        file.read((char*)&bc.lo, sizeof(bc.lo));
        file.read((char*)&de.hi, sizeof(de.hi));
        file.read((char*)&de.lo, sizeof(de.lo));
        file.read((char*)&hl.hi, sizeof(hl.hi));
        file.read((char*)&hl.lo, sizeof(hl.lo));
        file.read((char*)&pc, sizeof(pc));
        file.read((char*)&sp, sizeof(sp));
        file.read((char*)&m_cycles, sizeof(m_cycles));
        file.read((char*)&halted, sizeof(halted));
        file.read((char*)&interruptmaster, sizeof(interruptmaster));
        file.read((char*)&interruptdelay, sizeof(interruptdelay));
        file.read((char*)&skipinstruction, sizeof(skipinstruction));
        file.close();
        return true;
    }

    bool CPU::savecpu(string filename)
    {
        if (fexists(filename))
        {
            cout << "File already exists" << endl;
            return false;
        }
        
        fstream file(filename.c_str(), ios::out | ios::binary);
        
        if (!file.is_open())
        {
            cout << "Error opening cpu state" << endl;
            return false;
        }
        
        file.write((const char*)&af.hi, sizeof(af.hi));
        file.write((const char*)&af.lo, sizeof(af.lo));
        file.write((const char*)&bc.hi, sizeof(bc.hi));
        file.write((const char*)&bc.lo, sizeof(bc.lo));
        file.write((const char*)&de.hi, sizeof(de.hi));
        file.write((const char*)&de.lo, sizeof(de.lo));
        file.write((const char*)&hl.hi, sizeof(hl.hi));
        file.write((const char*)&hl.lo, sizeof(hl.lo));
        file.write((const char*)&pc, sizeof(pc));
        file.write((const char*)&sp, sizeof(sp));
        file.write((const char*)&m_cycles, sizeof(m_cycles));
        file.write((const char*)&halted, sizeof(halted));
        file.write((const char*)&interruptmaster, sizeof(interruptmaster));
        file.write((const char*)&interruptdelay, sizeof(interruptdelay));
        file.write((const char*)&skipinstruction, sizeof(skipinstruction));

        file.close();
        return true;
    }

	void CPU::reset()
	{
	    af.reg = 0x01B0;
	    bc.reg = 0x0013;
	    de.reg = 0x00D8;
	    hl.reg = 0x014D;
	    pc = 0x0100;
	    sp = 0xFFFE;

	    halted = false;
        paused = false;

	    interruptmaster = false;
	    interruptdelay = false;
	    skipinstruction = false;

	    m_bytes = 0;
	    m_cycles = 0;

	    cout << "CPU::Initialized" << endl;
	}

	void CPU::resetBIOS()
	{
	    af.reg = 0x0000;
	    bc.reg = 0x0000;
	    de.reg = 0x0000;
	    hl.reg = 0x0000;
	    pc = 0x0000;
	    sp = 0x0000;

	    halted = false;
        paused = false;

	    interruptmaster = false;
	    interruptdelay = false;
	    skipinstruction = false;

	    m_bytes = 0;
	    m_cycles = 0;

	    cout << "CPU::Initialized" << endl;
	}

	void CPU::dointerrupts()
	{
	    uint8_t req = mem->readByte(0xFF0F);
	    uint8_t enabled = mem->readByte(0xFFFF);	    

	    if (interruptdelay)
	    {
		interruptdelay = false;
		interruptmaster = true;
		return;
	    }	    

	    if (interruptmaster)
	    {
		if (TestBit(req, 0) && TestBit(enabled, 0))
		{
		    interruptmaster = false;
		    halted = false;
		    req = BitReset(req, 0);
		    sp -= 2;
		    mem->writeWord(sp, pc);
		    pc = 0x40;
		    advancecycles(36);
		}
		
		if (TestBit(req, 1) && TestBit(enabled, 1))
		{
		    interruptmaster = false;
		    halted = false;
		    req = BitReset(req, 1);
		    sp -= 2;
		    mem->writeWord(sp, pc);
		    pc = 0x48;
		    advancecycles(36);
		}

		if (TestBit(req, 2) && TestBit(enabled, 2))
		{
		    interruptmaster = false;
		    halted = false;
		    req = BitReset(req, 2);
		    sp -= 2;
		    mem->writeWord(sp, pc);
		    pc = 0x50;
		    advancecycles(36);
		}

		if (TestBit(req, 3) && TestBit(enabled, 3))
		{
		    interruptmaster = false;
		    halted = false;
		    req = BitReset(req, 3);
		    sp -= 2;
		    mem->writeWord(sp, pc);
		    pc = 0x58;
		    advancecycles(36);
		}

		if (TestBit(req, 4) && TestBit(enabled, 4))
		{
		    interruptmaster = false;
		    halted = false;
		    req = BitReset(req, 0);
		    sp -= 2;
		    mem->writeWord(sp, pc);
		    pc = 0x60;
		    advancecycles(36);
		}

		req |= 0xE0;
		mem->writeByte(0xFF0F, req);
		return;
	    }
	    else if ((req & enabled & 0x1F) && (!skipinstruction))
	    {
		halted = false;
		return;
	    }
	}

	void CPU::advancecycles(int cycles)
	{
	    timers->updatetimers(cycles);
	    gpu->updategraphics(cycles);
	}

	void CPU::executenextopcode()
	{
        uint8_t opcode = mem->readByte(pc);	    

	    if (halted)
	    {
		if (interruptmaster || !skipinstruction)
		{
		    m_cycles += 4;
		}
		else if (skipinstruction)
		{
		    halted = false;
		    skipinstruction = false;
		    executeopcode(opcode);
		}
	    }
	    else
	    {
		opcode = mem->readByte(pc++);		
		executeopcode(opcode);
	    }
	}

	// Stolen
	void CPU::daa()
	{
	    uint8_t tempF = af.lo & 0x50;

	    if (TestBit(af.lo, subtract))
	    {
		if (TestBit(af.lo, half))
		{
		    af.hi -= 0x06;
		}

		if (TestBit(af.lo, carry))
		{
		    af.hi -= 0x60;
		}
	    }
	    else
	    {
		if ((TestBit(af.lo, carry)) || (af.hi > 0x99))
		{
		    if ((TestBit(af.lo, half)) || (af.hi & 0x0F) > 0x09)
		    {
			af.hi += 0x66;
		    }
		    else
		    {
			af.hi += 0x60;
		    }
		    tempF |= 0x10;
		}
		else if ((TestBit(af.lo, half)) || (af.hi & 0x0F) > 0x09)
		{
		    af.hi += 0x06;
		}
	    }

	    if (af.hi == 0)
	    {
		tempF = BitSet(tempF, zero);
	    }

	    af.lo = tempF;

	    m_cycles += 4;
	}

	void CPU::stop()
	{
	    pc++;
	}

	uint8_t CPU::add8bit(uint8_t regone, uint8_t regtwo)
	{
	    af.lo = 0;

	    if (uint8_t(regone + regtwo) == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    if (((regone & 0xF) + (regtwo & 0xF)) > 0xF)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    if ((regone + regtwo) > 0xFF)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    return regone + regtwo;
	}

	uint8_t CPU::add8bitc(uint8_t regone, uint8_t regtwo)
	{
	    uint8_t carryflag = BitGetVal(af.lo, carry);
	    uint8_t carryset = 0;
	    uint8_t halfcarryset = 0;

	    uint8_t temp1 = 0;
	    uint8_t temp2 = 0;

	    temp1 = regone + carryflag;

	    if ((regone + carryflag) > 0xFF)
	    {
		carryset = 1;
	    }

	    if (((regone & 0x0F) + (carryflag & 0x0F)) > 0x0F)
	    {
		halfcarryset = 1;
	    }

	    temp2 = temp1 + regtwo;

	    if ((temp1 + regtwo) > 0xFF)
	    {
		carryset = 1;
	    }

	    if (((temp1 & 0x0F) + (regtwo & 0x0F)) > 0x0F)
	    {
		halfcarryset = 1;
	    }

	    af.lo = 0;

	    if (carryset == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (halfcarryset == 1)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    if (temp2 == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return temp2;
	}

	uint8_t CPU::sub8bit(uint8_t regone, uint8_t regtwo)
	{
	    af.lo = 0;

	    if (uint8_t(regone - regtwo) == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    af.lo = BitSet(af.lo, subtract);

	    if (regone < regtwo)
	    {
		af.lo = BitSet(af.lo, carry);
	    }
	
	    if ((regone & 0xF) < (regtwo & 0xF))
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    return regone - regtwo;
	}

	uint8_t CPU::sub8bitc(uint8_t regone, uint8_t regtwo)
	{
	    uint8_t carryflag = BitGetVal(af.lo, carry);
	    uint8_t carryset = 0;
	    uint8_t halfcarryset = 0;

	    uint8_t temp1 = 0;
	    uint8_t temp2 = 0;

	    temp1 = regone - carryflag;

	    if (regone < carryflag)
	    {
		carryset = 1;
	    }

	    if ((regone & 0x0F) < (carryflag & 0x0F))
	    {
		halfcarryset = 1;
	    }

	    temp2 = temp1 - regtwo;

	    if (temp1 < regtwo)
	    {
		carryset = 1;
	    }

	    if ((temp1 & 0x0F) < (regtwo & 0x0F))
	    {
		halfcarryset = 1;
	    }

	    af.lo = 0;

	    if (carryset == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (halfcarryset == 1)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    af.lo = BitSet(af.lo, subtract);

	    if (temp2 == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return temp2;
	}

	uint8_t CPU::and8bit(uint8_t regone, uint8_t regtwo)
	{
	    af.lo = 0;

	    if ((regone & regtwo) == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    af.lo = BitSet(af.lo, half);

	    return regone & regtwo;
	}

	uint8_t CPU::or8bit(uint8_t regone, uint8_t regtwo)
	{
	    af.lo = 0;

	    if ((regone | regtwo) == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone | regtwo;
	}

	uint8_t CPU::xor8bit(uint8_t regone, uint8_t regtwo)
	{
	    af.lo = 0;

	    if ((regone ^ regtwo) == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone ^ regtwo;
	}

	uint8_t CPU::inc8bit(uint8_t regone)
	{
	    uint8_t carryflag = TestBit(af.lo, carry) ? 1 : 0;
	    regone++;

	    af.lo = 0;

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    if ((regone & 0xF) == 0)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    return regone;
	}

	uint8_t CPU::dec8bit(uint8_t regone)
	{
	    uint8_t carryflag = TestBit(af.lo, carry) ? 1 : 0;
	    regone--;

	    af.lo = 0;

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    af.lo = BitSet(af.lo, subtract);

	    if ((regone & 0xF) == 0xF)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    return regone;
	}

	uint16_t CPU::add16bit(uint16_t regone, uint16_t regtwo)
	{
	    uint8_t zeroflag = TestBit(af.lo, zero) ? 1 : 0;
	    af.lo = 0;

	    if ((regone + regtwo) > 0xFFFF)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (((regone & 0x0FFF) + (regtwo & 0x0FFF)) > 0x0FFF)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    if (zeroflag == 1)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone + regtwo;
	}

	uint16_t CPU::adds8bit(uint16_t regone, uint8_t regtwo)
	{
	    int16_t regtwobsx = (int16_t)((int8_t)regtwo);
	    uint16_t result = regone + regtwobsx;

	    af.lo = 0;

	    if (((regone & 0xFF) + regtwo) > 0xFF)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (((regone & 0xF) + (regtwo & 0xF)) > 0xF)
	    {
		af.lo = BitSet(af.lo, half);
	    }

	    return result;
	}

	uint8_t CPU::swap(uint8_t regone)
	{
	    af.lo = 0;
	    uint8_t tempone = (regone & 0xF) << 4;
	    uint8_t temptwo = (regone >> 4) & 0xF;
	    regone = (tempone | temptwo);

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::rl(uint8_t regone)
	{	    
	    uint8_t oldcarry = TestBit(af.lo, carry) ? 1 : 0;
	    af.lo = 0;

	    uint8_t carryflag = BitGetVal(regone, 7);
	    regone = (regone << 1) + oldcarry;

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::rlc(uint8_t regone)
	{
	    af.lo = 0;

	    uint8_t carryflag = BitGetVal(regone, 7);
	    regone = (regone << 1) + carryflag;

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::rr(uint8_t regone)
	{
	    uint8_t oldcarry = TestBit(af.lo, carry) ? 1 : 0;
	    af.lo = 0;

	    uint8_t carryflag = BitGetVal(regone, 0);
	    regone = (regone >> 1) + (oldcarry << 7);

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::rrc(uint8_t regone)
	{
	    af.lo = 0;

	    uint8_t carryflag = BitGetVal(regone, 0);
	    regone = (regone >> 1) + (carryflag << 7);

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::sla(uint8_t regone)
	{
	    af.lo = 0;

	    uint8_t carryflag = TestBit(regone, 7) ? 1 : 0;
	    regone <<= 1;

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::sra(uint8_t regone)
	{
	    af.lo = 0;
	    uint8_t carryflag = TestBit(regone, 0) ? 1 : 0;
	    regone >>= 1;
	    regone |= ((regone & 0x40) << 1);

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	uint8_t CPU::srl(uint8_t regone)
	{
	    af.lo = 0;
	    uint8_t carryflag = TestBit(regone, 0) ? 1 : 0;
	    regone >>= 1;

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    if (regone == 0)
	    {
		af.lo = BitSet(af.lo, zero);
	    }

	    return regone;
	}

	void CPU::bit(uint8_t regone, int bit)
	{
	    uint8_t carryflag = TestBit(af.lo, carry) ? 1 : 0;
	    af.lo = 0;

	    if (carryflag == 1)
	    {
		af.lo = BitSet(af.lo, carry);
	    }

	    af.lo = BitSet(af.lo, half);

	    if (!TestBit(regone, bit))
	    {
		af.lo = BitSet(af.lo, zero);
	    }
	}

	uint8_t CPU::set(uint8_t regone, int bit)
	{
	    regone = BitSet(regone, bit);
	    return regone;
	}

	uint8_t CPU::res(uint8_t regone, int bit)
	{
	    regone = BitReset(regone, bit);
	    return regone;
	}

	void CPU::jr(uint8_t regone)
	{
            if (TestBit(regone, zero))
            {
                regone--;
                regone = ~regone;
                pc -= regone;
            }
            else
            {
                pc += regone;
            }
	}
}
