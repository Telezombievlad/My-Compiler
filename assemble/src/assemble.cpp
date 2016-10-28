// Includes:

#include <iostream>

#include "std0/Assembler.hpp"
#include "../../additional/exception/MyException.hpp"

// Defines:

#define FILENAME "src/assemble.cpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

using namespace MyExceptionCharStringRepresentation;

int main(int argc, const char* argv[])
{
	try
	{
		if (argc != 4)
		{
			throw Exception("Input pattern: assemble <src> --std=<std> <dest>", PROGRAM_POS);
		}

		if (std::strcmp(argv[2], "--std=0") == 0) AssemblerStd0::assemble(argv[1], argv[3]);
		else throw Exception("Unknown standard", PROGRAM_POS);
	}
	catch (Exception ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch (std::exception ex)
	{
		std::cout << ex.what() << std::endl;
	}	

	return EXIT_SUCCESS;
}

#undef FILENAME
#undef PROGRAM_POS