#ifndef HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED
#define HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED

// Includes:

#include <cstring>
#include <map>
#include <vector>
#include <string>

#include <iostream>

#include "../../../additional/exception/MyException.hpp"
#include "../../../additional/writing_files/FileWork.hpp"
#include "../../../standard/Standard1.hpp"

// Defines:

#define FILENAME "std1/Disassembler.hpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:

namespace DisAssemblerStd1
{
	using namespace MyExceptionCharStringRepresentation;

	namespace _additional
	{
		template <typename ToRead> 
		ToRead getFromBinaryFile(FileWork::ReadBinaryFile& stream)
		{
			std::vector<unsigned char> bytes{stream.getBytes(sizeof(ToRead))};

			ToRead* data = reinterpret_cast<ToRead*>(bytes.data());

			return *data;
		}

	} // namespace _additional

	namespace _registers
	{
		FileWork::Word readRegister(FileWork::ReadBinaryFile& istream)
		{
			MyStd1::RegAdr_t reg = _additional::getFromBinaryFile<MyStd1::RegAdr_t>(istream);

			if (reg >= MyStd1::_registers::REGISTER_COUNT)
			{
				throw Exception("Incorrect register read", PROGRAM_POS);
			}

			std::string toReturn = std::string(" ") + std::string(MyStd1::_registers::REGISTERS[reg]);
			return FileWork::Word(toReturn.c_str());
		}

	} // namespace _registers

	namespace _value
	{
		FileWork::Word readValue(FileWork::ReadBinaryFile& istream)
		{
			MyStd1::Val_t val = _additional::getFromBinaryFile<MyStd1::Val_t>(istream);

			std::string toReturn = std::string(" ") + std::to_string(val);
			return FileWork::Word(toReturn.c_str());
		}

	} // namespace _value

	namespace _nameTags
	{
		MyStd1::CmdNum_t readNameTag(FileWork::ReadBinaryFile& istream)
		{
			return _additional::getFromBinaryFile<MyStd1::CmdNum_t>(istream);
		}

		void insertNameTagsWhereNecessary
		(
			std::vector<FileWork::Word>& words,
			std::map<MyStd1::CmdNum_t, std::vector<size_t>>& placesToInsert
		)
		{
			for (auto& place : placesToInsert)
			{
				std::string nameTagName = std::string(" ") + std::to_string(place.first);

				for (auto& toInsertIndex : place.second)
				{
					words.at(toInsertIndex) = FileWork::Word(nameTagName.c_str());
				}
			}

			size_t shift = 0;
			for (auto& place : placesToInsert)
			{
				std::string toInsert = std::string("\n") + std::to_string(place.first) + std::string(":");
				words.insert
				(
					words.begin() + shift + place.first, 
					FileWork::Word(toInsert.c_str())
				);

				++shift;
			}
		}

	} // namespace _nameTags

	namespace _command
	{
		void readCommand
		(
			FileWork::ReadBinaryFile& istream, 
			std::vector<FileWork::Word>& words, 
			std::map<MyStd1::CmdNum_t, std::vector<size_t>>& placesToInsert
		)
		{
			try
			{
				MyStd1::Cmd_t cmd = _additional::getFromBinaryFile<MyStd1::Cmd_t>(istream);

				if (cmd >= MyStd1::_command::COMMAND_COUNT)
				{
					throw Exception("Incorrect command read", PROGRAM_POS);
				}

				std::string cmdName = std::string("\n") + std::string(MyStd1::_command::COMMANDS[cmd].name.word);
				words.push_back(FileWork::Word(cmdName.c_str()));

				for (auto argType : MyStd1::_command::COMMANDS[cmd].argTypes)
				{
					switch (argType)
					{
						case MyStd1::_command::ArgType::REGISTER_ADDRESS:
						{
							words.push_back(_registers::readRegister(istream));
							break;
						}
						case MyStd1::_command::ArgType::VALUE:
						{
							words.push_back(_value::readValue(istream));
							break;
						}
						case MyStd1::_command::ArgType::NAMETAG:
						{
							MyStd1::CmdNum_t cmdNum = _nameTags::readNameTag(istream);

							placesToInsert[cmdNum].push_back(words.size());

							words.push_back(FileWork::Word("Insert nametag here"));

							break;
						}
						default: 
							throw Exception("Unexpected argType", PROGRAM_POS); 
							break;
					}	
				}
			}
			catch (Exception& exc)
			{
				throw Exception("Unable to read command from executable", PROGRAM_POS, exc);
			}
		}

	} // namespace _command

	void disassemble(const char* inputFile, const char* outputFile)
	{
		FileWork::ReadBinaryFile istream{inputFile};
		FileWork::WriteTextFile  ostream{outputFile};

		std::vector<FileWork::Word> words{};
		std::map<MyStd1::CmdNum_t, std::vector<size_t>> placesToInsert{};

		try
		{
			if (_additional::getFromBinaryFile<MyStd1::MagicNum_t>(istream) != MyStd1::MAGIC_NUM)
			{
				throw Exception("Unknown file format", FILENAME, "-", 0);
			}

			if (_additional::getFromBinaryFile<MyStd1::StdNum_t>(istream) != MyStd1::STD_NUM)
			{
				throw Exception("Unknown standard", FILENAME, "-", 0);
			}
		}
		catch (Exception& exc)
		{
			throw Exception("Unable to read file", FILENAME, "-", 0, exc);
		}

		try
		{
			while (!istream.finished())
			{
				_command::readCommand(istream, words, placesToInsert);
			}

			_nameTags::insertNameTagsWhereNecessary(words, placesToInsert);

			for (auto& word : words)
			{
				ostream.writeWord(word);
			}
		}
		catch (Exception& exc)
		{
			throw Exception("Disassembling error", FILENAME, "-", 0, exc);
		}
	}

} // namespace DisAssemblerStd0

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_DISASSEMBLER_HPP_INCLUDED*/