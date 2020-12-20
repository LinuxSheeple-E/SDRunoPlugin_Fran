#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <sstream>
#include <chrono>
#include <Windows.h>
#include "csvlib/csv.h"

#include "SDRunoPlugin_Fran.h"
#include "SDRunoPlugin_FranUi.h"

constexpr int FONT_HEIGHT = 18; // The height of the font used by SDRuno for annotations (experimentally derived)

// There is only one "database" so make it static 
static std::vector<struct SWSKEDSRecord> stationRecords;
static std::vector<struct SWSKEDSRecord>::iterator vfoPtr;
static std::vector<std::string> sourceList;
static std::string source;

static long long vfo_frequency = 0LL;
static long long lower_limit = 0LL;
static long long upper_limit = 0LL;

struct sSP1Params SP1Params;

static bool freq_compare(const SWSKEDSRecord &a, const SWSKEDSRecord &b)
{
	return (a.frequency < b.frequency);
}

SDRunoPlugin_Fran::SDRunoPlugin_Fran(IUnoPluginController& controller) :
	IUnoPlugin(controller),
	m_form(*this, controller),
	m_worker(nullptr)
{
	GetAppDirectory();
	if (!m_AppDir.empty())
	{
		m_IniFile = m_AppDir;
		m_IniFile /= L"SDRuno.ini";
		GetIniParameters();
	}
	m_controller.RegisterAnnotator(this);
}

SDRunoPlugin_Fran::~SDRunoPlugin_Fran()
{
	m_controller.UnregisterAnnotator(this);

}

void SDRunoPlugin_Fran::HandleEvent(const UnoEvent& ev)
{
	m_form.HandleEvent(ev);	
}

void SDRunoPlugin_Fran::WorkerFunction()
{
	// Worker Function Code Goes Here
}

// This function is called by SDRuno to fetch the frequency annotators
void SDRunoPlugin_Fran::AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items)
{
	// Note: The current limit for Annotators is 64 so go up and down 32 entries from VFO
	IUnoAnnotatorItem ai;
	int i;
	std::vector<struct SWSKEDSRecord>::iterator recPtr;
	std::time_t t = std::time(nullptr);
	std::tm *tm = std::gmtime(&t);
	if (stationRecords.empty() || !valid)
		return;
	// for now just output up to MAX_ANNOTATORS centered on VFO Frequency
	ai.power = SP1Params.yMinALimit;  // For now just space things out
	i = 0;
	recPtr = vfoPtr;
	if (recPtr > stationRecords.begin())
	{
		recPtr--;
		while ((i < (MAX_ANNOTATORS / 2)) && (recPtr >= stationRecords.begin()))
		{
			if (BuildAnnotatorItem(recPtr, ai, tm))
			{
				items.emplace_back(ai);
				ai.power += SP1Params.yIncrement;
				if (ai.power > SP1Params.yMaxALimit)
					ai.power = SP1Params.yMinALimit;
				i++;
			}
			recPtr--;

		}
	}
	ai.power = SP1Params.yMaxALimit;
	recPtr = vfoPtr;
	while ((i < MAX_ANNOTATORS) && (recPtr < stationRecords.end()))
	{
		if (BuildAnnotatorItem(recPtr, ai, tm))
		{
			items.emplace_back(ai);
			ai.power -= SP1Params.yIncrement;
			if (ai.power < SP1Params.yMinALimit)
				ai.power = SP1Params.yMaxALimit;
			i++;
		}
		++recPtr;

	}
}

std::string & SDRunoPlugin_Fran::loadSwSkedsCsvFile(nana::filebox::path_type file)
{
	struct SWSKEDSRecord sr;
	valid = false;
	io::CSVReader<16, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>> inFile(file.generic_string().c_str());
	static std::string result;
	result.clear();
	try {
		inFile.read_header(io::ignore_extra_column, "Frequency", "M", "Station", "On", "Off", "Language", "Site", "TX_Country", "Days", "Target", "Notes", "Pwr", "Azi", "Org_Country", "Source", "Date");
		while (inFile.read_row(sr.frequency, sr.mode, sr.station, sr.on, sr.off, sr.language, sr.site, sr.tx_country, sr.days, sr.target, sr.notes, sr.power, sr.az, sr.org_country, sr.source, sr.date))
		{
			sr.station = sr.station.substr(0, 63); // SDRuno annotate text has a limit of 63 characters.
			if (sr.off == 0) // Fix blank records or in cases where 0000 is used instead of 2400
				sr.off = 2400;
			stationRecords.emplace_back(sr);
		}
	}
	// Uncaught exceptions and popup message boxes can cause SDRuno hangs.
	catch (std::exception& e)
	{
		result = "SWSKEDS File Error: " + std::string(e.what()) + "\n";
		return result;
	}
	if (!stationRecords.empty())
	{
		vfoPtr = stationRecords.begin();
		vfo_frequency = vfoPtr->frequency;
		valid = !stationRecords.empty();
	}
	result = "SWSKEDS File " + file.stem().string() + " read " + std::to_string(stationRecords.size()) + " records \n";
	return result;
}

std::string & SDRunoPlugin_Fran::loadS1bCsvFile(nana::filebox::path_type file)
{
	struct SWSKEDSRecord sr;
	std::string scanStr, utcStr, subMStr, filterStr, portStr;
	valid = false;
	io::CSVReader<8, io::trim_chars<' '>, io::double_quote_escape<',', '\"'>> inFile(file.generic_string().c_str());
	static std::string result;
	result.clear();
	try {
		inFile.set_header("Frequency", "Scan", "M", "Station", "UTC", "SubM", "Filter", "Port");
		while (inFile.read_row(sr.frequency, scanStr, sr.mode, sr.station, utcStr, subMStr, filterStr, portStr))
		{
			sr.station = sr.station.substr(0, 63); // SDRuno annotate text has a limit of 63 characters.
			sr.on = 0;
			sr.off = 2400;
			sr.days = "smtwtfs";
			sr.source = file.stem().string();
			stationRecords.emplace_back(sr);
		}
	}
	// Uncaught exceptions and popup message boxes can cause SDRuno hangs.
	catch (std::exception& e)
	{
		result = "Bank File " + file.stem().string() + " Error: " + std::string(e.what()) + "\n";
		return result;
	}
	if (!stationRecords.empty())
	{
		vfoPtr = stationRecords.begin();
		vfo_frequency = vfoPtr->frequency;
		valid = !stationRecords.empty();
	}
	result = "Bank File " + file.stem().string() + " read " + std::to_string(stationRecords.size()) + " records \n";
	return result;
}

void SDRunoPlugin_Fran::CalculateDisplayFactors()
{
	SP1Params.yIncrement = (((SP1Params.maxPower - SP1Params.minPower) * FONT_HEIGHT) + SP1Params.ySpectrumSize) / (SP1Params.ySpectrumSize);
	SP1Params.yMinALimit = SP1Params.minPower + (SP1Params.yIncrement * 2);
	SP1Params.yMaxALimit = SP1Params.maxPower - SP1Params.yIncrement;
}

void SDRunoPlugin_Fran::CalculateLimits()
{
	long long ll = static_cast<long long>(SP1Params.vfoFreq);
	lower_limit = static_cast<long long>(SP1Params.minFreq);  // Possible future use
	upper_limit = static_cast<long long>(SP1Params.maxFreq);  // Possible future use
	if ((ll > (vfo_frequency + 2500LL)) || (ll < (vfo_frequency - 2500LL)))
	{
		vfo_frequency = ll;
		if (stationRecords.empty())
			return; // nothing to do
		if (stationRecords.size() <= MAX_ANNOTATORS)
		{
			vfoPtr = stationRecords.begin() + (stationRecords.size() / 2);
		}
		else if ((vfo_frequency > vfoPtr->frequency) && ((vfoPtr + 1) != stationRecords.end()))
		{
			vfoPtr++;
			while ((vfo_frequency > vfoPtr->frequency) && ((vfoPtr + 1) != stationRecords.end()))
				vfoPtr++;
			if (vfoPtr != stationRecords.begin())
				vfoPtr--;


		}
		else if ((vfo_frequency < vfoPtr->frequency) && (vfoPtr != stationRecords.begin()))
		{
			vfoPtr--;
			while ((vfo_frequency <= vfoPtr->frequency) && (vfoPtr != stationRecords.begin()))
				vfoPtr--;
			if ((vfoPtr + 1) != stationRecords.end())
				vfoPtr++;
		}
	}

}

bool SDRunoPlugin_Fran::IsStationActive(struct SWSKEDSRecord &station, short time, std::tm * tmPtr)
{
	// This is complicated since several formats are used for the Days field.
	// smtwtfs varations are the most common with off days marked with .
	// 15th    varations of the day of the month
	// Jun 21  varations of three letter month and day of month
	// 1st Fr  varations of count of dow in the month
	// alt Fr  alternating dow
	// irr     irregular
	// alt     alternate
	// irr Su  irregular with dow
	// Ram     Ramadan (Muslim Holy Month)
	std::string digits("0123456789");
	std::vector<std::string> daysOfWeek{ "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };
	std::vector<std::string>  months{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	std::tm tm = *tmPtr; // Copy the structure in case we need to adjust it for this comparison
	size_t pos;
	unsigned int ui;
	// Check the time first
	if (station.on != 0 || station.off != 2400)
	{
		if (station.on > station.off)
		{
			if (time < station.on && time > station.off)
				return false;
			if (time <= station.off)
			{
				// The broadcast began yesterday, so leave messy boundary calculations to mktime
				--tm.tm_mday;
				std::mktime(&tm);
			}
		}
		else
		{
			if (time < station.on || time > station.off)
				return false;
		}
	}
	// A number is either day of month or count of dow
	if ((pos = station.days.find_first_of(digits, 0)) != std::string::npos) // TODO: Add npos where needed
	{
		int num;
		try {
			num = std::stoi(station.days.substr(pos));
		}
		catch (...)
		{
			return true;
		}
		if (num < 1 || num > 31)
			return true;  // a new format perhaps?
		for (ui = 0; ui < daysOfWeek.size(); ui++)
		{
			if (station.days.find(daysOfWeek.at(ui)) != std::string::npos)
			{
				if (num < 1 || num > 4)
					return true;  // a new format perhaps?
				if ((ui == tm.tm_wday) && (tm.tm_mday > ((num - 1) * 7)) && (tm.tm_mday <= (num * 7)))
					return true;
				return false;
			}
		}
		for (ui = 0; ui < months.size(); ui++)
		{
			if (station.days.find(months.at(ui)) != std::string::npos)
			{
				if (ui == tm.tm_mon && num == tm.tm_mday)
					return true;
				return false;
			}
		}
		if (num == tm.tm_mday)  // Assume it is day of month
			return true;
		return false;
	}
	for (ui = 0; ui < daysOfWeek.size(); ui++)  // Just do the days of the week
	{
		if (station.days.find(daysOfWeek.at(ui)) != std::string::npos)
		{
			if (ui == tm.tm_wday)
				return true;
			return false;
		}
	}
	// If we do not have alt or irr without a day of the week and the dow position is . then return false
	if ((station.days.find_first_of("aAiI") == std::string::npos) && (station.days.length() >= 7) && !isalpha(station.days.at(tm.tm_wday)))
		return false;
	return true;
}

void SDRunoPlugin_Fran::DeleteStations()
{
	stationRecords.clear();
}
void SDRunoPlugin_Fran::DeleteSources()
{
	sourceList.clear();
}
std::vector<std::string> & SDRunoPlugin_Fran::GetSources()
{
	return sourceList;
}

void SDRunoPlugin_Fran::SetSource(std::string & s)
{
	if (!s.compare("ALL"))
		source.clear();
	else
		source = s;
}

void SDRunoPlugin_Fran::StationFrequencySort()
{
	std::stable_sort(stationRecords.begin(), stationRecords.end(), freq_compare);
}

void SDRunoPlugin_Fran::EnumerateSources()
{
	for (auto & station : stationRecords)
	{
		if (!station.source.empty() && (FindString(station.source, sourceList) == sourceList.end()))
			sourceList.emplace_back(station.source);
	}
	std::stable_sort(sourceList.begin(), sourceList.end());
}

std::vector<std::string>::iterator SDRunoPlugin_Fran::FindString(std::string & s, std::vector<std::string> & list)
{
	std::vector<std::string>::iterator iter;
	for (iter = list.begin(); iter < list.end(); ++iter)
	{
		if (*iter == s)
			break;
	}
	return iter;
}
bool SDRunoPlugin_Fran::BuildAnnotatorItem(std::vector<struct SWSKEDSRecord>::iterator recPtr, IUnoAnnotatorItem & ai, std::tm *tm)
{
	short curTime = tm->tm_hour * 100 + tm->tm_min;
	if ((source.empty() || recPtr->source.empty() || (source == recPtr->source)) && IsStationActive(*recPtr, curTime, tm))
	{
		ai.frequency = recPtr->frequency;
		ai.style = AnnotatorStyleMarker;
		// A simple color the languages scheme
		if (!recPtr->language.compare("English"))
			ai.rgb = 0x00c000c0;
		else if (!recPtr->language.compare("Mandarin"))
			ai.rgb = 0xffffb000;
		else if (!recPtr->language.compare("Chinese"))
			ai.rgb = 0xffff8c00;
		else if (!recPtr->language.compare("Japanese"))
			ai.rgb = 0xffff0000;
		else if (!recPtr->language.compare("Spanish"))
			ai.rgb = 0xffffff00;
		else if (!recPtr->language.compare("French"))
			ai.rgb = 0xff00ffff;
		else if (!recPtr->language.compare("Russian"))
			ai.rgb = 0xffc00000;
		else if (!recPtr->language.compare("Arabic"))
			ai.rgb = 0xff00ff00;
		else
			ai.rgb = 0xffffffff;
		ai.text = recPtr->station;
		ai.lineToFreq = 0;
		ai.lineToPower = 0;
		return true;
	}
	return false;

}
// Get the directory where the .ini file is
void SDRunoPlugin_Fran::GetAppDirectory()
{
	DWORD size;
	LPTSTR directoryStr;
	size = GetCurrentDirectory(0, NULL);
	directoryStr = new WCHAR[size];
	if (GetCurrentDirectory(size, directoryStr))
	{
		m_AppDir = std::filesystem::path(directoryStr);
	}
	delete[] directoryStr;
}
 // Get useful information from the main SDRuno ini file and figure out parameters.
 // Note: Currently does not account for user changing or resizing the workspace!
void SDRunoPlugin_Fran::GetIniParameters()
{
	int i, iWorkspace;
	wchar_t buffer[513];
	GetPrivateProfileString(L"Inst0\\Main", L"sPluginDirectory", m_AppDir.c_str(), buffer, 513, m_IniFile.c_str());
	m_PluginDir = buffer;
	GetPrivateProfileString(L"Inst0\\Main", L"sMemoryFilePath", m_AppDir.c_str(), buffer, 513, m_IniFile.c_str());
	m_MemoryFileDir = buffer;
	iWorkspace = GetPrivateProfileInt(L"Inst0\\Main", L"iCurrentWorkspace", 0, m_IniFile.c_str());
	swprintf(buffer, L"Inst0\\VRX0\\Workspace%d", iWorkspace);
	SP1Params.xSize = GetPrivateProfileInt(buffer, L"iSP1PanelWidth", 800, m_IniFile.c_str());
	SP1Params.ySize = GetPrivateProfileInt(buffer, L"iSP1PanelHeight", 600, m_IniFile.c_str());
	GetPrivateProfileString(L"Inst0\\VRX0\\SP1", L"fSpectrumWaterfallDisplayShare", m_AppDir.c_str(), buffer, 513, m_IniFile.c_str());
	SP1Params.waterfallRatio = _wtof(buffer);
	i = GetPrivateProfileInt(L"Inst0\\VRX0\\SP1", L"eSP1PanelMode", 0, m_IniFile.c_str());
	if(i == 0) // SP+WF
		SP1Params.ySpectrumSize = static_cast<int>(ceil(static_cast<double>(SP1Params.ySize) * SP1Params.waterfallRatio));
	else // either no spectrum or full spectrum
		SP1Params.ySpectrumSize = SP1Params.ySize;
	i = GetPrivateProfileInt(L"Inst0\\VRX0\\SP1", L"iSpectrumBase", 50, m_IniFile.c_str());
	SP1Params.minPower = (i == 0) ? -160 :-160 + (i*4/5);
	SP1Params.maxPower = SP1Params.minPower + 10 + (200 - GetPrivateProfileInt(L"Inst0\\VRX0\\SP1", L"iSpectrumRange", 50, m_IniFile.c_str())) * 3 / 4;
	CalculateDisplayFactors();
}
std::filesystem::path SDRunoPlugin_Fran::GetPluginDir()
{
	return m_PluginDir;
}
std::filesystem::path SDRunoPlugin_Fran::GetMemoryFileDir()
{
	return m_MemoryFileDir;
}
