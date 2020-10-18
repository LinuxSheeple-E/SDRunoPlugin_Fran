/****************************************************************************
**
** Portions are Copyright (C) 2020 Eric A. Cottrell
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
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_FranForm.h"
#include "SDRunoPlugin_FranUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>

// Form constructor with handles to parent and uno controller - launches form Setup
SDRunoPlugin_FranForm::SDRunoPlugin_FranForm(SDRunoPlugin_FranUi& parent, IUnoPluginController& controller) :
	nana::form(nana::API::make_center(300, 200), nana::appearance(true, true, true, false, false, false, false)),
	m_parent(parent),
	m_controller(controller)
{
	Setup();
}

// Form deconstructor
SDRunoPlugin_FranForm::~SDRunoPlugin_FranForm()
{

}

// Start Form and start Nana UI processing
void SDRunoPlugin_FranForm::Run()
{
	show();
	nana::exec();
}

// Create the initial plugin form
void SDRunoPlugin_FranForm::Setup()
{
	// This first section is all related to the background and border
	// it shouldn't need to be changed
	nana::paint::image img_border;
	nana::paint::image img_inner;
	HMODULE hModule = NULL;
	HRSRC rc_border = NULL;
	HRSRC rc_inner = NULL;
	HBITMAP bm_border = NULL;
	HBITMAP bm_inner = NULL;
	BITMAPINFO bmInfo_border = { 0 };
	BITMAPINFO bmInfo_inner = { 0 };
	BITMAPFILEHEADER borderHeader = { 0 };
	BITMAPFILEHEADER innerHeader = { 0 };
	HDC hdc = NULL;
	BYTE* borderPixels = NULL;
	BYTE* innerPixels = NULL;
	const unsigned int rawDataOffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

	hModule = GetModuleHandle(L"SDRunoPlugin_Fran");
	hdc = GetDC(NULL);

	rc_border = FindResource(hModule, MAKEINTRESOURCE(IDB_BG_BORDER), RT_BITMAP);
	rc_inner = FindResource(hModule, MAKEINTRESOURCE(IDB_BACKGROUND), RT_BITMAP);

	bm_border = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_BG_BORDER), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	bm_inner = (HBITMAP)LoadImage(hModule, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);

	bmInfo_border.bmiHeader.biSize = sizeof(bmInfo_border.bmiHeader);
	bmInfo_inner.bmiHeader.biSize = sizeof(bmInfo_inner.bmiHeader);

	GetDIBits(hdc, bm_border, 0, 0, NULL, &bmInfo_border, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_inner, 0, 0, NULL, &bmInfo_inner, DIB_RGB_COLORS);

	bmInfo_border.bmiHeader.biCompression = BI_RGB;
	bmInfo_inner.bmiHeader.biCompression = BI_RGB;

	borderHeader.bfOffBits = rawDataOffset;
	borderHeader.bfSize = bmInfo_border.bmiHeader.biSizeImage;
	borderHeader.bfType = 0x4D42;

	innerHeader.bfOffBits = rawDataOffset;
	innerHeader.bfSize = bmInfo_inner.bmiHeader.biSizeImage;
	innerHeader.bfType = 0x4D42;

	borderPixels = new BYTE[bmInfo_border.bmiHeader.biSizeImage + rawDataOffset];
	innerPixels = new BYTE[bmInfo_inner.bmiHeader.biSizeImage + rawDataOffset];

	*(BITMAPFILEHEADER*)borderPixels = borderHeader;
	*(BITMAPINFO*)(borderPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_border;

	*(BITMAPFILEHEADER*)innerPixels = innerHeader;
	*(BITMAPINFO*)(innerPixels + sizeof(BITMAPFILEHEADER)) = bmInfo_inner;

	GetDIBits(hdc, bm_border, 0, bmInfo_border.bmiHeader.biHeight, (LPVOID)(borderPixels + rawDataOffset), &bmInfo_border, DIB_RGB_COLORS);
	GetDIBits(hdc, bm_inner, 0, bmInfo_inner.bmiHeader.biHeight, (LPVOID)(innerPixels + rawDataOffset), &bmInfo_inner, DIB_RGB_COLORS);

	img_border.open(borderPixels, bmInfo_border.bmiHeader.biSizeImage);
	img_inner.open(innerPixels, bmInfo_inner.bmiHeader.biSizeImage);

	ReleaseDC(NULL, hdc);

	bg_border.load(img_border, nana::rectangle(0, 0, 590, 340));
	bg_border.stretchable(0, 0, 0, 0);
	bg_border.transparent(true);
	bg_inner.load(img_inner, nana::rectangle(0, 0, 582, 299));
	bg_inner.stretchable(sideBorderWidth, 0, sideBorderWidth, bottomBarHeight);
	bg_inner.transparent(true);

	btn_file.fgcolor(nana::color_rgb(0x000000));
	btn_file.caption("File");
	btn_file.events().click([&]()
						{
							pick_file(true);
	                    });
	
	lbl_status.bgcolor(nana::color_rgb(0x264b5d));
	lbl_status.fgcolor(nana::color_rgb(0xffffff));

	lbl_version.bgcolor(nana::color_rgb(0x264b5d));
	lbl_version.fgcolor(nana::color_rgb(0xffffff));
	lbl_version.caption(FRAN_VERSION);

	lbl_sources.bgcolor(nana::color_rgb(0x264b5d));
	lbl_sources.fgcolor(nana::color_rgb(0xffffff));
	lbl_sources.caption("Sources");

	cmb_sources.bgcolor(nana::color_rgb(0x264b5d));
	cmb_sources.fgcolor(nana::color_rgb(0x000000));
	cmb_sources.events().selected([&](const nana::arg_combox & ar_cbx) {pick_source(ar_cbx.widget.text(ar_cbx.widget.option()));  });

	// Load X and Y location for the form from the ini file (if exists)
	int posX = m_parent.LoadX();
	int posY = m_parent.LoadY();
	move(posX, posY);

	// This code sets the plugin size, title and what to do when the X is pressed
	size(nana::size(formWidth, formHeight));
	caption("Frequency Annotate");
	events().destroy([&] { m_parent.FormClosed(); });

}
void SDRunoPlugin_FranForm::pick_file(bool is_open)
{
	using namespace nana;
	filebox fbox(*this, is_open);
	fbox.add_filter("SWSkeds", "*.csv");
	fbox.add_filter("Bank", "*.s1b");
	fbox.add_filter("All Files", "*.*");
	fbox.allow_multi_select(true);
	m_files = fbox.show();
	if(!m_files.empty())
		m_parent.ProcessFiles(m_files);
}
void SDRunoPlugin_FranForm::pick_source(std::string & source)
{
	m_parent.SetSource(source);
}

void SDRunoPlugin_FranForm::ShowStatus(std::string msg)
{
	lbl_status.caption(msg);
}
void SDRunoPlugin_FranForm::LoadSources(std::vector<std::string> &sources)
{
	cmb_sources.clear();
	cmb_sources.push_back("ALL");
	for (auto & source : sources)
	{
		cmb_sources.push_back(source);
	}
	cmb_sources.option(0);
}



