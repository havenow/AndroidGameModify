#include <unwind.h>
#include <dlfcn.h>
#include <vector>
#include <string>
#include <android/log.h>
#include <jni.h>

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    std::vector<_Unwind_Word> &stack = *(std::vector<_Unwind_Word>*)arg;
    stack.push_back(_Unwind_GetIP(context));
    return _URC_NO_REASON;
}

void callstackDump(std::string &dump) {
    std::vector<_Unwind_Word> stack;
    _Unwind_Backtrace(unwindCallback, (void*)&stack);
    dump.append("                                                               \n"
                "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n"
                 "pid: 17980, tid: 17980, name: callstack.dump  >>> callstack.dump <<<\n"
                 "signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0\n"
                 "r0 00000000  r1 00000000  r2 00000001  r3 00000001\n"
                 "r4 e8efe008  r5 e0537b99  r6 ff970b88  r7 ff970a98\n"
                 "r8 ff970de0  r9 e7904400  sl e790448c  fp ff970b14\n"
                 "ip e8ef985c  sp ff970a60  lr e8eca00f  pc e0537d86  cpsr 200b0030\n"
                 "backtrace:\n");

    char buff[256];
    for (size_t i = 0; i < stack.size(); i++) {
        Dl_info info;
        if (!dladdr((void*)stack[i], &info)) {
            continue;
        }
        int addr = (char*)stack[i] - (char*)info.dli_fbase - 1;
        if (info.dli_sname == NULL || strlen(info.dli_sname) == 0) {
            sprintf(buff, "#%02x pc %08x  %s\n", i, addr, info.dli_fname);
        } else {
            sprintf(buff, "#%02x pc %08x  %s (%s+00)\n", i, addr, info.dli_fname, info.dli_sname);
        }
        dump.append(buff);
    }
}

void callstackLogcat(int prio, const char* tag) {
    std::string dump;
    callstackDump(dump);
    __android_log_print(prio, tag, "%s", dump.c_str());
}

void fun1() {
    callstackLogcat(ANDROID_LOG_DEBUG, "MD_DEBUG");
}

void fun2() {
    fun1();
}

void fun3() {
    fun2();
}

extern "C" JNIEXPORT void Java_com_example_threadtest_PThread_start(JNIEnv *env, jclass clazz) {
    fun3();
}