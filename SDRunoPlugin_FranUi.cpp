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
#include <sstream>

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_Fran.h"
#include "SDRunoPlugin_FranUi.h"
#include "SDRunoPlugin_FranForm.h"

static constexpr int NUMBER_COLOURS = 9;
static const uint32_t defaultColours[NUMBER_COLOURS] = { 0xffffffff, 0x00c000c0, 0xffffb000, 0xffff8c00, 0xffff0000, 0xffffff00, 0xff00ffff, 0xffc00000, 0xff00ff00 };
static const char * defaultLanguages[NUMBER_COLOURS] = { "`", "English", "Mandarin", "Chinese", "Japanese", "Spanish", "French", "Russian", "Arabic" };

// Ui constructor - load the Ui control into a thread
SDRunoPlugin_FranUi::SDRunoPlugin_FranUi(SDRunoPlugin_Fran& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_form(nullptr),
	m_controller(controller)
{
	m_thread = std::thread(&SDRunoPlugin_FranUi::ShowUi, this);
	m_started = m_controller.IsStreamingEnabled(0);
	SP1Params.centerFreq = m_controller.GetCenterFrequency(0);
	SP1Params.sampleRate = m_controller.GetSampleRate(0);
	SP1Params.vfoFreq = m_controller.GetVfoFrequency(0);
#if UNOPLUGINAPIVERSION == 2
	SP1Params.minFreq = m_controller.GetSP1MinFrequency(0);
	SP1Params.maxFreq = m_controller.GetSP1MaxFrequency(0);
	SP1Params.minPower = m_controller.GetSP1MinPower(0);
	SP1Params.maxPower = m_controller.GetSP1MaxPower(0);
#else
	SP1Params.minFreq = SP1Params.centerFreq - (SP1Params.sampleRate / 2.0);
	SP1Params.maxFreq = SP1Params.centerFreq + (SP1Params.sampleRate / 2.0);
	// minPower and maxPower are initialized by the parent from the SDRuno.ini file
#endif
	m_parent.CalculateLimits();

}

// Ui destructor (the nana::API::exit_all();) is required if using Nana UI library
SDRunoPlugin_FranUi::~SDRunoPlugin_FranUi()
{	
	nana::API::exit_all();
	m_thread.join();
}

// Show and execute the form
void SDRunoPlugin_FranUi::ShowUi()
{	
	m_lock.lock();
	m_form = std::make_shared<SDRunoPlugin_FranForm>(*this, m_controller);
	m_lock.unlock();

	m_form->Run();
}

// Load X from the ini file (if exists)
int SDRunoPlugin_FranUi::LoadX()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Fran.X", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}

// Load Y from the ini file (if exists)
int SDRunoPlugin_FranUi::LoadY()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Fran.Y", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}
// Colours need to be loaded before languages!
void SDRunoPlugin_FranUi::LoadColours()
{
	int i;
	std::string key("Fran.ColourValue"), value;
	languageColours.resize(NUMBER_COLOURS);
	for (i = 0; i < NUMBER_COLOURS; i++)
	{
		m_controller.GetConfigurationKey(key + std::to_string(i), value);
		if (value.empty())
		{
			break;
		}
		languageColours.at(i).rgb = stoul(value, nullptr, 0);
	}
	for (; i < NUMBER_COLOURS; i++)
	{
		languageColours.at(i).rgb = defaultColours[i];
	}
}

// Colours need to be loaded before languages!
void SDRunoPlugin_FranUi::LoadLanguages()
{
	int i, count;
	std::string key("Fran.Language"), value;
	count = languageColours.size();
	m_controller.GetConfigurationKey(key + "Enable", value);
	if (value.empty())
	{
		value = "True";
	}
	if (!value.compare("0") || !value.substr(0, 1).compare("F") || !value.substr(0, 1).compare("f"))
		languageEnable = false;
	else
		languageEnable = true;
	languageColours.at(0).language = defaultLanguages[0];
	for (i = 1; i < count; i++)
	{
		m_controller.GetConfigurationKey(key + std::to_string(i), value);
		if (value.empty())
		{
			break;
		}
		languageColours.at(i).language = value;
	}
	for (; i < count; i++)
	{
		languageColours.at(i).language = defaultLanguages[i];
	}
}

void SDRunoPlugin_FranUi::SaveLocation()
{
	std::lock_guard<std::mutex> l(m_lock);
	nana::point position = m_form->pos();
	m_controller.SetConfigurationKey("Fran.X", std::to_string(position.x));
	m_controller.SetConfigurationKey("Fran.Y", std::to_string(position.y));
}

void SDRunoPlugin_FranUi::SaveColours()
{
	int i, count;
	std::string key("Fran.ColourValue"), value;
	count = languageColours.size();
	for (i = 0; i < count; i++)
	{
		std::stringstream s;
		s << "0x" << std::hex << languageColours.at(i).rgb;
		m_controller.SetConfigurationKey(key + std::to_string(i), s.str());
	}
}

void SDRunoPlugin_FranUi::SaveLanguages()
{
	int i, count;
	std::string key("Fran.Language");
	count = languageColours.size();
	m_controller.SetConfigurationKey(key + "Enable", (languageEnable)?"True":"False");
	for (i = 1; i < count; i++)
	{
		m_controller.SetConfigurationKey(key + std::to_string(i), languageColours.at(i).language);
	}
}

// Handle events from SDRuno
void SDRunoPlugin_FranUi::HandleEvent(const UnoEvent& ev)
{
	switch (ev.GetType())
	{
	case UnoEvent::FrequencyChanged:
		SP1Params.vfoFreq = m_controller.GetVfoFrequency(0);
		m_parent.CalculateLimits();
		break;

	case UnoEvent::CenterFrequencyChanged:
		SP1Params.centerFreq = m_controller.GetCenterFrequency(0);
#if UNOPLUGINAPIVERSION == 1
		SP1Params.minFreq = SP1Params.centerFreq - (SP1Params.sampleRate / 2.0);
		SP1Params.maxFreq = SP1Params.centerFreq + (SP1Params.sampleRate / 2.0);
#endif
		m_parent.CalculateLimits();
		break;

	case UnoEvent::SampleRateChanged:
		SP1Params.sampleRate = m_controller.GetSampleRate(0);
#if UNOPLUGINAPIVERSION == 1
		SP1Params.minFreq = SP1Params.centerFreq - (SP1Params.sampleRate / 2.0);
		SP1Params.maxFreq = SP1Params.centerFreq + (SP1Params.sampleRate / 2.0);
#endif
		m_parent.CalculateLimits();
		break;

	case UnoEvent::StreamingStarted:
		m_started = true;
		break;

	case UnoEvent::StreamingStopped:
		m_started = false;
		break;

	case UnoEvent::SavingWorkspace:
		SaveLocation();
		break;
#if UNOPLUGINAPIVERSION == 2
	case UnoEvent::ClosingDown:  // Actually added in 1.40.1 while keeping plugin at version 1
		SaveColours();
		SaveLanguages();
		break;
	case UnoEvent::SP1MinFreqChanged:
		SP1Params.minFreq = m_controller.GetSP1MinFrequency(0);
		m_parent.CalculateDisplayFactors();
		m_parent.CalculateLimits();
		break;
	case UnoEvent::SP1MaxFreqChanged:
		SP1Params.maxFreq = m_controller.GetSP1MaxFrequency(0);
		m_parent.CalculateDisplayFactors();
		m_parent.CalculateLimits();
		break;
	case UnoEvent::SP1MinPowerChanged:
		SP1Params.minPower = m_controller.GetSP1MinPower(0);
		m_parent.CalculateDisplayFactors();
		m_parent.CalculateLimits();
		break;
	case UnoEvent::SP1MaxPowerChanged:
		SP1Params.maxPower = m_controller.GetSP1MaxPower(0);
		m_parent.CalculateDisplayFactors();
		m_parent.CalculateLimits();
		break;
#endif
	default:
		break;
	}
}
void SDRunoPlugin_FranUi::ProcessFiles(std::vector<nana::filebox::path_type>&  files)
{
	std::string results;
	// Either show number of records or error
	for (auto & file : files)
	{
		if(!file.extension().compare(".s1b") || !file.extension().compare(".S1B"))
			results += m_parent.loadS1bCsvFile(file);
		else if (!file.extension().compare(".txt") || !file.extension().compare(".TXT"))
			results += m_parent.loadILGTxtFile(file);
		else
			results += m_parent.loadSwSkedsCsvFile(file);

	}
	m_form->ShowStatus(results);
	// Make sure class is properly updated
	m_parent.StationFrequencySort(); // Files must be in frequency order
	m_parent.EnumerateSources();
	m_form->LoadSources(m_parent.GetSources());
	m_parent.CalculateLimits();
}

void SDRunoPlugin_FranUi::SetSource(const std::string & source)
{
	m_parent.SetSource(source);
}


// Required to make sure the plugin is correctly unloaded when closed
void SDRunoPlugin_FranUi::FormClosed()
{
	SaveColours();
	SaveLanguages();
	m_controller.RequestUnload(&m_parent);
}
std::filesystem::path SDRunoPlugin_FranUi::GetPluginDir()
{
	return m_parent.GetPluginDir();
}
std::filesystem::path SDRunoPlugin_FranUi::GetMemoryFileDir()
{
	return m_parent.GetMemoryFileDir();
}
