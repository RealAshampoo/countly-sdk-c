#include "countly.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>
#include <iostream>

#include <countly_c.h>

void onException(std::exception& e) {
	std::cerr << std::endl << "Countly internal exception: " << e.what() << std::endl;
	std::cerr.flush();
}


#define COUNTLY_C_GUARD_BEGIN try {
#define COUNTLY_C_GUARD_END } \
	catch(std::exception& e) { \
		onException(e); \
		return COUNTLY_C_GENERIC_ERROR; \
	} \
	return COUNTLY_C_OK;


struct CountlyCContext {
	std::string serverHost;
	int 	    serverPort=0;
	std::string appKey;

	bool	    started=false;

	static CountlyCContext& get() {
		static CountlyCContext context;
		return context;
	}
};



extern "C" int countly_c_init(
	const char* serverHost,
	int serverPort,
	const char* appKey,
	const char* appVersion,
	const char* stateFilePath
) {
	COUNTLY_C_GUARD_BEGIN

	Countly& ct = Countly::getInstance();

	ct.SetMaxEventsPerMessage(40);
	ct.SetPath(stateFilePath);

	CountlyCContext& context = CountlyCContext::get();
	context.serverHost = serverHost;
	context.serverPort = serverPort;
	context.appKey = appKey;

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

	COUNTLY_C_GUARD_END
}

extern "C" int countly_c_setDeviceID(const char* deviceID) {
	COUNTLY_C_GUARD_BEGIN

	Countly::getInstance().setDeviceID(deviceID);

	COUNTLY_C_GUARD_END
}

extern "C" int countly_c_setFlushIntervalSeconds(int seconds) {
	COUNTLY_C_GUARD_BEGIN

	Countly::getInstance().SetMinUpdatePeriod(seconds*1000);

	COUNTLY_C_GUARD_END
}


extern "C" int countly_c_start() {
	COUNTLY_C_GUARD_BEGIN

	CountlyCContext& context = CountlyCContext::get();
	std::string serverProtocolAndHost = std::string("https://")+context.serverHost;

	Countly::getInstance().Start(context.appKey, serverProtocolAndHost, context.serverPort);

	context.started = true;

	COUNTLY_C_GUARD_END
}


void initEventSegmentation(
	Countly::Event& ev,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount) {

	for(int i=0; i<segmentationParamCount; i++) {
		ev.addSegmentation(segmentationParams[i].key, std::string(segmentationParams[i].value) );
	}
}


extern "C" int countly_c_recordEvent(
	const char* eventName,
	const char* screenName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount,
	int eventCount,
	double eventAmount
) {
	COUNTLY_C_GUARD_BEGIN

	if(! CountlyCContext::get().started)
		return COUNTLY_C_NOT_STARTED;


	Countly::Event ev(eventName, eventCount, eventAmount);
	initEventSegmentation(ev, segmentationParams, segmentationParamCount);
	if(screenName!=NULL && screenName[0]!=0) {
		ev.addSegmentation("view", std::string(screenName));
	}

	ev.setTimestamp();
	
	Countly::getInstance().addEvent(ev);

	COUNTLY_C_GUARD_END
}

extern "C" int countly_c_recordScreenView(
	const char* screenName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount
) {
	COUNTLY_C_GUARD_BEGIN

	if(! CountlyCContext::get().started)
		return COUNTLY_C_NOT_STARTED;

	Countly::Event ev("[CLY]_view");
	initEventSegmentation(ev, segmentationParams, segmentationParamCount);
	ev.addSegmentation("name", std::string(screenName));

	ev.setTimestamp();

	Countly::getInstance().addEvent(ev);

	COUNTLY_C_GUARD_END
}


extern "C" int countly_c_flush() {
	COUNTLY_C_GUARD_BEGIN

	if(! CountlyCContext::get().started)
		return COUNTLY_C_NOT_STARTED;
	
	Countly::getInstance().updateSession();

	COUNTLY_C_GUARD_END
}


extern "C" int countly_c_end() {
	COUNTLY_C_GUARD_BEGIN

	Countly::getInstance().stop();

	COUNTLY_C_GUARD_END
}


