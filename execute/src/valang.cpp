#include <cstdlib>
#include <iostream>

#include "std1/CPU.hpp"
#include "../../additional/exception/MyException.hpp"

// Defines:

#define FILENAME "std1/execute.cpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

using namespace MyExceptionCharStringRepresentation;

int main(int argc, const char* argv[])
{
	try
	{
		if (argc != 2)
		{
			throw Exception("Input pattern: valang <src>", PROGRAM_POS);
		}

		EmulatedProcessorStd1::execute(argv[1]);
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