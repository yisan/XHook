#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdarg.h>

#include "dexstuff.h"

#include "config.h"

// 获得进程中libdvm.so动态库的一些有用的函数或者全局变量的地址，
// 并将其保存在一个dexstuff_t（位于dexstuff.h源文件中）结构体中
static void *mydlsym(void *hand, const char *name) {
    void *ret = dlsym(hand, name);
    LOGD("%s = 0x%x\n", name, ret);
    return ret;
}

// 得到所有控制Dalvik虚拟机的重要函数的地址，并将其记录在dexstuff_t结构体中，方便后面hook的时候使用
void dexstuff_resolv_dvm(struct dexstuff_t *d) {
    d->dvm_hand = dlopen("libdvm.so", RTLD_NOW);
    LOGD("dvm_hand = 0x%x\n", d->dvm_hand);

    if (d->dvm_hand) {
        d->dvm_dalvik_system_DexFile = (DalvikNativeMethod*) mydlsym(d->dvm_hand, "dvm_dalvik_system_DexFile");
        d->dvm_java_lang_Class = (DalvikNativeMethod*) mydlsym(d->dvm_hand, "dvm_java_lang_Class");

        d->dvmThreadSelf_fnPtr = mydlsym(d->dvm_hand, "_Z13dvmThreadSelfv");
        if (!d->dvmThreadSelf_fnPtr) {
            d->dvmThreadSelf_fnPtr = mydlsym(d->dvm_hand, "dvmThreadSelf");
        }

        d->dvmStringFromCStr_fnPtr = mydlsym(d->dvm_hand, "_Z32dvmCreateStringFromCstrAndLengthPKcj");
        d->dvmGetSystemClassLoader_fnPtr = mydlsym(d->dvm_hand, "_Z23dvmGetSystemClassLoaderv");
        d->dvmIsClassInitialized_fnPtr = mydlsym(d->dvm_hand, "_Z21dvmIsClassInitializedPK11ClassObject");
        d->dvmInitClass_fnPtr = mydlsym(d->dvm_hand, "dvmInitClass");

        d->dvmFindVirtualMethodHierByDescriptor_fnPtr = mydlsym(d->dvm_hand,
                                                                "_Z36dvmFindVirtualMethodHierByDescriptorPK11ClassObjectPKcS3_");
        if (!d->dvmFindVirtualMethodHierByDescriptor_fnPtr) {
            d->dvmFindVirtualMethodHierByDescriptor_fnPtr = mydlsym(d->dvm_hand,
                                                                    "dvmFindVirtualMethodHierByDescriptor");
        }

        d->dvmFindDirectMethodByDescriptor_fnPtr = mydlsym(d->dvm_hand, "_Z31dvmFindDirectMethodByDescriptorPK11ClassObjectPKcS3_");
        if (!d->dvmFindDirectMethodByDescriptor_fnPtr) {
            d->dvmFindDirectMethodByDescriptor_fnPtr = mydlsym(d->dvm_hand,
                                                               "dvmFindDirectMethodByDescriptor");
        }
        if (!d->dvmFindDirectMethodByDescriptor_fnPtr) {
            LOGI("dvmFindDirectMethodByDescriptor can not found");
        }

        d->dvmIsStaticMethod_fnPtr = mydlsym(d->dvm_hand, "_Z17dvmIsStaticMethodPK6Method");
        d->dvmAllocObject_fnPtr = mydlsym(d->dvm_hand, "dvmAllocObject");
        d->dvmCallMethodV_fnPtr = mydlsym(d->dvm_hand, "_Z14dvmCallMethodVP6ThreadPK6MethodP6ObjectbP6JValueSt9__va_list");
        d->dvmCallMethodA_fnPtr = mydlsym(d->dvm_hand, "_Z14dvmCallMethodAP6ThreadPK6MethodP6ObjectbP6JValuePK6jvalue");
        d->dvmAddToReferenceTable_fnPtr = mydlsym(d->dvm_hand, "_Z22dvmAddToReferenceTableP14ReferenceTableP6Object");

        d->dvmSetNativeFunc_fnPtr = mydlsym(d->dvm_hand, "_Z16dvmSetNativeFuncP6MethodPFvPKjP6JValuePKS_P6ThreadEPKt");
        d->dvmUseJNIBridge_fnPtr = mydlsym(d->dvm_hand, "_Z15dvmUseJNIBridgeP6MethodPv");
        if (!d->dvmUseJNIBridge_fnPtr) {
            d->dvmUseJNIBridge_fnPtr = mydlsym(d->dvm_hand, "dvmUseJNIBridge");
        }

        d->dvmDecodeIndirectRef_fnPtr =  mydlsym(d->dvm_hand, "_Z20dvmDecodeIndirectRefP6ThreadP8_jobject");

        d->dvmLinearSetReadWrite_fnPtr = mydlsym(d->dvm_hand, "_Z21dvmLinearSetReadWriteP6ObjectPv");

        d->dvmGetCurrentJNIMethod_fnPtr = mydlsym(d->dvm_hand, "_Z22dvmGetCurrentJNIMethodv");

        d->dvmFindInstanceField_fnPtr = mydlsym(d->dvm_hand, "_Z20dvmFindInstanceFieldPK11ClassObjectPKcS3_");

        //d->dvmCallJNIMethod_fnPtr = mydlsym(d->dvm_hand, "_Z21dvmCheckCallJNIMethodPKjP6JValuePK6MethodP6Thread");
        d->dvmCallJNIMethod_fnPtr = mydlsym(d->dvm_hand, "_Z16dvmCallJNIMethodPKjP6JValuePK6MethodP6Thread");

        d->dvmDumpAllClasses_fnPtr = mydlsym(d->dvm_hand, "_Z17dvmDumpAllClassesi");
        d->dvmDumpClass_fnPtr = mydlsym(d->dvm_hand, "_Z12dvmDumpClassPK11ClassObjecti");

        d->dvmFindLoadedClass_fnPtr = mydlsym(d->dvm_hand, "_Z18dvmFindLoadedClassPKc");
        if (!d->dvmFindLoadedClass_fnPtr) {
            d->dvmFindLoadedClass_fnPtr = mydlsym(d->dvm_hand, "dvmFindLoadedClass");
        }
        d->dvmHashTableLock_fnPtr = mydlsym(d->dvm_hand, "_Z16dvmHashTableLockP9HashTable");
        d->dvmHashTableUnlock_fnPtr = mydlsym(d->dvm_hand, "_Z18dvmHashTableUnlockP9HashTable");
        d->dvmHashForeach_fnPtr = mydlsym(d->dvm_hand, "_Z14dvmHashForeachP9HashTablePFiPvS1_ES1_");

        d->dvmInstanceof_fnPtr = mydlsym(d->dvm_hand, "_Z13dvmInstanceofPK11ClassObjectS1_");

        d->gDvm = mydlsym(d->dvm_hand, "gDvm");
    }
    LOGD("dexstuff_resolv_dvm sucess!");
}

// 将指定dex文件（dex文件存放的具体路径由参数path指定）加载进Dalvik虚拟机中
// 并且返回其对应的句柄cookie
int dexstuff_loaddex(struct dexstuff_t *d, char *path) {
    jvalue pResult;
    jint result;

    LOGD("dexstuff_loaddex, path = 0x%x\n", path);
    void *jpath = d->dvmStringFromCStr_fnPtr(path, strlen(path), ALLOC_DEFAULT);
    u4 args[2] = { (u4)jpath, (u4)NULL };

    d->dvm_dalvik_system_DexFile[0].fnPtr(args, &pResult);
    result = (jint) pResult.l;
    LOGD("cookie = 0x%x\n", pResult.l);

    return result;
}

// 将指定类加载（load）进来并链接（link）起来
void* dexstuff_defineclass(struct dexstuff_t *d, char *name, int cookie) {
    u4 *nameObj = (u4*) name;
    jvalue pResult;

    LOGD("dexstuff_defineclass: %s using %x\n", name, cookie);

    void* cl = d->dvmGetSystemClassLoader_fnPtr();
    Method *m = d->dvmGetCurrentJNIMethod_fnPtr();
    LOGD("sys classloader = 0x%x\n", cl);
    LOGD("cur m classloader = 0x%x\n", m->clazz->classLoader);

    void *jname = d->dvmStringFromCStr_fnPtr(name, strlen(name), ALLOC_DEFAULT);
    //LOGD("called string...\n")

    u4 args[3] = { (u4)jname, (u4) m->clazz->classLoader, (u4) cookie };
    d->dvm_dalvik_system_DexFile[3].fnPtr( args , &pResult );

    jobject *ret = pResult.l;
    LOGD("class = 0x%x\n", ret);
    return ret;
}

void* getSelf(struct dexstuff_t *d) {
    return d->dvmThreadSelf_fnPtr();
}

void dalvik_dump_class(struct dexstuff_t *dex, char *clname) {
    if (strlen(clname) > 0) {
        void *target_cls = dex->dvmFindLoadedClass_fnPtr(clname);
        if (target_cls) {
            dex->dvmDumpClass_fnPtr(target_cls, (void *) 1);
        }
    } else {
        dex->dvmDumpAllClasses_fnPtr(0);
    }
}


