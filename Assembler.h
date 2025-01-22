#pragma once

#include "Foundation.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class ArgType
{
	REGISTER,
	ADDRESS,
};

class Assembler
{
private:

	static const std::unordered_map<std::string, std::pair<u8, std::vector<ArgType>>> Opcodes;

	static bool Match(std::string& source, std::string matcher)
	{
		if (matcher.length() > source.size())
		{
			return false;
		}

		std::string current = std::string(source.begin(), source.begin() + matcher.length());
		
		if (current != matcher)
		{
			return false;
		}

		current.erase(matcher.length());
		return true;
	}

	static std::string Next(std::string& line)
	{
		return std::string(line.begin(), line.begin() + line.find_first_of(" "));
	}

	static void ConsumeWhitespace(std::string& line)
	{
		line = std::string(line.begin() + line.find_first_not_of(" "), line.end());
	}

	static u8 ParseRegister(std::string& line)
	{
		// Check for 'r' prefix
		if (line.empty() || line[0] != 'r') {
			throw std::runtime_error("Expected a register (e.g., rN), but got: " + line);
		}

		// Extract the number after 'r'
		size_t pos = 1;  // Start after 'r'
		while (pos < line.size() && isdigit(line[pos])) {
			++pos;
		}

		if (pos == 1) {
			throw std::runtime_error("Missing register number after 'r': " + line);
		}

		int regNum = std::stoi(line.substr(1, pos - 1));
		if (regNum < 0 || regNum > 255) {  // Assuming registers are 8-bit
			throw std::runtime_error("Invalid register number: " + std::to_string(regNum));
		}

		// Remove the parsed register from the line
		line = line.substr(pos);

		return static_cast<u8>(regNum);
	}

	static u16 ParseAddress(std::string& line)
	{
		// Check for '0x' prefix
		if (line.size() < 2 || line[0] != '0' || (line[1] != 'x' && line[1] != 'X')) {
			throw std::runtime_error("Expected an address (e.g., 0xABCD), but got: " + line);
		}

		// Extract the hexadecimal part
		size_t pos = 2;  // Start after '0x'
		while (pos < line.size() && isxdigit(line[pos])) {
			++pos;
		}

		if (pos == 2) {
			throw std::runtime_error("Missing hexadecimal address after '0x': " + line);
		}

		// Convert the hexadecimal string to an integer
		uint16_t address = static_cast<uint16_t>(std::stoul(line.substr(2, pos - 2), nullptr, 16));

		// Remove the parsed address from the line
		line = line.substr(pos);

		return address;
	}

public:

	static void Assemble(const std::string& infilename, const std::string& outfilename)
	{
		std::ifstream infile(infilename);
		std::ofstream outfile(outfilename, std::ios::binary);

		std::stringstream sourceStream;
		sourceStream << infile.rdbuf();
		std::string source = sourceStream.str();

		std::string buffer;
		while (std::getline(infile, buffer))
		{
			std::string instruction = Next(source);
			ConsumeWhitespace(source);

			if (Opcodes.count(instruction))
			{
				const auto [opcode, args] = Opcodes.at(instruction);
				outfile.write(reinterpret_cast<const char*>(&opcode), sizeof(opcode));

				for (ArgType arg : args)
				{
					switch (arg)
					{
					case ArgType::REGISTER:
						u8 reg = ParseRegister(source);
						outfile.write(reinterpret_cast<const char*>(&reg), sizeof(reg));
					case ArgType::ADDRESS:
						u16 addr = ParseAddress(source);
						outfile.write(reinterpret_cast<const char*>(&addr), sizeof(addr));
					default:
						throw std::runtime_error("Unknown addressing mode.");
					}
					ConsumeWhitespace(source);
				}
			}
		}
	}

};

const std::unordered_map<std::string, std::pair<u8, std::vector<ArgType>>> Assembler::Opcodes = {
	{"ADD", {0x00, {}}},
	{"SUB", {0x01, {}}},
	{"MUL", {0x02, {}}},
	{"DIV", {0x03, {}}},
	{"AND", {0x04, {}}},
	{"OR",  {0x05, {}}},
	{"XOR", {0x06, {}}},
	{"NOT", {0x07, {}}},
	{"LOAD", {0x08, {ArgType::REGISTER, ArgType::ADDRESS}}},
	{"STORE", {0x09, {ArgType::ADDRESS, ArgType::REGISTER}}},
	{"MOVE", {0x0A, {ArgType::REGISTER, ArgType::REGISTER}}},
	{"JMP",  {0x0B, {ArgType::ADDRESS}}},
	{"JMPZ", {0x0C, {ArgType::ADDRESS}}},
	{"CALL", {0x0D, {ArgType::ADDRESS}}},
	{"RET",  {0x0E, {}}},
	{"HALT", {0x0F, {}}},
	{"SYSCALL", {0xA0, {}}},
};
