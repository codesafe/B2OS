/*
	MOS 6502 CPU Emulator
*/
#include "../Src/console.h"
#include "../Src/util.h"
#include "AppleCPU.h"
#include "AppleMem.h"

#define USEOLD	0

static bool log = true;
unsigned long cpu_tick;

// Registor
BYTE	A;		// Accumulator
BYTE	X;		// Index Registor
BYTE	Y;
BYTE	SP;		// Stack Pointer
WORD	PC;		// program control

typedef struct StatusFlags
{
	BYTE C : 1;			//0: Carry Flag	
	BYTE Z : 1;			//1: Zero Flag
	BYTE I : 1;			//2: Interrupt disable
	BYTE D : 1;			//3: Decimal mode
	BYTE B : 1;			//4: Break
	BYTE Unused : 1;	//5: Unused
	BYTE V : 1;			//6: Overflow
	BYTE N : 1;			//7: Negative
} StatusFlags;


union SF
{
	BYTE PS;
	struct StatusFlags Flag;
};


union SF _PS;

void ac_Init()
{
	cpu_tick = 0;
	ac_Reset_0();
}

void ac_Reset_0()
{
	A = 0;
	X = 0;
	Y = 0;
	_PS.PS = 0;				// processor status (flags)
	SP = STACK_POS;	// Stack pointer
	PC = PC_START;		// program control
}

// reset all register
void ac_Reset()
{
	A = 0;
	X = 0;
	Y = 0;
	//PS = 0;				// processor status (flags)
	_PS.PS = (_PS.PS | FLAG_INTERRUPT_DISABLE) & ~FLAG_DECIMAL_MODE;
	SP = STACK_POS;	// Stack pointer
	// ROM 로드후 정해짐
	PC = mem_ReadByte(0xFFFC) | (mem_ReadByte(0xFFFD) << 8);
}

void ac_Reboot()
{
	mem_WriteByte(0x3F4, 0);
	PC = mem_ReadByte(0xFFFC) | (mem_ReadByte(0xFFFD) << 8);
	SP = 0xFD;
	_PS.Flag.I = 1;
	_PS.Flag.Unused = 1;

	mem_ResetRam();
// 	mem_WriteByte(0x4D, 0xAA);   // Joust crashes if this memory location equals zero
// 	mem_WriteByte(0xD0, 0xAA);   // Planetoids won't work if this memory location equals zero
}
void ac_SetPCAddress(WORD addr)
{
	PC = addr;
}

void ac_SetRegister(BYTE type, BYTE value)
{
	switch (type)
	{
		case REGISTER_A :
			A = value;
			break;
		case REGISTER_X:
			X = value;
			break;
		case REGISTER_Y:
			Y = value;
			break;
	}
}

BYTE ac_GetRegister(BYTE type)
{
	switch (type)
	{
	case REGISTER_A:
		return A;
	case REGISTER_X:
		return X;
	case REGISTER_Y:
		return Y;
	}

	//assert(-1);
	return 0;
}

void ac_SetFlag(BYTE flag, bool set)
{
	if (set)
		_PS.PS |= flag;
	else
		_PS.PS &= ~flag;
}

bool ac_GetFlag(BYTE flag)
{
	return _PS.PS & flag;
}

void ac_SetZeroNegative(BYTE Register)
{
	//SetFlag(FLAG_ZERO, Register == 0);
	_PS.Flag.Z = (Register == 0);

	//SetFlag(FLAG_NEGATIVE, Register & FLAG_NEGATIVE);
	_PS.Flag.N = (Register & FLAG_NEGATIVE) > 0;
}

void ac_SetCarryFlag(WORD value)
{
	_PS.Flag.C = (value & 0xFF00) > 0;
	//SetFlag(FLAG_CARRY, (value & 0xFF00) > 0);

}

void ac_SetCarryFlagNegative(WORD value)
{
	_PS.Flag.C = (value < 0x100);
}

void ac_SetOverflow(BYTE oldv0, BYTE v0, BYTE v1)
{
	bool sign0 = !((oldv0 ^ v1) & FLAG_NEGATIVE);	// 계산전 부호
	bool sign1 = ((v0 ^ v1) & FLAG_NEGATIVE);		// 계산후 부호

	// Overflow는 같은 부호를 더했는데 다른 부호가 나오면 Overflow이다
	//SetFlag(FLAG_OVERFLOW, (sign0 != sign1));
	//Flag.V = (sign0 != sign1);
	_PS.Flag.V = sign0 && sign1;
}

BYTE ac_Fetch(unsigned long* cycle)
{
	BYTE c = mem_ReadByte(PC++);
	*cycle--;
	return c;
}

WORD ac_FetchWord(unsigned long* cycle)
{
	BYTE c0 = mem_ReadByte(PC++);
	BYTE c1 = mem_ReadByte(PC++);

	// 엔디안에 따라 c0 <--> c1해야 할수도 있다
	WORD w = (c1 << 8) | c0;
	*cycle-=2;
	return w;
}

// 메모리에서 읽는데 cycle소모 x / PC무관 할때 (Zero page같은것)
BYTE ac_ReadByte(WORD addr, unsigned long* cycle)
{
	BYTE c = mem_ReadByte(addr);
	*cycle--;
	return c;
}

WORD ac_ReadWord(WORD addr, unsigned long* cycle)
{
	WORD c = mem_ReadWord(addr);
	*cycle -= 2;
	return c;
}

void ac_WriteByte(BYTE value, WORD addr, unsigned long* cycle)
{
	mem_WriteByte(addr, value);
	*cycle --;
}

void ac_WriteWord(WORD value, WORD addr, unsigned long* cycle)
{
	mem_WriteWord(value, addr);
	*cycle-=2;
}

// 이거를 써야하는 이유는 
/*
	SP는 1byte이고 Stack 메모리는 0x01FF -> 0x0100까지 256 Byte이므로
	Address는 WORD이고 스택의 메모리 위치는 감소하기 때문에 이렇게 계산해햐함
*/
WORD ac_GetStackAddress()
{
	WORD sp = (WORD)STACK_ADDRESS + SP;
	return sp;
}

// Byte를 Stack에 Push
void ac_PushStackByte(BYTE value, unsigned long* cycle)
{
	ac_WriteByte(value, ac_GetStackAddress(), cycle);
	//SP--;
	SP = safeDec(SP, 1);
	*cycle--;
}

// Word를 Stack에 Push
void ac_PushStackWord(WORD value, unsigned long* cycle)
{
	// Hi byte 먼저
	ac_WriteByte(value >> 8, ac_GetStackAddress(), cycle);
	//SP--;
	SP = safeDec(SP, 1);
	// Lo byte 나중에
	ac_WriteByte(value & 0xFF, ac_GetStackAddress(), cycle);
	//SP--;
	SP = safeDec(SP, 1);
}

// 스택에서 1 byte POP
BYTE ac_PopStackByte(unsigned long* cycle)
{
 	//SP++;
	SP = safeInc(SP, 1);
 	BYTE popbyte = ac_ReadByte(ac_GetStackAddress(), cycle);
	*cycle--;
	return popbyte;
}

// Stack에서 Word pop
WORD ac_PopStackWord(unsigned long* cycle)
{
	//SP++;
	SP = safeInc(SP, 1);
	BYTE lo = ac_ReadByte(ac_GetStackAddress(), cycle);
	//SP++;
	SP = safeInc(SP, 1);
	BYTE hi = ac_ReadByte(ac_GetStackAddress(), cycle);
	WORD popWord = lo | (hi << 8);
	*cycle--;

	return popWord;
}

long long int ticks = 0;  // accumulated number of clock cycles
int logcount = 0;
int ac_Run(unsigned long _cycle)
{
	unsigned long cycle = _cycle;
	while ( cycle > 0 )
	{
		unsigned long prevcycle = cycle;
		WORD prevPC = PC;
		// 여기에서 cycle 하나 소모
		BYTE inst = ac_Fetch(&cycle);

		// if( log )
		// {
			//k_printnum(cycle);
			//k_printnum(inst);
			//k_printnum(PC);
			//k_printnum(SP);
			//k_print("--------");
			//logcount++;
			//if( logcount > 10)
			//log = false;
		//}

		switch (inst)
		{
			case LDA_IM: // 2 cycle
			{
				//LoadToRegister(mem, cycle, A);
				A = ac_Fetch(&cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case LDA_ZP: // 3 cycle
			{
				// $0000 to $00FF
				// Zero page에서 읽어서 A로
				WORD addr = ac_addr_mode_ZP(&cycle);
				// Zero page읽으면서 cycle 소모
				A = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case LDA_ZPX : // 4 cycle
			{
				// Zero page의 주소와 X 레지스터를 더한 주소에서 읽어 A로..
				// X 레지스터에 $0F이고 LDA $80, X 이면 $80+$0F = $8F에서 A로 읽게됨
/*				
				BYTE zpa = Fetch(mem, cycle);
				zpa += X;
				cycle--;
*/
				WORD addr = ac_addr_mode_ZPX(&cycle);
				// Zero page읽으면서 cycle 소모
				A = ac_ReadByte(addr, &cycle);

				ac_SetZeroNegative(A);
			}
			break;

			// 절대 주소 지정을 사용하는 명령어는 대상 위치를 식별하기 위해 전체 16 비트 주소를 포함합니다.
			case LDA_ABS: // 4 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				A = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case LDA_ABSX:// 4 cycle / 페이지 넘어가면 1 cycle 추가
			{
				// 메모리엑세스 페이지를 넘어가면 추가 사이클이 소요됨 (하드웨어가 그렇게 만들어짐?)
/*				
				BYTE lo = Fetch(mem, cycle);
				BYTE hi = Fetch(mem, cycle);
				
				WORD t = lo + X;
				if( t > 0xFF ) cycle--;
				WORD addr = (lo | (hi << 8)) + X;
*/				
// 				WORD addr = FetchWord(mem, cycle);
// 				if ( (addr + X) - addr >= 0xFF )
// 					cycle--;	// page 넘어감

				WORD addr = ac_addr_mode_ABSX(&cycle);
				A = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case LDA_ABSY:	// 4 cycle / 페이지 넘어가면 1 cycle 추가
			{
/*				
				BYTE lo = Fetch(mem, cycle);
				BYTE hi = Fetch(mem, cycle);

				WORD t = lo + Y;
				if (t > 0xFF) cycle--;
				WORD addr = (lo | (hi << 8)) + Y;
*/
				WORD addr = ac_addr_mode_ABSY(&cycle);
				A = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(A);

			}
			break;

			case LDA_INDX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				A = ac_ReadByte(addr, &cycle);

				ac_SetZeroNegative(A);
			}
			break;

			case LDA_INDY: // 5 ~ 6 cycle
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				A = ac_ReadByte(addr, &cycle);

				ac_SetZeroNegative(A);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			// LDX
			case LDX_IM:	// 2cycle
			{
				//LoadToRegister(mem, cycle, X);
				X = ac_Fetch(&cycle);
				ac_SetZeroNegative(X);
			}
			break;

			case LDX_ZP:	// 3cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
 				X = ac_ReadByte(addr, &cycle);
 				ac_SetZeroNegative(X);
			}
			break;

			case LDX_ZPY:	// 4cycle
			{
/*				
				BYTE zpage = Fetch(mem, cycle);
				zpage += Y;
				cycle--;
*/
				WORD addr = ac_addr_mode_ZPY(&cycle);
				X = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(X);
			}
			break;

			case LDX_ABS:	// 4cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				X = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(X);

			}
			break;

			case LDX_ABSY:	// 4 ~ 5 cycle
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				X = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(X);

			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			case LDY_IM : // 2 cycle
			{
				Y = ac_Fetch(&cycle);
				ac_SetZeroNegative(Y);
			}
			break;

			case LDY_ZP: // 3 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				Y = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(Y);
			}
			break;

			case LDY_ZPX : // 4 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				Y = ac_ReadByte(addr, &cycle);
				ac_SetZeroNegative(Y);
			}
			break;

			case LDY_ABS : // 4 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				Y = ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(Y);
			}
			break;

			case LDY_ABSX : // 4~5 cycle
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				Y = ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(Y);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			case STA_ZP	:	// 3 cycle
			{
				// ZeroPage에 A레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ZP(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_ZPX :	// 4 cycle
			{
				// ZP + X에 A레지스터 내용쓰기
				WORD addr = ac_addr_mode_ZPX(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_ABS:	// 4 cycle
			{
				// WORD address에 A레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ABS(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_ABSX:	// 5 cycle
			{
				// WORD address + X에 A레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_ABSY:	// 5 cycle
			{
				// WORD address + Y에 A레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ABSY_NoPage(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_INDX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			case STA_INDY:	// 6 cycle
			{
				// ZeroPage에서 WORD address얻고 address + Y에 가르키는곳에 A레지스터 내용쓰기 
				BYTE zp = ac_Fetch(&cycle);
				WORD addr = ac_ReadWord(zp, &cycle);
				addr += Y;
				cycle--;
				ac_WriteByte(A, addr, &cycle);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			case STX_ZP	:	// 3 cycle
			{
				// ZeroPage에 X레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ZP(&cycle);
				ac_WriteByte(X, addr, &cycle);
			}
			break;

			case STX_ZPY:	// 4 cycle
			{
				// ZP + Y에 X레지스터 내용쓰기
				WORD addr = ac_addr_mode_ZPY(&cycle);
				ac_WriteByte(X, addr, &cycle);
			}
			break;

			case STX_ABS:	// 4 cycle
			{
				// WORD address에 X레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ABS(&cycle);
				ac_WriteByte(X, addr, &cycle);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			case STY_ZP:	// 3 cycle
			{
				// ZeroPage에 X레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ZP(&cycle);
				ac_WriteByte(Y, addr, &cycle);

			}
			break;

			case STY_ZPX:	// 4 cycle
			{
				// ZP + X에 Y레지스터 내용쓰기
				WORD addr = ac_addr_mode_ZPX(&cycle);
				ac_WriteByte(Y, addr, &cycle);
			}
			break;

			case STY_ABS:	// 4 cycle
			{
				// WORD address에 Y레지스터 내용 쓰기
				WORD addr = ac_addr_mode_ABS(&cycle);
				ac_WriteByte(Y, addr, &cycle);
			}
			break;



			////////////////////////////////////////////////////////////////////////////// JUMP

			case JSR : // 6 cycle
			{
				// The JSR instruction pushes the address (minus one) of the return 
				// point on to the stack and then sets the program counter to the target memory address.
//   				WORD sr_addr = FetchWord(mem, cycle);
// 				// 스택에 PC-1을 Push
// 				PushStackWord(mem, PC - 1, cycle);
// 				PC = sr_addr;
// 				cycle--;

				BYTE lo = ac_ReadByte(PC, &cycle);
				PC++;
				//BYTE hi = (ac_ReadByte(PC, &cycle) << 8);
				WORD address = lo | (ac_ReadByte(PC, &cycle) << 8);

				ac_PushStackWord(PC, &cycle);
				PC = address;
				cycle--;
			}
			break;

			case JMP_ABS :	// 3 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				PC = addr;
			}
			break;

			case JMP_IND :	// 5 cycle
			{
				WORD addr = ac_FetchWord(&cycle);
				addr = ac_ReadWord(addr, &cycle);
				PC = addr;
			}
			break;

			case RTS :	// 6 cycle
			{
				WORD addr = ac_PopStackWord(&cycle);
				PC = addr + 1;
				cycle -= 2;

				// if( log )
				// {
				// 	k_printnum(inst);
				// 	k_printnum(PC);
				// 	k_printnum(SP);
				// 	k_print("--------");
				// 	log = false;
				// }
			}
			break;

			//////////////////////////////////////////////////////////////////////////////	STACK

			// Transfer (Stack Pointer) to X
			case TSX :	// 2 cycle
			{
				// 스택포인터를 X 레지스터로
				X = SP;
				cycle--;
				// Z / N flag
				ac_SetZeroNegative(X);
			}
			break;

			// Transfer X to (Stack Pointer)
			case TXS :	// 2 cycle
			{
				// X레지스터를 SP로
				SP = X;
				cycle--;
			}
			break;

			// Pushes a copy of the accumulator on to the stack.
			case PHA :	// 3 cycle
			{
				// A 레지스터를 스택에 Push
				ac_PushStackByte(A, &cycle);
			}
			break;

			// Pulls an 8 bit value from the stack and into the accumulator. 
			// The zero and negative flags are set as appropriate.
			case PLA :	// 4 cycle
			{
				// 스택에서 8비트를 pull --> A로
				A = ac_PopStackByte(&cycle);
				cycle--;
				// Z / N flag
				ac_SetZeroNegative(A);
			}
			break;

			// Pulls an 8 bit value from the stack and into the processor flags. 
			// The flags will take on new states as determined by the value pulled.
			case PLP :	// 4 cycle
			{
				// pop 8 bit를 --> PS (Flag) : 플레그들은 Pop된 값에의하여 새로운 플레그 상태를 갖음
#if USEOLD
				BYTE _PS = PopStackByte(mem, cycle);
				cycle--;
				_PS &= ~(FLAG_UNUSED | FLAG_BREAK);
				PS = 0;
				PS |= _PS;
				// B , Unused는 사용하지 않음
				_PS.Flag.B = 0;
				_PS.Flag.Unused = 0;
#else
				_PS.PS = ac_PopStackByte(&cycle) | FLAG_UNUSED;
				//SP++;
				//PS = ReadByte(mem, 0x100 + SP, cycle) | FLAG_UNUSED;
#endif
			}
			break;

			// Pushes a copy of the status flags on to the stack.
			case PHP :	// 3 cycle
			{
				// PS -> Stack에 Push
				BYTE _ps = _PS.PS | FLAG_BREAK;
				ac_PushStackByte(_ps, &cycle);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			case AND_IM :	// 2 cycle
			{
				A &= ac_Fetch(&cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case AND_ZP:	// 3 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				A &= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case AND_ZPX:	// 4 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				A &= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case AND_ABS:	// 4cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				A &= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case AND_ABSX:	// 4 cycle / 페이지 넘어가면 1 cycle 추가
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				A &= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case AND_ABSY:	// 4 cycle / 페이지 넘어가면 1 cycle 추가
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				A &= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case AND_INDX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				A &= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;

			case AND_INDY:	// 5 cycle / 페이지 넘어가면 1 cycle 추가
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				A &= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;


			case ORA_IM :
			{
				A |= ac_Fetch(&cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				A |= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				A |= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				A |= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				A |= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_ABSY:
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				A |= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case ORA_INDX:
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				A |= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;

			case ORA_INDY:
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				A |= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;


			case EOR_IM:
			{
				A ^= ac_Fetch(&cycle);
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_ABSY:
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;
				ac_SetZeroNegative(A);
			}
			break;

			case EOR_INDX:
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;

			case EOR_INDY:
			{
				WORD addr =ac_addr_mode_INDY(&cycle);
				A ^= ac_ReadByte(addr, &cycle);;

				ac_SetZeroNegative(A);
			}
			break;

			// Bit Test Zero page
			case BIT_ZP:
			{
				// Zp에서 읽은 값과 A를 & 테스트 하고 플레그들을 셋팅 / Set if the result if the AND is zero
				// N 플레그는 7bit, Set to bit 7 of the memory value
				// V 플레그는 6Bit , Set to bit 6 of the memory value
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE R = ac_ReadByte(addr, &cycle);;

				_PS.Flag.Z = !(A & R);	
				_PS.Flag.N = (R & FLAG_NEGATIVE) != 0;
				_PS.Flag.V = (R & FLAG_OVERFLOW) != 0;
			}
			break;

			case BIT_ABS :
			{
				// Zp에서 읽은 값과 A를 & 테스트 하고 플레그들을 셋팅
				// N 플레그는 7bit / V 플레그는 6Bit
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE R = ac_ReadByte(addr, &cycle);;

				_PS.Flag.Z = !(A & R);
				_PS.Flag.N = (R & FLAG_NEGATIVE) != 0;
				_PS.Flag.V = (R & FLAG_OVERFLOW) != 0;

			}
			break;

			////////////////////////////////////////////////////////////////////////////// Register Transfer

			case TAX :
			{
				// Transfer Accumulator to X
				X = A;
				cycle--;
				ac_SetZeroNegative(X);
			}
			break;

			case TAY:
			{
				//Transfer Accumulator to Y
				Y = A;
				cycle--;
				ac_SetZeroNegative(Y);
			}
			break;

			case TXA:
			{
				// Transfer X to Accumulator
				A = X;
				cycle--;
				ac_SetZeroNegative(A);
			}
			break;

			case TYA:
			{
				// Transfer Y to Accumulator
				A = Y;
				cycle--;
				ac_SetZeroNegative(A);
			}
			break;


			////////////////////////////////////////////////////////////////////////////// increase / decrease

			case INX :	// 2 cycle
			{
				// Increment X Register / X,Z,N = X+1
				X++;
				cycle--;
				_PS.Flag.Z = (X == 0);
				_PS.Flag.N = (X & FLAG_NEGATIVE) != 0;
			}
			break;

			case INY :	// 2 cycle
			{
				// Increment Y Register / Y,Z,N = Y+1
				Y++;
				cycle--;
				_PS.Flag.Z = (Y == 0);
				_PS.Flag.N = (Y & FLAG_NEGATIVE) != 0;
			}
			break;
			case DEX:	// 2 cycle
			{
				// Decrease X Register / X,Z,N = X+1
				X--;
				cycle--;
				_PS.Flag.Z = (X == 0);
				_PS.Flag.N = (X & FLAG_NEGATIVE) != 0;

			}
			break;

			case DEY:	// 2 cycle
			{
				// Decrement Y Register / Y,Z,N = Y+1
				Y--;
				cycle--;
				_PS.Flag.Z = (Y == 0);
				_PS.Flag.N = (Y & FLAG_NEGATIVE) != 0;

			}
			break;

			case INC_ZP:
			{
				// Increment Memory by One / M + 1 -> M
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v++;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);
			}
			break;
			case INC_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v++;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);
			}
			break;
			case INC_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v++;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);				
			}
			break;
			case INC_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v++;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);				
			}
			break;
			case DEC_ZP:// 5 cycle
			{
				// Decrement Memory by One / M - 1 -> M
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v--;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);

			}
			break;
			case DEC_ZPX: // 6 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v--;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);
			}
			break;
			case DEC_ABS:	// 6 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v--;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);
			}
			break;

			case DEC_ABSX:	// 7 cycle
			{
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				v--;
				cycle--;
				ac_WriteByte(v, addr, &cycle);
				ac_SetZeroNegative(v);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////	Arithmetic

			// Add with Carry
			// This instruction adds the contents of a memory location to the accumulator together with 
			// the carry bit.If overflow occurs the carry bit is set, this enables multiple byte addition to be performed.
			case ADC_IM	:	// 2 cycle
			{
				// A + M + C -> A, C
				BYTE v = ac_Fetch(&cycle);
				ac_Execute_ADC(v);
			}
			break;

			case ADC_ZP:	// 3 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_ZPX:	// 4 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_ABS:	// 4 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_ABSX:	// 4~5 cycle
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_ABSY:	// 4~5 cycle
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_INDX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			case ADC_INDY:	// 5~6 cycle
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ADC(v);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			// SBC - Subtract with Carry
			// A, Z, C, N = A - M - (1 - C)
			case SBC_IM	:	// 2 cycle
			{
				BYTE v = ac_Fetch(&cycle);
				ac_Execute_SBC(v);
			}
			break;

			case SBC_ZP:	// 3 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_ABSY:
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_INDX:
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			case SBC_INDY:
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_SBC(v);
			}
			break;

			////////////////////////////////////////////////////////////////////////////// Compare

			// Z,C,N = A-M
			// This instruction compares the contents of the accumulator with another memory held 
			// valueand sets the zeroand carry flags as appropriate.
			case CMP_IM:
			{
				BYTE v = ac_Fetch(&cycle);
				ac_Execute_CMP(v);
			}
			break;

			case CMP_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;
			
			case CMP_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;

			case CMP_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;

			case CMP_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;

			case CMP_ABSY:
			{
				WORD addr = ac_addr_mode_ABSY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;

			case CMP_INDX:
			{
				WORD addr = ac_addr_mode_INDX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}

			break;

			case CMP_INDY:
			{
				WORD addr = ac_addr_mode_INDY(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CMP(v);
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			// Compare X Register
			// Z, C, N = X - M
			// This instruction compares the contents of the X register with another memory
			// held value and sets the zero and carry flags as appropriate.
			case CPX_IM	:
			{
				BYTE v = ac_Fetch(&cycle);
				ac_Execute_CPX(v);
			}
			break;

			case CPX_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CPX(v);
			}
			break;

			case CPX_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CPX(v);
			}
			break;

			case CPY_IM:
			{
				BYTE v = ac_Fetch(&cycle);
				ac_Execute_CPY(v);
			}
			break;

			case CPY_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CPY(v);
			}
			break;

			case CPY_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_CPY(v);
			}
			break;

			////////////////////////////////////////////////////////////////////////////// SHIFT

			// Arithmetic Shift Left
			case ASL : // 2 cycle
			{
				// A,Z,C,N = M*2 or M,Z,C,N = M*2
				// Carry Bit 계산을 먼저해야한다. Shift할 값자체가 -(NEG)인 경우 왼쪽 shift는 Carry를 일으키기 때문
				ac_Execute_ASL(&A, &cycle);
			}
			break;

			case ASL_ZP: // 5 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ASL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;

			case ASL_ZPX: // 6 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ASL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;

			case ASL_ABS: // 6 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ASL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);

			}
			break;

			case ASL_ABSX: // 7 cycle
			{
				WORD addr = ac_addr_mode_ABSX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ASL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);

			}
			break;

			// Logical Shift Right : A,C,Z,N = A/2 or M,C,Z,N = M/2
			// Each of the bits in A or M is shift one place to the right. 
			// The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
			// Carry Flag :	Set to contents of old bit 0
			case LSR:	// 2 cycle
			{
				ac_Execute_LSR(&A, &cycle);
			}
			break;
			case LSR_ZP:	// 5 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_LSR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
}
			break;
			case LSR_ZPX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_LSR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case LSR_ABS:	// 6 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_LSR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case LSR_ABSX:	// 7 cycle
			{
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_LSR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;

			// Move each of the bits in either A or M one place to the left. 
			// Bit 0 is filled with the current value of the carry flag whilst 
			// the old bit 7 becomes the new carry flag value.
			case ROL:	// 2 cycle
			{
				ac_Execute_ROL(&A, &cycle);
			}
			break;
			case ROL_ZP:	// 5 cycle
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROL_ZPX:	// 6 cycle
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROL_ABS:	// 6 cycle
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROL_ABSX:	// 7 cycle
			{
				// 여기는 ABS No page
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROL(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;

			// Rotate Right
			// Move each of the bits in either A or M one place to the right.
			// Bit 7 is filled with the current value of the carry flag whilst 
			// the old bit 0 becomes the new carry flag value.
			case ROR:
			{
				ac_Execute_ROR(&A, &cycle);
			}
			break;
			case ROR_ZP:
			{
				WORD addr = ac_addr_mode_ZP(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROR_ZPX:
			{
				WORD addr = ac_addr_mode_ZPX(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);;
				ac_Execute_ROR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROR_ABS:
			{
				WORD addr = ac_addr_mode_ABS(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);
				ac_Execute_ROR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;
			case ROR_ABSX:
			{
				WORD addr = ac_addr_mode_ABSX_NoPage(&cycle);
				BYTE v = ac_ReadByte(addr, &cycle);
				ac_Execute_ROR(&v, &cycle);
				ac_WriteByte(v, addr, &cycle);
			}
			break;


			////////////////////////////////////////////////////////////////////////////// Branches

			// Branch if carry flag clear
			// If the carry flag is clear then add the relative displacement to the program 
			// counter to cause a branch to a new location.
			case BCC :
			{
				ac_Execute_BRANCH(_PS.Flag.C, false, &cycle);
/*
				BYTE offset = Fetch(mem, &cycle);
				if (!Flag.C)
				{
					// Page를 넘어가면 Cycle 증가
					BYTE lo = PC & 0x00FF;
					WORD t = lo + (char)offset;
					if (t > 0xFF) cycle--;

					PC += (char)offset;
					cycle--;
				}*/
			}
			break;

			// Branch if Carry Set
			// If the carry flag is set then add the relative displacement to the program 
			// counter to cause a branch to a new location.
			// BCC 반대
			case BCS: // 2 ~ 4 cycle
			{
				ac_Execute_BRANCH(_PS.Flag.C, true, &cycle);
			}
			break;

			// Branch if Equal
			// If the zero flag is set then add the relative displacement to the program counter 
			// to cause a branch to a new location.
			// 2 (+1 if branch succeeds +2 if to a new page)
			case BEQ:	// 2 cycle + Zero이면 1 cycle추가 + Page넘어가면 1 cycle 추가
			{
				ac_Execute_BRANCH(_PS.Flag.Z, true, &cycle);
			}
			break;

			// If the zero flag is clear then add the relative displacement 
			// to the program counter to cause a branch to a new location.
			// BEQ랑 반대
			case BNE:
			{
				ac_Execute_BRANCH(_PS.Flag.Z, false, &cycle);
			}
			break;

			// Branch if negative flag set
			case BMI:
			{
				ac_Execute_BRANCH(_PS.Flag.N, true, &cycle);
			}
			break;

			// Branch if negative flag clear
			case BPL:
			{
				ac_Execute_BRANCH(_PS.Flag.N, false, &cycle);
			}
			break;

			// Branch if overflow flag clear
			case BVC:
			{
				ac_Execute_BRANCH(_PS.Flag.V, false, &cycle);
			}
			break;

			// Branch if overflow flag set
			case BVS:
			{
				ac_Execute_BRANCH(_PS.Flag.V, true, &cycle);
			}
			break;

			////////////////////////////////////////////////////////////////////////////// Status Flag Changes

			// Clear carry flag
			case CLC :	// 2 cycle
			{
				_PS.Flag.C = 0;
				cycle--;
			}
			break;

			// Clear Decimal Mode
			case CLD:	// 2 cycle
			{
				_PS.Flag.D = 0;
				cycle--;
			}
			break;

			// Clear Interrupt Disable
			case CLI:	// 2 cycle
			{
				_PS.Flag.I = 0;
				cycle--;
			}
			break;

			// Clear Overflow Flag
			case CLV:	// 2 cycle
			{
				_PS.Flag.V = 0;
				cycle--;
			}
			break;

			// Set carry flag
			case SEC:	// 2 cycle
			{
				_PS.Flag.C = 1;
				cycle--;
			}
			break;

			// Set decimal mode flag
			case SED:	// 2 cycle
			{
				_PS.Flag.D = 1;
				cycle--;
			}
			break;

			// Set interrupt disable flag
			case SEI:	// 2 cycle
			{
				_PS.Flag.I = 1;
				cycle--;
			}
			break;

			//////////////////////////////////////////////////////////////////////////////

			// BRK 명령은 인터럽트 요청의 생성을 강제한다.
			// 프로그램 카운터 및 프로세서 상태가 스택에서 푸시된 다음 $ FFFE/F의 IRQ 인터럽트 벡터가 
			// PC에로드되고 상태의 중단 플래그가 1로 설정됩니다.
			case BRK :	// 7 cycle
			{
#if USEOLD
				// PC Push
				// BRK는 PC를 +1하지 않고 +2한다고 함. 그래서 PC+1 push
				// https://www.c64-wiki.com/wiki/BRK
				PushStackWord(mem, PC+1, &cycle);

				// SP Push
				BYTE _PS = PS | FLAG_BREAK | FLAG_UNUSED;
				PushStackByte(mem, _PS, &cycle);

				WORD interruptVector = ReadWord(mem, 0xFFFE, &cycle);
				PC = interruptVector;
				_PS.Flag.B = 1;
				_PS.Flag.I = 1;
#else
				PC++;
				ac_WriteByte(((PC) >> 8) & 0xFF, 0x100 + SP, &cycle);
				//SP--;
				SP = safeDec(SP, 1);
				ac_WriteByte(PC & 0xFF, 0x100 + SP, &cycle);
				//SP--;
				SP = safeDec(SP, 1);
				ac_WriteByte(_PS.PS | FLAG_BREAK, 0x100 + SP, &cycle);
				//SP--;
				SP = safeDec(SP, 1);
				_PS.Flag.I = 1;
				_PS.Flag.D = 0;
				PC = ac_ReadByte(0xFFFE, &cycle) | ac_ReadByte(0xFFFF, &cycle) << 8;
#endif
				//printf("BREAK!! : %x\n", PC);
			}
			break;

			// Return from Interrupt
			// RTI 명령은 인터럽트 처리 루틴의 끝에서 사용됩니다.
			// 프로그램 카운터 뒤에 오는 스택에서 프로세서 플래그를 가져옵니다.
			case RTI :	// 6 cycle
			{
				//BYTE PS = PopStackByte(mem, &cycle);
				//PC = PopStackWord(mem, &cycle);

				//SP++;
				SP = safeInc(SP, 1);
				_PS.PS = ac_ReadByte(0x100 + SP, &cycle);
				//SP++;
				SP = safeInc(SP, 1);
				PC = ac_ReadByte(0x100 + SP, &cycle);
				//SP++;
				SP = safeInc(SP, 1);
				PC |= ac_ReadByte(0x100 + SP, &cycle) << 8;
				cycle -= 5;
			}
			break;

			case NOP :
				cycle--;
			break;

			//////////////////////////////////////////////////////////////////////////////

			default:
				//printf("Unknown instruction : %x\n", inst);
				//throw -1;
				break;
		}

		if( prevcycle < cycle ) break;
		//tick+= prevcycle-cycle;
	}
	return cycle;
}

// void ac_LoadToRegister(unsigned long* cycle, BYTE *reg)
// {
// 	*reg = ac_Fetch(cycle);
// 	ac_SetZeroNegative(*reg);
// }

// ZP에 있는 값을 레지스터에 로드
// void ac_LoadToRegisterFromZP(unsigned long* cycle, BYTE *reg)
// {
// 	BYTE zpa = ac_Fetch(cycle);
// 	*reg = ac_ReadByte(zpa, cycle);
// 	ac_SetZeroNegative(*reg);
// }

////////////////////////////////////////////////////////////////////////////// memory addressing mode

// ZeroPage
WORD  ac_addr_mode_ZP(unsigned long*cycle)
{
	BYTE address = ac_Fetch(cycle);
	return address;
}

// Zero page + X
WORD ac_addr_mode_ZPX(unsigned long* cycle)
{
	BYTE address = ac_Fetch(cycle) + X;
	*cycle--;
	return address;
}

// Zero page + X
WORD ac_addr_mode_ZPY(unsigned long* cycle)
{
	BYTE address = ac_Fetch(cycle) + Y;
	*cycle--;
	return address;
}

// ABS
WORD ac_addr_mode_ABS(unsigned long* cycle)
{
	WORD address = ac_FetchWord(cycle);
	return address;
}

// ABS + X
WORD ac_addr_mode_ABSX(unsigned long* cycle)
{
#if 1
	BYTE lo = ac_Fetch(cycle);
	BYTE hi = ac_Fetch(cycle);
	WORD t = lo + X;
	if (t & 0xFF00) *cycle--;
	WORD address = (lo | (hi << 8)) + X;
	return address;

#else
	WORD address = Fetch(mem, &cycle);
	if ((address + X) & 0xFF00)
		cycle--;
	address |= Fetch(mem, cycle) << 8;
	address += X;
	return address;
#endif

}

// ABS + X : Page 넘어가는것 무시(그냥 하드웨어가 이렇게 생김)
WORD ac_addr_mode_ABSX_NoPage(unsigned long* cycle)
{
	WORD address = ac_FetchWord(cycle);
	address += X;
	*cycle--;
	return address;
}

// ABS + Y
WORD ac_addr_mode_ABSY(unsigned long* cycle)
{
	BYTE lo = ac_Fetch(cycle);
	BYTE hi = ac_Fetch(cycle);

	WORD t = lo + Y;
	if (t > 0xFF) *cycle--;
	WORD address = (lo | (hi << 8)) + Y;
	return address;
}

WORD ac_addr_mode_ABSY_NoPage(unsigned long* cycle)
{
	WORD address = ac_FetchWord(cycle);
	address += Y;
	*cycle--;
	return address;
}

WORD ac_addr_mode_INDX(unsigned long* cycle)
{
	BYTE t = ac_Fetch(cycle);
	BYTE inx = t + X;
	*cycle--;
	WORD address = ac_ReadWord(inx, cycle);
	return address;
}


WORD ac_addr_mode_INDY(unsigned long* cycle)
{
#if 1
	// zero page에서 word 읽고 Y레지스터와 더한 주소의 1바이트를 A에 로드
	// 읽을 주소가 page를 넘으면 1사이클 감소
	BYTE addr = ac_Fetch(cycle);
	BYTE lo = ac_ReadByte(addr, cycle);;
	BYTE hi = ac_ReadByte(addr + 1, cycle);

	WORD t = lo + Y;
	if (t > 0xFF) *cycle--;	// page 넘어감

	WORD index_addr = ((hi << 8) | lo) + Y;
	return index_addr;
#else
	BYTE addr = Fetch(mem, &cycle);
	WORD EAddr = ReadWord(mem, addr, &cycle);
	WORD EAddrY = EAddr + Y;
	const bool CrossedPageBoundary = (EAddr ^ EAddrY) >> 8;
	if (CrossedPageBoundary)
	{
		cycle--;
	}
	return EAddrY;
#endif
}

void ac_Execute_ADC(BYTE v)
{
#if !USEOLD
	BYTE oldA = A;
	WORD Result = A + v + _PS.Flag.C;
	// Decimal mode
	if (_PS.Flag.D)
		Result += ((((Result + 0x66) ^ A ^ v) >> 3) & 0x22) * 3;
	A = (Result & 0xFF);
	ac_SetZeroNegative(A);
	ac_SetCarryFlag(Result);
 	ac_SetOverflow(oldA, A, v);
#else
	// Decimal mode 무시하면 Lode runner에서 점수 Hex로 나옴

	WORD result = A + v + _PS.Flag.C;
	_PS.Flag.V = ((result ^ A) & (result ^ v) & 0x0080) != 0;
	if (_PS.Flag.D)
		result += ((((result + 0x66) ^ A ^ v) >> 3) & 0x22) * 3;
	_PS.Flag.C = result > 0xFF;
	A = result & 0xFF;
	ac_SetZeroNegative(A);
#endif
}

void ac_Execute_SBC(BYTE v)
{
#if !USEOLD
	ac_Execute_ADC(~v);
#else
	v ^= 0xFF;
	if (Flag.D)
		v -= 0x0066;
	WORD result = A + v + (Flag.C);
	Flag.V = ((result ^ A) & (result ^ v) & 0x0080) != 0;
	if (Flag.D)
		result += ((((result + 0x66) ^ A ^ v) >> 3) & 0x22) * 3;
	Flag.C = result > 0xFF;
	A = result & 0xFF;
	ac_SetZeroNegative(A);
#endif
}

void ac_Execute_CMP(BYTE v)
{
	WORD t = A - v;
	// 	Flag.N = (t & FLAG_NEGATIVE) > 0;	// Set if bit 7 of the result is set
	// 	Flag.Z = A == v;					// Set if A = M
	// 	Flag.C = A >= v;					// Set if A >= M

	_PS.Flag.Z = ((A - v) & 0xFF) == 0;
	_PS.Flag.N = ((A - v) & FLAG_NEGATIVE) != 0;
	_PS.Flag.C = (A >= v) != 0;
}

void ac_Execute_CPX(BYTE v)
{
	WORD t = X - v;
	// 	Flag.N = (t & FLAG_NEGATIVE) > 0;	// Set if bit 7 of the result is set
	// 	Flag.Z = X == v;					// Set if X = M
	// 	Flag.C = X >= v;					// Set if X >= M

	_PS.Flag.Z = ((X - v) & 0xFF) == 0;
	_PS.Flag.N = ((X - v) & FLAG_NEGATIVE) != 0;
	_PS.Flag.C = (X >= v) != 0;
}

void ac_Execute_CPY(BYTE v)
{
	WORD t = Y - v;
	// 	Flag.N = (t & FLAG_NEGATIVE) > 0;	// Set if bit 7 of the result is set
	// 	Flag.Z = Y == v;					// Set if Y = M
	// 	Flag.C = Y >= v;					// Set if Y >= M

	_PS.Flag.Z = ((Y - v) & 0xFF) == 0;
	_PS.Flag.N = ((Y - v) & FLAG_NEGATIVE) != 0;
	_PS.Flag.C = (Y >= v) != 0;
}

void ac_Execute_ASL(BYTE *v, unsigned long *cycle)
{
	_PS.Flag.C = (*v & FLAG_NEGATIVE) > 0;
	*v = *v << 1;
	*cycle--;
	ac_SetZeroNegative(*v);
}

void ac_Execute_LSR(BYTE *v, unsigned long* cycle)
{
	_PS.Flag.C = (*v & 0x01);
	*v = *v >> 1;
	*cycle--;
	ac_SetZeroNegative(*v);
}

/*
			   +------------------------------+
			   |         M or A               |
			   |   +-+-+-+-+-+-+-+-+    +-+   |
  Operation:   +-< |7|6|5|4|3|2|1|0| <- |C| <-+         N Z C I D V
				   +-+-+-+-+-+-+-+-+    +-+             / / / _ _ _
*/
void ac_Execute_ROL(BYTE *v, unsigned long* cycle)
{
	// 이전의 carry flag값을 Shift후의 0bit에 채워준다
	BYTE oldcarry = _PS.Flag.C ? 0x01 : 0x00;
	_PS.Flag.C = (*v & FLAG_NEGATIVE) > 0;
	*v <<= 1;
	*v |= oldcarry;
	*cycle--;
	ac_SetZeroNegative(*v);
}

/*
			   +------------------------------+
			   |                              |
			   |   +-+    +-+-+-+-+-+-+-+-+   |
  Operation:   +-> |C| -> |7|6|5|4|3|2|1|0| >-+         N Z C I D V
				   +-+    +-+-+-+-+-+-+-+-+             / / / _ _ _
*/
void ac_Execute_ROR(BYTE *v, unsigned long* cycle)
{
	// 최하비트가 1인가? -> 다음 캐리비트로 설정
	BYTE oldcarry = (*v & FLAG_CARRY) > 0;
	*v = *v >> 1;
	// 이전 Carry가 1이면 NEGATIVE 채움
	*v |= (_PS.Flag.C ? FLAG_NEGATIVE : 0);
	*cycle--;
	_PS.Flag.C = oldcarry;
	ac_SetZeroNegative(*v);
}


void ac_Execute_BRANCH(bool v, bool condition, unsigned long*cycle)
{
	SBYTE offset = (SBYTE)ac_Fetch(cycle);
	if (v == condition)
	{
#if !USEOLD
		// Page를 넘어가면 Cycle 증가
		BYTE lo = PC & 0x00FF;
		WORD t = lo + (SBYTE)offset;
		if (t > 0xFF) *cycle--;

		PC += (SBYTE)offset;
		*cycle--;
#else
		if (offset & FLAG_NEGATIVE)
			offset |= 0xFF00;  // jump backward
		if (((PC & 0xFF) + offset) & 0xFF00)  // page crossing
			cycle--;
		PC += offset;
		cycle--;
#endif
	}
}

