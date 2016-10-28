#ifndef HEADER_GUARD_MY_COMPILER_CPU_HPP_INCLUDED
#define HEADER_GUARD_MY_COMPILER_CPU_HPP_INCLUDED

// Includes:

#include "../../../additional/exception/MyException.hpp"
#include "../../../additional/writing_files/FileWork.hpp"
#include "../../../additional/stack/Stack.hpp"

#include "../../../standard/Standard0.hpp"

// Defines:

#define FILENAME "std0/CPU.hpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

namespace EmulatedProcessorStd0
{
	using namespace MyExceptionCharStringRepresentation;     
	using namespace MyStackStaticArrayRepresentation;    

	namespace _address
	{
		MyStd0::Adr_t getAddress(FileWork::ReadBinaryFile& stream)
		{
			try
			{
				std::vector<unsigned char> bytes{stream.getBytes(sizeof(MyStd0::Adr_t))};

				MyStd0::Adr_t* data = reinterpret_cast<MyStd0::Adr_t*>(bytes.data());

				return *data;
			}
			catch (std::exception exc)
			{
				throw Exception("Unable to get address", PROGRAM_POS, exc);
			}
		}
	} // namespace _address

	namespace _value
	{
		MyStd0::Val_t getValue(FileWork::ReadBinaryFile& stream)
		{
			try
			{
				std::vector<unsigned char> bytes{stream.getBytes(sizeof(MyStd0::Val_t))};

				MyStd0::Val_t* data = reinterpret_cast<MyStd0::Val_t*>(bytes.data());

				return *data;
			}
			catch (std::exception exc)
			{
				throw Exception("Unable to get value", PROGRAM_POS, exc);
			}
		}
	} // namespace _value

	namespace _command
	{
		MyStd0::Cmd_t getCommand(FileWork::ReadBinaryFile& stream)
		{
			try
			{
				std::vector<unsigned char> bytes{stream.getBytes(sizeof(MyStd0::Cmd_t))};

				MyStd0::Cmd_t* data = reinterpret_cast<MyStd0::Cmd_t*>(bytes.data());

				return *data;
			}
			catch (Exception exc)
			{
				throw Exception("Unable to get command", PROGRAM_POS, exc);
			}
			catch (std::exception exc)
			{
				throw Exception("Unable to get command", PROGRAM_POS, exc);
			}
		}
	}

	void execute(const char* filename)
	{
		FileWork::ReadBinaryFile stream{filename};

		unsigned char magicNum = stream.getBytes(1).at(0);
		if (magicNum != MyStd0::MAGIC_NUM)
		{
			throw Exception("Unknown file format", filename, "-", 0);
		}

		unsigned char stdNum = stream.getBytes(1).at(0);		
		if (stdNum != MyStd0::STD_NUM)
		{
			throw Exception("Unknown standard", filename, "-", 0);
		}

		MyStd0::_processor::CPU cpu{};

		try
		{
			MyStd0::Cmd_t curCmd = 0;
			MyStd0::Adr_t address = 0;
			MyStd0::Val_t value = 0;

			while (true)
			{
				try
				{
					curCmd = _command::getCommand(stream);
				}
				catch (...) { break; }

				for (auto argType : MyStd0::_command::COMMANDS[curCmd].argTypes)
				{
					if (argType == MyStd0::_command::ArgType::ADDRESS)
					{
						address = _address::getAddress(stream);
					}
					else if (argType == MyStd0::_command::ArgType::VALUE)
					{
						value = _value::getValue(stream);
					}
					else
					{
						throw Exception("Unexpected argType", PROGRAM_POS);
					}
				}

				MyStd0::_command::COMMANDS[curCmd].func(cpu, address, value);
			}
		}
		catch (Exception exc)
		{
			throw Exception("Runtime error", filename, "-", 0, exc);
		}
		catch (int returnFromEndFunction)
		{
			if (returnFromEndFunction != 0) throw Exception("Runtime error", filename, "Error code:", returnFromEndFunction);
		}
	}

} // EmulatedProcessorStd0

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_MY_COMPILER_CPU_HPP_INCLUDED*/