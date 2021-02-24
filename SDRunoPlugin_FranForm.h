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

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/dragger.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <iunoplugincontroller.h>

// Shouldn't need to change these
#define topBarHeight (27)
#define bottomBarHeight (8)
#define sideBorderWidth (8)

// TODO: Change these numbers to the height and width of your form
#define formWidth (297)
#define formHeight (240)

class SDRunoPlugin_FranUi;

class SDRunoPlugin_FranForm : public nana::form
{

public:

	SDRunoPlugin_FranForm(SDRunoPlugin_FranUi& parent, IUnoPluginController& controller);
	~SDRunoPlugin_FranForm();
	
	void Run();
	void ShowStatus(std::string msg);
	void LoadSources(std::vector<std::string> &sources);

private:

	void Setup();

	// Set these two to be relative to the size of the overall form
	nana::picture bg_border{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	// Added topBarHeight to accomodate new header style
	nana::picture bg_inner{ bg_border, nana::rectangle(sideBorderWidth, topBarHeight, formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight) };
	nana::picture header_bar{ *this, true };
	nana::label title_bar_label{ *this, true };
	nana::dragger form_dragger;
	// Add an "invisible" label the same size as the form to act as drag trigger for form
	nana::label form_drag_label{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	// Add images to hold bitmaps for button states...
	nana::paint::image img_min_normal;
	nana::paint::image img_min_down;
	nana::paint::image img_close_normal;
	nana::paint::image img_close_down;
	nana::paint::image img_header;
	// Add pictures to act as header buttons...
	nana::picture close_button{ *this, nana::rectangle(8, 25, 20, 15) };
	nana::picture min_button{ *this, nana::rectangle(8, 25, 20, 15) };

	nana::button btn_file{*this, nana::rectangle(18, 37, 50, 30)};
	nana::label  lbl_status{ *this,  nana::rectangle(20,140, 260, 60) };
	nana::label  lbl_version{ *this,  nana::rectangle(formWidth - 40, formHeight - 30, 30, 20) };
	nana::label  lbl_sources{ *this, nana::rectangle(18, 82, 70, 20) };
    nana::combox cmb_sources{*this, nana::rectangle(90, 80, 180, 20) };

	SDRunoPlugin_FranUi & m_parent;
	IUnoPluginController & m_controller;

	void pick_file(bool is_open);
	void pick_source(const std::string & source);

	std::vector< nana::filebox::path_type > m_files;
};
