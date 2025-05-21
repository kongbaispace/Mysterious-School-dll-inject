#pragma once
#include "engine.h"
#include "memory.h"
auto transmission = [](TUObjectArray* TUobject,int options) ->void {   //反射传送函数
    void* fn = TUObjectArray::FindObject(TUobject, "K2_SetActorLocation");
    UObject* player = (UObject*)GetmySelfAddress();
    struct
    {
        FVector                                ReturnValue;
        bool bSweep = false; void* SweepHitResult{ nullptr }; bool bTeleport = true;
    } Parms;
    switch (options) {                                  //传送判断
    case 1:                         
        Parms.ReturnValue = { 252.8045197,-4230.879395,100.891098 };    //出生点
        break;
    case 2:
        Parms.ReturnValue = { 262.288696,-1377.079590,997.802307 };      //三楼
        break;
    }
    player->ProcessEvent(fn, &Parms);
	
};