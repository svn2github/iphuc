/**
 * privateFunctions.cpp - Provides a mechanism for defining, initing
 *                        and accessing private DLL functions
 */
 
#include "privateFunctions.h"
#include <iostream>

#if defined(__APPLE__)
#include <dlfcn.h>
#endif

using namespace std;

static cmdsend   priv_sendCommandToDevice;
static cmdsend   priv_sendFileToDevice;
static rcmdsend  priv_performOperation;
static ricmdsend priv_socketForPort;

int initPrivateFunctions() {

	ifVerbose cout << "this is still not clean.  Architecture: ";

#if defined(WIN32)
 
	ifVerbose
	cout << "WIN32 ";
	
	//get sendCommandToDevice function pointer from dll
	HMODULE hGetProcIDDLL;
	hGetProcIDDLL = GetModuleHandle("iTunesMobileDevice.dll");
	
	if (!hGetProcIDDLL) {
		ifNotQuiet cout << "Could not find dll in memory" << endl;
		return EXIT_FAILURE;
	}
	
	//int __usercall sendCommandToDevice<eax>(AMRecoveryModeDevice *amrecd<esi>,void *cfsCommand) @ 10009290
    	//the address of AMRestorePerformRecoveryModeRestore is 10009F30
	priv_sendCommandToDevice=cmdsend((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore")-0x10009F30+0x10009290));
	
	//int __usercall sendFileToDevice<eax>(AMRecoveryModeDevice *amrd<ecx>,void *filename) @ 10009410
	//the address of AMRestorePerformRecoveryModeRestore is 10009F30
	priv_sendFileToDevice=cmdsend((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore")-0x10009F30+0x10009410));
	
	//int __usercall performOperation<eax>(AMRestoreModeDevice *device<esi>,void *message<ebx>) @ 100129C0
	//the address of AMRestorePerformRecoveryModeRestore is 10009F30
	priv_performOperation=rcmdsend((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore")-0x10009F30+0x100129C0));
	
	// @ 0x10012830 i think
	//the address of AMRestorePerformRecoveryModeRestore is 10009F30
	priv_socketForPort=ricmdsend((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore")-0x10009F30+0x10012830));
	
#elif defined(__APPLE__)
    // nm /System/Library/PrivateFrameworks/MobileDevice.framework/Versions/A/MobileDevice |
    //      egrep -e "(sendFileToDevice|performOperation|socketForPort|sendCommandToDevice)"
	// INTEL:
    // 3c39fa4b t __performOperation
    // 3c3a3e3b t __sendCommandToDevice
    // 3c3a4087 t __sendFileToDevice
    // 3c39f36c t __socketForPort
	// PPC:
    // 3c3a0e14 t __performOperation
    // 3c3a517c t __sendCommandToDevice
    // 3c3a52dc t __sendFileToDevice
    // 3c3a0644 t __socketForPort
#if defined(__POWERPC__)
	ifVerbose cout << "powerpc ";

	// For iTunes 7.3
	/*
	priv_sendCommandToDevice = (cmdsend)(0x3c3a517c);
	priv_sendFileToDevice = (cmdsend)(0x3c3a52dc);
	priv_performOperation = (rcmdsend)(0x3c3a0e14);
	priv_socketForPort = (ricmdsend)(0x3c3a0644);
	 */

	// For iTunes 7.4
	priv_sendCommandToDevice = (cmdsend)(0x3c3a5bb0);
	priv_sendFileToDevice = (cmdsend)(0x3c3a5d10);
	priv_performOperation = (rcmdsend)(0x3c3a0bc8);
	priv_socketForPort = (ricmdsend)(0x3c3a051c);
#else    
	ifVerbose cout << "i386 ";

	// For iTunes 7.3
	/*
	priv_sendCommandToDevice = (cmdsend)(0x3c3a3e3b);
	priv_sendFileToDevice = (cmdsend)(0x3c3a4087);
	priv_performOperation = (rcmdsend)(0x3c39fa4b);
	priv_socketForPort = (ricmdsend)(0x3c39f36c);
	 */

	// For iTunes 7.4
	priv_sendCommandToDevice = (cmdsend)(0x3c3a597f);
	priv_sendFileToDevice = (cmdsend)(0x3c3a5bcb);
	priv_performOperation = (rcmdsend)(0x3c3a0599);
	priv_socketForPort = (ricmdsend)(0x3c39ffa3);
#endif

#else
	ifVerbose cout << "NONE.  Platform not supported! " << endl;
#endif
	return EXIT_SUCCESS;
}

int sendCommandToDevice(am_recovery_device *rdev, CFStringRef cfs)
{
	int retval;
#if defined (WIN32)
	asm("movl %3, %%esi\n\tpush %1\n\tcall *%0\n\tmovl %%eax, %2"
		:
		:"m"(priv_sendCommandToDevice),  "m"(cfs), "m"(retval), "m"(rdev)
		:);
#elif defined (__APPLE__)
	retval = priv_sendCommandToDevice(rdev, cfs);
#else
	ifNotQuiet
	cout << "sendCommandToDevice not implemented for your platform." << endl;
#endif
    return retval;
}

int sendFileToDevice(am_recovery_device *rdev, CFStringRef filename)
{
	int retval;
#if defined (WIN32)
	asm("movl %3, %%ecx\n\tpush %1\n\tcall *%0\n\tmovl %%eax, %2"
		:
		:"m"(priv_sendFileToDevice),  "m"(filename), "m"(retval), "m"(rdev)
		:);
#elif defined (__APPLE__)
	retval = priv_sendFileToDevice(rdev, filename);
#else
	ifNotQuiet
	cout << "sendFileToDevice not implemented for your platform." << endl;
#endif
	return retval;
	
}

int performOperation(am_restore_device *rdev, CFMutableDictionaryRef message)
{
	int retval;
#if defined (WIN32)
	asm("movl %3, %%esi\n\tmovl %1, %%ebx\n\tcall *%0\n\tmovl %%eax, %2"
		:
		:"m"(priv_performOperation),  "m"(message), "m"(retval), "m"(rdev)
		:);
#elif defined (__APPLE__)
	retval = priv_performOperation(rdev, message);
#else
	ifNotQuiet
	cout << "performOperation not implemented for your platform." << endl;
#endif
	return retval;
}

int socketForPort(am_restore_device *rdev, unsigned int portnum)
{
    return priv_socketForPort(rdev, portnum);
}

///////////////////// CLEAN ME UP ///////////////////////////////
#if defined(WIN32)
void wCFShow(CFTypeRef tested) { 
	char buffer[4096]; 
	CFIndex got; 
	CFStringRef description = CFCopyDescription(tested); 
	CFStringGetBytes(description, 
		CFRangeMake(0, CFStringGetLength(description)), 
		CFStringGetSystemEncoding(), '?', TRUE, (UInt8 *)buffer, 4095, &got); 
	buffer[got] = (char)0; 
	cout << buffer << endl;
	CFRelease(description); 
}
#endif

void *base64toCFData(char *in)
{
//     string dec=base64_decode(in);
//   return (void *)CFDataCreate(NULL,(UInt8 *)dec.c_str(),dec.length());
}

void GhettoCFStringPrint(CFStringRef str)
{
  char output[4096];
  CFStringGetCString(str, output, 4096,  kCFStringEncodingASCII);
	cout << output << endl; 
}

void hexdump(void *memloc, int len)
{
     unsigned char* hexdump = (unsigned char*)memloc;
     for(int a=0; a<len; a++)
     {
      if((a-8)%16==0) cout << " ";
      if(a%16==0&&a!=0) cout << endl;
      if(hexdump[a]<16) cout << "0";
        printf("%x ", hexdump[a]);
     } 
     cout << endl;
}

void describe255(CFTypeRef tested) { 
	char buffer[256]; 
	CFIndex got; 
	CFStringRef description = CFCopyDescription(tested); 
	CFStringGetBytes(description, 
		CFRangeMake(0, CFStringGetLength(description)), 
		CFStringGetSystemEncoding(), '?', TRUE, (UInt8 *)buffer, 255, &got); 
	buffer[got] = (char)0; 
	fprintf(stdout, "%s\n", buffer); 
	CFRelease(description); 
}

//from www.adp-gmbh.ch/cpp/common/base64.html

static const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_decode(string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
  	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}
