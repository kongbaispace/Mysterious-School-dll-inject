#pragma once
#include<Windows.h>
#include<iostream>
#include<string>
#include<sstream>
#include<regex>
#include<vector>


typedef struct xyzw
{
	float x;
	float y;
	float z;
	float w;
}vec4;

typedef struct xyz
{
	float x;
	float y;
	float z;
}vec3;

typedef struct xy 
{
	float x;
	float y;
}vec2;

class data 
{
public:
	float* matrix = NULL;
};

#include <algorithm>

// ¸¨Öúº¯Êý£º½«×Ö·û´®×ª»»ÎªÐ¡Ð´
std::string toLower(const std::string& str);

uint64_t GetmySelfAddress();
std::string getnameforid(uint32_t id);

//Read Memory
DWORD64 ReadLong(const DWORD64 address);
uint32_t ReadUint(const DWORD64 address);
uint16_t ReadUShort(const DWORD64 address);
int ReadInt(const DWORD64 address);
float ReadFloat(const DWORD64 address);
double ReadDouble(const DWORD64 address);
BYTE ReadByte(const DWORD64 address);

//Write Memory
bool WriteLong(const DWORD64 address, const DWORD64 value);
bool WriteInt(const DWORD64 address, const int value);
bool WriteFloat(const DWORD64 address, const float value);
bool WriteDouble(const DWORD64 address, const double value);
bool WriteByte(const DWORD64 address, const BYTE value);
bool WriteBytes(const DWORD64 address, const std::vector<BYTE>& bytes);

void ReadMatrixArray(const DWORD64 matrixHead, float* Matrix_Array);
vec4 ReadVec4(uint64_t address);
vec3 ReadVec3(uint64_t address);