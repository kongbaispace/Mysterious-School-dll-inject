#pragma once
#include "engine.h"
#include "memory.h"
auto transmission = [](TUObjectArray* TUobject,int options) ->void {   //���䴫�ͺ���
    void* fn = TUObjectArray::FindObject(TUobject, "K2_SetActorLocation");
    UObject* player = (UObject*)GetmySelfAddress();
    struct
    {
        FVector                                ReturnValue;
        bool bSweep = false; void* SweepHitResult{ nullptr }; bool bTeleport = true;
    } Parms;
    switch (options) {                                  //�����ж�
    case 1:                         
        Parms.ReturnValue = { 252.8045197,-4230.879395,100.891098 };    //������
        break;
    case 2:
        Parms.ReturnValue = { 262.288696,-1377.079590,997.802307 };      //��¥
        break;
    }
    player->ProcessEvent(fn, &Parms);
	
};