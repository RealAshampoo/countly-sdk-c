#include "countly.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>

#include <countly_c.h>


#define COUNTLY_C_GUARD_BEGIN try {
#define COUNTLY_C_GUARD_END } \
	catch(std::exception& e) { \
		return COUNTLY_C_ERROR_UNKNOWN; \
	} \
	return COUNTLY_C_OK;


extern "C" int countly_c_init(
	const char* serverHost,
	int serverPort,
	const char* appKey,
	const char* appVersion,
	int updateIntervalSeconds
) {
	COUNTLY_C_GUARD_BEGIN

	Countly& ct = Countly::getInstance();

#ifdef WIN32
	OSVERSIONINFOEX info;
	memset(&info, 0, sizeof(info));
	info.dwOSVersionInfoSize = sizeof(info);

	::GetVersionEx((OSVERSIONINFO*)&info);

	std::string os;

	std::stringstream osVersionStream;
	osVersionStream << info.dwMajorVersion << "." << info.dwMinorVersion;
	std::string osVersion = osVersionStream.str();

	std::stringstream resolutionStream;
	resolutionStream << ::GetSystemMetrics(SM_CXSCREEN) << "x" << ::GetSystemMetrics(SM_CYSCREEN);
	std::string resolution = resolutionStream.str();

	ct.SetMetrics("Windows", osVersion, "", resolution, "", appVersion);

#else
#error OS not supported

#endif

	ct.SetMaxEventsPerMessage(40);
	ct.SetMinUpdatePeriod(updateIntervalSeconds*1000);

	std::string serverProtocolAndHost = std::string("https://")+serverHost;
	ct.Start(appKey, serverProtocolAndHost, serverPort);

	COUNTLY_C_GUARD_END
}

void initSegMap(
	std::map<std::string, std::string>& segMap,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount) {

	for(int i=0; i<segmentationParamCount; i++) {
		segMap[ segmentationParams[i].key ] = segmentationParams[i].value;
	}
}


int countly_c_recordEvent(
	const char* eventName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount,
	int eventCount,
	double eventAmount
) {
	COUNTLY_C_GUARD_BEGIN

	std::map<std::string, std::string> segMap;
	initSegMap(segMap, segmentationParams, segmentationParamCount);
	
	Countly& ct = Countly::getInstance();
	ct.RecordEvent( eventName, segMap, eventCount, eventAmount);

	COUNTLY_C_GUARD_END
}

int countly_c_recordView(
	const char* viewName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount
) {
	COUNTLY_C_GUARD_BEGIN

	std::map<std::string, std::string> segMap;
	initSegMap(segMap, segmentationParams, segmentationParamCount);

	segMap["name"] = viewName;
	
	Countly& ct = Countly::getInstance();
	ct.RecordEvent( "[CLY]_view", segMap, 1, 0);

	COUNTLY_C_GUARD_END
}


