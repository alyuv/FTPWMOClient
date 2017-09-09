#include "stdafx.h"
#include "Settings.h"


CSettings::CSettings()
{
	ftpwmohost_ = "";
	user_ = "";
	password_ = "";
}

CSettings::CSettings(const CSettings & rhs)
{
	ftpwmohost_ = rhs.ftpwmohost_;
	user_ = rhs.user_;
	password_ = rhs.password_;
}

CSettings & CSettings::operator=(const CSettings & rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	ftpwmohost_ = rhs.ftpwmohost_;
	user_ = rhs.user_;
	password_ = rhs.password_;	
	return *this;
}


CSettings::~CSettings()
{
}

void CSettings::saveSettings(YAML::Node & settings)
{
}

int CSettings::loadSettings(YAML::Node & settings)
{
	return 0;
}

std::string CSettings::getCircuitChannelType()
{
	return circuitChannelType_;
}

int CSettings::setCircuitChannelType(std::string val)
{
	circuitChannelType_ = val;
	return 0;
}
