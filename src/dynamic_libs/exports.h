#ifndef __EXPORTS_H_
#define __EXPORTS_H_

#include <coreinit/dynload.h>
#include <coreinit/debug.h>

#define EXPORT_DECL(res, func, ...)     res (* func)(__VA_ARGS__) __attribute__((section(".data"))) = 0;
#define EXPORT_VAR(type, var)           type var __attribute__((section(".data")));


#define EXPORT_FUNC_WRITE(func, val)    *(u32*)(((u32)&func) + 0) = (u32)val

#define OS_FIND_EXPORT(handle, func)    funcPointer = 0;                                                                \
                                        OSDynLoad_FindExport(handle, 0, # func, (void**) &funcPointer);                 \
                                        if(!funcPointer)                                                                \
                                            OSFatal("Function " # func " is NULL");                                     \
                                        EXPORT_FUNC_WRITE(func, funcPointer);

#define OS_FIND_EXPORT_EX(handle, func, func_p)                                                                         \
                                        funcPointer = 0;                                                                \
                                        OSDynLoad_FindExport(handle, 0, # func, (void**) &funcPointer);                 \
                                        if(!funcPointer)                                                                \
                                            OSFatal("Function " # func " is NULL");                                     \
                                        EXPORT_FUNC_WRITE(func_p, funcPointer);

#endif
