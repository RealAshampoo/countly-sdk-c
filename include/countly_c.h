#ifndef COUNTLY_C_ONCE
#define COUNTLY_C_ONCE

#define COUNTLY_C_OK 0
#define COUNTLY_C_ERROR_UNKNOWN 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Countly_C_SegmentationParam
{
	const char* key;
	const char* value;
} Countly_C_SegmentationParam;


int countly_c_init(
	const char* serverHost,
	int serverPort,
	const char* appKey,
	const char* appVersion,
	int updateIntervalSeconds
);


int countly_c_recordEvent(
	const char* eventName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount,
	int eventCount,
	double eventAmount
);

int countly_c_recordView(
	const char* viewName,
	Countly_C_SegmentationParam* segmentationParams,
	int segmentationParamCount
);


#ifdef __cplusplus
}
#endif

#endif