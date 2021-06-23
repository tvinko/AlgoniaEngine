//Boby Thomas - march 2006

#ifndef os_call_h
#define os_call_h

#if defined(_WIN32) // Microsoft compiler
#include <windows.h>
#elif defined(__GNUC__) || defined (__gnu_linux__) // GNU compiler
#include <string.h>
#include <dlfcn.h>
#else
#error define your copiler
#endif

//#include<string>
/*
#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 4
*/

void* LoadSharedLibrary(char *pcDllname)
{
#if defined(_WIN32) // Microsoft compiler
	return (void*)LoadLibraryA(pcDllname);
#elif defined(__GNUC__) || defined (__gnu_linux__) // GNU compiler
	strcat(pcDllname, ".so");
	return dlopen(pcDllname, RTLD_LAZY);
#endif
	return NULL;
}
void *GetFunction(void *Lib, char *Fnname)
{
#if defined(_WIN32) // Microsoft compiler
	return (void*)GetProcAddress((HINSTANCE)Lib, Fnname);
#elif defined(__GNUC__) // GNU compiler
	return dlsym(Lib, Fnname);
#endif
}

int FreeSharedLibrary(void *hDLL)
{
#if defined(_WIN32) // Microsoft compiler
	return FreeLibrary((HINSTANCE)hDLL);
#elif defined(__GNUC__) // GNU compiler
	return dlclose(hDLL);
#endif
}


#endif //os_call_h

