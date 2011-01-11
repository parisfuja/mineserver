/*
   Copyright (c) 2010, Kasper F. Brandt
   All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the The Mineserver Project nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cliScreen.h"

#include <iostream>
#include <sstream>

#ifndef WIN32
  #include <poll.h>
  #include <unistd.h>
#endif

#ifdef WIN32
DWORD WINAPI CliScreen::_stdinThreadProc(LPVOID lpParameter)
{
  return ((CliScreen *)lpParameter)->stdinThreadProc();
}

DWORD WINAPI CliScreen::stdinThreadProc()
{
  while (true)
  {
	std::string s;
	std::getline(std::cin, s);
	if (std::cin.rdstate() != std::ios::goodbit)
	{
		EnterCriticalSection(&ccAccess);
		currentCommand.clear();
		LeaveCriticalSection(&ccAccess);
		return -1;
	}
    EnterCriticalSection(&ccAccess);
	currentCommand += s;
	_hasCommand = true;
	LeaveCriticalSection(&ccAccess);
  }
}
#endif

void CliScreen::init(std::string version)
{
#ifdef WIN32
	InitializeCriticalSection(&ccAccess);
	stdinThread = CreateThread(NULL, 0, _stdinThreadProc, (void *)this, 0,
		NULL);
#endif
}

void CliScreen::end()
{
#ifdef WIN32
	TerminateThread(stdinThread, 0);
	DeleteCriticalSection(&ccAccess);
#endif
}

void CliScreen::log(LogType::LogType type, const std::string& source, const std::string& message)
{
	std::cout << "[" << currentTimestamp(true) << "] " << source << ": "
		<< message << std::endl;
}

void CliScreen::updatePlayerList(std::vector<User *> users)
{
}

bool CliScreen::hasCommand()
{
#ifdef WIN32
  return _hasCommand;
#else
  char readchar;
  pollfd stdinfd[1];

  while (true)
  {
	stdinfd[0].fd = fileno(stdin);
    stdinfd[0].events = POLLIN;
    if (!poll(stdinfd, 1, 0))
      return false;
    if (read(STDIN_FILENO, &readchar, 1) == -1)
      return false;
	if (readchar == '\n')
	  return true;
	else
	  currentCommand += readchar;
  }
#endif
}

std::string CliScreen::getCommand()
{
  std::string command;
#ifdef WIN32
  EnterCriticalSection(&ccAccess);
  //Get first line from currentCommand, and remove it
  std::istringstream is;
  is.str(currentCommand);
  std::getline(is, command);
  currentCommand = is.str();
#else
  command = currentCommand;
#endif
  currentCommand.clear();
#ifdef WIN32
  _hasCommand = currentCommand.empty();
  LeaveCriticalSection(&ccAccess);
#endif
  return command;
}
