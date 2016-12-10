#include "std1/Disassembler.hpp"
#include "../../additional/exception/MyException.hpp"

// Defines:

#define FILENAME "std1/disassemble.cpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

using namespace MyExceptionCharStringRepresentation;

int main(int argc, const char* argv[])
{
	try
	{
		if (argc != 3)
		{
			throw Exception("Input pattern: disassemble <src> <dest>", PROGRAM_POS);
		}

		DisAssemblerStd1::disassemble(argv[1], argv[2]);
	}
	catch (Exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}	

	return EXIT_SUCCESS;
}

#undef FILENAME
#undef PROGRAM_POS