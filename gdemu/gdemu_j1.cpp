/**
* J1Class
* $Id$
* \file gdemu_j1.cpp
* \brief J1Class
* \date 2011-05-26 19:52GMT
*/

// #include <...>
#include "gdemu_j1.h"

// System includes

// Project includes
#include "WProgram.h"
#include "GD.h"
#include "gdemu_gameduino_spi.h"
#include "gdemu_system.h"

// using namespace ...;

namespace GDEMU {

J1Class J1;

bool s_Resetting = true;

namespace J1CPU {

static unsigned short t;  
static unsigned short d[32]; /* data stack */
static unsigned short r[32]; /* return stack */
static unsigned short pc;    /* program counter, counts CELLS */
static unsigned char dsp, rsp; /* point to top entry */
static int sx[4] = { 0, 1, -2, -1 }; /* 2-bit sign extension */

static int hold_pc, hold_t, hold_n;
static int hold_insn;

static long cycles;

static void resetCpu()
{
	t = 0;
	dsp = 0;
	rsp = 0;
	pc = 0;
	hold_n = 0;
	hold_pc = 0;
	hold_t = 0;
	hold_insn = 0x4000 | (J1_CODE >> 1); // First insn: "call entrypoint"
}

static void push(int v) // push v on the data stack
{
	dsp = 31 & (dsp + 1);
	d[dsp] = t;
	t = v;
}

static int pop(void) // pop value from the data stack and return it
{
	int v = t;
	t = d[dsp];
	dsp = 31 & (dsp - 1);
	return v;
}

//int lastinsn = 0;

static void execute(int insnLimit, int microsLimit)
{
	int _pc = hold_pc, _t = hold_t, n = hold_n;
	int insn = hold_insn; // First insn: "call entrypoint"

	int insnNb = 0;
	long microsStart = System.getMicros();
	int microsNb = 0;

	unsigned char *gameduinoRam = GameduinoSPI.getRam();
	short *gameduinoRam16 = ((short *)(void *)gameduinoRam);
	
	for (;;) 
	{
		if (GameduinoSPI.getJ1Reset())
		{
			// printf("runtime reset\n");
			resetCpu();
			_pc = hold_pc, _t = hold_t, n = hold_n;
			insn = hold_insn;
		}

		/*if (lastinsn == (insn & 0xFFFF)) 
			printf("insn %i\n", (insn & 0xFFFF));
		lastinsn= (insn & 0xFFFF);*/
		
		_pc = pc + 1;
		if (insn & 0x8000) // literal
			push(insn & 0x7fff);
		else
		{
			int target = insn & 0x1fff;
			switch (insn >> 13) 
			{
			case 0: // jump
				if ((_pc - 1) == target)
				{
					System.prioritizeDuinoThread();
					System.switchThread(); // save cpu on insanity loops :)
					System.unprioritizeDuinoThread();
				}
				_pc = target;
				break;
			case 1: // conditional jump
				if ((_pc - 1) == target)
				{
					System.prioritizeDuinoThread();
					System.switchThread(); // save cpu on insanity loops :)
					printf("woooo\n");
					System.unprioritizeDuinoThread();
				}
				if (pop() == 0)
					_pc = target;
				break;
			case 2: // call
				rsp = 31 & (rsp + 1);
				r[rsp] = _pc << 1;
				_pc = target;
				break;
			case 3: // ALU
				if (insn & 0x1000) /* R->PC */
					_pc = r[rsp] >> 1;
				n = d[dsp];
				switch ((insn >> 8) & 0xf) 
				{
				case 0:   _t = t; break;
				case 1:   _t = n; break;
				case 2:   _t = t + n; break;
				case 3:   _t = t & n; break;
				case 4:   _t = t | n; break;
				case 5:   _t = t ^ n; break;
				case 6:   _t = ~t; break;
				case 7:   _t = -(t == n); break;
				case 8:   _t = -((signed short)n < (signed short)t); break;
				case 9:   _t = n >> t; break;
				case 10:  _t = t - 1; break;
				case 11:  _t = r[rsp]; break;
				case 12:  _t = GameduinoSPI.readRam16(t); break;
				case 13:  _t = n * t; break;                 // different
				case 14:  _t = (t >> 8) | (t << 8); break;   // different
				case 15:  _t = -(n < t); break;
				}
				dsp = 31 & (dsp + sx[insn & 3]);
				rsp = 31 & (rsp + sx[(insn >> 2) & 3]);
				if (insn & 0x80) /* T->N */
					d[dsp] = t;
				if (insn & 0x40) /* T->R */
					r[rsp] = t;
				if (insn & 0x20) /* N->[T] */
					GameduinoSPI.writeRam16(t, n);

				t = _t;
				break;
			}
		}
		pc = _pc;
		insn = gameduinoRam16[pc];

		++J1CPU::cycles;
		++insnNb;
		
		if (pc < (J1_CODE >> 1) || pc >= (J1_CODE >> 1) + 128)
			printf("pc %i outside J1_CODE %i\n", (int)pc, (int)(J1_CODE >> 1));

		if (!rsp)
		{
			resetCpu();
			_pc = hold_pc, _t = hold_t, n = hold_n;
			insn = hold_insn;
			System.switchThread();
		}

		if (insnLimit > 0 && insnNb >= insnLimit)
		{
			break;
		}

		if (microsLimit > 0 && (insnNb % (1024)) == 0 && insnNb > 0) // some extra cycles not to call System.getMicros() too often
		{
			microsNb = (int)(System.getMicros() - microsStart);
			if (microsNb >= microsLimit)
				break;
		}

	}
	
	hold_n = n;
	hold_pc = _pc;
	hold_t = _t;
	hold_insn = insn;
}

} /* namespace J1CPU */

void J1Class::begin()
{
	J1CPU::cycles = 0;
	flagReset(true);
}

void J1Class::execute(int insnLimit, int microsLimit)
{
	// MUST CHECK ISRESETTING MANUALLY BEFORE!!
	J1CPU::execute(insnLimit, microsLimit);
}

void J1Class::flagReset(bool resetting)
{
	s_Resetting = resetting;
	if (resetting) J1CPU::resetCpu();
}

bool J1Class::isResetting()
{
	return s_Resetting;
}

void J1Class::end()
{

}

long J1Class::getCycles() 
{ 
	return J1CPU::cycles; 
}

} /* namespace GDEMU */

/* end of file */
