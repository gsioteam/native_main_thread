//
// Created by gen on 11/19/21.
//

#ifdef __ANDROID__

#include <jni.h>

JavaVM* bmt_vm = NULL;

void bmt_sendEvent(const char *name, const char *data) {
    if (bmt_vm) {
        JNIEnv *env = NULL;
        if ((*bmt_vm)->AttachCurrentThread(bmt_vm, &env, NULL) != 0) {
        }

        if (env) {
            jclass cls = (*env)->FindClass(env, "com/neo/native_main_thread/NativeMainThreadPlugin");
            jmethodID method = (*env)->GetStaticMethodID(env, cls, "sendEvent",
                    "(Ljava/lang/String;Ljava/lang/String;)V");

            jstring jname = (*env)->NewStringUTF(env, name);
            jstring jdata = (*env)->NewStringUTF(env, data);

            (*env)->CallStaticVoidMethod(env, cls, method, jname, jdata);

            (*env)->DeleteLocalRef(env, jname);
            (*env)->DeleteLocalRef(env, jdata);
            (*env)->DeleteLocalRef(env, cls);

            if ((*env)->ExceptionCheck(env)) {
                (*env)->ExceptionDescribe(env);
            }

            (*bmt_vm)->DetachCurrentThread(bmt_vm);
        }
    }
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    bmt_vm = vm;
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    bmt_vm = NULL;
}

#else

#include <objc/runtime.h>

typedef void (*IMP_sendEvent)(Class, SEL, const char *name, const char *data);

extern void bmt_sendEvent(const char *name, const char *data) {
    Class NativeMainThreadPlugin = objc_getClass("NativeMainThreadPlugin");
    SEL sel = sel_registerName("sendEvent:withData:");
    IMP_sendEvent imp = (IMP_sendEvent)class_getMethodImplementation(NativeMainThreadPlugin, sel);
    imp(NativeMainThreadPlugin, sel, name, data);
}
#endif
