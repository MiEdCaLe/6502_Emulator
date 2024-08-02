// 6502_Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

using Byte = unsigned char; //a character is 1 byte in length, so we define BYTE as a character.
using Word = unsigned short;//a short is 2 bytes in length, so we define WORD as a short.

using u32 = unsigned int; // u32 = unsigned 32 bit number -> 4 Bytes

struct Mem
{
    static constexpr u32 MAX_MEMORY = 1024 * 64; //~64 000
    Byte Data[MAX_MEMORY]; // 64 kb

    void Initialize()
    {
        for (u32 i{ 0 } ; i < MAX_MEMORY ; i++)
        {
            Data[i] = 0;
        }
    }

    // Read Byte
    Byte operator[]( u32 Address ) const
    {
        return Data[Address];
    }

    // Write 1 Byte
    Byte& operator[](u32 Address) {
        return Data[Address];
    }

    //Write a word
    void WriteWord(Word Value, u32 Address, u32& Cycles)
    {
        Data[Address] = (Value & 0xFF);
        Data[Address + 1] = (Value >> 8);
        Cycles -= 2;
    }
};

struct CPU 
{
    Word ProgramCounter; // 6502 boats a 16bit register as a program counter.
    Byte StackPointer;  // 6502 has an 8 bit stack pointer.

    // Registers
    Byte A;
    Byte X;
    Byte Y; 

    // CPU flags register (8 bits, 1 unused)
    Byte C : 1;
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    void Reset(Mem& Memory) // Reset processsor
    {
        ProgramCounter = 0xFFFC;
        StackPointer = 0x100; // Zero Page
        D = 0;
        
        // Clear flags
        C = 0;
        Z = 0;
        I = 0;
        D = 0;
        B = 0;
        V = 0;
        N = 0;

        //Clear registers
        A = 0;
        X = 0;
        Y = 0;

        Memory.Initialize();
    }

    Byte FetchByte(u32& Cycles, Mem& Memory) 
    {
        Byte Data = Memory[ProgramCounter];
        ProgramCounter++;
        Cycles--;
        return Data;
    }

    Word FetchWord(u32& Cycles, Mem& Memory)
    {
        //6502 is little endian
        Word Data = Memory[ProgramCounter];
        ProgramCounter++;

        Data |= (Memory[ProgramCounter] << 8);
        ProgramCounter++;
        Cycles += 2;
        return Data;
    }

    Byte ReadByte(u32& Cycles, Byte Address, Mem& Memory) {
        Byte Data = Memory[Address];
        Cycles--;
        return Data;
    }

    // OP Codes
    static constexpr Byte INS_LDA_IM = 0xA9;
    static constexpr Byte INS_LDA_ZP = 0xA5;
    static constexpr Byte INS_LDA_ZPX = 0xB5;
    static constexpr Byte INS_JSR = 0x20;

    void LDASetStatus()
    {
        Z = (A == 0); // If A == 0, Z = 1;
        N = (A & 0b10000000) > 0; // If bit 7 of accumulator is set, N flag is set.
    }

    void Execute(u32 Cycles,Mem& Memory)
    {
        while (Cycles > 0)
        {
            Byte Instruction = FetchByte(Cycles, Memory); //First Byte of the instruction is the OPCODE
            switch (Instruction)
            {
            case INS_LDA_IM:
            {
                Byte Value = FetchByte(Cycles, Memory); //Second Byte of the instruction is the DATA
                A = Value;
                LDASetStatus();
            } break;

            case INS_LDA_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, Memory);
                A = ReadByte(Cycles,ZeroPageAddress, Memory);
                LDASetStatus();
            } break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, Memory);
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageAddress, Memory);
                LDASetStatus();
            } break;
            case INS_JSR:
            {
                Word SubAddress = FetchWord(Cycles, Memory);
                Memory.WriteWord(ProgramCounter - 1, StackPointer, Cycles);
                ProgramCounter = SubAddress;
                Cycles--;
            } break;
            default:
            {
                std::cout << "ERROR\n";
            } break;

            }
        }
    }
};

int main()
{   
    Mem Memory;
    CPU p6502;
    p6502.Reset(Memory);
    // Inline Program
    Memory[0xFFFC] = CPU::INS_LDA_ZP;
    Memory[0xFFFD] = 0x42;
    Memory[0x0042] = 0x84;
    // Inline prigram End
    p6502.Execute(3, Memory);
    return 0;
}
