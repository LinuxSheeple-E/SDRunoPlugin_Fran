#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunoannotator.h>

#include "SDRunoPlugin_FranUi.h"

#if UNOPLUGINAPIVERSION > 2
#error "Unsupported Plugin Version - Update Code"
#endif

#if UNOPLUGINAPIVERSION < 2
#pragma message ("WARNING: Reduced Functionality - Update Plugin API")
#endif

// Note: SP1 Window Size (max/min frequency and max/min power) is only directly provided in API Version 2 and above
struct sSP1Params {
	double minFreq;
	double maxFreq;
	double centerFreq;
	double sampleRate;
	double vfoFreq;
	int minPower;
	int maxPower;
	// Read from the main inifile
	int xSize;
	int ySize;
	double waterfallRatio;
	// Calculated
	int ySpectrumSize;
	int yMinALimit;
	int yMaxALimit;
	int yIncrement;
};

extern struct sSP1Params SP1Params;

constexpr auto MAX_ANNOTATORS = 64;  // SDRuno only supports 64 annotators

/* CSV Format is geared toward using SWSKEDS group at groups.io for source
Frequency, M, Station, On, Off, Language, Site, TX_Country, Days, Target, Notes, Pwr, Azi, Org_Country, Source, Date
Note: several fields are not being currently used
*/

struct SWSKEDSRecord {
	long long frequency;
	std::string mode;
	std::string station;
	short on;
	short off;
	std::string language;
	std::string site;
	std::string tx_country;
	std::string days;
	std::string target;
	std::string notes;
	float power;
	std::string az;
	std::string org_country;
	std::string source;
	std::string date;
	// Added for date ranges used in ILG Radio files
	long on_date;  // In YYMMDD or MMDD format
	long off_date;  // In YYMMDD or MMDD format
};

class SDRunoPlugin_Fran : public IUnoPlugin, public IUnoAnnotator
{

public:
	
	SDRunoPlugin_Fran(IUnoPluginController& controller);
	virtual ~SDRunoPlugin_Fran();

	virtual const char* GetPluginName() const override { return "SDRuno Frequency Annotate Plugin"; }

	// IUnoPlugin
	virtual void HandleEvent(const UnoEvent& ev) override;

	// IUnoAnnotator
	void AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items) override;

	std::string & loadSwSkedsCsvFile(nana::filebox::path_type file);
	std::string & loadILGTxtFile(nana::filebox::path_type file);
	std::string & loadS1bCsvFile(nana::filebox::path_type file);
	void CalculateDisplayFactors();
	void CalculateLimits();
	void DeleteStations();
	void DeleteSources();
	void SetSource(const std::string & s);

	std::vector<std::string> & GetSources();

	void StationFrequencySort();
	void EnumerateSources();
	std::filesystem::path GetPluginDir();
	std::filesystem::path GetMemoryFileDir();

private:
	
	void WorkerFunction();
	std::thread* m_worker;
	std::mutex m_lock;
	SDRunoPlugin_FranUi m_form;

	bool BuildAnnotatorItem(std::vector<struct SWSKEDSRecord>::iterator recPtr, IUnoAnnotatorItem & ai, std::tm *tm);
	bool IsStationActive(struct SWSKEDSRecord &station, short time, std::tm * tmPtr);
	std::vector<std::string>::iterator FindString(std::string & s, std::vector<std::string> & list);
	void GetAppDirectory();
	void GetCurDirectory();
	int  GetColumnCount(nana::filebox::path_type file);


	bool valid = false;
	std::filesystem::path m_AppDir;
	std::filesystem::path m_CurDir;
	std::filesystem::path m_PluginDir;
	std::filesystem::path m_MemoryFileDir;
	std::filesystem::path m_IniFile;


};
