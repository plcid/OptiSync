#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <codecvt>
#include <string>
#include <locale>
#include <iostream>
#include "nvapi.h"
#include "NvApiDriverSettings.h"

struct NvAPI_Session {
	NvDRSSessionHandle handle;
	NvDRSProfileHandle profile;
};

struct NvAPI_SettingRequest {
	NvU32 uid;
	NvU32 val;
};

namespace nvw {
	inline NvAPI_Session* session;

	NvAPI_Status init();
	NvAPI_Status exit();
	NvAPI_Status getSetting(NvU32 id, NVDRS_SETTING* outSetting);
	NvAPI_Status setSetting(NvAPI_SettingRequest nvsrq);
	const char*	 getStatusMessage(NvAPI_Status status);
	const char*  NvUnicodeStrToCStr(NvAPI_UnicodeString& unistr);
}

#define checkSafe(status) \
			if (status != NVAPI_OK) { \
				nvw::exit(); \
				return -1; \
			}