#pragma once
#include <yaml-cpp/yaml.h>

class CSettings
{
public:
	CSettings();
	CSettings(const CSettings & rhs);
	CSettings & operator = (const CSettings & rhs);
	~CSettings();

	void saveSettings(YAML::Node & settings);
	int loadSettings(YAML::Node & settings);
	
	std::string getCircuitChannelType();
	int setCircuitChannelType(std::string val);

private:
	std::string circuitChannelType_;

	std::string ftpwmohost_;
	std::string user_;
	std::string password_;

};

