//==================================================================================== <br />
// Gene Lam <br />
// Professor: Michael Vulis <br />
// CSC21000-E Assembly Language <br />
//==================================================================================== <br />

This x86 emulator is written in C++ and reads bytecode from COM files. <br />

My emulator supports the following methods, which are categorized by their EXAMPLE COM Files: <br />
EXAMPLE1.COM: Assignment - NOP, MOV (between registers), XCHG <br />
EXAMPLE2.COM: Arithmetic Operators - ADD, SUB, INC, DEC, NEG, CMP, <br />
EXAMPLE3.COM: Logical Operators - AND, TEST, OR, XOR, NOT <br />
EXAMPLE4.COM: Shift, Rotate, Multiply, Divide, and Jumps - SHL, SHR, SAR, ROL, ROR, RCL, RCR, MUL, IMUL, DIV, IDIV, 
		JC, JNC, JZ, JNZ, JBE, JNBE, JS, JNS, JLE <br />

The methods that I didn't have time to cover, but was mentioned in class are: loops and stack operators <br />

The emulator reads COM files into mem[0x100000] and performs the functions associated with individual bytecodes. <br />
These instructions act on 8-bit or 16-bit registers: AL, AH, BL, BH, CL, CH, DL, DH, AX, BX, CX, and DX. <br />
The following flags are also supported: Zero Flag, Carry Flag, Sign Flag. <br />
Note that additional code segments exist (ie DS, SP, BP, SI, and DI). <br />
These additional codesegments are encoded into the emulator, but not the instructions that act on them due to time constraints. <br />

To run the emulator: <br />
1. Click on the executable file, which will bring up a terminal asking to input the COM file directory path. <br />
2. Copy the file path to COM and paste into the terminal. For me, it was: C:\Users\Gene\Downloads\GeneLam_v1\Examples\EXAMPLE1.COM <br />
3. My program will then ask if you'd like to see the register and flag values for each step. This option was to help debug the program. <br />
4. If you type in 'y', the program will run and display the contents of the registers and flags, along with the bytecode it read. <br />
5. The program also displays the execution time in microseconds, number of instructions it performed, <br />
and the translated assembly instructions to verify that the correct instruction was performed. <br />
