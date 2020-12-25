//======================================================================================================================
// Gene Lam 
// Professor: Michael Vulis 
// CSC21000-E Assembly Language
//======================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE // These are added to prevent deprecation calls in VSCode
#define _CRT_NONSTDC_NO_DEPRECATE
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <chrono>
#include <math.h> /* pow */

using namespace std;

#define byte unsigned char
#define word unsigned short

byte mem[0x100000];
byte AL, AH, BL, BH, CL, CH, DL, DH = 0; // 8 bit registers
word AX, BX, CX, DX = 0; // 16 bit registers
byte codebyte;
word IP = 0x100; // instruction pointer
word CS, DS, ES, FS, GS, SS = 0x10000; // additionally code segments
word SP, BP = 0xFFFE; // stack and base pointer
word SI, DI = 0; // source and destination index
word flag = 0; // zero, carry, sign flags

int linelength = 110;
queue<string> instructions;
int fakeFlag[3] = { 0,0,0 }; // zero, carry, sign
int temp;

// C:\Users\Gene\Downloads\csc210\CDRIVE\Examples\Example4.COM

// Auxiliary Functions: Print Results, Convert from AX to AH and AL, Convert from AL and AH to AX, Convert Array Flags to Hex Flag
void printResults(int i) {
	if(i==0) cout << "Final Register Values: \n";
	cout << "AX=" << hex << (int)AX << "\t AH=" << hex << (int)AH << "\t AL=" << hex << (int)AL <<
		"\t CX=" << hex << (int)CX << "\t CH=" << hex << (int)CH << "\t CL=" << hex << (int)CL <<
		"\t DX=" << hex << (int)DX << "\t DH=" << hex << (int)DH << "\t DL=" << hex << (int)DL <<
		"\t BX=" << hex << (int)BX << "\t BH=" << hex << (int)BH << "\t BL=" << hex << (int)BL << "\n";
	if (i == 0) cout << "Final Flag Values: \n";
	cout << hex << showbase << (int)flag << "\t Sign Flag: " << hex << to_string(fakeFlag[2]) << "\t Zero Flag: " << hex << to_string(fakeFlag[0]) << "\t Carry Flag: " << to_string(fakeFlag[1]) << "\n";
}
void X_to_HL(const word& GX, byte& GH, byte& GL) {
	GH = 0x00; GL = 0x00;
	int tempGX = GX;
	int i = 0;
	while (tempGX != 0 && i < 4) {
		if (i < 2) {
			GL += (tempGX % 16 * pow(16, i));
			tempGX /= 16;
		}
		else {
			GH += (tempGX % 16 * pow(16, i - 2));
			tempGX /= 16;
		}
		i++;
	}
	// cout << hex << showbase << (int)AH << " " << hex << showbase << (int)AL << "\n"; // Debugging
}
void HL_to_X1(const string destination) {
	if (destination.compare("AL") == 0 || destination.compare("AH") == 0) { AX = 256 * AH + AL; }
	else if (destination.compare("CL") == 0 || destination.compare("CH") == 0) { CX = 256 * CH + CL; }
	else if (destination.compare("DL") == 0 || destination.compare("DH") == 0) { DX = 256 * DH + DL; }
	else if (destination.compare("BL") == 0 || destination.compare("BH") == 0) { BX = 256 * BH + BL; }
}
void HL_to_X2(const string destination, const string source) {
	if (destination.compare("AL") == 0 || destination.compare("AH") == 0) { AX = 256 * AH + AL; }
	else if (destination.compare("CL") == 0 || destination.compare("CH") == 0) { CX = 256 * CH + CL; }
	else if (destination.compare("DL") == 0 || destination.compare("DH") == 0) { DX = 256 * DH + DL; }
	else if (destination.compare("BL") == 0 || destination.compare("BH") == 0) { BX = 256 * BH + BL; }
	if (source.compare("AL") == 0 || source.compare("AH") == 0) { AX = 256 * AH + AL; }
	else if (source.compare("CL") == 0 || source.compare("CH") == 0) { CX = 256 * CH + CL; }
	else if (source.compare("DL") == 0 || source.compare("DH") == 0) { DX = 256 * DH + DL; }
	else if (source.compare("BL") == 0 || source.compare("BH") == 0) { BX = 256 * BH + BL; }
}
void convertFlag() {
	flag = fakeFlag[1]+16*fakeFlag[0]+256*fakeFlag[2];
}
int MSB(int i) { 
	return (i / 128 == 1);
}
int LSB(int i) {
	return (i % 2 == 1);
}
void checkZero(int i) {
	if (i == 0) fakeFlag[0] == 1;
	else fakeFlag[0] == 0;
	convertFlag();
}

// METHODS: NOP, MOV, XCHG, ADD, SUB, INC, DEC, NEG, CMP, AND, TEST, OR, XOR, NOT, SHL, SHR, SAR, ROL, ROR, RCL, RCR, MUL, IMUL, DIV, IDIV
void NOP(word& instructionPointer) {
	instructionPointer++;
	instructions.push("NOP ;instruction pointer increments by 1");
}

template <class T>
void XCHG(T& destination, T& source) {
	if (sizeof(destination) == 1 && sizeof(source) == 1) {
		byte temp = destination;
		destination = source;
		source = temp;
		HL_to_X2(to_string(destination), to_string(source));
	}
	else if (sizeof(destination) == 2 && sizeof(source) == 2) {
		word temp = destination;
		destination = source;
		source = temp;
		if (to_string(destination).compare("AX") == 0) { X_to_HL(AX, AH, AL); }
		else if (to_string(destination).compare("CX") == 0) { X_to_HL(CX, CH, CL); }
		else if (to_string(destination).compare("DX") == 0) { X_to_HL(DX, DH, DL); }
		else if (to_string(destination).compare("BX") == 0) { X_to_HL(BX, BH, BL); }
		if (to_string(source).compare("AX") == 0) { X_to_HL(AX, AH, AL); }
		else if (to_string(source).compare("CX") == 0) { X_to_HL(CX, CH, CL); }
		else if (to_string(source).compare("DX") == 0) { X_to_HL(DX, DH, DL); }
		else if (to_string(source).compare("BX") == 0) { X_to_HL(BX, BH, BL); }
	}
}

template <class T>
void op_add(T& destination, T source) {
	byte temp = destination + source;
	// Carry Flag
	if (sizeof(destination) == 1 && sizeof(source) == 1 && (destination + source) > 255) fakeFlag[1] = 1; // 8-bit
	else if (sizeof(destination) == 2 && sizeof(source) == 2 && (destination + source) > 65535) fakeFlag[1] = 1; // 16-bit
	else fakeFlag[1] = 0;

	// Signed Flag forgot we didnt have to do signed flag
	/*if (sizeof(destination) == 1 && sizeof(source) == 1 && 
		destination >=128 && source>=128 && temp<128) fakeFlag[2] = 1;
	else if (sizeof(destination) == 1 && sizeof(source) == 1 &&
		destination < 128 && source < 128 && temp>= 128) fakeFlag[2] = 1;
	else if (sizeof(destination) == 2 && sizeof(source) == 2 &&
		destination >= 32768 && source >= 32768 && temp < 32768) fakeFlag[2] = 1;
	else if (sizeof(destination) == 2 && sizeof(source) == 2 &&
		destination < 32768 && source < 32768 && temp >= 32768) fakeFlag[2] = 1;
	else fakeFlag[2] = 0;*/

	destination = temp;
	// Signed Flag
	fakeFlag[2] = MSB(destination);
	// Zero Flag
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_sub(T& destination, T source) {
	byte temp = destination - source;
	// Carry Flag
	if (sizeof(destination) == 1 && sizeof(source) == 1 && (destination - source) < 0) fakeFlag[1] = 1; // 8-bit
	else if (sizeof(destination) == 2 && sizeof(source) == 2 && (destination - source) < 0) fakeFlag[1] = 1; // 16-bit
	else fakeFlag[1] = 0;

	// Overflow Flag - Forgot we didnt have to do overflow flag
	/* if (sizeof(destination) == 1 && sizeof(source) == 1 &&
		destination >= 128 && source < 128 && temp < 128) fakeFlag[2] = 1;
	else if (sizeof(destination) == 1 && sizeof(source) == 1 &&
		destination < 128 && source >= 128 && temp >= 128) fakeFlag[2] = 1;
	else if (sizeof(destination) == 2 && sizeof(source) == 2 &&
		destination >= 32768 && source < 32768 && temp < 32768) fakeFlag[2] = 1;
	else if (sizeof(destination) == 2 && sizeof(source) == 2 &&
		destination < 32768 && source >= 32768 && temp >= 32768) fakeFlag[2] = 1;
	else fakeFlag[2] = 0; */

	destination = temp;
	// Signed Flag
	fakeFlag[2] = MSB(destination);
	// Zero Flag
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_neg(T& destination) {
	if (destination == 0) fakeFlag[1] = 0;
	else fakeFlag[1] = 1; 
	destination = -destination;
	fakeFlag[2] = MSB(destination);
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_cmp(T destination, T source) {
	if (destination < source) {
		fakeFlag[0] = 0; fakeFlag[1] = 1; // ZF = 0, CF = 1
	}
	else if (destination = source) {
		fakeFlag[0] = 1; fakeFlag[1] = 0; // ZF = 1, CF = 0
	}
	else {
		fakeFlag[0] = 0; fakeFlag[1] = 0; // ZF = 0 CF =0
	}
	convertFlag();
}

template <class T>
void op_and(T& destination, T source) {
	destination = destination&source;
	fakeFlag[1] = 0;
	fakeFlag[2] = MSB(destination);
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_test(T destination, T source) {
	temp = (int)destination & (int)source;
	if (destination==0) fakeFlag[0] = 1; 
	else fakeFlag[0] = 0;
	fakeFlag[2] = MSB(destination);
	fakeFlag[1] = 0; 
	convertFlag();
}

template <class T>
void op_or(T& destination, T source) {
	destination = destination | source;
	fakeFlag[1] = 0;
	fakeFlag[2] = MSB(destination);
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_xor(T& destination, T source) {
	destination = destination ^ source;
	fakeFlag[1] = 0;
	fakeFlag[2] = MSB(destination);
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_not(T& destination) {
	destination = ~destination;
}

template <class T>
void op_shift(T& destination, const string typeShift) {
	if (typeShift.compare("HL") == 0) {
		fakeFlag[1] = MSB(destination);
		destination = destination << 1;
	}
	else if (typeShift.compare("HR") == 0) {
		fakeFlag[1] = LSB(destination);
		destination = destination >> 1;
	}
	else if (typeShift.compare("AR") == 0) { 
		fakeFlag[1] = LSB(destination);
		destination = destination >> 1;
		destination += 128;
	}
	else if (typeShift.compare("ROL") == 0) {
		destination = destination << 1;
		if(MSB(destination)==1) destination++;
	}
	else if (typeShift.compare("ROR") == 0) {
		destination = destination >> 1;
		if (LSB(destination) == 1) destination+=128;
	}
	else if (typeShift.compare("RCL") == 0) {
		temp = fakeFlag[1];
		fakeFlag[1] = MSB(destination);
		destination = destination << 1;
		if (temp == 1) destination ++;
	}
	else if (typeShift.compare("RCR") == 0) {
		temp = fakeFlag[1];
		fakeFlag[1] = LSB(destination);
		destination = destination >> 1;
		if (temp == 1) destination += 128;
	}
	fakeFlag[2] = MSB(destination); // signed flag
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_mul(T& destination) {
	if (sizeof(destination) == 1) {
		AX = destination * AL;
		X_to_HL(AX, AH, AL);
		if (AX > 255) fakeFlag[1] = 1;
		else fakeFlag[1] = 0;
	}
	else if (sizeof(destination) == 2) {
		int temp = (int)destination * (int)AX;
		AX *= destination;
		DX = temp - AX;
		X_to_HL(AX, AH, AL);
		X_to_HL(DX, DH, DL);
		if (AX > 65535) fakeFlag[1] = 1;
		else fakeFlag[1] = 0;
	}
	fakeFlag[2] = 0;
	fakeFlag[0] = 0;
	convertFlag();
}

template <class T>
void op_imul(T& destination) {
	if (sizeof(destination) == 1) {
		AX = AL * destination; 
		if (AL == AX) fakeFlag[1] = 0;
		else fakeFlag[1] = 1;
		X_to_HL(AX, AH, AL);
	}
	else if (sizeof(destination) == 2) {
		int temp = (int)destination * (int)AX;
		AX *= destination;
		DX = temp - AX;
		X_to_HL(AX, AH, AL);
		X_to_HL(DX, DH, DL);
		if (AX > 65535) fakeFlag[1] = 1;
		else fakeFlag[1] = 0;
	}
	fakeFlag[2] = MSB(destination);
	checkZero(destination);
	convertFlag();
}

template <class T>
void op_div(T& destination) {
	if (destination == 0) return;
	if (sizeof(destination) == 1) {
		AL = AX / destination;
		AH = AX % destination;
		AX = 256 * AH + AL;
	}
	else if (sizeof(destination) == 2) {
		int temp = (int)DX * 65536 + (int)AX * 256;
		AX = temp / destination;
		DX = temp % destination;
		X_to_HL(AX, AH, AL);
		X_to_HL(DX, DH, DL);
	}
	fakeFlag[2] = 0;
	fakeFlag[1] = 0;
	fakeFlag[0] = 0;
	convertFlag();
}

template <class T>
void op_idiv(T& destination) {
	if (destination == 0) return;
	if (sizeof(destination) == 1) {
		temp = AX / destination;
		if (temp > 0x7F || temp < 0x80) return;
		else {
			AL = temp;
			AH = AX % destination; 
		}
		AX = 256 * AH + AL;
	}
	else if (sizeof(destination) == 2) {
		int temp = (int)DX * 65536 + (int)AX * 256;
		AX = temp / destination;
		if (AX > 0x7FFF || AX < 0x8000) return;
		else {
			DX = temp % destination;
		}
		X_to_HL(AX, AH, AL);
		X_to_HL(DX, DH, DL);
	}
	fakeFlag[2] = 0;
	fakeFlag[1] = 0;
	fakeFlag[0] = 0;
	checkZero(destination);
	convertFlag();
}

void rrChangeDest(byte& destination, byte& source, const string instructionString) {
	if (instructionString.compare("MOV") == 0) { destination = source; }
	else if (instructionString.compare("ADD") == 0) { op_add(destination, source); }
	else if (instructionString.compare("SUB") == 0) { op_sub(destination, source); }
	else if (instructionString.compare("AND") == 0) { op_and(destination, source); }
	else if (instructionString.compare("OR") == 0) { op_or(destination, source); }
	else if (instructionString.compare("XOR") == 0) { op_xor(destination, source); }
	HL_to_X1(to_string(destination));
}

void rrAllocator(byte& destination, byte& source, const string instructionString) {
	if (instructionString.compare("MOV") == 0 || instructionString.compare("ADD") == 0 || instructionString.compare("SUB") == 0 || 
		instructionString.compare("AND") == 0 || instructionString.compare("OR") == 0 || 
		instructionString.compare("XOR") == 0) { rrChangeDest(destination, source, instructionString); }
	else if (instructionString.compare("XCHG") == 0) { XCHG(destination, source); }
	else if (instructionString.compare("CMP") == 0) { op_cmp(destination, source); }
	else if (instructionString.compare("TEST") == 0) { op_test(destination, source); } 
}

// Switch Statement for Reading 2nd Codebyte
void reg_to_reg(word& instructionPointer, const string instructionString) {
	codebyte = mem[instructionPointer+1];
	string destination; string source;
	switch (codebyte) {
	case 0xC0:
		destination = "AL"; source = "AL";
		rrAllocator(AL, AL, instructionString);
		break;
	case 0xC1:
		destination = "AL"; source = "CL";
		rrAllocator(AL, CL, instructionString);
		break;
	case 0xC2:
		destination = "AL"; source = "DL";
		rrAllocator(AL, DL, instructionString);
		break;
	case 0xC3:
		destination = "AL"; source = "BL";
		rrAllocator(AL, BL, instructionString);
		break;
	case 0xC4:
		destination = "AL"; source = "AH"; 
		rrAllocator(AL, AH, instructionString);
		break;
	case 0xC5:
		destination = "AL"; source = "CH";
		rrAllocator(AL, CH, instructionString);
		break;
	case 0xC6:
		destination = "AL"; source = "DH";
		rrAllocator(AL, DH, instructionString);
		break;
	case 0xC7:
		destination = "AL"; source = "BH";
		rrAllocator(AL, BH, instructionString);
		break;
	case 0xC8:
		destination = "CL"; source = "AL";
		rrAllocator(CL, AL, instructionString);
		break;
	case 0xC9:
		destination = "CL"; source = "CL";
		rrAllocator(CL, CL, instructionString);
		break;
	case 0xCA:
		destination = "CL"; source = "DL";
		rrAllocator(CL, DL, instructionString);
		break;
	case 0xCB:
		destination = "CL"; source = "BL";
		rrAllocator(CL, BL, instructionString);
		break;
	case 0xCC:
		destination = "CL"; source = "AH";
		rrAllocator(CL, AH, instructionString);
		break;
	case 0xCD:
		destination = "CL"; source = "CH";
		rrAllocator(CL, CH, instructionString);
		break;
	case 0xCE:
		destination = "CL"; source = "DH";
		rrAllocator(CL, DH, instructionString);
		break;
	case 0xCF:
		destination = "CL"; source = "BH";
		rrAllocator(CL, BH, instructionString);
		break;
	case 0xD0:
		destination = "DL"; source = "AL";
		rrAllocator(DL, AL, instructionString);
		break;
	case 0xD1:
		destination = "DL"; source = "CL";
		rrAllocator(DL, CL, instructionString);
		break;
	case 0xD2:
		destination = "DL"; source = "DL";
		rrAllocator(DL, DL, instructionString);
		break;
	case 0xD3:
		destination = "DL"; source = "BL"; 
		rrAllocator(DL, BL, instructionString);
		break;
	case 0xD4:
		destination = "DL"; source = "AH";
		rrAllocator(DL, AH, instructionString);
		break;
	case 0xD5:
		destination = "DL"; source = "CH";
		rrAllocator(DL, CH, instructionString);
		break;
	case 0xD6:
		destination = "DL"; source = "DH";
		rrAllocator(DL, DH, instructionString);
		break;
	case 0xD7:
		destination = "DL"; source = "BH";
		rrAllocator(DL, BH, instructionString);
		break;
	case 0xD8:
		destination = "BL"; source = "AL";
		rrAllocator(BL, AL, instructionString);
		break;
	case 0xD9:
		destination = "BL"; source = "CL"; 
		rrAllocator(BL, CL, instructionString);
		break;
	case 0xDA:
		destination = "BL"; source = "DL";
		rrAllocator(BL, DL, instructionString);
		break;
	case 0xDB:
		destination = "BL"; source = "BL";
		rrAllocator(BL, BL, instructionString);
		break;
	case 0xDC:
		destination = "BL"; source = "AH";
		rrAllocator(BL, AH, instructionString);
		break;
	case 0xDD:
		destination = "BL"; source = "CH";
		rrAllocator(BL, CH, instructionString);
		break;
	case 0xDE:
		destination = "BL"; source = "DH";
		rrAllocator(BL, DH, instructionString);
		break;
	case 0xDF:
		destination = "BL"; source = "BH";
		rrAllocator(BL, BH, instructionString);
		break;
	case 0xE0:
		destination = "AH"; source = "AL";
		rrAllocator(AH, AL, instructionString);
		break;
	case 0xE1:
		destination = "AH"; source = "CL";
		rrAllocator(AH, CL, instructionString);
		break;
	case 0xE2:
		destination = "AH"; source = "DL";
		rrAllocator(AH, DL, instructionString);
		break;
	case 0xE3:
		destination = "AH"; source = "BL";
		rrAllocator(AH, BL, instructionString);
		break;
	case 0xE4:
		destination = "AH"; source = "AH";
		rrAllocator(AH, AH, instructionString);
		break;
	case 0xE5:
		destination = "AH"; source = "CH";
		rrAllocator(AH, CH, instructionString);
		break;
	case 0xE6:
		destination = "AH"; source = "DH";
		rrAllocator(AH, DH, instructionString);
		break;
	case 0xE7:
		destination = "AH"; source = "BH";
		rrAllocator(AH, BH, instructionString);
		break;
	case 0xE8:
		destination = "CH"; source = "AL";
		rrAllocator(CH, AL, instructionString);
		break;
	case 0xE9:
		destination = "CH"; source = "CL";
		rrAllocator(CH, CL, instructionString);
		break;
	case 0xEA:
		destination = "CH"; source = "DL";
		rrAllocator(CH, DL, instructionString);
		break;
	case 0xEB:
		destination = "CH"; source = "BL";
		rrAllocator(CH, BL, instructionString);
		break;
	case 0xEC:
		destination = "CH"; source = "AH";
		rrAllocator(CH, AH, instructionString);
		break;
	case 0xED:
		destination = "CH"; source = "CH";
		rrAllocator(CH, CH, instructionString);
		break;
	case 0xEE:
		destination = "CH"; source = "DH";
		rrAllocator(CH, DH, instructionString);
		break;
	case 0xEF:
		destination = "CH"; source = "BH";
		rrAllocator(CH, BH, instructionString);
		break;
	case 0xF0:
		destination = "DH"; source = "AL";
		rrAllocator(DH, AL, instructionString);
		break;
	case 0xF1:
		destination = "DH"; source = "CL";
		rrAllocator(DH, CL, instructionString);
		break;
	case 0xF2:
		destination = "DH"; source = "DL";
		rrAllocator(DH, DL, instructionString);
		break;
	case 0xF3:
		destination = "DH"; source = "BL";
		rrAllocator(DH, BL, instructionString);
		break;
	case 0xF4:
		destination = "DH"; source = "AH";
		rrAllocator(DH, AH, instructionString);
		break;
	case 0xF5:
		destination = "DH"; source = "CH";
		rrAllocator(DH, CH, instructionString);
		break;
	case 0xF6:
		destination = "DH"; source = "DH";
		rrAllocator(DH, DH, instructionString);
		break;
	case 0xF7:
		destination = "DH"; source = "BH";
		rrAllocator(DH, BH, instructionString);
		break;
	case 0xF8:
		destination = "BH"; source = "AL";
		rrAllocator(BH, AL, instructionString);
		break;
	case 0xF9:
		destination = "BH"; source = "CL";
		rrAllocator(BH, CL, instructionString);
		break;
	case 0xFA:
		destination = "BH"; source = "DL";
		rrAllocator(BH, DL, instructionString);
		break;
	case 0xFB:
		destination = "BH"; source = "BL";
		rrAllocator(BH, BL, instructionString);
		break;
	case 0xFC:
		destination = "BH"; source = "AH";
		rrAllocator(BH, AH, instructionString);
		break;
	case 0xFD:
		destination = "BH"; source = "CH";
		rrAllocator(BH, CH, instructionString);
		break;
	case 0xFE:
		destination = "BH"; source = "DH";
		rrAllocator(BH, DH, instructionString);
		break;
	case 0xFF:
		destination = "BH"; source = "BH";
		rrAllocator(BH, BH, instructionString);
		break;
	}
	instructionPointer+=2;
	if (instructionString.compare("MOV") == 0) { instructions.push("MOV " + destination + ", " + source); }
	else if (instructionString.compare("XCHG") == 0) { instructions.push("XCHG " + destination + ", " + source); }
	else if (instructionString.compare("ADD") == 0) { instructions.push("ADD " + destination + ", " + source); }
	else if (instructionString.compare("SUB") == 0) { instructions.push("SUB " + destination + ", " + source); }
	else if (instructionString.compare("CMP") == 0) { instructions.push("CMP " + destination + ", " + source); }
	else if (instructionString.compare("AND") == 0) { instructions.push("AND " + destination + ", " + source); }
	else if (instructionString.compare("TEST") == 0) { instructions.push("TEST " + destination + ", " + source); }
	else if (instructionString.compare("OR") == 0) { instructions.push("OR " + destination + ", " + source); }
	else if (instructionString.compare("XOR") == 0) { instructions.push("XOR " + destination + ", " + source); }
}

void bbChangeDest(word& destination, word& source, const string instructionString) {
	if (instructionString.compare("MOV") == 0) { destination = source; }
	else if (instructionString.compare("ADD") == 0) { op_add(destination, source); }
	else if (instructionString.compare("SUB") == 0) { op_sub(destination, source); }
	else if (instructionString.compare("AND") == 0) { op_and(destination, source); }
	else if (instructionString.compare("OR") == 0) { op_or(destination, source); }
	else if (instructionString.compare("XOR") == 0) { op_xor(destination, source); }
	if (to_string(destination).compare("AX") == 0) { X_to_HL(AX, AH, AL); }
	else if (to_string(destination).compare("CX") == 0) { X_to_HL(CX, CH, CL); }
	else if (to_string(destination).compare("DX") == 0) { X_to_HL(DX, DH, DL); }
	else if (to_string(destination).compare("BX") == 0) { X_to_HL(BX, BH, BL); }
}

void bbAllocator(word& destination, word& source, const string instructionString) {
	if (instructionString.compare("MOV") == 0 || instructionString.compare("ADD") == 0 || instructionString.compare("SUB") == 0 || 
		instructionString.compare("AND") == 0 || instructionString.compare("TEST") == 0 || instructionString.compare("OR") == 0 || 
		instructionString.compare("XOR") == 0) { bbChangeDest(destination, source, instructionString); }
	else if (instructionString.compare("XCHG") == 0) { XCHG(destination, source); }
	else if (instructionString.compare("CMP") == 0) { op_cmp(destination, source); }
	else if (instructionString.compare("TEST") == 0) { op_test(destination, source); } 
}

void XCHG_bigreg_to_AX(word& instructionPointer, word& source, const string sourceReg) {
	word bigtemp = AX;
	AX = source;
	source = bigtemp;
	X_to_HL(AX, AH, AL);
	if (sourceReg.compare("DX") == 0) { X_to_HL(DX, DH, DL); }
	else if (sourceReg.compare("CX") == 0) { X_to_HL(CX, CH, CL); }
	else if (sourceReg.compare("BX") == 0) { X_to_HL(BX, BH, BL); }
	instructionPointer++;
	instructions.push("XCHG AX, " + sourceReg);
}

// Switch Statement for Reading 2nd Codebyte
void bigreg_to_bigreg(word& instructionPointer, const string instructionString) {
	string sourceReg; string dstReg; word bigtemp;
	codebyte = mem[instructionPointer + 1];
	switch (codebyte) {
	case(0xC0):
		dstReg = "AX"; sourceReg = "AX";
		bbAllocator(AX, AX, instructionString);
		break;
	case(0xC1):
		dstReg = "AX"; sourceReg = "CX";
		bbAllocator(AX, CX, instructionString);
		break;
	case(0xC2):
		dstReg = "AX"; sourceReg = "DX";
		bbAllocator(AX, DX, instructionString);
		break;
	case(0xC3):
		dstReg = "AX"; sourceReg = "BX";
		bbAllocator(AX, BX, instructionString);
		break;
	case(0xC8):
		dstReg = "CX"; sourceReg = "AX";
		bbAllocator(CX, AX, instructionString);
		break;
	case(0xC9):
		dstReg = "CX"; sourceReg = "CX";
		bbAllocator(CX, CX, instructionString);
		break;
	case(0xCA):
		dstReg = "CX"; sourceReg = "DX";
		bbAllocator(CX, DX, instructionString);
		break;
	case(0xCB):
		dstReg = "CX"; sourceReg = "BX";
		bbAllocator(CX, BX, instructionString);
		break;
	case(0xD0):
		dstReg = "DX"; sourceReg = "AX";
		bbAllocator(DX, AX, instructionString);
		break;
	case(0xD1):
		dstReg = "DX"; sourceReg = "CX";
		bbAllocator(DX, CX, instructionString);
		break;
	case(0xD2):
		dstReg = "DX"; sourceReg = "DX";
		bbAllocator(DX, DX, instructionString);
		break;
	case(0xD3):
		dstReg = "DX"; sourceReg = "BX";
		bbAllocator(DX, BX, instructionString);
		break;
	case(0xD8):
		dstReg = "BX"; sourceReg = "AX";
		bbAllocator(BX, AX, instructionString);
		break;
	case(0xD9):
		dstReg = "BX"; sourceReg = "CX";
		bbAllocator(BX, CX, instructionString);
		break;
	case(0xDA):
		dstReg = "BX"; sourceReg = "DX";
		bbAllocator(BX, DX, instructionString);
		break;
	case(0xDB):
		sourceReg = "BX"; dstReg = "BX";
		bbAllocator(BX, BX, instructionString);
		break;
	}
	if (instructionString.compare("MOV") == 0) { instructions.push("MOV " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("XCHG") == 0) { instructions.push("XCHG " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("ADD") == 0) { instructions.push("ADD " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("SUB") == 0) { instructions.push("SUB " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("CMP") == 0) { instructions.push("CMP " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("AND") == 0) { instructions.push("AND " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("TEST") == 0) { instructions.push("TEST " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("OR") == 0) { instructions.push("OR " + dstReg + ", " + sourceReg); }
	else if (instructionString.compare("XOR") == 0) { instructions.push("XOR " + dstReg + ", " + sourceReg); }
	instructionPointer+=2;
}

void MOV_num_to_reg(word& instructionPointer, byte& reg, const string registerName) {
	codebyte = mem[instructionPointer + 1]; // get the location of the value in memory
	reg = codebyte; 
	instructionPointer += 2;
	HL_to_X1(registerName);
	instructions.push("MOV " + registerName + ", " + to_string((int)codebyte));
}

// Switch Statement for Reading 2nd Codebyte and 3rd Codebyte
void OP_num_to_reg(word& instructionPointer) {
	string registerName = "AL"; string opName; byte num;
	if (mem[instructionPointer] == 0x04 || mem[instructionPointer] == 0x2C || mem[instructionPointer] == 0x3C || 
		mem[instructionPointer] == 0x24 || mem[instructionPointer] == 0x0C || mem[instructionPointer] == 0x34 || mem[instructionPointer] == 0xA8) {
		num = mem[instructionPointer + 1]; codebyte = mem[instructionPointer];
		switch (mem[instructionPointer]) {
		case 0x04:
			opName = "ADD"; op_add(AL,num); 
			break;
		case 0x2C:
			opName = "SUB"; op_sub(AL, num);
			break;
		case 0x3C:
			opName = "CMP"; op_cmp(AL, num);
			break;
		case 0x24:
			opName = "AND"; op_and(AL, num);
			break;
		case 0xA8:
			opName = "TEST"; op_test(AL, num);
			break;
		case 0x0C:
			opName = "OR"; op_or(AL, num);
			break;
		case 0x34:
			opName = "XOR"; op_xor(AL, num);
			break;
		}
		instructionPointer += 2;
	} 
	else {
		num = mem[instructionPointer + 2]; codebyte = mem[instructionPointer+1];
		switch (codebyte) {
		case 0xC1:
			registerName = "CL"; opName = "ADD"; 	
			op_add(CL, num); 
			break;
		case 0xC2:
			registerName = "DL"; opName = "ADD";
			op_add(DL, num);
			break;
		case 0xC3:
			registerName = "BL"; opName = "ADD";
			op_add(BL, num);
			break;
		case 0xC4:
			registerName = "AH"; opName = "ADD";
			op_add(AH, num);
			break;
		case 0xC5:
			registerName = "CH"; opName = "ADD";
			op_add(CH, num);
			break;
		case 0xC6:
			registerName = "DH"; opName = "ADD";
			op_add(DH, num);
			break;
		case 0xC7:
			registerName = "BH"; opName = "ADD";
			op_add(BH, num);
			break;
		case 0xC9:
			registerName = "CL"; opName = "OR";
			op_or(CL, num); 
			break;
		case 0xCA:
			registerName = "DL"; opName = "OR";
			op_or(DL, num);
			break;
		case 0xCB:
			registerName = "BL"; opName = "OR";
			op_or(BL, num);
			break;
		case 0xCC:
			registerName = "AH"; opName = "OR";
			op_or(AH, num);
		case 0xCD:
			registerName = "CH"; opName = "OR";
			op_or(CH, num);
			break;
		case 0xCE:
			registerName = "DH"; opName = "OR";
			op_or(DH, num);
			break;
		case 0xCF:
			registerName = "BH"; opName = "OR";
			op_or(BH, num);
			break;
		case 0xE1:
			registerName = "CL";  opName = "AND";
			op_and(CL, num);
			break;
		case 0xE2:
			registerName = "DL"; opName = "AND";
			op_and(DL, num);
			break;
		case 0xE3:
			registerName = "BL"; opName = "AND";
			op_and(BL, num);
			break;
		case 0xE4:
			registerName = "AH"; opName = "AND";
			op_and(AH, num);
			break;
		case 0xE5:
			registerName = "CH"; opName = "AND";
			op_and(CH, num);
			break;
		case 0xE6:
			registerName = "DH"; opName = "AND";
			op_and(DH, num);
			break;
		case 0xE7:
			registerName = "BH"; opName = "AND";
			op_and(BH, num);
			break;
		case 0xE9:
			registerName = "CL"; opName = "SUB";
			op_sub(CL, num);
			break;
		case 0xEA:
			registerName = "DL"; opName = "SUB";
			op_sub(DL, num);
			break;
		case 0xEB:
			registerName = "BL"; opName = "SUB";
			op_sub(BL, num);
			break;
		case 0xEC:
			registerName = "AH"; opName = "SUB";
			op_sub(AH, num);
			break;
		case 0xED:
			registerName = "CH"; opName = "SUB";
			op_sub(CH, num);
			break;
		case 0xEE:
			registerName = "DH"; opName = "SUB";
			op_sub(DH, num);
			break;
		case 0xEF:
			registerName = "BH"; opName = "SUB";
			op_sub(BH, num);
			break;
		case 0xF1:
			registerName = "CL"; opName = "XOR";
			op_xor(CL, num);
			break;
		case 0xF2:
			registerName = "DL"; opName = "XOR";
			op_xor(DL, num);
			break;
		case 0xF3:
			registerName = "BL"; opName = "XOR";
			op_xor(BL, num);
			break;
		case 0xF4:
			registerName = "AH"; opName = "XOR";
			op_xor(AH, num);
			break;
		case 0xF5:
			registerName = "CH"; opName = "XOR";
			op_xor(CH, num);
			break;
		case 0xF6:
			registerName = "DH"; opName = "XOR";
			op_xor(DH, num);
			break;
		case 0xF7:
			registerName = "BH"; opName = "XOR";
			op_xor(BH, num);
			break;
		case 0xF9:
			registerName = "CL"; opName = "CMP";
			op_cmp(CL, num);
			break;
		case 0xFA:
			registerName = "DL"; opName = "CMP";
			op_cmp(DL, num);
			break;
		case 0xFB:
			registerName = "BL"; opName = "CMP";
			op_cmp(AH, num);
			break;
		case 0xFC:
			registerName = "AH"; opName = "CMP";
			op_cmp(AH, num);
			break;
		case 0xFD:
			registerName = "CH"; opName = "CMP";
			op_cmp(CH, num);
			break;
		case 0xFE:
			registerName = "DH"; opName = "CMP";
			op_cmp(DH, num);
			break;
		case 0xFF:
			registerName = "BH"; opName = "CMP";
			op_cmp(BH, num);
			break;
		}
		instructionPointer += 3;
	}
	HL_to_X1(registerName);
	if (opName.compare("ADD")==0){ instructions.push("ADD " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("SUB") == 0) { instructions.push("SUB " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("CMP") == 0) { instructions.push("CMP " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("AND") == 0) { instructions.push("AND " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("TEST") == 0) { instructions.push("TEST " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("OR") == 0) { instructions.push("OR " + registerName + ", " + to_string((int)num)); }
	else if (opName.compare("XOR") == 0) { instructions.push("XOR " + registerName + ", " + to_string((int)num)); }
}

void IncDec(word& instructionPointer, word& reg, const string operation, const string regName) {
	if (operation.compare("INC") == 0) { 
		reg++;
		// Signed Flag
		fakeFlag[2] = MSB(reg);
		// Zero Flag
		checkZero(reg);
		convertFlag();
		instructions.push("INC " + regName);
	}
	else if (operation.compare("DEC") == 0) { 
		reg--; 
		// Signed Flag
		fakeFlag[2] = MSB(reg);
		// Zero Flag
		checkZero(reg);
		convertFlag();
		instructions.push("DEC " + regName); 
	}
	if (regName.compare("AX") == 0) { X_to_HL(AX, AH, AL); }
	else if (regName.compare("CX") == 0) { X_to_HL(CX, CH, CL); }
	else if (regName.compare("DX") == 0) { X_to_HL(DX, DH, DL); }
	else if (regName.compare("BX") == 0) { X_to_HL(BX, BH, BL); }
	instructionPointer++;
}

// Switch Statement for Reading 2nd Codebyte 
void OP_num_to_reg_2(word& instructionPointer, const string sizeReg) {
	string registerName; string sourceName;  string opName; codebyte = mem[instructionPointer + 1];
	if (sizeReg.compare("EIGHT")==0) {
		switch (codebyte) {
		case 0xC0:
			registerName = "AL"; opName = "ROL";
			op_shift(AL, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC1:
			registerName = "CL"; opName = "ROL";
			op_shift(CL, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC2:
			registerName = "DL"; opName = "ROL";
			op_shift(DL, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC3:
			registerName = "BL"; opName = "ROL";
			op_shift(BL, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC4:
			registerName = "AH"; opName = "ROL";
			op_shift(AH, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC5:
			registerName = "CH"; opName = "ROL";
			op_shift(CH, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC6:
			registerName = "DH"; opName = "ROL";
			op_shift(DH, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC7:
			registerName = "BH"; opName = "ROL";
			op_shift(BH, "ROL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC8:
			registerName = "AL"; opName = "ROR";
			op_shift(AL, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xC9:
			registerName = "CL"; opName = "ROR";
			op_shift(CL, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCA:
			registerName = "DL"; opName = "ROR";
			op_shift(DL, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCB:
			registerName = "BL"; opName = "ROR";
			op_shift(BL, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCC:
			registerName = "AH"; opName = "ROR";
			op_shift(AH, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCD:
			registerName = "CH"; opName = "ROR";
			op_shift(CH, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCE:
			registerName = "DH"; opName = "ROR";
			op_shift(DH, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xCF:
			registerName = "BH"; opName = "ROR";
			op_shift(BH, "ROR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD0:
			registerName = "AL"; opName = "RCL";
			op_shift(AL, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD1:
			registerName = "CL"; opName = "RCL";
			op_shift(CL, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD2:
			registerName = "DL"; opName = "RCL";
			op_shift(DL, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD3:
			registerName = "BL"; opName = "RCL";
			op_shift(BL, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD4:
			registerName = "AH"; opName = "RCL";
			op_shift(AH, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD5:
			registerName = "CH"; opName = "RCL";
			op_shift(CH, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD6:
			registerName = "DH"; opName = "RCL";
			op_shift(DH, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD7:
			registerName = "BH"; opName = "RCL";
			op_shift(BH, "RCL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD8:
			registerName = "AL"; opName = "RCR";
			op_shift(AL, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD9:
			registerName = "CL"; opName = "RCR";
			op_shift(CL, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDA:
			registerName = "DL"; opName = "RCR";
			op_shift(DL, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDB:
			registerName = "BL"; opName = "RCR";
			op_shift(BL, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDC:
			registerName = "AH"; opName = "RCR";
			op_shift(AH, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDD:
			registerName = "CH"; opName = "RCR";
			op_shift(CH, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDE:
			registerName = "DH"; opName = "RCR";
			op_shift(DH, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDF:
			registerName = "BH"; opName = "RCR";
			op_shift(BH, "RCR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE0:
			registerName = "AL"; opName = "SHL";
			op_shift(AL, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE1:
			registerName = "CL"; opName = "SHL";
			op_shift(CL, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE2:
			registerName = "DL"; opName = "SHL";
			op_shift(DL, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE3:
			registerName = "BL"; opName = "SHL";
			op_shift(BL, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE4:
			registerName = "AH"; opName = "SHL";
			op_shift(AH, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE5:
			registerName = "CH"; opName = "SHL";
			op_shift(CH, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE6:
			registerName = "DH"; opName = "SHL";
			op_shift(DH, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE7:
			registerName = "BH"; opName = "SHL";
			op_shift(BH, "HL"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE8:
			registerName = "AL"; opName = "SHR";
			op_shift(AL, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE9:
			registerName = "CL"; opName = "SHR";
			op_shift(CL, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xEA:
			registerName = "DL"; opName = "SHR";
			op_shift(DL, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xEB:
			registerName = "BL"; opName = "SHR";
			op_shift(BL, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xEC:
			registerName = "AH"; opName = "SHR";
			op_shift(AH, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xED:
			registerName = "CH"; opName = "SHR";
			op_shift(CH, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xEE:
			registerName = "DH"; opName = "SHR";
			op_shift(DH, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xEF:
			registerName = "BH"; opName = "SHR";
			op_shift(BH, "HR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xF8:
			registerName = "AL"; opName = "SAR";
			op_shift(AL, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xF9:
			registerName = "CL"; opName = "SAR";
			op_shift(CL, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFA:
			registerName = "DL"; opName = "SAR";
			op_shift(DL, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFB:
			registerName = "BL"; opName = "SAR";
			op_shift(BL, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFC:
			registerName = "AH"; opName = "SAR";
			op_shift(AH, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFD:
			registerName = "CH"; opName = "SAR";
			op_shift(CH, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFE:
			registerName = "DH"; opName = "SAR";
			op_shift(DH, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xFF:
			registerName = "BH"; opName = "SAR";
			op_shift(BH, "AR"); HL_to_X1(registerName); instructionPointer += 2;
			break;
		}
	}
	else if (sizeReg.compare("SIXTEEN")==0) {
		switch (codebyte) {
		case 0xC0:
			registerName = "AX"; opName = "ROL";
			op_shift(AX, "ROL"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xC1:
			registerName = "CX"; opName = "ROL";
			op_shift(CX, "ROL"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xC2:
			registerName = "DX"; opName = "ROL";
			op_shift(DX, "ROL"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xC3:
			registerName = "BX"; opName = "ROL";
			op_shift(BX, "ROL"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xC8:
			registerName = "AX"; opName = "ROR";
			op_shift(AX, "ROR"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xC9:
			registerName = "CX"; opName = "ROR";
			op_shift(CX, "ROR"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xCA:
			registerName = "DX"; opName = "ROR";
			op_shift(DX, "ROR"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xCB:
			registerName = "BX"; opName = "ROR";
			op_shift(BX, "ROR"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xD0:
			registerName = "AX"; opName = "RCL";
			op_shift(AX, "RCL"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xD1:
			registerName = "CX"; opName = "RCL";
			op_shift(CX, "RCL"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xD2:
			registerName = "DX"; opName = "RCL";
			op_shift(DX, "RCL"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xD3:
			registerName = "BX"; opName = "RCL";
			op_shift(BX, "RCL"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xD8:
			registerName = "AX"; opName = "RCR";
			op_shift(AX, "RCR"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xD9:
			registerName = "CX"; opName = "RCR";
			op_shift(CX, "RCR"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xDA:
			registerName = "DX"; opName = "RCR";
			op_shift(DX, "RCR"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xDB:
			registerName = "BX"; opName = "RCR";
			op_shift(BX, "RCR"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xE0:
			registerName = "AX"; opName = "SHL";
			op_shift(AX, "HL"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xE1:
			registerName = "CX"; opName = "SHL";
			op_shift(CX, "HL"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xE2:
			registerName = "DX"; opName = "SHL";
			op_shift(DX, "HL"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xE3:
			registerName = "BX"; opName = "SHL";
			op_shift(BX, "HL"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xE8:
			registerName = "AX"; opName = "SHR";
			op_shift(AX, "HR"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xE9:
			registerName = "CX"; opName = "SHR";
			op_shift(CX, "HR"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xEA:
			registerName = "DX"; opName = "SHR";
			op_shift(DX, "HR"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xEB:
			registerName = "BX"; opName = "SHR";
			op_shift(BX, "HR"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xF8:
			registerName = "AX"; opName = "SAR";
			op_shift(AX, "AR"); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xF9:
			registerName = "CX"; opName = "SAR";
			op_shift(CX, "AR"); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xFA:
			registerName = "DX"; opName = "SAR";
			op_shift(DX, "AR"); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xFB:
			registerName = "BX"; opName = "SAR";
			op_shift(BX, "AR"); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		}
	}
	if (opName.compare("SHL") == 0) { instructions.push("SHL " + registerName); }
	else if (opName.compare("SHR") == 0) { instructions.push("SHR " + registerName); }
	else if (opName.compare("SAR") == 0) { instructions.push("SAR " + registerName); }
	else if (opName.compare("ROL") == 0) { instructions.push("ROL " + registerName); }
	else if (opName.compare("ROR") == 0) { instructions.push("ROR " + registerName); }
	else if (opName.compare("RCL") == 0) { instructions.push("RCL " + registerName); }
	else if (opName.compare("RCR") == 0) { instructions.push("RCR " + registerName); }
}

// Switch Statement for Reading 2nd Codebyte and 3rd Codebyte
void NegTest(word& instructionPointer, const string sizeReg) {
	string registerName; string sourceName;  string opName; codebyte = mem[instructionPointer + 1]; byte num = mem[instructionPointer + 2];
	if (sizeReg.compare("EIGHT")==0) {
		switch (codebyte) {
		case 0xC1:
			registerName = "CL"; sourceName = to_string((int)num); opName = "TEST";
			op_test(CL, num); instructionPointer += 3;
			break;
		case 0xC2:
			registerName = "DL"; sourceName = to_string((int)num); opName = "TEST";
			op_test(DL, num); instructionPointer += 3;
			break;
		case 0xC3:
			registerName = "BL"; sourceName = to_string((int)num); opName = "TEST";
			op_test(BL, num); instructionPointer += 3;
			break;
		case 0xC4:
			registerName = "AH"; sourceName = to_string((int)num); opName = "TEST";
			op_test(AH, num); instructionPointer += 3;
			break;
		case 0xC5:
			registerName = "CH"; sourceName = to_string((int)num); opName = "TEST";
			op_test(CH, num); instructionPointer += 3;
			break;
		case 0xC6:
			registerName = "DH"; sourceName = to_string((int)num); opName = "TEST";
			op_test(DH, num); instructionPointer += 3;
			break;
		case 0xC7:
			registerName = "BH"; sourceName = to_string((int)num); opName = "TEST";
			op_test(BH, num); instructionPointer += 3;
			break;
		case 0xD0:
			registerName = "AL"; opName = "NOT";
			op_not(AL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD1:
			registerName = "CL"; opName = "NOT";
			op_not(CL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD2:
			registerName = "DL"; opName = "NOT";
			op_not(DL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD3:
			registerName = "BL"; opName = "NOT";
			op_not(BL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD4:
			registerName = "AH"; opName = "NOT";
			op_not(AH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD5:
			registerName = "CH"; opName = "NOT";
			op_not(CH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD6:
			registerName = "DH"; opName = "NOT";
			op_not(DH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD7:
			registerName = "BH"; opName = "NOT";
			op_not(BH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD8:
			registerName = "AL"; opName = "NEG";
			op_neg(AL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xD9:
			registerName = "CL"; opName = "NEG";
			op_neg(CL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDA:
			registerName = "DL"; opName = "NEG";
			op_neg(DL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDB:
			registerName = "BL"; opName = "NEG";
			op_neg(BL); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDC:
			registerName = "AH"; opName = "NEG";
			op_neg(AH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDD:
			registerName = "CH"; opName = "NEG";
			op_neg(CH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDE:
			registerName = "DH"; opName = "NEG";
			op_neg(DH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xDF:
			registerName = "BH"; opName = "NEG";
			op_neg(BH); HL_to_X1(registerName); instructionPointer += 2;
			break;
		case 0xE0:
			registerName = "AL"; opName = "MUL";
			op_mul(AL); instructionPointer += 2;
			break;
		case 0xE1:
			registerName = "CL"; opName = "MUL";
			op_mul(CL); instructionPointer += 2;
			break;
		case 0xE2:
			registerName = "DL"; opName = "MUL";
			op_mul(DL); instructionPointer += 2;
			break;
		case 0xE3:
			registerName = "BL"; opName = "MUL";
			op_mul(BL); instructionPointer += 2;
			break;
		case 0xE4:
			registerName = "AH"; opName = "MUL";
			op_mul(AH); instructionPointer += 2;
			break;
		case 0xE5:
			registerName = "CH"; opName = "MUL";
			op_mul(CH); instructionPointer += 2;
			break;
		case 0xE6:
			registerName = "DH"; opName = "MUL";
			op_mul(DH); instructionPointer += 2;
			break;
		case 0xE7:
			registerName = "BH"; opName = "MUL";
			op_mul(BH); instructionPointer += 2;
			break;
		case 0xE8:
			registerName = "AL"; opName = "IMUL";
			op_imul(AL); instructionPointer += 2;
			break;
		case 0xE9:
			registerName = "CL"; opName = "IMUL";
			op_imul(CL); instructionPointer += 2;
			break;
		case 0xEA:
			registerName = "DL"; opName = "IMUL";
			op_imul(DL); instructionPointer += 2;
			break;
		case 0xEB:
			registerName = "BL"; opName = "IMUL";
			op_imul(BL); instructionPointer += 2;
			break;
		case 0xEC:
			registerName = "AH"; opName = "IMUL";
			op_imul(AH); instructionPointer += 2;
			break;
		case 0xED:
			registerName = "CH"; opName = "IMUL";
			op_imul(CH); instructionPointer += 2;
			break;
		case 0xEE:
			registerName = "DH"; opName = "IMUL";
			op_imul(DH); instructionPointer += 2;
			break;
		case 0xEF:
			registerName = "BH"; opName = "IMUL";
			op_imul(BH); instructionPointer += 2;
			break;
		case 0xF0:
			registerName = "AL"; opName = "DIV";
			op_div(AL); instructionPointer += 2;
			break;
		case 0xF1:
			registerName = "CL"; opName = "DIV";
			op_div(CL); instructionPointer += 2;
			break;
		case 0xF2:
			registerName = "DL"; opName = "DIV";
			op_div(DL); instructionPointer += 2;
			break;
		case 0xF3:
			registerName = "BL"; opName = "DIV";
			op_div(BL); instructionPointer += 2;
			break;
		case 0xF4:
			registerName = "AH"; opName = "DIV";
			op_div(AH); instructionPointer += 2;
			break;
		case 0xF5:
			registerName = "CH"; opName = "DIV";
			op_div(CH); instructionPointer += 2;
			break;
		case 0xF6:
			registerName = "DH"; opName = "DIV";
			op_div(DH); instructionPointer += 2;
			break;
		case 0xF7:
			registerName = "BH"; opName = "DIV";
			op_div(BH); instructionPointer += 2;
			break;
		case 0xF8:
			registerName = "AL"; opName = "IDIV";
			op_idiv(AL); instructionPointer += 2;
			break;
		case 0xF9:
			registerName = "CL"; opName = "IDIV";
			op_idiv(CL); instructionPointer += 2;
			break;
		case 0xFA:
			registerName = "DL"; opName = "IDIV";
			op_idiv(DL); instructionPointer += 2;
			break;
		case 0xFB:
			registerName = "BL"; opName = "IDIV";
			op_idiv(BL); instructionPointer += 2;
			break;
		case 0xFC:
			registerName = "AH"; opName = "IDIV";
			op_idiv(AH); instructionPointer += 2;
			break;
		case 0xFD:
			registerName = "CH"; opName = "IDIV";
			op_idiv(CH); instructionPointer += 2;
			break;
		case 0xFE:
			registerName = "DH"; opName = "IDIV";
			op_idiv(DH); instructionPointer += 2;
			break;
		case 0xFF:
			registerName = "BH"; opName = "IDIV";
			op_idiv(BH); instructionPointer += 2;
			break;
		}
	}
	else if (sizeReg.compare("SIXTEEN")==0) {
		switch (codebyte) {
		case 0xD0:
			registerName = "AX"; opName = "NOT";
			op_not(AX); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xD1:
			registerName = "CX"; opName = "NOT";
			op_not(CX); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xD2:
			registerName = "DX"; opName = "NOT";
			op_not(DX); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xD3:
			registerName = "BX"; opName = "NOT";
			op_not(BX); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xD8:
			registerName = "AX"; opName = "NEG";
			op_neg(AX); X_to_HL(AX, AH, AL); instructionPointer += 2;
			break;
		case 0xD9:
			registerName = "CX"; opName = "NEG";
			op_neg(CX); X_to_HL(CX, CH, CL); instructionPointer += 2;
			break;
		case 0xDA:
			registerName = "DX"; opName = "NEG";
			op_neg(DX); X_to_HL(DX, DH, DL); instructionPointer += 2;
			break;
		case 0xDB:
			registerName = "BX"; opName = "NEG";
			op_neg(BX); X_to_HL(BX, BH, BL); instructionPointer += 2;
			break;
		case 0xE0:
			registerName = "AX"; opName = "MUL";
			op_mul(AX); instructionPointer += 2;
			break;
		case 0xE1:
			registerName = "CX"; opName = "MUL";
			op_mul(CX); instructionPointer += 2;
			break;
		case 0xE2:
			registerName = "DX"; opName = "MUL";
			op_mul(DX); instructionPointer += 2;
			break;
		case 0xE3:
			registerName = "BX"; opName = "MUL";
			op_mul(BX); instructionPointer += 2;
			break;
		case 0xE8:
			registerName = "AX"; opName = "IMUL";
			op_imul(AX); instructionPointer += 2;
			break;
		case 0xE9:
			registerName = "CX"; opName = "IMUL";
			op_imul(CX); instructionPointer += 2;
			break;
		case 0xEA:
			registerName = "DX"; opName = "IMUL";
			op_imul(DX); instructionPointer += 2;
			break;
		case 0xEB:
			registerName = "BX"; opName = "IMUL";
			op_imul(BX); instructionPointer += 2;
			break;
		case 0xF0:
			registerName = "AX"; opName = "DIV";
			op_div(AX); instructionPointer += 2;
			break;
		case 0xF1:
			registerName = "CX"; opName = "DIV";
			op_div(CX); instructionPointer += 2;
			break;
		case 0xF2:
			registerName = "DX"; opName = "DIV";
			op_div(DX); instructionPointer += 2;
			break;
		case 0xF3:
			registerName = "BX"; opName = "DIV";
			op_div(BX); instructionPointer += 2;
			break;
		case 0xF8:
			registerName = "AX"; opName = "IDIV";
			op_idiv(AX); instructionPointer += 2;
			break;
		case 0xF9:
			registerName = "CX"; opName = "IDIV";
			op_idiv(CX); instructionPointer += 2;
			break;
		case 0xFA:
			registerName = "DX"; opName = "IDIV";
			op_idiv(DX); instructionPointer += 2;
			break;
		case 0xFB:
			registerName = "BX"; opName = "IDIV";
			op_idiv(BX); instructionPointer += 2;
			break;
		}
	}
	if (opName.compare("TEST") == 0) { instructions.push("TEST " + registerName + ", " + sourceName); }
	else{ 
		instructions.push(opName + " " + registerName); 
	}
}

void JMP(word& instructionPointer, const string jmpType) {
	temp = (int)mem[instructionPointer + 1]; 	
	if (jmpType.compare("JMP") == 0){
		instructionPointer += (temp+2);
	} else if (jmpType.compare("JC") == 0) {
		if (fakeFlag[1] == 1) instructionPointer += temp + 2;
		else instructionPointer += 2;
	} else if (jmpType.compare("JNC") == 0) {
		if (fakeFlag[1] == 0) instructionPointer += (temp+2); 
		else instructionPointer += 2;
	} else if (jmpType.compare("JZ") == 0) {
		if (fakeFlag[0] == 1) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JNZ") == 0) {
		if (fakeFlag[0] == 0) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JBE") == 0) {
		if (fakeFlag[0] == 1 || fakeFlag[1]==1) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JNBE") == 0) {
		if (fakeFlag[0] == 0 && fakeFlag[1] == 0) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JS") == 0) {
		if (fakeFlag[2]==1) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JNS") == 0) {
		if (fakeFlag[2] == 0) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} else if (jmpType.compare("JLE") == 0) {
		if (fakeFlag[0] == 1) instructionPointer += (temp + 2);
		else instructionPointer += 2;
	} 
	char locationPointer[8];
	itoa(instructionPointer, locationPointer, 16); 
	instructions.push(jmpType + " " + locationPointer);
}

// Switch Statement for Reading 1st Codebyte
void switchInstruction(byte codebyte) {
	switch (codebyte) {
		case 0x02: 
			reg_to_reg(IP, "ADD");
			break;
		case 0x03:
			bigreg_to_bigreg(IP, "ADD");
			break;
		case 0x04:
			OP_num_to_reg(IP);
			break;
		case 0x0A:
			reg_to_reg(IP, "OR");
			break;
		case 0x0B:
			bigreg_to_bigreg(IP, "OR");
			break;
		case 0x0C:
			OP_num_to_reg(IP);
			break;
		case 0x22:
			reg_to_reg(IP, "AND");
			break;
		case 0x23:
			bigreg_to_bigreg(IP,"AND");
			break;
		case 0x24:
			OP_num_to_reg(IP);
			break;
		case 0x2A:
			reg_to_reg(IP, "SUB");
			break;
		case 0x2B:
			bigreg_to_bigreg(IP, "SUB");
			break;
		case 0x2C:
			OP_num_to_reg(IP);
			break;
		case 0x32:
			reg_to_reg(IP, "XOR");
			break;
		case 0x33:
			bigreg_to_bigreg(IP, "XOR");
			break;
		case 0x34:
			OP_num_to_reg(IP);
			break;
		case 0x3A:
			reg_to_reg(IP, "CMP");
			break;
		case 0x3B:
			bigreg_to_bigreg(IP, "CMP");
			break;
		case 0x3C:
			OP_num_to_reg(IP);
			break;
		case 0x40: 
			IncDec(IP, AX, "INC","AX");
			break;
		case 0x41:
			IncDec(IP, CX, "INC","CX");
			break;
		case 0x42:
			IncDec(IP, DX, "INC", "DX");
			break;
		case 0x43:
			IncDec(IP, BX, "INC", "BX");
			break;
		case 0x44:
			IncDec(IP, SP, "INC", "SP");
			break;
		case 0x45:
			IncDec(IP, BP, "INC", "BP");
			break;
		case 0x46:
			IncDec(IP, SI, "INC", "SI");
			break;
		case 0x47:
			IncDec(IP, DI, "INC", "DI");
			break;
		case 0x48:
			IncDec(IP, AX, "DEC", "AX");
			break;
		case 0x49:
			IncDec(IP, CX, "DEC", "CX");
			break;
		case 0x4A:
			IncDec(IP, DX, "DEC", "DX");
			break;
		case 0x4B:
			IncDec(IP, BX, "DEC", "BX");
			break;
		case 0x4C:
			IncDec(IP, SP, "DEC", "SP");
			break;
		case 0x4D:
			IncDec(IP, BP, "DEC", "BP");
			break;
		case 0x4E:
			IncDec(IP, SI, "DEC", "SI");
			break;
		case 0x4F:
			IncDec(IP, DI, "DEC", "DI");
			break;
		case 0x72:
			JMP(IP, "JC");
			break;
		case 0x73:
			JMP(IP, "JNC");
			break;
		case 0x74:
			JMP(IP, "JZ");
			break;
		case 0x75:
			JMP(IP, "JNZ");
			break;
		case 0x76:
			JMP(IP, "JBE");
			break;
		case 0x77:
			JMP(IP, "JNBE");
			break;
		case 0x78:
			JMP(IP, "JS");
			break;
		case 0x79:
			JMP(IP, "JNS");
			break;
		case 0x7E:
			JMP(IP, "JLE");
			break;
		case 0x80:
			OP_num_to_reg(IP);
			break;
		case 0x84:
			reg_to_reg(IP, "TEST");
			break;
		case 0x85:
			bigreg_to_bigreg(IP, "TEST");
			break;
		case 0x86:
			reg_to_reg(IP,"XCHG");
			break;
		case 0x87:
			bigreg_to_bigreg(IP,"XCHG");
			break;
		case 0x8A:
			reg_to_reg(IP,"MOV");
			break;
		case 0x8B:
			bigreg_to_bigreg(IP, "MOV");
			break;
		case 0x90:
			NOP(IP);
			break;
		case 0x91:
			XCHG_bigreg_to_AX(IP, CX, "CX");
			break;
		case 0x92:
			XCHG_bigreg_to_AX(IP, DX, "DX");
			break;
		case 0x93:
			XCHG_bigreg_to_AX(IP, BX, "BX");
			break;
		case 0xA8:
			OP_num_to_reg(IP);
			break;
		case 0xB0:
			MOV_num_to_reg(IP, AL, "AL");
			break;
		case 0xB1:
			MOV_num_to_reg(IP, CL, "CL");
			break;
		case 0xB2:
			MOV_num_to_reg(IP, DL, "DL");
			break;
		case 0xB3:
			MOV_num_to_reg(IP, BL, "BL");
			break;
		case 0xB4:
			MOV_num_to_reg(IP, AH, "AH");
			break;
		case 0xB5:
			MOV_num_to_reg(IP, CH, "CH");
			break;
		case 0xB6:
			MOV_num_to_reg(IP, DH, "DH");
			break;
		case 0xB7:
			MOV_num_to_reg(IP, BH, "BH");
			break;
		case 0xB8:
			// Another MOV function
			break;
		case 0xCD: 
			if (mem[IP+1] == 0x20) exit(0);
			break;
		case 0xD0:
			OP_num_to_reg_2(IP,"EIGHT");
			break;
		case 0xD1:
			OP_num_to_reg_2(IP,"SIXTEEN");
			break;
		case 0xEB:
			JMP(IP, "JMP");
			break;
		case 0xF6:
			NegTest(IP, "EIGHT");
			break;
		case 0xF7:
			NegTest(IP, "SIXTEEN");
			break;
	}
}

int main()
{
    // Open file
    FILE* file;
    cout << "What is the file name? (format: C:...FILENAME.COM): ";
	string fileName; int debug = -1; char debugOption;
    cin >> fileName;
    file = fopen(fileName.c_str(), "rb");
    while (file == NULL) {
        cout << "Error. What is the file name: ";
        string fileName;
        cin >> fileName;
        file = fopen(fileName.c_str(), "rb");
    }
	cout << "Would you like debugging options? This shows individual steps and changes to the register (y/n): ";
	while (debug == -1) {
		cin >> debugOption;
		if (debugOption == 'y') debug = 1;
		else if (debugOption == 'n') debug = 0;
	}
    // Read file into mem[0x100000] with a deviation of 0x100
    byte comFileInstructions[0x10000-0x100];
    fread(comFileInstructions, sizeof(byte), 0x10000-0x100, file);
    for (int i = 0; i < 0x10000-0x100; i++) {
        mem[i+0x100] = comFileInstructions[i];
    }
	if (debug == 1) {
		for (int line = 1; line <= linelength; line++) { cout << "*"; } cout << "\n"; // print out line of stars
		cout << "DEBUG SPACE" << "\n";
	}
    // DEBUG: prints out contents of mem[i+0x100] up to arbitrary value of 500
    /*for (int i = 0x100; i < 500; i++) {
        cout << hex << showbase << mem[i] << " ";
        if (i % 8 == 2) {
            cout << " \n";
        }
    }*/


	// Perform instructions using instruction pointer
	auto start = chrono::steady_clock::now();
    while (mem[IP]!=int(0xCC)) {
		byte codebyte = mem[IP];
		if (debug == 1) cout << "Codebyte: " << hex << showbase << (int)codebyte << endl;
        switchInstruction(codebyte);
		if (debug == 1) printResults(1);
    }
	// Calculates approximate execution time and prints out instructions performed
	auto end = chrono::steady_clock::now();
	for (int line = 1; line <= linelength; line++){ cout << "*"; } cout << "\n"; // print out line of stars
    cout << "Execution Time: " << to_string(chrono::duration_cast<chrono::microseconds>(end-start).count()) << " microseconds \n";
	cout << "Instructions performed: " << to_string(instructions.size()) << "\n";
	for (int line = 1; line <= linelength; line++) { cout << "*"; } cout << "\n"; // print out line of stars
	printResults(0);
	for (int line = 1; line <= linelength; line++) { cout << "*"; } cout << "\n"; // print out line of stars
	cout << "ASM Instructions: \n";
	while (!instructions.empty()) {
		cout << instructions.front() << "\n";
		instructions.pop();
	}
	for (int line = 1; line <= linelength; line++) { cout << "*"; } cout << "\n"; // print out line of stars
	system("pause");
    fclose(file);
    return 0;
}