#pragma once

#include "Foundation.h"

#include <array>
#include <atomic>
#include <thread>

class Processor
{
private:

	u8  R[8];  // general purpose registers
	u8  SR;    // status register
	u16 SP;    // stack pointer
	u16 PC;    // program counter

	static constexpr u8 ZERO = 0x01;

	static constexpr u16 STACK_START = 0x00FF;
	static constexpr u16 RESET_VECTOR = 0x0100;

	std::array<u8, 65536> memory;  // 64 KiB

	std::thread executor;
	std::atomic<bool> running;

	void Reset();

	u8 FetchByte();
	u16 FetchWord();

	void Push(u8 value);
	u8 Pop();

	void SetZero(u8 value);
	
	bool Zero();

public:

	Processor(const std::array<u8, 65536>& rom);

	void FetchDecodeExecute();

	void ADD();
	void SUB();
	void MUL();
	void DIV();

	void AND();
	void OR();
	void XOR();
	void NOT();

	void LOAD();
	void STORE();
	void MOVE();

	void JMP();
	void JMPZ();
	void CALL();
	void RET();

	void HALT();
	void SYSCALL();

};
