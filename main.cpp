#include <config.h>
#include "MobileDevice.h"
#include "Shell.h"

using namespace std;

#if !defined(__APPLE__)
bool run = true;
#endif

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

COMMAND normal_shell[] = 
{
	{ "help",          sh_help,         ">> help <command> - Display help information on <command>.  No args lists commands." },
	{ "cd",            n_cd,            ">> cd <path> - Change directory to <path>" },
	{ "lcd",           n_lcd,            ">> lcd <path> - Change local directory to <path>" },
	{ "ls",            n_ls,            ">> ls [path] - List directory at current working path or [path]" },
	{ "mkdir",         n_mkdir,         ">> mkdir <path> - Create directory at <path>" },		
	{ "rmdir",         n_rmdir,         ">> rmdir <path> - Remove directory at <path>" },
	{ "activate",      n_activate,      ">> activate <path> - Activate iPhone with .plist file at <path>" },
	{ "deactivate",    n_deactivate,    ">> deactivate - Deactivate iPhone" },
	{ "readvalue",     n_readvalue,     ">> readvalue <value> - Read <value>.  No args lists knownn values." },
	{ "enterrecovery", n_enterrecovery, ">> enterrecovery - Enter recovery mode. **WARNING: You'll need to restore the iPhone." },
	{ "reconnect",        n_reconnect,        ">> reconnect - Exit current shell and reconnect to device." },
	{ "startservice",  n_startservice,  ">> startservice <service> - Starts service <service> on the iPhone.  No args lists services." },
	{ "deviceinfo",    n_deviceinfo,    ">> deviceinfo - Display device info." },
	{ "getfilesize",   n_getfilesize,   ">> getfilesize <path> - Display size of file at <path>" },
	{ "getfile",       n_getfile,       ">> getfile <path> [localpath] - Get file on iPhone at <path> and write it to [localpath]" },
	{ "putfile",       n_putfile,       ">> putfile <localpath> [path] - Put file at <localpath> on iPhone at [path]" },
	{ "fileinfo",      n_fileinfo,      ">> fileinfo <path> - Display info for file at <path>" },
	{ "exit",          n_exit,          ">> exit - Escape to shell.  The other shell, the one whos child i am." },
	{ "lpwd",          n_lpwd,          ">> lpwd - Display the current local working directory." },
	{ "pwd",           n_pwd,           ">> pwd - Display the current remote working directory." },
	{ "setafc",	   n_setafc,		">> setafc <name string> - Set the name of the afc service to use."},
	{ (char *)NULL, (shell_funct *)NULL, (char *)NULL }
};

COMMAND restore_shell[] =
{
	{ "mount",		restore_mount,		">> mount <device> <path> - Mount device at path."},
	{ "partition",		restore_partition,	">> partition <device> - Partition device."},
	{ "erase",		restore_erase,		">> erase <device> - Erase device."},
	{ "ditto",		restore_ditto,		">> ditto <path1> <path2> - Copy file at path1 to path2."},
	{ "umount",		restore_umount,		">> umount <path> - Unmount device from path."},
	{ "FileSystemCheck",	restore_filesystemcheck,	">> filesystemcheck <device> - Check filesytem on device"	},
	{ "mkdir",		restore_mkdir,			">> mkdir <path> - Make directory 'path'."},
	{ "force",		restore_force,		">> force <command> - send cmd to phone."},
	{ "help",		sh_help, 	        ">> help <command> - Display help information on <command>.  No args lists commands." },
	{ "exit",		restore_exit,         	">> exit - Disconnect and wait for device normal reconnect." },
	{ (char *)NULL, (shell_funct *)NULL, (char *)NULL }
};

COMMAND recovery_shell[] =
{
	{ "restore",		recovery_restore,		">> restore - enter restore mode. "},
	{ "grestore",		recovery_grestore,		">> grestore <restorebundle> - enter restore mode interactively."},
	{ "filecopytophone",	recovery_filecopytophone,	">> filecopytophone"},
	{ "serial",		recovery_serial,		">> serial"},
	{ "cmd",		recovery_cmd,			">> cmd <command> - send command to phone."},
	{ "exit",         	n_exit,          		">> exit - Escape to shell.  The other shell, the one whos child i am." },	
	{ "help",		sh_help,			">> help <command> - Display help information on <command>.  No args lists commands." },
	{ (char *)NULL, (shell_funct *)NULL, (char *)NULL }
};

void dfu_connect_callback(am_recovery_device *rdev) {
	cout << "Detected a DFU connection: How did you do this?" << endl;
}

void dfu_disconnect_callback(am_recovery_device *rdev) {
	cout << "Detected a DFU disconnection: How did you do this?" << endl;
}

void recovery_connect_callback(am_recovery_device *rdev)
{
	cout << "recovery callback: Logging in restore.log: " << AMRestoreEnableFileLogging("restore.log") << endl;
	cout << "recovery callback: Connected in Recovery Mode" << endl;
	
	struct shell_state *sh = new shell_state();
	sh->dev = NULL;
	sh->restore_dev = NULL;
	sh->recovery_dev = rdev;
	sh->shell_mode = SHELL_RECOVERY;
	sh->command_array = recovery_shell;
	sh->remote_path = "#";
	sh->local_path = "#";
	sh->prompt_string = "(iPHUC Recovery) ";

	//enter shell		
	cout << "recovery callback: Entering shell in Recovery Mode." << endl;
	int ret = shell(sh);
	cout << "recovery callback: shell returned: " << ret << endl;
	delete sh;
	
}

void recovery_disconnect_callback(am_recovery_device *rdev)
{
	cout << endl << ">> Recovery Mode Disconnect." << endl;
}

void notification(struct am_device_notification_callback_info *info)
{
	struct am_device *dev = info->dev;
	unsigned int msg = info->msg;
	int retval;
	int shell_return_value;
	
	//  Need more verbosity here.
	if (msg == ADNCI_MSG_CONNECTED)
	{

		cout << "notification: iPhone attached." << endl;
	
		retval = AMDeviceConnect(dev);	
		cout << "AMDeviceConnect: " << retval << endl;
		if (retval)
		{
			// Check for restore mode
			cout << "notification: Could not connect." << endl;
			cout << ">> Attempting to connect to device in Restore mode." << endl;
		
			// build the shell_state
			struct shell_state *sh = new shell_state();
			sh->dev = dev;
			sh->restore_dev = AMRestoreModeDeviceCreate( 0 , AMDeviceGetConnectionID(dev),0);
			
			sh->restore_dev->port = socketForPort(sh->restore_dev, 0xf27e);
			cout << ">> Restore Mode Port: " << sh->restore_dev->port << endl;
			
			sh->shell_mode = SHELL_RESTORE;
			sh->command_array = restore_shell;
			sh->remote_path = "#";
			sh->local_path = "#";
			sh->prompt_string = "(iPHUC Restore) ";
		
			//enter shell
			cout << "notification: Entering shell in Restore Mode." << endl;
			shell_return_value = shell(sh);
			delete sh;

		} else {
		
			struct shell_state *sh = new shell_state();
			sh->dev = dev;
		
			// Enter normal mode
			cout 	<< "AMDeviceIsPaired: " << AMDeviceIsPaired(sh->dev) << endl;
			cout 	<< "AMDeviceValidatePairing: " << AMDeviceValidatePairing(sh->dev) << endl;
			cout 	<< "AMDeviceStartSession: " << AMDeviceStartSession(sh->dev) << endl;
			
			// Start AFC service
			cout	<< "AMDeviceStartService AFC: "
				<< AMDeviceStartService(sh->dev, AMSVC_AFC, &(sh->afch), NULL)
				<< endl;

			// Open an AFC Connection
			cout	<< "AFCConnectionOpen: "
				<< AFCConnectionOpen(sh->afch, 0, &(sh->conn))
				<< endl;

			/* Turns debug mode on if the environment variable AFCDEBUG is set to a numeric
			 * value, or if the file '/AFCDEBUG' is present and contains a value. */
	#if defined(__APPLE__)
			cout	<< "AFCPlatformInit: (no retval)" << endl;
			AFCPlatformInit();
	#endif
		
			// build the rest of shell_state
			sh->shell_mode = SHELL_NORMAL;
			sh->command_array = normal_shell;
		
			sh->remote_path = "/";
			sh->local_path = "/";
			sh->prompt_string = "(iPHUC) ";
		
			//enter shell		
			cout << "notification: Entering shell in Normal Mode." << endl;
			shell_return_value = shell(sh);
	
			delete sh;
		}
	
		cout << "notification: Shell returned " << shell_return_value << endl;
	
	} else if ( msg == ADNCI_MSG_DISCONNECTED ) {
		cout << endl << "notification: Disconnected.  Waiting for suitable device reconnect." << endl;
		shell_return_value = SHELL_WAIT;
	}
	
	switch (shell_return_value)
	{
		case SHELL_TERMINATE:
			cout << ">> Nothing left to do. Exiting." << endl;
#if !defined(__APPLE__)
			run = false;
#else
			exit(0);
#endif
			break;
		case SHELL_WAIT:
			cout << ">> Waiting for device reconnect." << endl;
			break;
		default:
			cout << ">> Shell could not recover.  Exiting." << endl;
#if !defined(__APPLE__)
			run = false;
#else
			exit(0);
#endif
	}
	
}

int main(int argc, char **argv)
{
	struct am_device_notification *notif; 

	cout << PACKAGE_STRING;

#ifdef HAVE_READLINE_COMPLETION
 	cout << " with tab completion."<< endl;
#else
	cout << endl;
#endif

	cout << ">> By The iPhoneDev Team: " << AUTHOR_NICK_STRING << endl;
	
	//Call to SERIOUS_HACKERY
	cout << "initPrivateFunctions: "; initPrivateFunctions(); cout << endl;
	//End SERIOUS_HACKERY

	cout	<< "AMDeviceNotificationSubscribe: "
		<< AMDeviceNotificationSubscribe(notification, 0, 0, 0, &notif)
		<< endl;
	
	unsigned int ret = AMRestoreRegisterForDeviceNotifications(
					dfu_connect_callback,
					recovery_connect_callback,
					dfu_disconnect_callback,
					recovery_disconnect_callback,
					0,
					NULL);
					
	if (ret != 0) {
		cout << "Problem registering recovery callback: " << ret << endl;
		return EXIT_FAILURE;
	}
	
	cout << "CFRunLoop: Waiting for iPhone." << endl;

#if defined(__APPLE__)
	CFRunLoopRun();
	cout << "main: CFRunLoop returned somehow, exiting.  Please report." << endl;
	return 1;
#else
	while (run) {
		Sleep(1);
	}
	return 0;
#endif

	
}

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */
