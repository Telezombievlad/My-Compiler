#ifndef HEADER_GUARD_MY_COMPILER_STANDARD0_HPP_INCLUDED
#define HEADER_GUARD_MY_COMPILER_STANDARD0_HPP_INCLUDED

// Includes:

#include <string>
#include <array>
#include <iostream>

#include "../additional/exception/MyException.hpp"
#include "../additional/writing_files/FileWork.hpp"
#include "../additional/stack/Stack.hpp"

// Defines:

#define FILENAME "standard/Standard0.hpp"
#define PROGRAM_POS FILENAME, __FUNCTION__, __LINE__

namespace MyStd0
{
	using namespace MyExceptionCharStringRepresentation;
	using namespace MyStackStaticArrayRepresentation;

	using Adr_t = unsigned char;
	using Val_t = unsigned int;
	using Cmd_t = unsigned char;

	unsigned char MAGIC_NUM = 0xBA;
	unsigned char STD_NUM = 0;

	const char* SINGLE_LINE_COMMENT = "//";

	//-----------------------------------------------------------------------------

	namespace _address
	{
		const Adr_t ADDRESS_COUNT = 4;

		const char* ADDRESSES[ADDRESS_COUNT] =
		{
			"AX",
			"BX",
			"CX",
			"DX"
		};

		const std::array<Val_t, ADDRESS_COUNT> DEFAULT_REGISTERS{{0, 0, 0, 0}};

	} // namespace _address

	//-----------------------------------------------------------------------------

	namespace _value
	{
		// I am sure, something useful will be there in future versions

	} // namespace _value

	//-----------------------------------------------------------------------------

	namespace _processor
	{
		const size_t STACK_SIZE = 256;

		struct CPU
		{
		public:
			// Variables:
				std::array<Val_t, _address::ADDRESS_COUNT> regs;
				Stack<Val_t, STACK_SIZE> st; 

			// Ctor:
				CPU() : 
					regs (_address::DEFAULT_REGISTERS),
					st   (Stack<Val_t, STACK_SIZE>())
				{}
		};
	}

	//-----------------------------------------------------------------------------

	namespace _command
	{
		using namespace _processor;

		enum class ArgType
		{
			ADDRESS,
			VALUE
		};

		struct Command
		{
		public:
			// Variables:
				const char* name;
				const std::vector<ArgType> argTypes;

				std::function<void(CPU&, Adr_t, Val_t)> func;			
		};

		const Cmd_t COMMAND_COUNT = 9;

		const Command COMMANDS[COMMAND_COUNT] =
		{
			{  "END",                 {}, [](CPU&    , Adr_t    , Val_t    ){ throw 0;                                               } },
			{ "PUSH",   {ArgType::VALUE}, [](CPU& cpu, Adr_t    , Val_t val){ cpu.st.push(val);                                      } },
			{"PUSHR", {ArgType::ADDRESS}, [](CPU& cpu, Adr_t adr, Val_t    ){ cpu.st.push(cpu.regs[adr]);                            } },
			{  "POP",                 {}, [](CPU& cpu, Adr_t    , Val_t    ){ cpu.st.pop();                                          } },
			{ "POPR", {ArgType::ADDRESS}, [](CPU& cpu, Adr_t adr, Val_t    ){ cpu.regs[adr] = cpu.st.pop();                          } }, 
			{  "SUM",                 {}, [](CPU& cpu, Adr_t    , Val_t    ){ Val_t r = cpu.st.pop(); cpu.st.push(cpu.st.pop() + r); } },
			{  "SUB",                 {}, [](CPU& cpu, Adr_t    , Val_t    ){ Val_t r = cpu.st.pop(); cpu.st.push(cpu.st.pop() - r); } },
			{  "MUL",                 {}, [](CPU& cpu, Adr_t    , Val_t    ){ Val_t r = cpu.st.pop(); cpu.st.push(cpu.st.pop() * r); } },
			{  "OUT",                 {}, [](CPU& cpu, Adr_t    , Val_t    ){ std::cout << cpu.st.head() << std::endl;               } }
		};

		Cmd_t getCommand(FileWork::ReadBinaryFile& stream)
		{
			try
			{
				std::vector<unsigned char> bytes{stream.getBytes(sizeof(Cmd_t))};

				Cmd_t* data = reinterpret_cast<Cmd_t*>(bytes.data());

				return *data;
			}
			catch (std::exception exc)
			{
				throw Exception("Unable to get command", PROGRAM_POS, exc);
			}
		}

	} // namespace _command

} // namespace MyCompilerStandard0

#undef FILENAME
#undef PROGRAM_POS

#endif /*HEADER_GUARD_MY_COMPILER_STANDARD0_HPP_INCLUDED*/