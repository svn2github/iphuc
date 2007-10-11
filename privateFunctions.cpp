/**
 * privateFunctions.cpp - Provides a mechanism for defining, initing
 *                        and accessing private DLL functions
 */
 
#include "privateFunctions.h"
#include <iostream>

#if defined(__APPLE__)
#include <dlfcn.h>
#include <mach-o/nlist.h>
#endif

using namespace std;

static cmdsend   priv_sendCommandToDevice;
static cmdsend   priv_sendFileToDevice;
static rcmdsend  priv_performOperation;
static ricmdsend priv_socketForPort;

#if defined(__APPLE__)
#define MD_FRAMEWORK_LIB	"/System/Library/PrivateFrameworks/MobileDevice.framework/Versions/A/MobileDevice"

static unsigned int lookupSymbol(char *libfile, char *name)
{
	struct nlist nl[2];
	bzero(&nl, sizeof(struct nlist) * 2);
	nl[0].n_un.n_name = name;
	if((nlist(libfile, nl) < 0) || (nl[0].n_type == N_UNDF)) {
		ifNotQuiet cout << "could not locate symbol '" << name << "' in '" << libfile << "."  << endl;
		return NULL;
	}
	return nl[0].n_value;	
}
#endif

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
	ifVerbose cout << "MacOS X Architecture: ";
	
#if defined(__POWERPC__)
	ifVerbose cout << "powerpc ";
#else
	ifVerbose cout << "i386 ";
#endif
	ifVerbose cout << endl;
	
	priv_sendCommandToDevice = (cmdsend)lookupSymbol(MD_FRAMEWORK_LIB, "__sendCommandToDevice");
	if(!priv_sendCommandToDevice) {
		return EXIT_FAILURE;
	}
	priv_sendFileToDevice = (cmdsend)lookupSymbol(MD_FRAMEWORK_LIB, "__sendFileToDevice");
	if(!priv_sendFileToDevice) {
		return EXIT_FAILURE;
	}
	priv_socketForPort = (ricmdsend)lookupSymbol(MD_FRAMEWORK_LIB, "__socketForPort");
	if(!priv_socketForPort) {
		return EXIT_FAILURE;
	}
	priv_performOperation = (rcmdsend)lookupSymbol(MD_FRAMEWORK_LIB, "__performOperation");
	if(!priv_performOperation) {
		// some versions have it under an alternate name
		priv_performOperation = (rcmdsend)lookupSymbol(MD_FRAMEWORK_LIB, "__iTunes73x_performOperation");
		if(!priv_performOperation) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
#else
	ifVerbose cout << "NONE.  Platform not supported! " << endl;
	return EXIT_FAILURE;
#endif
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
  return false;
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
