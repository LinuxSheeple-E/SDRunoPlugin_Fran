/****************************************************************************
**
** Copyright (C) 2020 Eric A. Cottrell
** Contact: eric.c.boston@gmail.com
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/
#pragma once

#include <iunoplugin.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>

#include <iunoplugincontroller.h>
#include "SDRunoPlugin_FranForm.h"

// Forward reference
class SDRunoPlugin_Fran;

class SDRunoPlugin_FranUi
{
public:

	SDRunoPlugin_FranUi(SDRunoPlugin_Fran& parent, IUnoPluginController& controller);
	~SDRunoPlugin_FranUi();

	void HandleEvent(const UnoEvent& evt);
	void FormClosed();

	void ShowUi();

	int LoadX();
	int LoadY();
	void ProcessFiles(std::vector <nana::filebox::path_type>& files);
	void SetSource(std::string & source);
	std::filesystem::path GetPluginDir();
	std::filesystem::path GetMemoryFileDir();

private:
	void SaveLocation();
	
	SDRunoPlugin_Fran & m_parent;
	std::thread m_thread;
	std::shared_ptr<SDRunoPlugin_FranForm> m_form;

	bool m_started;

	std::mutex m_lock;

	IUnoPluginController & m_controller;
};
