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

/**
 * privateFunctions.h - Provides a mechanism for defining, initing
 *                      and accessing private DLL functions
 */

#ifndef PRIVATEFUNCTIONS_H
#define PRIVATEFUNCTIONS_H

//#include <string.h>
#include "Shell.h"
#include "MobileDevice.h"

using namespace std;

//// CLEAN ME UP ////////////////////////////////////
#if !defined(WIN32)
	#define __cdecl
#endif

#if defined(WIN32)
	void wCFShow(CFTypeRef tested);
#endif

void GhettoCFStringPrint(CFStringRef str);
void hexdump(void *memloc, int len);
void describe255(CFTypeRef tested);
inline bool is_base64(unsigned char c);
//string base64_decode(string const& encoded_string);
/////////////////////////////////////////////////////


typedef int (__cdecl * cmdsend)  (am_recovery_device *, CFStringRef) __attribute__ ((regparm(2)));
typedef int (__cdecl * rcmdsend) (am_restore_device  *, CFMutableDictionaryRef) __attribute__ ((regparm(2)));
typedef int (__cdecl * ricmdsend)(am_restore_device  *, unsigned int) __attribute__ ((regparm(2)));

int initPrivateFunctions();
int sendCommandToDevice(am_recovery_device *rdev, CFStringRef cfs);
int sendFileToDevice(am_recovery_device *rdev, CFStringRef filename);
int performOperation(am_restore_device *rdev, CFMutableDictionaryRef message);
int socketForPort(am_restore_device *rdev, unsigned int portnum);

#endif // PRIVATEFUNCTIONS_H
