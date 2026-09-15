#pragma once

/*====================================================*/
/* Wrapper definitions for system dependent functions */
/*====================================================*/

#if defined(_WIN32)

/* Windows, MSVC++ */
#define _CRT_SECURE_NO_WARNINGS 1

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <string.h>
#include <memory.h>

#define fileno _fileno
#define access _access
#define strcmpi _strcmpi
#define stricmp _stricmp
#define strnicmp _strnicmp
#define strlwr _strlwr

#ifdef _FELLOW_DEBUG_CRT_MALLOC
#define _CRTDBG_MAP_ALLOC
#endif

#include <cstdlib>

#ifdef _FELLOW_DEBUG_CRT_MALLOC
#include <crtdbg.h>
#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

#else

/* POSIX-compatible hosts */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#define strcmpi strcasecmp
#define stricmp strcasecmp
#define strnicmp strncasecmp

#endif

/*========*/
/* setjmp */
/*========*/

#include <setjmp.h>
