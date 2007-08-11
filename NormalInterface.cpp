#include "normalinterface.h"
#include "Shell.h"

int n_pwd(string *args, struct shell_state *sh)
{
	cout << sh->remote_path << endl;
	return SHELL_CONTINUE;
}

int n_lpwd(string *args, struct shell_state *sh)
{
	cout << sh->local_path << endl;
	return SHELL_CONTINUE;
}

int n_setafc(string *args, struct shell_state *sh)
{
	
	if( args[1] == "" )
	{
		cout << "setafc: please provide a service name.";
	} else {
		if (sh->shell_mode == SHELL_NORMAL)
			AFCConnectionClose(sh->conn);
		
		// Start AFC service
		
		signed int retval = AMDeviceStartService(sh->dev,
			CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII),
			&(sh->afch), NULL);
		
		if( retval )
		{
			cout	<< "setafc: AMDStartService Failed.  Retrying once." << endl;
			
			retval = 	AMDeviceStartService(sh->dev,
					CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII),
					&(sh->afch), NULL);
		}
		
		cout	<< "AMDeviceStartService AFC: " << retval << endl;
			
		// Open an AFC Connection
		cout	<< "AFCConnectionOpen: "
			<< AFCConnectionOpen(sh->afch, 0, &(sh->conn))
			<< endl;
	}
		
	return SHELL_CONTINUE;
}

int n_cd(string *args, struct shell_state *sh)
{
	
	// check for argument
	if(args[1] == "")
	{
		cout << "cd: Please specify a path." << endl;
		return SHELL_CONTINUE;
		
	} else {
		// assume relative path
		string temp = "/";
		
		if(args[1].c_str()[0]=='/')
			args[1] = args[1].substr(1);
		else
			temp = sh->remote_path;
		
		processRelativePath( &temp, &args[1] );
		
		if (dirExists(sh->conn, (char *)temp.c_str()))
			sh->remote_path=temp; 
		else
			cout << "cd: No such file or directory '" << temp << "'" << endl;
	}
	return SHELL_CONTINUE;
}

int n_lcd(string *args, struct shell_state *sh)
{
	string temp = "/";
	
	// check for argument
	if(args[1] == "")
	{
		cout << "lcd: Please specify a path." << endl;
		return SHELL_CONTINUE;
	} else {
		
		if(args[1].c_str()[0]=='/')
			args[1] = args[1].substr(1);
		else
			temp = sh->local_path;
		
		processRelativePath( &temp, &args[1] );
	}

	DIR *f = opendir(temp.c_str());
	if (f == NULL) {
		cout << "lcd: local file '" << temp << "' does not exist or is not a directory." << endl;
	} else {
		closedir(f);
		sh->local_path=temp;
	}

	return SHELL_CONTINUE;
}

int n_ls(string *args, struct shell_state *sh)
{
	//Remote LS userspace function
	struct afc_directory *dir;
	char *dirent;
	
	string ls_dir = "/";
	
	if ( args[1] != "" )
	{

		if(args[1].c_str()[0]=='/')
			args[1] = args[1].substr(1);
		else
			ls_dir = sh->local_path;
		
		processRelativePath( &ls_dir, &args[1] );
	
	} else {
		
		// we want to ls .
		ls_dir = sh->remote_path;
	
	}
	
	// open the dir for reading
	if (AFCDirectoryOpen(sh->conn, (char *)ls_dir.c_str(), &dir)) {
		cout << "ls: Directory '" << ls_dir << "' does not exist." << endl;
		return SHELL_CONTINUE;
	}
	
	// list the dir
	while (1) {
		AFCDirectoryRead(0, dir, &dirent);
		if (!dirent) break;
//		if( strcmp(dirent,".") != 0 && strcmp(dirent,"..") != 0 )  // filter . and ..
		cout << dirent << endl;
	}

	AFCDirectoryClose(0, dir);
	return SHELL_CONTINUE;
}

int n_mkdir(string *args, struct shell_state *sh)
{
	afc_error_t retval;
	string path = "/";
	
	if( args[1] == "" )
	{
	
		// assume we want the size of the current remote dir
		path = sh->remote_path;
	} else {
		
		if(args[1].c_str()[0]=='/')
		{
			args[1] = args[1].substr(1);
			processRelativePath(&path, &args[1]);
			
		} else {
			path = sh->local_path;
			processRelativePath( &path, &args[1] );
		}
	}
	
	retval = AFCDirectoryCreate(sh->conn, (char *)path.c_str() );
	if ( retval )
		cout << "mkdir: AFCDirectoryCreate returned unknown error code: " << retval << endl;
	
	return SHELL_CONTINUE;
}

int n_rmdir(string *args, struct shell_state *sh)
{
	afc_error_t retval;
	string path;
	
	if( args[1] == "" )
	{
	
		// assume we want the size of the current remote dir
		path = sh->remote_path;
	} else if ( args[1].at(0) == '/' ) {
	
		// assume an abs path
		path = args[1];
	} else {
	
		// assume we want the size of something relative to remote_path
		path = sh->remote_path;
		processRelativePath(&path, &args[1]);
	}
	
	retval = AFCRemovePath(sh->conn, (char *)path.c_str() );
	switch(retval)
	{
		case 1:
			cout << "rmdir: Could not remove directory, make sure it is empty." << endl;
		default:
			cout << "rmdir: AFCRemovePath returned unknown error code: " << retval << endl;
	}
	
	return SHELL_CONTINUE;
}

int n_activate(string *args, struct shell_state *sh)
{
	cout << "Implement me!" << endl;
	return SHELL_CONTINUE;
}

int n_deactivate(string *args, struct shell_state *sh)
{
	cout << "AMDeviceDeavtivate: " << AMDeviceDeactivate(sh->dev) << endl;
	cout << "deactivate: device deactivated." << endl;
	return SHELL_CONTINUE;
}

int n_deviceinfo(string *args, struct shell_state *sh)
{
	char *key = NULL, *val = NULL;
	afc_dictionary *devinfo;
	afc_error_t ret;
	
	cout << "deviceinfo: " << endl;

	ret = AFCDeviceInfoOpen(sh->conn, &devinfo);
	if (ret == 0) {
		AFCKeyValueRead(devinfo, &key, &val);
		while (key && val) {
			cout << key << ": " << val << endl;
			AFCKeyValueRead(devinfo, &key, &val);
		}
		AFCKeyValueClose(devinfo);
	} else
		cout << "error " << ret << endl;

	return SHELL_CONTINUE;
}

int n_readvalue(string *args, struct shell_state *sh)
{
	if( args[1] == "" )
	{
		cout << "readvalue: Please enter a value to be read" << endl;
		return SHELL_CONTINUE;
	}
	
	__CFString * result = AMDeviceCopyValue(sh->dev, 0, 
						CFStringCreateWithCString(NULL, args[1].c_str(), kCFStringEncodingASCII));
	if (result)
		//Windows handling code goes here!
		CFShow(result);
	else
		cout << "readvalue: Error reading value '" << args[1] << "'" << endl;
		
	return SHELL_CONTINUE;
}

int n_enterrecovery(string *args, struct shell_state *sh)
{
	cout << "AMDeviceEnterRecovery: " << AMDeviceEnterRecovery(sh->dev) << endl;
	return SHELL_CONTINUE;
}

int n_reconnect(string *args, struct shell_state *sh)
{
	cout << "shell: Terminating and waiting for device reconnect." << endl;
	return SHELL_WAIT;
}

int n_startservice(string *args, struct shell_state *sh)
{
	
	
	cout << "shell: Starting service '" << args[1] << "'" << endl; 
	
	afc_error_t ret = AMDeviceStartService(	sh->dev,
						CFStringCreateWithCString(	NULL,
						 				args[1].c_str(),
										kCFStringEncodingASCII),
						&sh->alt_service,
						NULL);
	
	if( !ret )
		cout << "AMDeviceStartService: Service started" << endl;
	else
		cout << "AMDeviceStartService: Service not found: " << ret << endl;
		
	return SHELL_CONTINUE;
	
}

int n_getfilesize(string *args, struct shell_state *sh)
{
	unsigned int size;
	string path;
	
	if( args[1] == "" )
	{
	
		// assume we want the size of the current remote dir
		path = sh->remote_path;
	} else if ( args[1].at(0) == '/' ) {
	
		// assume an abs path
		path = args[1];
	} else {
	
		// assume we want the size of something relative to remote_path
		path = sh->remote_path;
		processRelativePath(&path, &args[1]);
	}
	
	size = get_file_size(sh->conn, (char *)path.c_str());
	cout << "getfilesize: " << size << " bytes" << endl;

	return SHELL_CONTINUE;
}

int n_getfile(string *args, struct shell_state *sh)
{
	string path;
	
	// make sure we're only talkin one file here
	if( dirExists(sh->conn, (char *)args[1].c_str() ) )
	{
		cout	<< "getfile: Cannot retreive '" << args[1]
			<< "' as it appears to be a directory." << endl;
		
		return SHELL_CONTINUE;
	}
	
	if( args[1] == "" )
	{
	
		// Must provide a path to a file to get.
		cout	<< "getfile:  Please provide a path to a remote file."
			<< endl;
		
		return SHELL_CONTINUE;
		
	} else if ( args[1].at(0) == '/' ) {
	
		// assume an abs path
		path = args[1];
	
	} else {
	
		// assume we want something relative to remote_path
		path = sh->remote_path;
		processRelativePath(&path, &args[1]);
		args[1] = path;
	}
	
	// 2nd arg is optional
	if( args[2] == "" )
	{
		// if nothing is specified, simply copy to the current local directory
		args[2] = sh->local_path;
		string temp = args[1].substr( args[1].rfind("/",0) + 1, args[1].length() - 1 );
		
		processRelativePath(&args[2], &temp);
	
	} else if (args[2].at(0) != '/') {
		
		// assume we want something relative to local_path
		string temp = sh->local_path;
		processRelativePath(&temp, &args[2]);
		args[2] = temp;
	}

	// check to see if args[2] is a directory.  If it is, should append the file
	// name from args[1] to the end of args[2]
	
	DIR *tempdir = opendir( (const char*)args[2].c_str() );
	if( tempdir )
	{
		string temp = args[1].substr( args[1].rfind("/",0) + 1, args[1].length() - 1 );
		processRelativePath(&args[2], &temp);
		
		closedir(tempdir);
	}
	
	get_file(sh, (char *)args[2].c_str(), (char *)args[1].c_str());
	
	return SHELL_CONTINUE;
}

int n_putfile(string *args, struct shell_state *sh)
{
	string path;
	
	if( args[1] == "" )
	{
		cout	<< "putfile: please specify a file to send" << endl;
		return SHELL_CONTINUE;
	} else if ( args[1].at(0) == '/' ) {
	
		// assume an abs path
		path = args[1];
	} else {
	
		// assume we want something relative to local_path
		path = sh->local_path;
		processRelativePath(&path, &args[1]);
		args[1] = path;
	}
	
	// 2nd arg is optional
	if( args[2] == "" )
	{
		string temp;
		
		temp = args[1].substr( args[1].rfind("/",args[1].length() -1 ) + 1, (args[1].length() - 1) );
		args[2] = sh->remote_path;
		
		processRelativePath(&args[2], &temp);
	
	} else if (args[2].at(0) != '/') {

		// assume we want something relative to local_path
		string temp = sh->remote_path;
		processRelativePath(&temp, &args[2]);
		args[2] = temp;

	}
	
	// check to see if args[2] is a directory.  If it is, should append the file
	// name from args[1] to the end of args[2]
	
	struct afc_directory *tempdir = (afc_directory *)NULL;
	AFCDirectoryOpen(sh->conn, (char*)args[2].c_str() , &tempdir);
	if( tempdir )
	{
		string temp = args[1].substr( args[1].rfind("/",0) + 1, args[1].length() - 1 );
		processRelativePath(&args[2], &temp);
		
		AFCDirectoryClose(0, tempdir);
	}
	
	put_file(sh, (char *)args[1].c_str(), (char *)args[2].c_str() );
	
	return SHELL_CONTINUE;
}

int n_fileinfo(string *args, struct shell_state *sh)
{
	string path;
	char *key, *val;
	struct afc_dictionary *info;
	afc_error_t ret;
	
	if( args[1] == "" )
	{
		cout << "fileinfo: please provide a file to get info on." << endl;
		return SHELL_CONTINUE;
	} else if (args[1].at(0) != '/') {
		path = sh->remote_path;
		processRelativePath(&path, &args[1]);
	} else {
		path = sh->remote_path;
	}
	
	ret = AFCFileInfoOpen(sh->conn, (char *)path.c_str(), &info);
	if(ret)
	{
		cout << "AFCFileInfo: cannot open file '" << path << "': " << ret << endl;
		return SHELL_CONTINUE;
	}
	
	AFCKeyValueRead(info, &key, &val);
	while (key) {
		
		if (val)
			cout << key << ": " << val << endl;
		else
			cout << key << ": NULL" << endl;
			
		AFCKeyValueRead(info, &key, &val);
	}
	AFCKeyValueClose(info);
	
	return SHELL_CONTINUE;
}

int n_exit(string *args, struct shell_state *sh)
{
	return SHELL_TERMINATE;
}

void put_file(struct shell_state *sh, char *local_path, char *remote_path)
{
	FILE *f;
	struct stat sb;
	unsigned char *buf;
	unsigned int size;
	afc_file_ref ref;
	std::string remotePathStr;
	afc_error_t ret;
	
	cout << "putfile: Opening local file '" << local_path << "'" << endl;
	
	f = fopen(local_path, "rb");
	if (!f) {
		cout << "putfile: Failed to open local file '" << local_path << "'" << endl;
		return;
	}

	// If remote_path isn't set, use cp semantics
	if(remote_path == NULL)
	{
		// putfile foo/bar
		// should create a file named bar in the current dir
		string local = local_path;
		if(local[local.length() - 1] == '/')
		{
			cout << "putfile: Writing directories is currently unsupported" << endl;
			return;
		}

		string::size_type lastSlash = local.find_last_of("/");
		if(lastSlash == string::npos)
		{
			// simple filename in local_path
			remotePathStr = local_path;
		} else {
			remotePathStr = local.substr(lastSlash + 1);
		}
	
	} else {
		remotePathStr = remote_path;
	}
	
	remote_path = (char *) remotePathStr.c_str();

	size = fstat(fileno(f), &sb);
	
	// AFCFileRefWrite: Writing to remote file
	// putfile: Failed to write to remote file: 11
	// is usually caused by trying to write a file with size 0
	if( sb.st_size == 0 )
	{
		cout << "putfile: Cannot write file with size " << sb.st_size << endl;
		return;
	}
	
	buf = (unsigned char *)malloc(sb.st_size);
	
	if(!buf)
	{
		cout << "putfile: Could not allocate buffer.  Aborting." << endl;
		return;
	}
	
	fread(buf, sb.st_size, 1, f);
	fclose(f);
	
	cout << "AFCFileRefOpen: opening remote file '" << remote_path << "'" << endl;
	ret = AFCFileRefOpen(sh->conn, remote_path, 3, &ref);
	
	if (ret) {
		cout << "putfile: Failed to open remote file: " << ret << endl;
		return;
	}
	
	cout << "AFCFileRefWrite: Writing to remote file" << endl;
	ret = AFCFileRefWrite(sh->conn, ref, buf, sb.st_size);
	
	if (ret) {
		cout << "putfile: Failed to write to remote file: " << ret << endl;
		return;
	}
	
	cout << "AFCFileRefClose: Closing remote file. " << endl;
	ret = AFCFileRefClose(sh->conn, ref);
	
	if (ret) {
		cout << "putfile: Failed to close remote file: " << ret << endl;
		return;
	}

	free(buf);

	cout << "putfile: Transfer successful" << endl;
}

unsigned int get_file_size(struct afc_connection *conn, char *path)
{
	char *key, *val;
	unsigned int size;
	struct afc_dictionary *info;

	if (AFCFileInfoOpen(conn, path, &info))
	{
		cout << "AFCFileInfoOpen: could not open file '" << path << "'" << endl;
		return 0;
	}
	
	while (1) {
		AFCKeyValueRead(info, &key, &val);
		if (!key || !val)
			break;

		if (!strcmp(key, "st_size")) {
			sscanf(val, "%u", &size);
			AFCKeyValueClose(info);
			return size;
		}
	}

	AFCKeyValueClose(info);

	return 0;
}

void get_file(struct shell_state *sh, char *local_path, char *remote_path)
{
	FILE *f;
	unsigned char *buf;
	unsigned int remote_file_size;
	afc_file_ref ref;
	afc_error_t ret;
	
	// here we assume we are being passed absolute paths 
	// in both remote and local paths
	
	cout	<< "AFCFileRefOpen: opening remote path '"
		<< remote_path << "'" << endl;
	
	ret = AFCFileRefOpen(sh->conn, remote_path, 1, &ref);
	
	if (ret)
	{
		cout << "getfile: Failed to open remote file: " << ret << endl;
		return;
	}
	
	remote_file_size = get_file_size(sh->conn, remote_path); 

	cout	<< "AFCFileRefRead: reading " << remote_file_size
		<< " bytes into buffer" << endl;
	
	buf = (unsigned char *)malloc(remote_file_size);

	if (!buf)
	{
		cout << "getfile: could not allocate buffer. Aborting." << endl;
		return;
	}
	
	ret = AFCFileRefRead(sh->conn, ref, buf, &remote_file_size);
	
	if (ret) {
		cout << "getfile: Failed to read from remote file: " << ret << endl;
		return;
	}
	
	ret = AFCFileRefClose(sh->conn, ref);
	
	if (ret) {
		cout << "getfile: Failed to close remote file: " << ret << endl;
		return;
	}

	f = fopen(local_path, "w+b");
	if (f == NULL) {
		cout << "getfile: Failed to open local file '" << local_path << "'" << endl;;
		return;
	}
	
	cout << "getfile: Writing file to local path '" << local_path << "'" << endl;
	
	fwrite(buf, remote_file_size, 1, f);
	fclose(f);
	free(buf);

	cout << "getfile: Transfer successful." << endl;
}

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */
