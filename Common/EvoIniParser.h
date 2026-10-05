#pragma once
#include "Inipp.h"
#include <string>

#define EVO_INITIALISATION_FILE "EvoConfig.ini"

class EvoIniParser
{
public:
	EvoIniParser();
	EvoIniParser(std::string filename);

	void ReParse();
	std::string GetValue(std::string section, std::string key);		
	void SetValue(std::string section, std::string key, std::string value);

	void LoadStandardValues();
	void UpdateIni();

	

private:
	std::string m_filename;
	inipp::Ini<char> m_inipp;
};

