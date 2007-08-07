#include "RestoreInterface.h"

int restore_mount(string *args, struct shell_state *sh)
{
	
	if( args[1]!="" && args[2]!= "")
	{	
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
		
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Mount"));
		CFDictionarySetValue(req, CFSTR("DeviceName"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII));
		CFDictionarySetValue(req, CFSTR("MountPoint"),
			CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII));
	
		cout	<< "mount: mounting '" << args[1] << "' at '" << args[2] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
			
	} else {
		cout << "mount: please provide both arguments." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_partition(string *args, struct shell_state *sh)
{
	
	CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
								&kCFTypeDictionaryKeyCallBacks,
								&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Partition"));
	
	cout	<< "partition: returned error code: "
		<< performOperation(sh->restore_dev, req) << endl;
	
	return SHELL_CONTINUE;
}

int restore_erase(string *args, struct shell_state *sh)
{
	
	if( args[1]!="" && args[2]!= "")
	{	
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
		
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Erase"));
		CFDictionarySetValue(req, CFSTR("DeviceName"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		CFDictionarySetValue(req, CFSTR("VolumeName"),
			CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "erase: erasing device '" << args[1] << "' at volume '" << args[2] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
		
	} else {
		cout << "erase: please provide both arguments." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_ditto(string *args, struct shell_state *sh)
{
	
	if( args[1]!="" && args[2]!= "")
	{
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
		
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Ditto"));
		CFDictionarySetValue(req, CFSTR("SourcePath"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		CFDictionarySetValue(req, CFSTR("DestinationPath"),
			CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "ditto: copying file '" << args[1] << "' to '" << args[2] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
		
	}else {
		cout << "ditto: please provide both arguments." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_umount(string *args, struct shell_state *sh)
{
	if( args[1]!="" )
	{
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
		
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("Unmount"));
		CFDictionarySetValue(req, CFSTR("MountPoint"),
		CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "umount: Unmounting '" << args[1] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
			
	} else {
		cout << "umount: please provide both arguments." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_filesystemcheck(string *args, struct shell_state *sh)
{
	if( args[1]!="")
	{
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
		
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("FilesystemCheck"));
		CFDictionarySetValue(req, CFSTR("DeviceName"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "FileSystemCheck: checking device '" << args[1] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
	} else {
		cout << "filesystemcheck: please provide a path to a disk to check." << endl;
	}
	
	
	return SHELL_CONTINUE;
}

int restore_mkdir(string *args, struct shell_state *sh)
{
	if( args[1]!="" && args[2]!= "")
	{
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
									
		CFDictionarySetValue(req, CFSTR("Operation"), CFSTR("MakeDirectory"));
		CFDictionarySetValue(req, CFSTR("Mode"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		CFDictionarySetValue(req, CFSTR("Path"),
			CFStringCreateWithCString(NULL, args[2].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "mkdir: creating directory with modes '" << args[1] << "' at location '" << args[2] <<  "': "
			<< performOperation(sh->restore_dev, req) << endl;
			
	} else {
		cout << "mkdir: please provide both arguments." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_force(string *args, struct shell_state *sh)
{
	if( args[1]!="")
	{
		CFMutableDictionaryRef req = CFDictionaryCreateMutable(	kCFAllocatorDefault, 0,
									&kCFTypeDictionaryKeyCallBacks,
									&kCFTypeDictionaryValueCallBacks);
	
		CFDictionarySetValue(req, CFSTR("Operation"),
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) );
		
		cout	<< "force: sending operation: '" << args[1] << "': "
			<< performOperation(sh->restore_dev, req) << endl;
	} else {
		cout << "force: please provide a command." << endl;
	}
	
	return SHELL_CONTINUE;
}

int restore_exit(string *args, struct shell_state *sh)
{
	cout << "shell: exiting restore mode.  If connected, the phone _should_ restart." << endl;
	return SHELL_WAIT;
}