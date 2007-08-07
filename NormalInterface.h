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

/*	
	NormalInterface.h - Functions performed in normal mode
*/

#ifndef NORMALINTERFACE_H
#define NORMALINTERFACE_H

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "MobileDevice.h"

using namespace std;

void put_file(struct shell_state *sh, char *local_path, char *remote_path);
void get_file(struct shell_state *sh, char *local_path, char *remote_path);
unsigned int get_file_size(struct afc_connection *conn, char *path);
void set_afc_service_name(string *name);
const char *get_afc_service_name();

//Functions called by the shell in normal mode
int n_cd(string *args, struct shell_state *sh);
int n_lcd(string *args, struct shell_state *sh);
int n_ls(string *args, struct shell_state *sh);
int n_mkdir(string *args, struct shell_state *sh);
int n_rmdir(string *args, struct shell_state *sh);
int n_activate(string *args, struct shell_state *sh);
int n_deactivate(string *args, struct shell_state *sh);
int n_deviceinfo(string *args, struct shell_state *sh);
int n_readvalue(string *args, struct shell_state *sh);
int n_enterrecovery(string *args, struct shell_state *sh);
int n_reconnect(string *args, struct shell_state *sh);
int n_startservice(string *args, struct shell_state *sh);
int n_getfilesize(string *args, struct shell_state *sh);
int n_getfile(string *args, struct shell_state *sh);
int n_putfile(string *args, struct shell_state *sh);
int n_fileinfo(string *args, struct shell_state *sh);
int n_exit(string *args, struct shell_state *sh);
int n_setafc(string *args, struct shell_state *sh);
int n_pwd(string *args, struct shell_state *sh);
int n_lpwd(string *args, struct shell_state *sh);

#endif // NORMALINTERFACE_H

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */