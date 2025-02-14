#ifndef COUNTLY_C_ONCE
#define COUNTLY_C_ONCE

#define COUNTLY_C_OK 0
#define COUNTLY_C_GENERIC_ERROR 1
#define COUNTLY_C_NOT_STARTED 2		// countly_c_start has not been called yet

#ifdef __cplusplus
extern "C" {
#endif


/** All functions return a result constand (COUNTLY_C_OK, COUNTLY_C_GENERIC_ERROR, COUNTLY_C_NOT_STARTED). More
 *  Constants might be added in the future.
 * 
 * How to use:
 * 
 * - Initialize with countly_c_init
 * - Set device ID with countly_c_setDeviceID
 * - Optional: Set flush interval with countly_c_setFlushIntervalSeconds
 * - Start with countly_c_start
 * - Send events and screen views with countly_c_recordEvent and countly_c_recordScreenView.
 * - When done: flush buffered data with countly_c_flush (recommended)
 *   Then stop and clean up with countly_c_end.
*/


/** Initializes the library.
 * 	serverHost the host name of the Countly server to use (ONLY the host name - no protocol of path!)
 *  serverPort port of the Countly server. Usually 443 (HTTPS). Note that HTTPS is always used!
 *  appKey Application ID as understood by the Server.
 *  appVersion Application version
 *  stateFilePath a path to a file where the SDK can store its state data.
 *  
*/
int countly_c_init(
	const char* serverHost,
	int serverPort,
	const char* appKey,
	const char* appVersion,
	const char* stateFilePath
);


/** Sets the device ID / client ID.*/
int countly_c_setDeviceID(const char* deviceID);

/** Set the flush interval in seconds.*/
int countly_c_setFlushIntervalSeconds(int seconds);

/** Start sending data.*/
int countly_c_start();


typedef struct Countly_C_SegmentationParam_
{
	const char* key;
	const char* value;
} Countly_C_SegmentationParam;


/** Records an event.
 *  eventName name of the event.
 *  screenName optional, name of the screen where the event occurred. Can be NULL or empty if no screen
 *    is associated.
 *  segmentationParams an array of additional parameters to associate with the event.
 *  Can be NULL if segmentationParamCount is 0.
 *  eventCount represents an integer count >=0. Should be 1 if no specific count should be associated
 *  with the event.
 *  eventAmount is an arbitrary floating point value associated with the event.
*/
int countly_c_recordEvent(
	const char* eventName,
	const char* screenName=NULL,
	Countly_C_SegmentationParam* segmentationParams=NULL,
	int segmentationParamCount=0,
	int eventCount=1,
	double eventAmount=0
);

/** Records an screen view.
 *  screenName name of the screen.
 *  segmentationParams an array of additional parameters to associate with the screen view.
 *  Can be NULL if segmentationParamCount is 0.
*/
int countly_c_recordScreenView(
	const char* screenName,
	Countly_C_SegmentationParam* segmentationParams=NULL,
	int segmentationParamCount=0
);

/** Sends all cached data to the server immediately.*/
int countly_c_flush();


/** Stops everything and cleans up. This should be the last call to the library.*/
int countly_c_end();


/** All possible logging levels, corresponding to the internal Countly class logging levels */
enum CountlyLogLevel
{
	CountlyLogLevelDebug=0,
	CountlyLogLevelInfo=1,
	CountlyLogLevelWarning=2,
	CountlyLogLevelError=3,
	CountlyLogLevelFatal=4
};

/** Logging function prototype, to be passed to countly_c_setLogFunction
  \param level log level
  \param message text to log
 */
typedef void (*countly_log_function_t)(CountlyLogLevel level, const char* message);

/** Sets the logging function for all log levels */
int countly_c_setLogFunction(countly_log_function_t logFunction);


#ifdef __cplusplus
}
#endif

#endif