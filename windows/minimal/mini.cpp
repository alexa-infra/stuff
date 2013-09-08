// Basic windows executable - minimal size
// no need for any includes and linkings against libs at all
// since we will do it the hardcore way
// note: it is not absolute minimum :) there are many ways to decrease size of executable file more

#pragma comment(linker, "/Entry:mainCRTStartup")
#pragma comment(linker, "/Merge:.rdata=.text")
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/INCREMENTAL:NO")

typedef unsigned int u32;
typedef unsigned long DWORD;

typedef struct _UNICODE_STR {
	unsigned short len;
	unsigned short maxlen;
	wchar_t* buffer;
} UNICODE_STR;

typedef struct __LIST_ENTRY {
	__LIST_ENTRY* flink;
	__LIST_ENTRY* blink;
} LISTENTRY;

typedef struct _LDR_MODULE {
	LISTENTRY					InLoadOrderModuleList;				// 0x00 - 0x08
	LISTENTRY					InMemoryOrderModuleList;			// 0x08 - 0x10
	LISTENTRY					InInitializationOrderModuleList;	// 0x10 - 0x18
	void*						BaseAddress;						// 0x18 - 0x1C
	void*						EntryPoint;							// 0x1c - 0x20
	unsigned long				SizeOfImage;						// 0x20 - 0x24
	UNICODE_STR					FullDllName;						// 0x24 - 0x28
	UNICODE_STR					BaseDllName;						// 0x28 - 0x2c
	//unsigned long				Flags;
	//short						LoadCount;
	//short						TlsIndex;
	//void*						HashTableEntry;
	//unsigned long				TimeDateStamp;
} LDR_MODULE;

template<typename Ch>
bool _strcmp(const Ch* a, u32 alen, const Ch* b, u32 blen) {
	if (alen != blen) return false;
	for (u32 i=0; i<alen; i++) {
		if (a[i] != b[i]) return false;
	}
	return true;
}

u32 _strlen(const char* a) {
	int i;
	for(i=0; a[i] != '\0'; i++);
	return i;
}

DWORD get_kernel32_base_address()
{
	LISTENTRY* hdr;

	// TEB, PEB address for 32, wow64, 64:
	// http://www.exploit-monday.com/2012/04/architecture-independent-windows.html
	// iteration:
	// http://blog.harmonysecurity.com/2009_06_01_archive.html

	__asm {
		xor edx, edx			// zero edx
		mov edx, fs:[0x30]		// get a pointer to the PEB
		mov edx, [edx+0x0C]		// get PEB->Ldr
		mov edx, [edx+0x10]		// get the first module from the InMemoryOrder module list
		mov hdr, edx
	}

	LISTENTRY* head = hdr;
	while ( hdr->blink != head )
	{
		LDR_MODULE* ptr = (LDR_MODULE*)(hdr);
		if (_strcmp(L"kernel32.dll", 12, ptr->BaseDllName.buffer, ptr->BaseDllName.len / 2)) {
			return (DWORD)ptr->BaseAddress;
		}
		hdr = hdr->blink;
	}
	return 0;
}

#define STDCALL __stdcall

typedef DWORD(STDCALL *TLoadLibraryA)(const char*);
TLoadLibraryA LoadLibraryA;

typedef int (STDCALL *TMessageBoxA)(DWORD hWnd, const char* text, const char* caption, u32 type);
TMessageBoxA MessageBoxA;

DWORD getProcAddress(DWORD module, const char* name, u32 len = 0) {
	if (len == 0)
		len = _strlen(name);

	long tmp = *(long*)(*(long*)(module + 0x3C) + module + 0x78) + module;
	int numImp = *(long*)(tmp + 0x18); // number of imports
	while (--numImp >= 0)
	{
		// resolve import #numImp
		const char *importName = (char*) (*(long*)(*(long*)(tmp + 0x20) + module + numImp * 4) + module);

		// compare import name
		u32 importLen = _strlen(importName);
		if (_strcmp(name, len, importName, importLen))
		{
			// resolve function entry
			#define __IMPORT_INDEX *(unsigned short*) (*(long*)((tmp + 0x24)) + module + numImp * 2)
			return (DWORD)(*(long*)(*(long*)(tmp + 0x1C) + module + 4 * __IMPORT_INDEX) + module);
			#undef __IMPORT_INDEX
		}
	}
	return 0;
}

void STDCALL mainCRTStartup()
{
	DWORD kernel32 = get_kernel32_base_address();
	LoadLibraryA = (TLoadLibraryA)getProcAddress(kernel32, "LoadLibraryA");

	DWORD user32 = LoadLibraryA("user32");
	MessageBoxA = (TMessageBoxA)getProcAddress(user32, "MessageBoxA");

	MessageBoxA(0, "hello world", "minimal", 0);
}
