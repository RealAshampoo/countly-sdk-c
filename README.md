# Countly C SDK

A Countly SDK with a plain C interface.

Internally this SDK uses the Countly C++ SDK code and simply adds a C interface on top of it.

The SDK currently only supports Windows and builds a self-contained DLL without any dependencies (also no C runtime library dependencies).
Metrics are auto-detected.


## How to use

Include the countly_c.h header in your applications.

All functions return a result constant (COUNTLY_C_OK, COUNTLY_C_GENERIC_ERROR, COUNTLY_C_NOT_STARTED).

Typical call order:

- Initialize with countly_c_init
- Set device ID with countly_c_setDeviceID
- Optional: Set flush interval with countly_c_setFlushIntervalSeconds
- Start with countly_c_start
- Send events and screen views with countly_c_recordEvent and countly_c_recordScreenView.
- When done: flush buffered data with countly_c_flush (recommended)
  Then stop and clean up with countly_c_end.



