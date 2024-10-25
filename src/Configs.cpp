#include "Configs.h"
#include "SimpleIni.h"
#include <fstream>
#include <nlohmann/json.hpp>

uint32_t Configs::keyFireHook = 0;
float Configs::maxSpeed = 400.f;
float Configs::maxZSpeed = 70.f;
float Configs::maxVelocity = 300.f;
float Configs::maxDistance = 3000.f;
float Configs::detachDistance = 150.f;
float Configs::detachSeconds = 5.f;
float Configs::hookCooldown = 3.f;
float Configs::ropeThickness = 2.f;
uint32_t Configs::ropeType = 0;
uint32_t Configs::ropeAttachPoint = 0;

void Configs::LoadConfigs()
{
	std::string path = "Data\\MCM\\Config\\GrapplingHook\\settings.ini";
	if (std::filesystem::exists("Data\\MCM\\Settings\\GrapplingHook.ini")) {
		path = "Data\\MCM\\Settings\\GrapplingHook.ini";
	}
	CSimpleIniA ini(true, false, false);
	SI_Error result = ini.LoadFile(path.c_str());
	if (result >= 0) {
		maxSpeed = std::stof(ini.GetValue("Main", "fMaxSpeed", "400.0"));
		maxZSpeed = std::stof(ini.GetValue("Main", "fMaxZSpeed", "70.0"));
		maxVelocity = std::stof(ini.GetValue("Main", "fMaxVelocity", "300.0"));
		maxDistance = std::stof(ini.GetValue("Main", "fMaxDistance", "3000.0"));
		detachDistance = std::stof(ini.GetValue("Main", "fDetachDistance", "150.0"));
		detachSeconds = std::stof(ini.GetValue("Main", "fDetachSeconds", "5.0"));
		hookCooldown = std::stof(ini.GetValue("Main", "fCooldown", "3.0"));
		ropeThickness = std::stof(ini.GetValue("Main", "fRopeThickness", "2.0"));
		ropeType = std::stoi(ini.GetValue("Main", "iRopeType", "0"));
		ropeAttachPoint = std::stoi(ini.GetValue("Main", "iAttachPoint", "0"));
	}
	ini.Reset();
	std::string hotkeyPath = "Data\\MCM\\Settings\\Keybinds.json";
	if (std::filesystem::exists(hotkeyPath)) {
		std::ifstream reader;
		reader.open(hotkeyPath);
		nlohmann::json keyBinds;
		reader >> keyBinds;
		reader.close();
		if (keyBinds.contains("keybinds")) {
			for (auto& key : keyBinds["keybinds"]) {
				try {
					if (key["id"].get<std::string>() == "keyFireHook") {
						keyFireHook = key["keycode"].get<int>();
					}
				} catch (...) {
					continue;
				}
			}
		}
	}
}
