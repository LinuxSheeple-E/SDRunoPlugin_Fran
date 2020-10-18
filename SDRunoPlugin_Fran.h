#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunoannotator.h>

#include "SDRunoPlugin_FranUi.h"

constexpr auto MAX_ANNOTATORS = 64;  // For now SDRuno only supports 64 annotators

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
	void AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items);

	std::string & loadSwSkedsCsvFile(nana::filebox::path_type file);
	std::string & loadS1bCsvFile(nana::filebox::path_type file);
	void CalculateLimits(double vfoFreq, double centerFreq, double sampleRate);
	void DeleteStations();
	void DeleteSources();
	void SetSource(std::string & s);

	std::vector<std::string> & GetSources();

	void StationFrequencySort();
	void EnumerateSources();

private:
	
	void WorkerFunction();
	std::thread* m_worker;
	std::mutex m_lock;
	SDRunoPlugin_FranUi m_form;

	bool SDRunoPlugin_Fran::BuildAnnotatorItem(std::vector<struct SWSKEDSRecord>::iterator recPtr, IUnoAnnotatorItem & ai, std::tm *tm);
	bool IsStationActive(struct SWSKEDSRecord &station, short time, std::tm * tmPtr);
	std::vector<std::string>::iterator FindString(std::string & s, std::vector<std::string> & list);

	bool valid = false;

};