#include "EvoIniParser.h"
#include <fstream>

#include <Windows.h>  // for dbgout
#include <tchar.h>	  // for dbgout
#include "DbgOut_C.h"

EvoIniParser::EvoIniParser()
{
	m_filename = EVO_INITIALISATION_FILE;
	ReParse();
}

EvoIniParser::EvoIniParser(std::string filename)
{
	m_filename = filename;
	ReParse();
}

std::string EvoIniParser::GetValue(std::string section, std::string key)
{
	std::string retval;
	if (!inipp::get_value(m_inipp.sections[section.c_str()], key.c_str(), retval)) {
		DBGOUT("EvoIniParser: Entry not found. Section = %s, Key = %s\n", section.c_str(), key.c_str());
	}
	return retval;
}

void EvoIniParser::SetValue(std::string section, std::string key, std::string value)
{
	auto& graphics_section = m_inipp.sections[section];
	graphics_section.insert(std::pair<std::string, std::string>(key, value));
}

void EvoIniParser::LoadStandardValues()
{
	m_inipp.clear();
	auto& graphics_section = m_inipp.sections["Graphics"];
	graphics_section.insert(std::make_pair<std::string, std::string>("VSync", "0"));
	graphics_section.insert(std::make_pair<std::string, std::string>("AntiAliasLv", "1"));
	graphics_section.insert(std::make_pair<std::string, std::string>("AntiAliasQuality", "3"));
	graphics_section.insert(std::make_pair<std::string, std::string>("TexFilterType", "3"));
	graphics_section.insert(std::make_pair<std::string, std::string>("AnisothropicLv", "16"));
	graphics_section.insert(std::make_pair<std::string, std::string>("DisableParallel", "1"));
}

void EvoIniParser::UpdateIni()
{
	std::fstream ini_fstream;
	ini_fstream.open(m_filename.c_str(), std::fstream::out);
	m_inipp.generate(ini_fstream);
	ini_fstream.close();
}

void EvoIniParser::ReParse()
{
	std::fstream ini_fstream;
	ini_fstream.open(m_filename.c_str(), std::fstream::in);
	if (ini_fstream.is_open())
	{
		m_inipp.clear();
		m_inipp.parse(ini_fstream);
		ini_fstream.close();
	}
	else
	{
		LoadStandardValues();
		UpdateIni();
	}
}
