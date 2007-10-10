#include "DFUInterface.h"

void dfu_progress_callback()
{
	fprintf(stderr, "DFU progress callback...\n");
}

int dfu_restore(string *args, struct shell_state *sh)
{
	
	mach_error_t ret;
	
	if (args[1] == "")
	{
		ifNotQuiet cout << "args[1] must be RestoreBundlePath" << endl;
		return SHELL_CONTINUE;
	}
	
	D("Building CFMutableDictionary");
	CFMutableDictionaryRef opts;
	D("Getting AMRestoreCreateDefaultOptions");
	opts = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);

	CFStringRef value = CFStringCreateWithCString(kCFAllocatorDefault, args[1].c_str(),
												  kCFStringEncodingMacRoman);
	CFDictionarySetValue(opts, CFSTR("RestoreBundlePath"), value );

#if 0
        // we don't have AMRestorePerformDFURestore at the moment. FIXME
	describe255(opts);
	ret = AMRestorePerformDFURestore( sh->recovery_dev, opts,
                                          (void*)dfu_progress_callback, NULL );
#endif
	CFRelease(value);
	ifVerbose cout 	<< "AMRestorePerformDFURestore: " << ret << endl;

	return SHELL_CONTINUE;
}

int dfu_exit(string *args, struct shell_state *sh)
{
	D("exiting iphuc");
	exit(0);
}

int dfu_disconnect(string *args, struct shell_state *sh)
{
	return SHELL_WAIT;
}
