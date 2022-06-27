#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include "countly_c.h"

using namespace std;


int main() {

	int result = countly_c_init(
		"anon-analytics.data.ashampoo.com",
		443,
		"test-app-analytics-Eyz2DS494TRv",
		"7.8.9",
		"C:/dev/countlydb.db"
	);
	if(result!=0) {
		cout << "countly_c_init failed: "<<result;
		return result;
	}

	result = countly_c_setFlushIntervalSeconds(60);
	if(result!=0) {
		cout << "countly_c_setFlushIntervalSeconds failed: "<<result;
		return result;
	}

	result = countly_c_setDeviceID("5f2422f8-1a97-4abe-a505-0c44839031e0");
	if(result!=0) {
		cout << "countly_c_setDeviceID failed: "<<result;
		return result;
	}

	result = countly_c_start();
	if(result!=0) {
		cout << "countly_c_start failed: "<<result;
		return result;
	}

	result = countly_c_recordScreenView("testViewFromC", NULL, 0);
	if(result!=0) {
		cout << "countly_c_recordScreenView failed: "<<result;
		return result;
	}

	result = countly_c_recordEvent("testEventFromC", "screenWithEvent", NULL, 0);
	if(result!=0) {
		cout << "countly_c_recordEvent failed: "<<result;
		return result;
	}

	{
			Countly_C_SegmentationParam seg[2];
			seg[0].key = "p1";
			seg[0].value = "v1";
			seg[1].key = "p2";
			seg[1].value = "v2";

			result = countly_c_recordEvent("testEventFromCWithSeg", NULL, seg, 2, 0, 0);
			if(result!=0) {
				cout << "countly_c_recordEvent failed: "<<result;
				return result;
			}
	}
	


	result = countly_c_flush();
	if(result!=0) {
		cout << "flush error "<<result;
		return result;
	}

	result = countly_c_end();
	if(result!=0) {
		cout << "end error "<<result;
		return result;
	}

	cout << "OK";

	return 0;
}

