// 6502_Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

using Byte = unsigned char; //a character is 1 byte in length, so we define BYTE as a character.
using Word = unsigned short;//a short is 2 bytes in length, so we define WORD as a short.

using u32 = unsigned int; // u32 = unsigned 32 bit number

struct Mem
{
    static constexpr u32 MAX_MEMORY = 1024 * 64; //64 kB
    Byte Data[MAX_MEMORY];

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

    void Execute(u32 Cycles,Mem& Memory)
    {
        while (Cycles > 0)
        {
            Byte Instruction = FetchByte(Cycles, Memory); 
            (void)Instruction;
        }
    }
};



int main()
{   
    Mem Memory;
    CPU p6502;
    p6502.Reset(Memory);
    p6502.Execute(2, Memory);
    return 0;
}
