#include"engine.h"
#include "memory.h"

std::string UObject::GetFullName()
{
    if (this && Class)
    {
        std::string Temp;

        for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer)
        {
            Temp = NextOuter->GetName() + "." + Temp;
        }

        std::string Name = Class->GetName();
        Name += " ";
        Name += Temp;
        Name += GetName();

        return Name;
    }

    return "None";
}
std::string UObject::GetName()
{

    FNameEntryHandle Handle(Name.ComparisonIndex);

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

    return  std::string(buffer, namelen);
}

void UObject::ProcessEvent(void* fn, void* prams)
{
    auto addr = GetProcessEventAddr();
    reinterpret_cast<void(*)(void*, void*, void*)>(addr)(this, fn, prams);
}


UObject* TUObjectArray::GetObjectPtr(uint32_t id) const
{
    if (id >= NumElements) return nullptr;
    uint64_t chunckIndex = id / 65536;
    if (chunckIndex >= NumChunks) return nullptr;
    auto chunk = Objects[chunckIndex];
    if (!chunk)return nullptr;
    uint32_t withinChunkIndex = id % 65536 * 24;
    auto item = *reinterpret_cast<UObject**>(chunk + withinChunkIndex);

    return item;
}

UObject* TUObjectArray::FindObject(TUObjectArray* objArray, const char* name)
{
    if (!objArray || !name) return nullptr;

    std::regex target_regex(name, std::regex_constants::icase);

    for (int32_t Index = 0; Index < objArray->NumElements; ++Index)
    {
        UObject* Obj = objArray->GetByIndex(Index);
        if (!Obj) continue;

        std::string ObjName = Obj->GetFullName();

        if (std::regex_search(ObjName, target_regex))
        {
            /*std::cout << "[Found] ID: " << Index << " | Name: " << ObjName << " | Ptr: " << Obj << std::endl;*/
            return Obj;
        }
    }

    return nullptr;
}

FVector AActor::K2_GetActorLocation()
{
    if (this->RootComponent)
    {
        return this->RootComponent->RelativeLocation;
    }
    // 如果没有 RootComponent，返回默认值或 NaN，或者你自定义的错误值
    return FVector{ 0.f, 0.f, 0.f };
}
