#ifndef __WINDLL_H__
#define __WINDLL_H__

// DLL interface stuff.... windows.. bleh..
#ifdef WIN32
# define DLL_EXPORT __declspec(dllexport)
# define DLL_IMPORT __declspec(dllimport)
#else
# define DLL_EXPORT
# define DLL_IMPORT
#endif

#ifdef USE_DLL_EXPORT
#define DLL_INFO DLL_EXPORT
#else
#define DLL_INFO DLL_IMPORT
#endif
// end of DLL interface

// now we just declare DLL_INFO on anything we want to be public outside of the 
// DLL, and in turn in the libsemblis properties we just define USE_DLL_EXPORT

#endif // __WINDLL_H__
