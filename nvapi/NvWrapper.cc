#pragma comment(lib, "nvapi64.lib")
#include "NvWrapper.hh"
#include <string>

NvAPI_Status nvw::init() {
	int status = NVAPI_OK;

	session = new NvAPI_Session();

	status ^= (int)NvAPI_Initialize();

	status ^= (int)NvAPI_DRS_CreateSession(&session->handle);

	status ^= (int)NvAPI_DRS_LoadSettings(session->handle);

	status ^= (int)NvAPI_DRS_GetBaseProfile(session->handle, &session->profile);

	return (NvAPI_Status)status;
}

NvAPI_Status nvw::exit() {
	int status = NVAPI_OK;

	status ^= NvAPI_DRS_DestroySession(session->handle);

	status ^= NvAPI_Unload();

	session = nullptr;

	return (NvAPI_Status)status;
}

NvAPI_Status nvw::getSetting(NvU32 id, NVDRS_SETTING* outSetting) {
	int status = NVAPI_OK;

	outSetting->version = NVDRS_SETTING_VER;

	status ^= NvAPI_DRS_GetSetting(session->handle, session->profile, id, outSetting);

	//printf("| %s [ID: 0x%010X]: %s\n",
	//	outSetting->settingName,
	//	outSetting->settingId,
	//	nvw::getStatusMessage((NvAPI_Status)status)
	//);

	return (NvAPI_Status)status;
}

NvAPI_Status nvw::setSetting(NvAPI_SettingRequest nvsrq) {
	int status = NVAPI_OK;

	NVDRS_SETTING setting;
	nvw::getSetting(nvsrq.uid, &setting);
	setting.u32CurrentValue = nvsrq.val;

	status ^= (int)NvAPI_DRS_SetSetting(session->handle, session->profile, &setting);

	status ^= (int)NvAPI_DRS_SaveSettings(session->handle);

	printf("| %s [ID: 0x%010X]: %s\n",
		nvw::NvUnicodeStrToCStr(setting.settingName),
		setting.settingId,
		nvw::getStatusMessage((NvAPI_Status)status)
	);

	return (NvAPI_Status)status;
}

const char* nvw::getStatusMessage(NvAPI_Status status) {
	static NvAPI_ShortString message;
	NvAPI_GetErrorMessage(status, message);
	return (const char*)message;
}

const char* nvw::NvUnicodeStrToCStr(NvAPI_UnicodeString& unicodeString) {
	size_t length = 0;
	while (length < 2048 && unicodeString[length] != 0) {
		++length;
	}
	std::wstring_convert<std::codecvt_utf8_utf16<unsigned short>, unsigned short> converter;
	std::string narrowStr = converter.to_bytes(unicodeString, unicodeString + length);
	char* result = new char[narrowStr.length() + 1];
	std::strcpy(result, narrowStr.c_str());

	return result;
}