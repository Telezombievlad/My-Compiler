#ifndef HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED
#define HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED

// Includes:

#include <cstring>
#include <string>
#include <cstdlib>

#include "../../../additional/exception/MyException.hpp"
#include "../../../additional/writing_files/FileWork.hpp"
#include "../../../standard/Standard0.hpp"

// Defines:

#define FILENAME "std0/Disassembler.hpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

namespace DisAssemblerStd0
{
	using namespace MyExceptionCharStringRepresentation;

	namespace _address
	{
		void readAndWriteAddress(FileWork::ReadBinaryFile& istream, FileWork::WriteTextFile& ostream)
		{
			try
			{
				std::vector<unsigned char> bytes{istream.getBytes(sizeof(MyStd0::Adr_t))};

				MyStd0::Adr_t* pAddress = reinterpret_cast<MyStd0::Adr_t*>(bytes.data());

				if (*pAddress < 0 || *pAddress >= MyStd0::_address::ADDRESS_COUNT)
				{
					throw Exception("Incorrect address read", "", "-", 0);
				}

				ostream.writeWord(FileWork::Word(MyStd0::_address::ADDRESSES[*pAddress]));
			}
			catch (Exception exc)
			{
				throw Exception("Unable to read address", "", "-", 0, exc);
			}
		}

	} // namespace _address

	namespace _value
	{
		void readAndWriteValue(FileWork::ReadBinaryFile& istream, FileWork::WriteTextFile& ostream)
		{
			try
			{
				std::vector<unsigned char> bytes{istream.getBytes(sizeof(MyStd0::Val_t))};

				MyStd0::Val_t* pValue = reinterpret_cast<MyStd0::Val_t*>(bytes.data());
 
				std::string valStr = std::to_string(*pValue);

				ostream.writeWord(FileWork::Word(valStr.c_str()));
			}
			catch (Exception exc)
			{
				throw Exception("Unable to read value", "", "-", 0, exc);
			}
		}

	} // namespace _value

	namespace _command
	{
		void readAndWriteCommand(FileWork::ReadBinaryFile& istream, FileWork::WriteTextFile& ostream)
		{
			try
			{
				std::vector<unsigned char> bytes{istream.getBytes(sizeof(MyStd0::Cmd_t))};

				MyStd0::Cmd_t* pCommand = reinterpret_cast<MyStd0::Cmd_t*>(bytes.data());

				MyStd0::Cmd_t command = *pCommand;

				if (command < 0 || command >= MyStd0::_command::COMMAND_COUNT)
				{
					throw Exception("Incorrect command read", "", "-", 0);
				}

				ostream.writeWord(FileWork::Word(MyStd0::_command::COMMANDS[command].name));

				for (auto argType : MyStd0::_command::COMMANDS[command].argTypes)
				{
					ostream.writeWord(FileWork::Word(" "));

					if (argType == MyStd0::_command::ArgType::ADDRESS)
					{
						_address::readAndWriteAddress(istream, ostream);
					}
					else if (argType == MyStd0::_command::ArgType::VALUE)
					{
						_value::readAndWriteValue(istream, ostream);
					}
					else
					{
						throw Exception("Unexpected argType", PROGRAM_POS);
					}
				}

				ostream.writeWord(FileWork::Word("\n"));
			}
			catch (Exception exc)
			{
				throw Exception("Unable to read command", "", "-", 0, exc);
			}
		}

	} // namespace _command

	void disassemble(const char* inputFile, const char* outputFile)
	{
		FileWork::ReadBinaryFile istream{inputFile};
		FileWork::WriteTextFile  ostream{outputFile};

		unsigned char magicNum = istream.getBytes(1).at(0);
		if (magicNum != MyStd0::MAGIC_NUM)
		{
			throw Exception("Unknown file format", inputFile, "-", 0);
		}

		unsigned char stdNum = istream.getBytes(1).at(0);		
		if (stdNum != MyStd0::STD_NUM)
		{
			throw Exception("Unknown standard", inputFile, "-", 0);
		}

		while (true)
		{
			try
			{
				_command::readAndWriteCommand(istream, ostream);
			}
			catch (...) { break; }
		}
	}

} // namespace DisAssemblerStd0

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED*/