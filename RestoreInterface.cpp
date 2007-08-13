#include "RestoreInterface.h"

int restore_mount(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" || args[2] == "")
	{
		ifNotQuiet cout << "mount: please provide both arguments." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Mount"));
	CFDictionarySetValue(req, CFSTR("DeviceName"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII));
	CFDictionarySetValue(req, CFSTR("MountPoint"),
		CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII));
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "mount: mounting '" << args[1] << "' at '" << args[2] << endl;
	
	// build a list of known error codes
	ifNotQuiet
	{
		switch (retval)
		{
			case 0:
				ifVerbose cout << "mount: Successful." << endl;
				break;
			case 6:
				cout << "mount: iPhone returned '6' - invalid argument." << endl;
				break;
			default:
				cout << "mount: iPhone returned '" << retval << "' - unknown error code." << endl;
				break;
		}
	}
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_partition(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Partition"));
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "partition: returned error code: "
			<< retval << endl;
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_erase(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" || args[2] == "")
	{
		ifNotQuiet cout << "mount: please provide both arguments." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Erase"));
	CFDictionarySetValue(req, CFSTR("DeviceName"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	CFDictionarySetValue(req, CFSTR("VolumeName"),
		CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "erase: erasing device '" << args[1] << "' at volume '" << args[2] << "': " << retval << endl;
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
		
	return SHELL_CONTINUE;
}

int restore_ditto(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" || args[2] == "")
	{
		ifNotQuiet cout << "ditto: please provide both arguments." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Ditto"));
	CFDictionarySetValue(req, CFSTR("SourcePath"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	CFDictionarySetValue(req, CFSTR("DestinationPath"),
		CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "ditto: copying file '" << args[1] << "' to '" << args[2] << "': " << retval << endl;
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_umount(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" )
	{
		ifNotQuiet cout << "umount: please provide an argument." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Unmount"));
	CFDictionarySetValue(req, CFSTR("MountPoint"),
	CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "umount: Unmounting '" << args[1] << "': " << retval << endl;
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_filesystemcheck(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" )
	{
		ifNotQuiet cout << "filesystemcheck: please provide a path to a disk to check." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("FilesystemCheck"));
	CFDictionarySetValue(req, CFSTR("DeviceName"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "FileSystemCheck: checking device '" << args[1] << "': " << retval << endl;
	
	//Here we should check for 'unknown device' error code to assume that anything else means
	//that fsck could not repair the disk, which we should alert the user to.
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_mkdir(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" || args[2] == "")
	{
		ifNotQuiet cout << "mkdir: please provide both arguments." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	D("setting values");							
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("MakeDirectory"));
	CFDictionarySetValue(req, CFSTR("Mode"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	CFDictionarySetValue(req, CFSTR("Path"),
		CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);
	
	ifVerbose cout	<< "mkdir: creating directory with modes '"
			<< args[1] << "' at location '" << args[2] <<  "': " << retval << endl;
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
			
	return SHELL_CONTINUE;
}

int restore_force(string *args, struct shell_state *sh)
{
	mach_error_t retval;
	
	if( args[1] == "" )
	{
		ifNotQuiet cout << "force: please provide a command." << endl;
		set_rfr(1);
		return SHELL_CONTINUE;
	}
	
	D("allocating CFMutableDictionary");
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
							
	D("setting values");
	CFDictionarySetValue(req, CFSTR("Operation"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
	
	D("sending dictionary");
	retval = performOperation(sh->restore_dev, req);

	ifVerbose cout	<< "force: sending operation: '" << args[1] << "'" << endl;
	
	ifNotQuiet
	{
		switch(retval)
		{
			case 0:
				ifVerbose cout << "force: Successful." << endl;
				break;
			case 8:
				cout << "force: Unknown command." << endl;
				break;
			default:
				cout << "force: Unknown error code '" << retval << "'.  Please report." << endl;
				break;
		}
	}
	
	// set return function retval so we can break a script if this doesn't work for some reason
	set_rfr(retval);
	
	return SHELL_CONTINUE;
}

int restore_exit(string *args, struct shell_state *sh)
{
	ifNotQuiet cout << "shell: exiting restore mode." << endl;
	ifVerbose cout << "shell: If connected, the phone may restart itself. Otherwise use button combo." << endl;
	set_rfr(0);
	return SHELL_WAIT;
}