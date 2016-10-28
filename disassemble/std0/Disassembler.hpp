#ifndef HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED
#define HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED

#include <cstdio>
#include "../../additional/exception/Exception.hpp"

#define FILENAME "std0/Assembler.hpp"
#define PROGRAM_POS __FILENAME__, __FUNCTION__, __LINE__

using namespace MyExceptionCharStringRepresentation;

const char    MAGIC_NUM = 0xBA;
const char STANDARD_NUM = 0;

enum class : unsigned Commands
{
	END,
	PUSH,
	POP,
	SUM,
	SUB,
	MUL
};

void disassemble(const char const* filename, const char const* newFilePath)
{
	std::ifstream opened(filename, std::ios::binary);

	// Checking stuff:
	
	if (MAGIC_NUM != opened.get())
	{
		throw Exception("disassemble(): Incorrect file format", PROGRAM_POS);
	}

	if (STANDARD_NUM < opened.get())
	{
		throw Exception("disassemble(): Unable to parse files of future standards", PROGRAM_POS);
	}
}

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED*/