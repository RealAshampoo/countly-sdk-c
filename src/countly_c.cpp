#include "countly.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include <sstream>
#include <iostream>

#include <countly_c.h>

#undef ERROR

CountlyLogLevel mapCountlyLogLevel(Countly::LogLevel level) {
	switch(level) {
	case Countly::LogLevel::DEBUG:		return CountlyLogLevelDebug;
	case Countly::LogLevel::INFO:		return CountlyLogLevelInfo;
	case Countly::LogLevel::WARNING:	return CountlyLogLevelWarning;
	case Countly::LogLevel::ERROR:		return CountlyLogLevelError;
	case Countly::LogLevel::FATAL:		return CountlyLogLevelFatal;
	default:				return CountlyLogLevelInfo;
	}
}



struct CountlyCContext {
	std::string serverHost;
	int 	    serverPort=0;
	std::string appKey;

	bool	    started=false;

	countly_log_function_t logFunction = nullptr;

	static CountlyCContext& get() {
		static CountlyCContext context;
		return context;
	}
};

/** Default function to relay internal Countly log messages to
 *  the user specific logging function
 */
void onCountlyLog(Countly::LogLevel level, const std::string& message) {
	countly_log_function_t logFunction = CountlyCContext::get().logFunction;
	if (logFunction != nullptr) {
		logFunction(mapCountlyLogLevel(level), message.c_str());
	}
}

void onException(std::exception& e) {
	std::string logMessage = std::string("Countly internal exception: ") + e.what();
	countly_log_function_t logFunction = CountlyCContext::get().logFunction;
	if (logFunction != nullptr) {
		logFunction(CountlyLogLevelError, logMessage.c_str());
	}
	else {
		std::cerr << std::endl << logMessage << std::endl;
		std::cerr.flush();
	}
}


#define COUNTLY_C_GUARD_BEGIN try {
#define COUNTLY_C_GUARD_END } \
	catch(std::exception& e) { \
		onException(e); \
		return COUNTLY_C_GENERIC_ERROR; \
	} \
	return COUNTLY_C_OK;




extern "C" int countly_c_init(
	const char* serverHost,
	int serverPort,
	const char* appKey,
	const char* appVersion,
	const char* stateFilePath
) {
	COUNTLY_C_GUARD_BEGIN

	Countly& ct = Countly::getInstance();

	ct.setLogger(onCountlyLog);
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


extern "C" int countly_c_setLogFunction(countly_log_function_t logFunction) {
	COUNTLY_C_GUARD_BEGIN

	CountlyCContext::get().logFunction = logFunction;

	COUNTLY_C_GUARD_END
}


