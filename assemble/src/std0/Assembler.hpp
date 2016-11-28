#ifndef HEADER_GUARD_MY_COMPILER_ASSEMBLER_HPP_INCLUDED
#define HEADER_GUARD_MY_COMPILER_ASSEMBLER_HPP_INCLUDED

// Includes:

#include <vector>

#include <algorithm>
#include <utility>

#include "../../../additional/exception/MyException.hpp"
#include "../../../additional/writing_files/FileWork.hpp"
#include "../../../standard/Standard0.hpp"

// Defines:

#define FILENAME "std0/Assembler.hpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

// Code:
namespace AssemblerStd0
{
	using namespace MyExceptionCharStringRepresentation;

	namespace _preprocess
	{
		// Preprocessing: 

			std::vector<FileWork::Word> preprocessAndSeperate(const char* filename)
			{
				FileWork::ReadTextFile stream{filename};

				std::vector<FileWork::Word> toReturn{};

				bool inComment = false;
				size_t commentLine = 0;
				for (FileWork::Word curWord = stream.getWord(); curWord != FileWork::NULL_WORD; curWord = stream.getWord())
				{
					if (commentLine != curWord.line) inComment = false;

					if (!inComment) 
					{
						inComment = std::strstr(curWord.word, MyStd0::SINGLE_LINE_COMMENT) == curWord.word;
						commentLine = curWord.line;
					}

					if (!inComment)
					{
						toReturn.push_back(curWord);
					}
				}

				return toReturn;
			}

	} // namespace _preprocess

	namespace _address
	{
		void writeAdressByWord(FileWork::WriteBinaryFile& stream, const FileWork::Word& word)
		{
			using namespace MyStd0::_address;

			for (MyStd0::Adr_t addrI = 0; addrI < MyStd0::_address::ADDRESS_COUNT; ++addrI)
			{
				if (std::strcmp(word.word, ADDRESSES[addrI]) == 0) 
				{
					unsigned char* bytes = reinterpret_cast<unsigned char*>(&addrI);

					stream.writeBytes(std::vector<unsigned char>(bytes, bytes + sizeof(MyStd0::Adr_t)));

					return;
				}
			}
			
			throw Exception("Unable to recognise adress", word.file, word.word, word.line);
		}

	} // namespace _address

	namespace _value
	{
		void writeValueByWord(FileWork::WriteBinaryFile& stream, const FileWork::Word& word)
		{
			using namespace MyStd0;

			try
			{
				Val_t value = static_cast<MyStd0::Val_t>(std::atoi(word.word));

				unsigned char* bytes = reinterpret_cast<unsigned char*>(&value);

				stream.writeBytes(std::vector<unsigned char>(bytes, bytes + sizeof(MyStd0::Val_t)));
			}
			catch (std::exception exc)
			{
				throw Exception("Unable to recognise value", word.file, word.word, word.line, exc);
			}
		}

	} // namespace _value

	namespace _command
	{
		void writeCmdByWord
		(
			FileWork::WriteBinaryFile& stream,
			const std::vector<FileWork::Word>& words, 
			size_t& wordPos
		)
		{
			using namespace MyStd0::_command;

			if (wordPos >= words.size())
			{
				throw Exception("wordPos >= words.size()", PROGRAM_POS);
			}

			for (MyStd0::Cmd_t cmdI = 0; cmdI < COMMAND_COUNT; ++cmdI)
			{
				if (std::strcmp(words[wordPos].word, COMMANDS[cmdI].name) == 0)
				{
					unsigned char bytes[sizeof(MyStd0::Cmd_t)];

					std::memset(bytes, cmdI, sizeof(MyStd0::Cmd_t));

					stream.writeBytes(std::vector<unsigned char>(bytes, bytes + sizeof(MyStd0::Cmd_t)));

					MyStd0::Cmd_t cmdNamePos = wordPos;

					for (auto argType : COMMANDS[cmdI].argTypes)
					{
						++wordPos;

						if (wordPos >= words.size())
						{
							throw Exception("Argument mismatch", words[cmdNamePos].file, COMMANDS[cmdI].name, words[cmdNamePos].line);
						}

						if (argType == ArgType::ADDRESS)
						{
							_address::writeAdressByWord(stream, words[wordPos]);
						}
						else if (argType == ArgType::VALUE)
						{
							_value::writeValueByWord(stream, words[wordPos]);
						}
						else
						{
							throw Exception("Unexpected argType", PROGRAM_POS);
						}
					}

					return;
				}
			}	

			throw Exception("Unable to recognise command name", words[wordPos].file, words[wordPos].word, words[wordPos].line);	
		}

	} // namespace _command

	void assemble(const char* src, const char* dest)
	{
		FileWork::WriteBinaryFile stream{dest};

		stream.writeBytes(std::vector<unsigned char>(1, MyStd0::MAGIC_NUM));
		stream.writeBytes(std::vector<unsigned char>(1, MyStd0::STD_NUM));

		std::vector<FileWork::Word> words{_preprocess::preprocessAndSeperate(src)};

		for (size_t curWord = 0; curWord < words.size(); ++curWord)
		{
			_command::writeCmdByWord(stream, words, curWord);
		}
	}
}

// Undefs:

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_MY_COMPILER_ASSEMBLER_HPP_INCLUDED*/