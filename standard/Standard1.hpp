#ifndef HEADER_GUARD_MY_COMPILER_STANDARD1_HPP_INCLUDED
#define HEADER_GUARD_MY_COMPILER_STANDARD1_HPP_INCLUDED

// Includes:

#include <string>
#include <array>
#include <vector>

#include <memory>
#include <functional>
#include <limits>
#include <cmath>

#include <cstdio>


#include "../additional/exception/MyException.hpp"
#include "../additional/writing_files/FileWork.hpp"
#include "../additional/stack/Stack.hpp"

// Defines:

// Passed to Exception
#define NOTHING "", "", 0

namespace MyStd1
{
	using namespace MyExceptionCharStringRepresentation;
	using namespace MyStackStaticArrayRepresentation;

	using RegAdr_t = unsigned char;
	using Val_t    = double; // Change PRINT_FORMAT
	using Cmd_t    = unsigned char;
	using CmdNum_t = unsigned short;

	using MagicNum_t = unsigned char;
	using   StdNum_t = unsigned char;

	MagicNum_t MAGIC_NUM = 0xBA;
	StdNum_t STD_NUM = 1;

	const char*  INPUT_FORMAT = "%lf";
	const char* OUTPUT_FORMAT = "%.03lf\n";	

	const char* SINGLE_LINE_COMMENT = "//";

	//-----------------------------------------------------------------------------

	namespace _registers
	{
		const size_t REGISTER_COUNT = 4;

		const char* REGISTERS[REGISTER_COUNT] = 
		{
			"AX",
			"BX",
			"CX",
			"DX"
		};

	} // namespace _address

	//-----------------------------------------------------------------------------

	namespace _command
	{
		const size_t VALUE_STACK_SIZE = 1024;
		const size_t  CALL_STACK_SIZE = 1024;

		struct Command;

		struct CPU
		{
		public:
			// Variables:
				std::vector<Command*> cmdArr;
				CmdNum_t curCmd;
				Stack<CmdNum_t,  CALL_STACK_SIZE> callSt;
				Stack<   Val_t, VALUE_STACK_SIZE>  valSt; 
				std::array<Val_t, _registers::REGISTER_COUNT> regs;

			// Ctor:

				CPU() :
					cmdArr   (),
					curCmd   (0),
					callSt   (),
					valSt    (),
					regs     ()
				{
					regs.fill(0);
				}
		};
	
		struct Command
		{
		public:
			// Dtor:
				virtual ~Command() = default; // pure
			
			// Functions:
				virtual void execute(CPU& cpu) = 0; // pure
		};

		// Commands:

			// Entry and end points of the program:

				struct CmdBeg : public Command
				{
					// Functions:
						CmdBeg() = default;
						virtual ~CmdBeg() = default;
						virtual void execute(CPU& cpu) override {}
				};

				struct CmdEnd : public Command
				{
					// Functions:
						CmdEnd() = default;
						virtual ~CmdEnd() = default;
						virtual void execute(CPU& cpu) override
						{
							cpu.curCmd = cpu.cmdArr.size();
						}
				};

			// Stack operations:

				struct CmdPush : public Command
				{
					// Variables:
						Val_t toPush_;

					// Functions:
						explicit CmdPush(Val_t toPush) : toPush_ (toPush) {}
						virtual ~CmdPush() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.full())
							{
								throw Exception("PUSH: Unable to push to full value stack", NOTHING);
							}

							cpu.valSt.push(toPush_);
						}
				};

				struct CmdPushR : public Command
				{
					// Variables:
						RegAdr_t regAdr_;

					// Functions:
						explicit CmdPushR(RegAdr_t regAdr) : regAdr_ (regAdr) {};
						virtual ~CmdPushR() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.full())
							{
								throw Exception("PUSHR: Unable to push to full value stack", NOTHING);
							}

							if (regAdr_ >= _registers::REGISTER_COUNT)
							{
								throw Exception("PUSHR: Unable to find the register", NOTHING);
							}

							cpu.valSt.push(cpu.regs.at(regAdr_));
						}
				};

				struct CmdPop : public Command
				{
					// Functions:
						CmdPop() = default;
						virtual ~CmdPop() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("POP: Unable to pop from value stack", NOTHING);
							}

							cpu.valSt.pop();
						}
				};

				struct CmdPopR : public Command
				{
					// Variables:
						RegAdr_t regAdr_;

					// Functions:
						explicit CmdPopR(RegAdr_t regAdr) : regAdr_ (regAdr) {};
						virtual ~CmdPopR() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("POPR: Unable to pop from value stack", NOTHING);
							}

							if (regAdr_ >= _registers::REGISTER_COUNT)
							{
								throw Exception("POPR: Unable to find the register", NOTHING);
							}

							cpu.regs.at(regAdr_) = cpu.valSt.pop();
						}
				};

			// Arithmetics:

				struct CmdAdd : public Command
				{
					// Functions:
						CmdAdd() = default;
						virtual ~CmdAdd() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("ADD: 2 operands are missing", NOTHING);
							}

							Val_t summand = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("ADD: 1 operand is missing", NOTHING);
							}

							cpu.valSt.push(cpu.valSt.pop() + summand);
						}
				};

				struct CmdSub : public Command
				{
					// Functions:
						CmdSub() = default;
						virtual ~CmdSub() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("SUB: 2 operands are missing", NOTHING);
							}

							Val_t subtrahend = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("SUB: 1 operand is missing", NOTHING);
							}

							cpu.valSt.push(cpu.valSt.pop() - subtrahend);
						}
				};

				struct CmdMul : public Command
				{
					// Functions:
						CmdMul() = default;
						virtual ~CmdMul() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("MUL: 2 operands are missing", NOTHING);
							}

							Val_t multiplier = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("MUL: 1 operand is missing", NOTHING);
							}

							cpu.valSt.push(cpu.valSt.pop() * multiplier);
						}
				};

				struct CmdDiv : public Command
				{
					// Functions:
						CmdDiv() = default;
						virtual ~CmdDiv() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("DIV: 2 operands are missing", NOTHING);
							}

							Val_t divisor = cpu.valSt.pop();

							if (std::abs(divisor) <= std::numeric_limits<Val_t>::epsilon() * 5)
							{
								throw Exception("DIV: Unable to divide by 0!", NOTHING);
							}

							if (cpu.valSt.empty())
							{
								throw Exception("DIV: 1 operand is missing", NOTHING);
							}

							cpu.valSt.push(cpu.valSt.pop() / divisor);
						}
				};

				struct CmdSqrt : public Command
				{
					// Functions:
						CmdSqrt() = default;
						virtual ~CmdSqrt() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("SQRT: 1 operand is missing", NOTHING);
							}

							Val_t popped = cpu.valSt.pop();

							if (popped < 0)
							{
								throw Exception("SQRT: Root of negative number doesn't exist", NOTHING);
							}

							cpu.valSt.push(std::sqrt(popped));
						}
				};

			// IO:

				struct CmdOut : public Command
				{
					// Functions:
						CmdOut() = default;
						virtual ~CmdOut() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("OUT: Value stack is empty", NOTHING);
							}

							std::printf(OUTPUT_FORMAT, cpu.valSt.pop());
						}
				};

				struct CmdIn : public Command
				{
					// Functions:
						CmdIn() = default;
						virtual ~CmdIn() = default;
						virtual void execute(CPU& cpu) override
						{
							Val_t toRead = 0;

							std::scanf(INPUT_FORMAT, &toRead);

							if (cpu.valSt.full())
							{
								throw Exception("IN: Can't read into full value stack", NOTHING);
							}

							cpu.valSt.push(toRead);
						}
				};

			// Jumps:

				struct CmdJmp : public Command
				{
					// Variables:
						CmdNum_t toJump_;
					// Functions:
						explicit CmdJmp(CmdNum_t toJump) : toJump_ (toJump) {};
						virtual ~CmdJmp() = default;
						virtual void execute(CPU& cpu) override
						{
							cpu.curCmd = toJump_ - 1; // That -1 is because ++cpu.curCmd is called int the cycle end
						}
				};

				struct CmdJe : public CmdJmp
				{
					// Functions:
						explicit CmdJe(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJe() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JE: 2 operands are missing", NOTHING);
							}

							Val_t arg0 = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JE: 1 operand is missing", NOTHING);
							}

							Val_t arg1 = cpu.valSt.pop();

							if (std::abs(arg0 - arg1) <=
								std::abs(arg0 + arg1) * 
								std::numeric_limits<Val_t>::epsilon() * 5)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

				struct CmdJne : public CmdJmp
				{
					// Functions:
						explicit CmdJne(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJne() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JNE: 2 operands are missing", NOTHING);
							}

							Val_t arg0 = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JNE: 1 operand is missing", NOTHING);
							}

							Val_t arg1 = cpu.valSt.pop();

							if (std::abs(arg0 - arg1) >
								std::abs(arg0 + arg1) * 
								std::numeric_limits<Val_t>::epsilon() * 5)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

				struct CmdJa : public CmdJmp
				{
					// Functions:
						explicit CmdJa(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJa() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JA: 2 operands are missing", NOTHING);
							}

							Val_t popped = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JA: 1 operand is missing", NOTHING);
							}

							if (cpu.valSt.pop() > popped)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

				struct CmdJae : public CmdJmp
				{
					// Functions:
						explicit CmdJae(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJae() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JAE: 2 operands are missing", NOTHING);
							}

							Val_t popped = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JAE: 1 operand is missing", NOTHING);
							}

							if (cpu.valSt.pop() >= popped)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

				struct CmdJb : public CmdJmp
				{
					// Functions:
						explicit CmdJb(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJb() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JB: 2 operands are missing", NOTHING);
							}

							Val_t popped = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JB: 1 operand is missing", NOTHING);
							}

							if (cpu.valSt.pop() < popped)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

				struct CmdJbe : public CmdJmp
				{
					// Functions:
						explicit CmdJbe(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdJbe() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.valSt.empty())
							{
								throw Exception("JBE: 2 operands are missing", NOTHING);
							}

							Val_t popped = cpu.valSt.pop();

							if (cpu.valSt.empty())
							{
								throw Exception("JBE: 1 operand is missing", NOTHING);
							}

							if (cpu.valSt.pop() <= popped)
							{
								CmdJmp::execute(cpu);
							}
						}
				};

			// Function support:

				struct CmdCall : public CmdJmp
				{
					// Functions:
						explicit CmdCall(CmdNum_t toJump) : CmdJmp(toJump) {};
						virtual ~CmdCall() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.callSt.full())
							{
								throw Exception("CALL: Stack overflow", NOTHING);
							}

							cpu.callSt.push(cpu.curCmd);
							CmdJmp::execute(cpu);
						}
				};

				struct CmdRet : public Command
				{
					// Functions:
						CmdRet() = default;
						virtual ~CmdRet() = default;
						virtual void execute(CPU& cpu) override
						{
							if (cpu.callSt.empty())
							{
								throw Exception("RET: Call stack is empty", NOTHING);
							}

							cpu.curCmd = cpu.callSt.pop();
						}
				};

			// Dump:

				struct CmdDump : public Command
				{
					// Functions:
						CmdDump() = default;
						virtual ~CmdDump() = default;
						virtual void execute(CPU& cpu) override
						{
							std::printf("--------------STACK----------------\n");
							for (size_t i = cpu.valSt.filledSize() - 1; i < cpu.valSt.filledSize(); --i)
							{
								std::printf(OUTPUT_FORMAT, cpu.valSt.at(i));
							}
							std::printf("------------CALL-STACK-------------\n");
							for (size_t i = cpu.callSt.filledSize() - 1; i < cpu.callSt.filledSize(); --i)
							{
								std::printf(OUTPUT_FORMAT, cpu.callSt.at(i));
							}
							std::printf("------------REGISTERS-------------\n");
							for (size_t i = 0; i < cpu.regs.size(); ++i)
							{
								std::printf("%s: %lf\n", _registers::REGISTERS[i], cpu.regs.at(i));
							}
							std::printf("----------------------------------\n");
						}
				};

		//-----------------------------------------------------------------------------

		// Now info for assembler and disassembler:
		
		enum class ArgType
		{
			REGISTER_ADDRESS,
			VALUE,
			NAMETAG
		};

		struct CommandType
		{
		public:
			// Variables:
				FileWork::Word name;
				const std::vector<ArgType> argTypes;			
		};

		const size_t COMMAND_COUNT = 23;

		using FileWork::Word;

		const CommandType COMMANDS[COMMAND_COUNT] =
		{
			{Word(  "BEG"),                          {}}, //0
			{Word(  "END"),                          {}}, //1
			{Word( "PUSH"), {ArgType::VALUE           }}, //2
			{Word("PUSHR"), {ArgType::REGISTER_ADDRESS}}, //3
			{Word(  "POP"),                          {}}, //4
			{Word( "POPR"), {ArgType::REGISTER_ADDRESS}}, //5
			{Word(  "ADD"),                          {}}, //6
			{Word(  "SUB"),                          {}}, //7
			{Word(  "MUL"),                          {}}, //8
			{Word(  "DIV"),                          {}}, //9 
			{Word( "SQRT"),                          {}}, //10
			{Word(  "OUT"),                          {}}, //11
			{Word(   "IN"),                          {}}, //12
			{Word(  "JMP"),          {ArgType::NAMETAG}}, //13
			{Word(   "JE"),          {ArgType::NAMETAG}}, //14
			{Word(  "JNE"),          {ArgType::NAMETAG}}, //15
			{Word(   "JA"),          {ArgType::NAMETAG}}, //16
			{Word(  "JAE"),          {ArgType::NAMETAG}}, //17
			{Word(   "JB"),          {ArgType::NAMETAG}}, //18
			{Word(  "JBE"),          {ArgType::NAMETAG}}, //19
			{Word( "CALL"),          {ArgType::NAMETAG}}, //20
			{Word(    "@"),                          {}}, //21
			{Word(  "RET"),                          {}}  //22
		};

	} // namespace _command

} // namespace MyCompilerStandard0
#undef NOTHING

#endif /*HEADER_GUARD_MY_COMPILER_STANDARD0_HPP_INCLUDED*/