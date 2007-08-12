#include "RecoveryInterface.h"

void recovery_progress_callback()
{
	fprintf(stderr, "Recovery progress callback...\n");
}

int recovery_restore(string *args, struct shell_state *sh)
{
	
	if (args[1] == "")
	{
		cout << "args[1] must be RestoreBundlePath" << endl;
		return SHELL_CONTINUE;
	}
	
	CFMutableDictionaryRef opts;
	opts = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);
	CFDictionarySetValue(opts, CFSTR("RestoreBundlePath"), args[1].c_str() );
        
	//    describe255(opts);
        
    	cout 	<< "restore: " 
		<< AMRestorePerformRecoveryModeRestore(	sh->recovery_dev,
							opts,
							(void *)recovery_progress_callback,
							NULL )
		<< endl;
	
	return SHELL_CONTINUE;
}

int recovery_grestore(string *args, struct shell_state *sh)
{
	
	int ret;
	
	if (args[1] == "")
	{
		cout << "args[1] must be RestoreBundlePath" << endl;
		return SHELL_CONTINUE;
	}
	
	// check for trailing /
	if ( args[1].at(args[1].length() - 1) != '/' )
		args[1] = args[1] + '/';
	
	string temp = args[1] + "694-5259-38.dmg";
	
	cout << "ENTERING G-RESTORE" << endl;
	
	
	
	cout << "Sending ramdisk...     " << endl << flush;
	ret = sendFileToDevice(sh->recovery_dev, CFStringCreateWithCString(NULL, temp.c_str(), kCFStringEncodingASCII));
	if (ret != 0) {
		cout << "failed (" << ret << ") Aborting." << endl;
		return SHELL_CONTINUE;
	} else {
		cout << "OK" << endl;
	}

	cout << "Loading ramdisk...     " << endl;
	ret = sendCommandToDevice(sh->recovery_dev, CFSTR("ramdisk"));
	if (ret != 0) {
		cout << "failed (" << ret << ") Aborting." << endl;
		return SHELL_CONTINUE;
	} else {
		cout << "OK" << endl;
	}
	
	temp = args[1]+ "kernelcache.restore.release.s5l8900xrb";

	cout << "Sending kernelcache... " << endl;
	ret = sendFileToDevice(sh->recovery_dev, CFStringCreateWithCString(NULL, temp.c_str(), kCFStringEncodingASCII));
	if (ret != 0) {
		cout << "failed (" << ret << ") Aborting." << endl;
		return SHELL_CONTINUE;
	} else {
		cout << "OK" << endl;
	}
			
	cout << "Setting bootargs...    " << endl;
	ret = sendCommandToDevice(sh->recovery_dev, CFSTR("setenv boot-args rd=md0 -v"));
	if (ret != 0) {
		cout << "failed (" << ret << ") Aborting." << endl;
		return SHELL_CONTINUE;
	} else {
		cout << "OK" << endl;
	}
	
	cout << "Loading kernelcache... " << endl;
	ret = sendCommandToDevice(sh->recovery_dev, CFSTR("bootx"));
	if (ret != 0) {
		cout << "failed (" << ret << ") Aborting." << endl;
		return SHELL_CONTINUE;
	} else {
		cout << "OK" << endl;
	}	
	return SHELL_CONTINUE;
}

int recovery_filecopytophone(string *args, struct shell_state *sh)
{
	cout	<< "filecopytophone: "
		<< sendFileToDevice(sh->recovery_dev, CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) )
		<< endl;
	//cout << "filecopytophone: " << filecopytophone(sh->recovery_dev, cline.c_str(), 0x09000000) << " bytes copied" << endl;
	
	
	return SHELL_CONTINUE;
}

int recovery_serial(string *args, struct shell_state *sh)
{
	cout << "Restarting in serial mode" << endl;
        sendCommandToDevice(sh->recovery_dev, CFSTR("bgcolor 255 255 0"));
        sendCommandToDevice(sh->recovery_dev, CFSTR("setenv debug-uarts 1"));
        sendCommandToDevice(sh->recovery_dev, CFSTR("saveenv"));
        sendCommandToDevice(sh->recovery_dev, CFSTR("reboot"));
	
	//im ASSUMING we dont want to kill the app here, but that we want to
	// at least exit recovery mode (?)
	
	return SHELL_CONTINUE;
}

int recovery_cmd(string *args, struct shell_state *sh)
{
	
	if ( args[1] == "" )
	{
		cout << "Please provide a command to send to the phone." << endl;
		return SHELL_CONTINUE;
	}
	
	cout	<< "cmd: " << args[1] << ": "
		<< sendCommandToDevice(sh->recovery_dev, CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII) )
		<< endl; 
	
	return SHELL_CONTINUE;
}

int recovery_disconnect(string *args, struct shell_state *sh)
{
	return SHELL_WAIT;
}