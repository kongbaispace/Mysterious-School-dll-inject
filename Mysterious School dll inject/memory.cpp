#include "memory.h"
#include "engine.h"


DWORD64 ReadLong(const DWORD64 address)
{
    DWORD64 value = -1;
    __try
    {
        value = *(DWORD64*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1;
    }
    return value;
}

uint32_t ReadUint(const DWORD64 address)
{
    uint32_t value = 0;
    __try
    {
        value = *(uint32_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}
uint16_t ReadUShort(const DWORD64 address)
{
    uint16_t value = 0;
    __try
    {
        value = *(uint16_t*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}

int ReadInt(const DWORD64 address)
{
    int value = -1;
    __try
    {
        value = *(int*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1;
    }
    return value;
}

float ReadFloat(const DWORD64 address)
{
    float value = -1.0f;
    __try
    {
        value = *(float*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1.0f;
    }
    return value;
}

double ReadDouble(const DWORD64 address)
{
    double value = -1.0;
    __try
    {
        value = *(double*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = -1.0;
    }
    return value;
}

BYTE ReadByte(const DWORD64 address)
{
    BYTE value = 0;
    __try
    {
        value = *(BYTE*)address;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        value = 0;
    }
    return value;
}

bool WriteLong(const DWORD64 address, DWORD64 value)
{
    __try
    {
        *(long long*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        
        return false;
    }
}

bool WriteInt(const DWORD64 address, int value)
{
    __try
    {
        *(int*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        
        return false;
    }
}

bool WriteFloat(const DWORD64 address, float value)
{
    __try
    {
        *(float*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

        return false;
    }
}

bool WriteDouble(const DWORD64 address, double value)
{
    __try
    {
        *(double*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

        return false;
    }
}

bool WriteByte(const DWORD64 address, BYTE value)
{
    __try
    {
        *(BYTE*)address = value;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

        return false;
    }
}

bool WriteBytes(const DWORD64 address, const std::vector<BYTE>& buffer)
{
    __try
    {
        memcpy((void*)address, buffer.data(), buffer.size());
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}











std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

uint64_t GetmySelfAddress() {
    const uint64_t baseoffset = 0x4B60010;
    const uint64_t offsets[] = { 0x190, 0x30, 0x618, 0x300, 0x2A0 };  // 依次应用的偏移量
    uint64_t address = 0;

    uint64_t moduleBase = (uint64_t)GetModuleHandle(NULL);

    // 使用 SafeReadPointer 函数读取地址
    address = ReadLong(moduleBase + baseoffset);

    for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
        address = ReadLong(address + offsets[i]);
        if (address == 0) {
            std::cerr << "Invalid address encountered at offset " << i << std::endl;
            return 0;
        }
    }

    return address;
}

std::string getnameforid(uint32_t id)
{
    FNameEntryHandle Handle(id);

    uint64_t nameptr = ReadLong((uint64_t)GetModuleHandle(NULL) + 0x49F3280 + 0x10 + (Handle.Block * 8));
    if (!nameptr) {
        return "NULL!";
    }

    // 偏移计算
    nameptr += 0x2 * Handle.Offset;
    // 读取字符串长度
    uint32_t namelen = ReadUShort(nameptr) >> 6;


    // 读取字符串内容
    char buffer[1024] = { 0 };
    
    memcpy(buffer, (void*)(nameptr + 0x2), namelen);

    return  std::string(buffer,namelen);
}

void ReadMatrixArray(const DWORD64 matrixHead, float* Matrix_Array)
{
    for (int i = 0; i < 16; i++)
    {
        Matrix_Array[i] = ReadFloat(matrixHead + i * 0x4);
    }
}

vec4 ReadVec4(uint64_t address) {
    vec4 value = { 0 };
    __try {
        value = *(reinterpret_cast<vec4*>(address));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        std::cerr << "Exception occurred while reading memory at address " << std::hex << address << std::endl;
    }
    return value;
}

vec3 ReadVec3(uint64_t address) {
    vec3 value = { 0 };
    __try {
        value = *(reinterpret_cast<vec3*>(address));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        std::cerr << "Exception occurred while reading memory at address " << std::hex << address << std::endl;
    }
    return value;
}


void WorldToScreenPoint(float* matrix, FVector xyz, vec2& xy)
{
    //float vorx, vory, vorz, vory2, row;
    //vec4 clipCoords;
    //clipCoords.x = xyz.X * matrix[0] + xyz.Y * matrix[4] + xyz.Z * matrix[8] + matrix[12];
    //clipCoords.y = xyz.X * matrix[1] + xyz.Y * matrix[5] + xyz.Z * matrix[9] + matrix[13];
    //clipCoords.z = xyz.X * matrix[2] + xyz.Y * matrix[6] + xyz.Z * matrix[10] + matrix[14];
    //clipCoords.w = xyz.X * matrix[3] + xyz.Y * matrix[7] + xyz.Z * matrix[11] + matrix[15];
    //vorz = xyz.X * matrix[3] + xyz.Y * matrix[7] + xyz.Z * matrix[11] + matrix[15];
    //row = 1 / vorz;
    //vorx = (width / 2) + (pos.X * matrix[0] + pos.Y * matrix[4] + pos.Z * matrix[8] + matrix[12]) * row * (width / 2);
    //vory = (height / 2) - (pos.X * matrix[1] + pos.Y * matrix[5] + pos.Z * matrix[9] + matrix[13]) * row * (height / 2);
    //vory2 = (height / 2) - (pos.X * matrix[1] + pos.Y * matrix[5] + (pos.Z + 160) * matrix[9] + matrix[13]) * row * (height / 2);

}