#include <stdio.h>
#include <stdlib.h>
#include "MobileDevice.h"
#include "Shell.h"

using namespace std;

// The following is a hack so that we can write clean
// code and still use readline.  Fix but only if you grok.
COMMAND *cur;
shell_state *rl_sh;

char *dupstr( char *s )
{
	char *r = xmalloc(strlen(s)+1);
	strcpy(r,s);
	return(r);
}


char** cmd_completer(const char *text, int start, int end)
{
	char **matches = (char **)NULL;

#ifdef HAVE_READLINE_COMPLETION
	/* If this word is at the start of the line, then it is a command
	to complete.  Otherwise it is the name of a file in the current
	directory. */
	if (start == 0)
	{
		rl_completion_append_character = ' ';
		matches = rl_completion_matches(text, cmd_generator);
	} else if ( rl_sh->shell_mode == SHELL_NORMAL ) {
		matches = rl_completion_matches(text, rl_remote_complete);
	}
#endif
	
	return matches;
}


char* cmd_generator(const char *text, int state)
{
	static int list_index, len;
	char *name;
	
	if (!state)
	{
		list_index = 0;
		len = strlen(text);
	}
	
	while (name = cur[list_index].name )
	{
		list_index++;
		if (strncmp (name, text, len) == 0)
			return (dupstr(name));
	}
	
	return ((char *) NULL);
}

void initialize_readline()
{
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "iPHUC";


	// Tell the completer that we want a crack first.
	rl_attempted_completion_function = cmd_completer;

	/* readline signal handling */
	rl_catch_signals = 1;
	rl_catch_sigwinch = 1;
	rl_set_signals();
	
}

int exec_line( char *line, struct shell_state *sh )
{
	int t, count;
	size_t i;
	COMMAND *command = (COMMAND *)NULL;
	string word; 
	string *args = new string[MAX_ARGUMENTS];
		
	//strip leading white
	i = 0;
	while ( line[i] && line[i] == ' ' )
		i++;
	word = line + i;
	
	//run through *line and find spaces
	//string::size_type index;
	
	t = 0;
	count = 0;
	for( i = 0 ; i < word.length() && count < (MAX_ARGUMENTS - 1); i++)
	{
		if( word[i] == '\\' )
		{
			if ( (i+1) < word.length() )
			{
				i++;
				args[count] = args[count] + word[i];
			}
			continue;
		}
		
		if( word[i] == ' ' )
		{
			count++;
			
			while ( (i+1) < word.length() && word[i+1] == ' ' )
				i++;
				
			continue;
		}
		args[count] = args[count] + word[i];
	}
	
	// check to see if it's a valid command invocation
	for (i = 0; sh->command_array[i].name; i++)
	{
		if ( args[0].compare( sh->command_array[i].name ) == 0)
			command = &sh->command_array[i];
	}
	
	if (!command)
	{
		cout << "shell: No such command '" << args[0] << "'" << endl;
		return -1;
	}
	
	// call function
	return ((*(command->func)) (args, sh));
	
	delete[] args;
}

bool dirExists(afc_connection *hAFC, char *path)
{
	afc_directory *hAFCDir;
	int retval = AFCDirectoryOpen(hAFC, path, &hAFCDir);
	
	if(retval == 0)
		return true;
	else
		return false;
	
	//Should close dir, but AFCDirectoryClose crashes
	//AFCDirectoryClose(hAFC, &hAFCDir);
}

void processRelativePath(string *basePath, string *cdPath)
{
	// NOTE:	we assume that basePath is an absolute path.
	//		we also assume that cdPath is NOT an absolute path.
	// This function is meant to chdir basePath to cdPath
	
	string::size_type index = 0;
	string::size_type length = cdPath->length();
	string::size_type bp_end = basePath->length() - 1;
	
	if (*cdPath == "" || *cdPath == "." )
		return;
	
	while( index < length )
	{	
		//strip leading '/'
		while( index < length && cdPath->at(index) == PATH_DELIMITER_CHAR )
			index++;
		
		//stay in bounds
		if( index >= length )
			break;
		
		//Test for 'special' paths
		if( cdPath->at(index) == '.' )
		{
			index++;
			
			if( index >= length )
			{
				
			} else if( cdPath->at(index) != '.' ) {
				
				//its a '.'
				if ( cdPath->at(index) != PATH_DELIMITER_CHAR )
					index--;
				
				//break;
				
			} else {
				
				index++;
				
				if( index < length && cdPath->at(index) != PATH_DELIMITER_CHAR )
				{
					// the file name starts with '..'
					index -= 2;
				
				} else {
					
					
					if( *basePath == PATH_DELIMITER_STRING )
					{
						// cant go back on root dir
					} else {
					
						//do a '..' on basePath
						//remove initial '/'
						if( basePath->at(bp_end) == PATH_DELIMITER_CHAR )
							bp_end--;
					
						//remove until you get to another '/'
						while( bp_end > 0 && basePath->at(bp_end) != PATH_DELIMITER_CHAR )
							bp_end--;
					
						if( bp_end > 0 )
						{
							*basePath = basePath->substr(0,bp_end);
							bp_end = basePath->length() - 1;
						} else {
							*basePath = PATH_DELIMITER_STRING;
							bp_end = 0;
						}
					
					}
				}
			}
		}
		
		if( basePath->at(bp_end) != PATH_DELIMITER_CHAR )
		{
			*basePath += PATH_DELIMITER_CHAR;
			bp_end++;
		}
		
		while( index < length && cdPath->at(index) != PATH_DELIMITER_CHAR )
		{
			
			if( cdPath->at(index) != '\\' )
			{
				*basePath += cdPath->at(index);
				bp_end++;
				index++;
			} else {
				index++;
			}
		}
	}
	
	//if there is a trailing / on the end of cdPath, add it
	//as long as there isn't already one at the end of basePath
	if(	basePath->at(basePath->length() - 1) != PATH_DELIMITER_CHAR && cdPath->at(cdPath->length() - 1 ) == PATH_DELIMITER_CHAR )
		*basePath += PATH_DELIMITER_CHAR;
}

char *rl_remote_complete(const char *text, int state)
{
	static struct afc_directory *directory = (afc_directory *)NULL;
	static char *current_directory;
	static string *temp;
	char *retval = (char *)NULL;
	char *ret = (char *)NULL;
	
	string::size_type pcount;
	string t = text;
	string partial;
	
	// this is a new query, reset everything
	if( !state )
	{
		
		if (directory != (afc_directory *)NULL)
		{
			AFCDirectoryClose(0, directory);
			if( current_directory)
				free(current_directory);
		}

		//make an absolute path
		if(temp)
			delete temp;

		if( strlen(text) == 0 )
		{
			//assume "." if no relative path is supplied
			temp = new string( rl_sh->remote_path );
		} else {
			
			
			
			if( text[0] != PATH_DELIMITER_CHAR ) {
				
				//if it's not absolute, append it to the
				//current rl_sh->remote_path
				temp = new string( rl_sh->remote_path );
				processRelativePath(temp, &t);
				
			} else {
				//is already absolute
				temp = new string( text );
			}			
			
			//here we must remove any incomplete filenames
			//at the end of temp, so we truncate to the
			//right-most PATH_DELIMITER_CHAR
			string::size_type index = temp->rfind(PATH_DELIMITER_CHAR);
			if (index != 0 && index != string::npos)
			{
				if( index != (temp->length()-1) )
				{
					*temp = temp->substr(0,index);
					*temp += PATH_DELIMITER_STRING;
				}
			} else {
				*temp = PATH_DELIMITER_STRING;
			}
		}
		
		current_directory = (char *)xmalloc(temp->length());
		current_directory = savestring(temp->c_str());
		
		AFCDirectoryOpen(rl_sh->conn, current_directory, &directory);
		
	}
	
	pcount = t.rfind(PATH_DELIMITER_CHAR);	
	if( pcount == t.length() )
	{
		AFCDirectoryRead(0, directory, &retval);
	} else {
		
		if( pcount != string::npos )
			partial = t.substr(pcount+1);
		else
			partial = t;
		
		// read the next value from the dir
		retval = (char*)NULL;
		while(directory)
		{	
			char *tempptr;

			AFCDirectoryRead(0, directory, &tempptr);

			if( !tempptr )
				break;

			if( !strncmp(tempptr, partial.c_str(), partial.length() ) )
			{
				retval = tempptr;
				break;
			}
		}
	}
	
	
	if( !retval )
	{
		// clean up
		if (directory)
		{	
			AFCDirectoryClose(0, directory);
			directory = (afc_directory *)NULL;
			if( current_directory )
				free(current_directory);
		}
		
		return (char *)NULL;
				
	} else {
		
		int len = 1 + strlen(retval) + temp->length();
		
		ret = (char *)malloc( len );
		strcpy(ret, temp->c_str() );
		strncat(ret, retval, (len - temp->length()) );
		
		struct afc_directory *tmp = (afc_directory*)NULL;
		if ( !AFCDirectoryOpen(rl_sh->conn, ret, &tmp) )
			rl_completion_append_character = PATH_DELIMITER_CHAR;
		else 
			rl_completion_append_character = ' ';
		if( tmp )
			AFCDirectoryClose(0, tmp);
		
		return (ret);
	}
	
}

int shell(struct shell_state *sh)
{
	char *line;
	string prompt;
	int retval;
	
	cout << "shell: Entering loop." << endl;
	
	//initialize readline
	cur = sh->command_array; //hack
	rl_sh = sh; //hack
	
	initialize_readline();
	
	while (1) {

		prompt = sh->prompt_string + sh->remote_path+ ": ";
		line = readline(prompt.c_str());
		
		if (line == (char *)NULL)
		{
			return SHELL_TERMINATE;
		}
		
		add_history(line);
		retval = exec_line(line, sh);
		
		switch( retval )
		{
			case SHELL_CONTINUE:
				free(line);
				break;
			
			case SHELL_TERMINATE:
				free(line);
				if (sh->shell_mode == SHELL_NORMAL)
					AFCConnectionClose(sh->conn);
				// AMDeviceStopSession(dev);
				return retval;
			
			case SHELL_WAIT:
				free(line);
				if (sh->shell_mode == SHELL_NORMAL)
					AFCConnectionClose(sh->conn);
				// AMDeviceStopSession(dev);
				return retval;
				
			default:
				free(line);
				cout	<< "shell: Function returned unknown error: "
					<< retval << endl;
				return retval;
		}
		
	}
}

int sh_help(string *args, struct shell_state *sh)
{
	COMMAND *command = (COMMAND *)NULL;
	
	if( args[1] == "" )
	{
		cout << "help: Enter a command to display its documentation string." << endl;
		cout << "Commands accessable in this mode: " << endl;
		for (int i = 0; sh->command_array[i].doc; i++)
		{			
			cout << sh->command_array[i].doc << endl;
		}
		return SHELL_CONTINUE;
	}
	
	for (int i = 0; sh->command_array[i].name; i++)
	{			
		if ( args[1].compare( sh->command_array[i].name ) == 0 )
		{
			command = &sh->command_array[i];
			break;
		}
	}
	
	//cout << "help: info for '" << args[1] << "': " << endl;
	
	if(command)
		cout << command->doc << endl;
	else
		cout << "help: could not find command '" << args[1] << "'" << endl;
	
	
	return SHELL_CONTINUE;
}

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */
