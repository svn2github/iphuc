/*

Copyright (C) 2007  iPhoneDev Team ( http://iphone.fiveforty.net/wiki/ )

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <iostream>
#include <fstream>

extern "C"
{
#include <readline/history.h>
#include <readline/readline.h>
}

#include "MobileDevice.h"
#include "NormalInterface.h"
#include "RecoveryInterface.h"
#include "DFUInterface.h"
#include "RestoreInterface.h"
#include "privateFunctions.h"

#include "config.h"

//REMOVE
#define DEBUG 1

/* Defines for testing command line arguments passed to iphuc */
// max number of command line options is limited to sizeof(short int)
// which is always 2 bytes (16 bits)
#define OPT_QUIET	1	//q quiet
#define OPT_VERBOSE	2	//v verbose
#define OPT_SCRIPT	4	//s script
#define OPT_ONESHOT	8	//o oneshot
#define OPT_AFCNAME	16	//a afcname
#define OPT_DEBUG	32	//d debug
#define OPT_RECOVERY	64	//r wait for recovery mode
#define OPT_NORMAL	128	//n wait for normal mode
#define OPT_RESTORE	256	//e wait for restore mode (probably shouldn't use)

#define ifNotQuiet		if( !(getcliflags() & OPT_QUIET) )
#define ifVerbose		if( getcliflags() & OPT_VERBOSE )

#ifdef DEBUG
#define D(string)		if( getcliflags() & OPT_DEBUG ) cout << "debug: " << string << endl
#else 
#define D(string)		
#endif

/* Unix version of a hidden file.  Could be different on other systems. */
#define HIDDEN_FILE(fname)	((fname)[0] == '.')

// TODO:  Add windows version of this?
#define PATH_DELIMITER_CHAR '/'
#define PATH_DELIMITER_STRING "/"

#ifndef savestring
#define savestring(x) strcpy ((char *)malloc (1 + strlen (x)), (x))
#endif

/* Shell defines */
#define SHELL_UNKNOWN	-1
#define SHELL_NORMAL	0
#define	SHELL_RESTORE	1
#define SHELL_RECOVERY	2
#define SHELL_DFU		3

#define MAX_ARGUMENTS	5

// userspace function exit values
#define SHELL_CONTINUE	-1	// continue reading from interactive shell
#define	SHELL_TERMINATE	0	// terminate application 
#define SHELL_WAIT	1	// wait for device reconnect

typedef int shell_funct (string*, shell_state*);

typedef struct {
	char *name;		// User printable name
	shell_funct *func;	// Function pointer
	char *doc;		// Documentation string
} COMMAND;

// readline
typedef struct shell_state {
	struct afc_connection *afch;
	struct afc_connection *conn;
	struct afc_connection *alt_service;
	struct am_restore_device *restore_dev;
	struct am_recovery_device *recovery_dev;
	struct am_device *dev;
	
	int shell_mode;
	
	string prompt_string;
	string remote_path;
	string local_path;
	
	COMMAND *command_array;
};

struct afc_dirent {
	uint8_t d_namlen;
	char *d_name;
};

#define D_NAMLEN(d)	((d).d_namlen)

typedef int shell_funct (string*, shell_state*);
typedef char *rl_compentry_func_t (const char *, int);

extern "C" int _rl_match_hidden_files;
extern "C" int rl_complete_with_tilde_expansion;
extern "C" int _rl_completion_case_fold;
extern "C" char **rl_completion_matches(const char *, rl_compentry_func_t *);
extern "C" int rl_catch_signals;
extern "C" int rl_catch_sigwinch;
extern "C" void rl_free_line_state(void);
extern "C" void rl_cleanup_after_signal(void);
extern "C" int rl_set_signals(void);

int _rl_stricmp (char *string1, char *string2);
int _rl_strnicmp (char *string1, char *string2, int count);

char *dupstr( char *s );
char **cmd_completer(const char *text, int start, int end);
char *cmd_generator(const char *text, int state);
int exec_line( char *line, struct shell_state *sh );
void initialize_readline();

// shell
bool dirExists(afc_connection *hAFC, char *path);
void processRelativePath(string *basePath, string *cdPath);
int shell(struct shell_state *sh);
int sh_help(string *args, struct shell_state *sh);
int sh_run( string *args, struct shell_state *sh);
char *rl_remote_complete(const char *text, int state);
void setcliflags( short int flags );
short int getcliflags();
void setscriptpath( char *path );
void set_rfr(short int val);

#endif //SHELL_H

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */
