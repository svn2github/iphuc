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

#ifndef DFUINTERFACE_H
#define DFUINTERFACE_H

#include "Shell.h"
#include "MobileDevice.h"

void dfu_progress_callback();
int dfu_restore(string *args, struct shell_state *sh);
int dfu_grestore(string *args, struct shell_state *sh);
int dfu_filecopytophone(string *args, struct shell_state *sh);
int dfu_serial(string *args, struct shell_state *sh);
int dfu_cmd(string *args, struct shell_state *sh);
int dfu_disconnect(string *args, struct shell_state *sh);
int dfu_exit(string *args, struct shell_state *sh);

#endif // DFUINTERFACE_H

/* -*- mode:c; indent-tabs-mode:nil; c-basic-offset:2; tab-width:2; */