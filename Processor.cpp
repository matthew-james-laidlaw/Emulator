#include "Processor.h"

#include <stdexcept>

Processor::Processor(const std::array<u8, 65536>& rom)
{
	std::copy(rom.begin(), rom.end(), memory.begin());
	Reset();
	executor = std::thread([&]()
		{
			running = true;
			while (running)
			{
				FetchDecodeExecute();
			}
		});
}

void Processor::Reset()
{
	SR = 0x00;
	SP = STACK_START;
	PC = RESET_VECTOR;
}

u8 Processor::FetchByte()
{
	return memory[PC++];
}

u16 Processor::FetchWord()
{
	u8 hi = FetchByte();
	u8 lo = FetchByte();
	return (static_cast<u16>(hi) << 8) | static_cast<u16>(lo);
}

void Processor::Push(u8 value)
{
	memory[--SP] = value;
}

u8 Processor::Pop()
{
	return memory[SP++];
}

void Processor::SetZero(u8 value)
{
	value == 0 ? SR |= ZERO : SR &= ~ZERO;
}

bool Processor::Zero()
{
	return (SR & (1 << ZERO)) != 0;
}

void Processor::FetchDecodeExecute()
{
	u8 opcode = FetchByte();
	switch (opcode)
	{
	case 0x00: ADD(); break;
	case 0x01: SUB(); break;
	case 0x02: MUL(); break;
	case 0x03: DIV(); break;
	case 0x04: AND(); break;
	case 0x05: OR(); break;
	case 0x06: XOR(); break;
	case 0x07: NOT(); break;
	case 0x08: LOAD(); break;
	case 0x09: STORE(); break;
	case 0x0A: MOVE(); break;
	case 0x0B: JMP(); break;
	case 0x0C: JMPZ(); break;
	case 0x0D: CALL(); break;
	case 0x0E: RET(); break;
	case 0x0F: HALT(); break;
	case 0xA0: SYSCALL(); break;
	default: throw std::runtime_error("Unknown opcode.");
	}
}

void Processor::ADD()
{
	R[0] = R[1] + R[2];
	SetZero(R[0]);
}

void Processor::SUB()
{
	R[0] = R[1] - R[2];
	SetZero(R[0]);
}

void Processor::MUL()
{
	R[0] = R[1] * R[2];
	SetZero(R[0]);
}

void Processor::DIV()
{
	R[0] = R[1] / R[2];
	SetZero(R[0]);
}

void Processor::AND()
{
	R[0] = R[1] & R[2];
	SetZero(R[0]);
}

void Processor::OR()
{
	R[0] = R[1] | R[2];
	SetZero(R[0]);
}

void Processor::XOR()
{
	R[0] = R[1] ^ R[2];
	SetZero(R[0]);
}

void Processor::NOT()
{
	R[0] = ~R[1];
	SetZero(R[0]);
}

void Processor::LOAD()
{
	u8 reg = FetchByte();
	u16 addr = FetchWord();
	R[reg] = memory[addr];
}

void Processor::STORE()
{
	u8 reg = FetchByte();
	u16 addr = FetchWord();
	memory[addr] = R[reg];
}

void Processor::MOVE()
{
	u8 r1 = FetchByte();
	u8 r2 = FetchByte();
	R[r1] = R[r2];
}

void Processor::JMP()
{
	u16 addr = FetchWord();
	PC = addr;
}

void Processor::JMPZ()
{
	u16 addr = FetchWord();
	if (Zero())
	{
		PC = addr;
	}
}

void Processor::CALL()
{
	u8 lo = static_cast<u8>(PC & 0xFF);
	u8 hi = static_cast<u8>((PC >> 8) & 0xFF);
	Push(lo);
	Push(hi);
	u16 addr = FetchWord();
	PC = addr;
}

void Processor::RET()
{
	u8 hi = Pop();
	u8 lo = Pop();
	u16 addr = (static_cast<u16>(hi) << 8) | static_cast<u16>(lo);
	PC = addr;
}

void Processor::HALT()
{
	running = false;
	executor.join();
}

void Processor::SYSCALL()
{
	throw std::runtime_error("Not yet implemented.");
}
