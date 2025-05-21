#pragma once
#include<Windows.h>
#include<iostream>
enum { FNameMaxBlockBits = 13 };
enum { FNameBlockOffsetBits = 16 };
enum { FNameMaxBlocks = 1 << FNameMaxBlockBits };
enum { FNameBlockOffsets = 1 << FNameBlockOffsetBits };
struct FNameEntryHandle
{
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t InBlock, uint32_t InOffset)
		: Block(InBlock)
		, Offset(InOffset)
	{
	}

	FNameEntryHandle(uint32_t Id) : Block(Id >> FNameBlockOffsetBits), Offset(Id& (FNameBlockOffsets - 1))
	{
	}

	explicit operator bool() const { return Block | Offset; }
};

struct view_matrix_t
{
	float operator[](int index)
	{
		return matrix[index];
	}

	float matrix[16];
};

struct FVector {
	float X, Y, Z;

	FVector WorldToScreen(view_matrix_t matrix, int screenWidth, int screenHeight) const
	{

		float _x = X * matrix[0] + Y * matrix[4] + Z * matrix[8] + matrix[12];
		float _y = X * matrix[1] + Y * matrix[5] + Z * matrix[9] + matrix[13];
		float z = X * matrix[2] + Y * matrix[6] + Z * matrix[10] + matrix[14];
		float w = X * matrix[3] + Y * matrix[7] + Z * matrix[11] + matrix[15];

		if (w < 0.001f)
			return FVector{ -1.f, -1.f, 0.f }; // 失败返回特殊值

		float mX = screenWidth / 2.0f;
		float mY = screenHeight / 2.0f;


		FVector NDC;
		NDC.X = _x / w;
		NDC.Y = _y / w;
		NDC.Z = z / w;

		float x = _x / w;
		float y = _y / w;

		return {
			(mX * NDC.X) + (NDC.X + mX),
			-(mY * NDC.Y) + (NDC.Y + mY),
			0.f
		};
	};
};


template<class T>
class TArray
{
public:
	T* Data;
	INT32 NumElements;
	INT32 MaxElements;
};

struct FName
{
public:

	__int32                                         ComparisonIndex;
	__int32                                         Number;
};


class UObject
{
public:

	void* VTable;
	__int32 Flags;
	__int32 Index;
	class UClass* Class;
	FName Name;
	class UObject* Outer;

	std::string GetFullName();
	std::string GetName();

	void ProcessEvent(void* fn, void* prams);

	std::uintptr_t GetProcessEventAddr() {
		auto vtable = reinterpret_cast<void***>(VTable);
		return (std::uintptr_t)vtable[0x44];
	}
};

using fu = void(__thiscall*)(UObject*, UObject*, void* prams);

class UField : public UObject
{
public:
	UField* Next;
};

class FField
{
public:
	void* VTable;
	class FFieldClass* ClassPrivate;
	UINT64                                 Owner;
	class FField* Next;
	FName                                         Name;
	INT32                                         ObjFlags;
};

class UStruct : public UField
{
public:
	UStruct* SuperStruct;           //0x0038
	UField* Children;               //0x0040
	FField* ChildProperties;            //0x0048
	INT32 PropertiesSize;           //0x004C        结构体大小
	INT32 MinAlignment;         //0x0050
};

class UClass : public UStruct
{
public:
	UINT8 Pad_B0[0x20];                                      // 0x00B0(0x0020)(Fixing Size After Last Property [ Dumper-7 ])
	UINT64 CastFlags;                                         // 0x00D0(0x0008)(NOT AUTO-GENERATED PROPERTY)
	UINT8    Pad_D8[0x40];                                      // 0x00D8(0x0040)(Fixing Size After Last Property [ Dumper-7 ])
	class UObject* DefaultObject;                                     // 0x0118(0x0008)(NOT AUTO-GENERATED PROPERTY)
	UINT8     Pad_120[0x110];                                    // 0x0120(0x0110)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

template <typename KeyType, typename ValueType>
class TPair
{
public:
	KeyType First;
	ValueType Second;

public:
	TPair(KeyType Key, ValueType Value)
		: First(Key), Second(Value)
	{
	}

public:
	inline       KeyType& Key() { return First; }
	inline const KeyType& Key() const { return First; }

	inline       ValueType& Value() { return Second; }
	inline const ValueType& Value() const { return Second; }
};

class FProperty : public FField
{
public:
	int                                         ArrayDim;                                          // 0x0038(0x0004)(NOT AUTO-GENERATED PROPERTY)
	int                                         ElementSize;                                       // 0x003C(0x0004)(NOT AUTO-GENERATED PROPERTY)
	UINT64                                        PropertyFlags;                                     // 0x0040(0x0008)(NOT AUTO-GENERATED PROPERTY)
	UINT8                                         Pad_48[0x4];                                       // 0x0048(0x0004)(Fixing Size After Last Property [ Dumper-7 ])
	INT32                                         Offset;                                            // 0x004C(0x0004)(NOT AUTO-GENERATED PROPERTY)
	UINT8                                         Pad_50[0x28];                                      // 0x0050(0x0028)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

//class UFunction : public UStruct
//{
//public:
//	using FNativeFuncPtr = void (*)(void* Context, void* TheStack, void* Result);
//	UINT32 FunctionFlags;                       //uint32 函数标识
//	UINT8 NumParms;                             //参数数量
//	UINT16 ParmsSize;                               //内存中参数的大小  bool=1 int32=4 
//	UINT16 ReturnValueOffset;                       //返回值内存偏移
//	UINT16 RPCId;
//	UINT16 RPCResponseId;
//	FProperty* FirstPropertyToInit;
//	UFunction* EventGraphFunction;
//	INT32 EventGraphCallOffset;
//	FNativeFuncPtr Func;                            //函数地址
//};

struct FUObjectItem
{
public:
	class UObject* Object;                                            // 0x0000(0x0008)(NOT AUTO-GENERATED PROPERTY)
	CHAR                                         Pad_8[0x10];                                       // 0x0008(0x0010)(Fixing Struct Size After Last Property [ Dumper-7 ])
};

class TUObjectArray
{
private:
	static inline auto DecryptPtr = [](void* ObjPtr) -> uint8_t*
		{
			return reinterpret_cast<uint8_t*>(ObjPtr);
		};

public:
	enum
	{
		ElementsPerChunk = 0x10000,
	};
public:
	FUObjectItem** Objects;
	CHAR Pad_0[0x08];
	int MaxElements;
	int NumElements;
	int MaxChunks;
	int NumChunks;
	

	inline FUObjectItem** GetDecrytedObjPtr() const
	{
		return reinterpret_cast<FUObjectItem**>(DecryptPtr(Objects));
	}

	inline class UObject* GetByIndex(const INT32 Index) const
	{
		const INT32 ChunkIndex = Index / ElementsPerChunk;
		const INT32 InChunkIdx = Index % ElementsPerChunk;

		if (ChunkIndex >= NumChunks || Index >= NumElements)
			return nullptr;

		FUObjectItem* ChunkPtr = GetDecrytedObjPtr()[ChunkIndex];
		if (!ChunkPtr) return nullptr;

		return ChunkPtr[InChunkIdx].Object;
	}

	UObject* GetObjectPtr(uint32_t id) const;

	static UObject* FindObject(TUObjectArray* objArray, const char* name);

	bool IsValidIndex(int32_t Index) const
	{
		return Index < NumElements && Index >= 0;
	}
};


class UWorld : public UObject
{
public:
	UINT8                                         Pad_28[0x8];                                       // 0x0028(0x0008)(Fixing Size After Last Property [ Dumper-7 ])
	class ULevel* PersistentLevel;                                   // 0x0030(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8	             Pad_38[0x8];                                         // 0x0038(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8	             Pad_40[0x8];                             // 0x0040(0x0008)(ExportObject, ZeroConstructor, Transient, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8	             Pad_48[0x8];                            // 0x0048(0x0008)(ExportObject, ZeroConstructor, Transient, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8	             Pad_50[0x8];                             // 0x0050(0x0008)(ExportObject, ZeroConstructor, Transient, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8	             Pad_58[0x8];                                    // 0x0058(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class UPhysicsCollisionHandler* PhysicsCollisionHandler;                           // 0x0060(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	TArray<class UObject*>                        ExtraReferencedObjects;                            // 0x0068(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPublic)
	TArray<class UObject*>                        PerModuleDataObjects;                              // 0x0078(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPublic)
	UINT8	             Pad_88[0x10];                                   // 0x0088(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPrivate)
	UINT8	             Pad_98[0x28];                         // 0x0098(0x0028)(Transient, DuplicateTransient, NativeAccessSpecifierPrivate)
	UINT8                                 Pad_C0[0x10];                             // 0x00C0(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class ULevel* CurrentLevelPendingVisibility;                     // 0x00D0(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	class ULevel* CurrentLevelPendingInvisibility;                   // 0x00D8(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	class UDemoNetDriver* DemoNetDriver;                                     // 0x00E0(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class AParticleEventManager* MyParticleEventManager;                            // 0x00E8(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	class APhysicsVolume* DefaultPhysicsVolume;                              // 0x00F0(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_F8[0x16];                                      // 0x00F8(0x0016)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8                                         BitPad_10E_0 : 2;                                  // 0x010E(0x0001)(Fixing Bit-Field Size Between Bits [ Dumper-7 ])
	UINT8                                         bAreConstraintsDirty : 1;                          // 0x010E(0x0001)(BitIndex: 0x02, PropSize: 0x0001 (Transient, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic))
	UINT8                                         Pad_10F[0x1];                                      // 0x010F(0x0001)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8										  Pad_110[0x8];                                  // 0x0110(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8										  Pad_118[0x8];                                 // 0x0118(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8										  Pad_120[0x8];                                         // 0x0120(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8										  Pad_128[0x8];                                          // 0x0128(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8										  Pad_130[0x8];                                  // 0x0130(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	TArray<class ULevel*>                         Levels;                                            // 0x0138(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPrivate)
	UINT8										  Pad_148[0x10];                                  // 0x0148(0x0010)(ZeroConstructor, Transient, NonTransactional, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_158[0x28];                                     // 0x0158(0x0028)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8                                         Pad_180[0x8];             // 0x0180(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_188[0x10];                      // 0x0188(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_198[0x8];        // 0x0198(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_1A0[0x8];       // 0x01A0(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_1A8[0x50];                                     // 0x01A8(0x0050)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8                                         Pad_1F8[0x8];                     // 0x01F8(0x0008)(ExportObject, ZeroConstructor, Transient, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8                                         Pad_200[0x50];   // 0x0200(0x0050)(ExportObject, Transient, NonTransactional, ContainsInstancedReference, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_250[0x10];   // 0x0250(0x0010)(ExportObject, ZeroConstructor, Transient, NonTransactional, ContainsInstancedReference, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_260[0x10];   // 0x0260(0x0010)(ExportObject, ZeroConstructor, Transient, NonTransactional, ContainsInstancedReference, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_270[0x370];                                    // 0x0270(0x0370)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8										  Pad_5E0[0x8];                                  // 0x05E0(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UINT8                                         Pad_5E8[0x90];                                     // 0x05E8(0x0090)(Fixing Size After Last Property [ Dumper-7 ])
	UINT8				                          Pad_678[0x58];;                                           // 0x0678(0x0058)(ContainsInstancedReference, NativeAccessSpecifierPrivate)
	UINT8                                         Pad_6D0[0xC8];                                     // 0x06D0(0x00C8)(Fixing Struct Size After Last Property [ Dumper-7 ])
};
class ULevel : public UObject
{
public:
	char                                         Pad_28[0x70];									    // 0x0028(0x0070)(Fixing Size After Last Property [ Dumper-7 ])
	class TArray<class AActor*>                   Actors;										    // 0x0098(0x0010)(THIS IS THE ARRAY YOU'RE LOOKING FOR! [NOT AUTO-GENERATED PROPER
	char                                         Pad_A8[0x10];									    // 0x00A8(0x0010)(Fixing Size After Last Property [ Dumper-7 ])
	class UWorld* OwningWorld;																	    // 0x00B8(0x0008)(ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGet
	class UModel* Model;																		    // 0x00C0(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char										Pad_C8[0x10];								    // 0x00C8(0x0010)(ExportObject, ZeroConstructor, ContainsInstancedReference, Nativ
	char										Pad_D8[0x8];													    // 0x00D8(0x0008)(ExportObject, ZeroConstructor, Transient, InstancedReference, Du
	int                                         NumTextureStreamingUnbuiltComponents;			    // 0x00E0(0x0004)(ZeroConstructor, IsPlainOldData, NonTransactional, NoDestructor,
	int                                         NumTextureStreamingDirtyResources;				    // 0x00E4(0x0004)(ZeroConstructor, IsPlainOldData, NonTransactional, NoDestructor,
	char										Pad_E8[0x8];									    // 0x00E8(0x0008)(ZeroConstructor, IsPlainOldData, NonTransactional, NoDestructor,
	char										Pad_F0[0x8];													    // 0x00F0(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char										Pad_F8[0x8];													    // 0x00F8(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char									     Pad_100[0x10];// 0x0100(0x0010)(ZeroConstructor, NativeAccessSpecifierPublic)
	float                                        LightmapTotalSize;							    // 0x0110(0x0004)(Edit, ZeroConstructor, EditConst, IsPlainOldData, NoDestructor, 
	float                                        ShadowmapTotalSize;							    // 0x0114(0x0004)(Edit, ZeroConstructor, EditConst, IsPlainOldData, NoDestructor, 
	TArray<struct FVector>                       StaticNavigableGeometry;						    // 0x0118(0x0010)(ZeroConstructor, NativeAccessSpecifierPublic)
	char									     Pad_128[0x10];						    // 0x0128(0x0010)(ZeroConstructor, NativeAccessSpecifierPublic)
	char									     Pad_138[0x98];									    // 0x0138(0x0098)(Fixing Size After Last Property [ Dumper-7 ])
	char										 Pad_1D0[0x10];								    // 0x01D0(0x0010)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char										 Pad_1E0[0x8];									    // 0x01E0(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, NonPIEDuplicateTr
	char			                             Pad_1E8[0xC];						    // 0x01E8(0x000C)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char                                         bIsLightingScenario : 1;						    // 0x01F4(0x0001)(BitIndex: 0x00, PropSize: 0x0001 (NoDestructor, HasGetValueTypeH
	char                                         BitPad_1F4_1 : 2;								    // 0x01F4(0x0001)(Fixing Bit-Field Size Between Bits [ Dumper-7 ])
	char                                         bTextureStreamingRotationChanged : 1;			    // 0x01F4(0x0001)(BitIndex: 0x03, PropSize: 0x0001 (NoDestructor, HasGetValueTypeH
	char                                         bStaticComponentsRegisteredInStreamingManager : 1; // 0x01F4(0x0001)(BitIndex: 0x04, PropSize: 0x0001 (Transient, DuplicateTransient,
	char                                         bIsVisible : 1;								    // 0x01F4(0x0001)(BitIndex: 0x05, PropSize: 0x0001 (Transient, NoDestructor, HasGe
	char                                         Pad_1F5[0x63];									    // 0x01F5(0x0063)(Fixing Size After Last Property [ Dumper-7 ])
	class AWorldSettings* WorldSettings;														    // 0x0258(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHa
	char                                         Pad_260[0x8];									    // 0x0260(0x0008)(Fixing Size After Last Property [ Dumper-7 ])
	TArray<class UAssetUserData*>                 AssetUserData;								    // 0x0268(0x0010)(ExportObject, ZeroConstructor, ContainsInstancedReference, Prote
	char                                         Pad_278[0x10];									    // 0x0278(0x0010)(Fixing Size After Last Property [ Dumper-7 ])
	char										 Pad_288[0x10];							           // 0x0288(0x0010)(ZeroConstructor, Transient, NativeAccessSpecifierPrivate)
};


class AActor : public UObject
{
public:
	UINT8			 				              Pad_28[0x30];										 // 0x0028(0x0030)
	UINT8                                         bNetTemporary : 1;                                 // 0x0058(0x0001)
	UINT8                                         bNetStartup : 1;                                   // 0x0058(0x0001)
	UINT8                                         bOnlyRelevantToOwner : 1;                          // 0x0058(0x0001)
	UINT8                                         bAlwaysRelevant : 1;                               // 0x0058(0x0001)
	UINT8                                         bReplicateMovement : 1;                            // 0x0058(0x0001)
	UINT8                                         bHidden : 1;                                       // 0x0058(0x0001)
	UINT8                                         bTearOff : 1;                                      // 0x0058(0x0001)
	UINT8                                         bForceNetAddressable : 1;                          // 0x0058(0x0001)
	UINT8                                         bExchangedRoles : 1;                               // 0x0059(0x0001)
	UINT8                                         bNetLoadOnClient : 1;                              // 0x0059(0x0001)
	UINT8                                         bNetUseOwnerRelevancy : 1;                         // 0x0059(0x0001)
	UINT8                                         bRelevantForNetworkReplays : 1;                    // 0x0059(0x0001)
	UINT8                                         bRelevantForLevelBounds : 1;                       // 0x0059(0x0001)
	UINT8                                         bReplayRewindable : 1;                             // 0x0059(0x0001)
	UINT8                                         bAllowTickBeforeBeginPlay : 1;                     // 0x0059(0x0001)
	UINT8                                         bAutoDestroyWhenFinished : 1;                      // 0x0059(0x0001)
	UINT8                                         bCanBeDamaged : 1;                                 // 0x005A(0x0001)
	UINT8                                         bBlockInput : 1;                                   // 0x005A(0x0001)
	UINT8                                         bCollideWhenPlacing : 1;                           // 0x005A(0x0001)
	UINT8                                         bFindCameraComponentWhenViewTarget : 1;            // 0x005A(0x0001)
	UINT8                                         bGenerateOverlapEventsDuringLevelStreaming : 1;    // 0x005A(0x0001)
	UINT8                                         bIgnoresOriginShifting : 1;                        // 0x005A(0x0001)
	UINT8                                         bEnableAutoLODGeneration : 1;                      // 0x005A(0x0001)
	UINT8                                         bIsEditorOnlyActor : 1;                            // 0x005A(0x0001)
	UINT8                                         bActorSeamlessTraveled : 1;                        // 0x005B(0x0001)
	UINT8                                         bReplicates : 1;                                   // 0x005B(0x0001)
	UINT8                                         bCanBeInCluster : 1;                               // 0x005B(0x0001)
	UINT8                                         bAllowReceiveTickEventOnDedicatedServer : 1;       // 0x005B(0x0001)
	UINT8                                         BitPad_5B_4 : 4;                                   // 0x005B(0x0001)
	UINT8                                         BitPad_5C_0 : 3;                                   // 0x005C(0x0001)
	UINT8                                         bActorEnableCollision : 1;                         // 0x005C(0x0001)
	UINT8                                         bActorIsBeingDestroyed : 1;                        // 0x005C(0x0001)
	UINT8					                      UpdateOverlapsMethodDuringLevelStreaming;          // 0x005D(0x0001)
	UINT8					                      DefaultUpdateOverlapsMethodDuringLevelStreaming;   // 0x005E(0x0001)
	UINT8	                                      RemoteRole;                                        // 0x005F(0x0001)
	char										  Pad60[0x34];										 // 0x0060(0x0034)
	float                                         InitialLifeSpan;                                   // 0x0094(0x0004)
	float                                         CustomTimeDilation;                                // 0x0098(0x0004)
	UINT8                                         Pad_9C[0x4];                                       // 0x009C(0x0004)
	UINT8                                         Pad_A0[0x40];			                             // 0x00A0(0x0040)
	class AActor* Owner;																			 // 0x00E0(0x0008)
	class FName                                   NetDriverName;                                     // 0x00E8(0x0008)
	UINT8	                                      Role;                                              // 0x00F0(0x0001)
	UINT8		                                  NetDormancy;                                       // 0x00F1(0x0001)
	UINT8										  SpawnCollisionHandlingMethod;                      // 0x00F2(0x0001)
	UINT8										  AutoReceiveInput;									 // 0x00F3(0x0001)
	UINT8                                         InputPriority;                                     // 0x00F4(0x0004)
	class UInputComponent* InputComponent;															 // 0x00F8(0x0008)
	float                                         NetCullDistanceSquared;                            // 0x0100(0x0004)
	UINT8                                         NetTag;                                            // 0x0104(0x0004)
	float                                         NetUpdateFrequency;                                // 0x0108(0x0004)
	float                                         MinNetUpdateFrequency;                             // 0x010C(0x0004)
	float                                         NetPriority;                                       // 0x0110(0x0004)
	UINT8                                         Pad_114[0x4];                                      // 0x0114(0x0004)
	class APawn* Instigator;														                 // 0x0118(0x0008)
	TArray<class AActor*>                         Children;                                          // 0x0120(0x0010)
	class USceneComponent* RootComponent;												             // 0x0130(0x0008)
	TArray<class AMatineeActor*>                  ControllingMatineeActors;                          // 0x0138(0x0010)
	UINT8                                         Pad_148[0x8];                                      // 0x0148(0x0008)
	TArray<class FName>                           Layers;                                            // 0x0150(0x0010)
	UINT8                                         Pad_160[0x8];                                      // 0x0160(0x0008)
	UINT8                                         Pad_168[0x8];                                      // 0x0168(0x0008)
	TArray<class FName>                           Tags;                                              // 0x0170(0x0010)
	UINT8                                         Pad_180[0x1];                             // 0x0180(0x0001)
	UINT8                                         Pad_181[0x1];                             // 0x0181(0x0001)
	UINT8                                         Pad_182[0x1];                             // 0x0182(0x0001)
	UINT8                                         Pad_183[0x1];                             // 0x0183(0x0001)
	UINT8                                         Pad_184[0x1];                             // 0x0184(0x0001)
	UINT8                                         Pad_185[0x1];                             // 0x0185(0x0001)
	UINT8                                         Pad_186[0x1];                             // 0x0186(0x0001)
	UINT8                                         Pad_187[0x1];                             // 0x0187(0x0001)
	UINT8                                         Pad_188[0x1];                             // 0x0188(0x0001)
	UINT8                                         Pad_189[0x1];                             // 0x0189(0x0001)
	UINT8                                         Pad_18A[0x1];                             // 0x018A(0x0001)
	UINT8                                         Pad_18B[0x1];                             // 0x018B(0x0001)
	UINT8                                         Pad_18C[0x1];                             // 0x018C(0x0001)
	UINT8                                         Pad_18D[0x1];                             // 0x018D(0x0001)
	UINT8                                         Pad_18E[0x1];                             // 0x018E(0x0001)
	UINT8                                         Pad_18F[0x1];                             // 0x018F(0x0001)
	UINT8                                         Pad_190[0x60];                                     // 0x0190(0x0060)
	TArray<class UActorComponent*>                InstanceComponents;                                // 0x01F0(0x0010)
	TArray<class UActorComponent*>                BlueprintCreatedComponents;                        // 0x0200(0x0010)
	UINT8                                         Pad_210[0x10];                                     // 0x0210(0x0010)

	 FVector  K2_GetActorLocation();
};


class USceneComponent
{
public:
	uint8_t Pad_0[0x11C];
	struct FVector                                RelativeLocation;
	UCHAR                                         Pad_B8[0xC];
	struct FVector                                RelativeScale3D;
	struct FVector                                ComponentVelocity;
	uint8_t Flags_14C;                  // 0x014C
	uint8_t Flags_14D;                  // 0x014D
	uint8_t Pad_14E[1];                 // 0x014E
	uint8_t Mobility;                   // 0x014F
	uint8_t DetailMode;                 // 0x0150
	uint8_t PhysicsVolumeChangedDelegate; // 0x0151
	uint8_t Pad_152[0xA6];              // 0x0152
};