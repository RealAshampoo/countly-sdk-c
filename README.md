# Countly C SDK

A Countly SDK with a plain C interface.

Internally, this SDK uses the Countly C++ SDK code and simply adds a C interface on top of it.

The SDK currently only supports Windows and builds a self-contained DLL without any dependencies (also no external CRT dependencies).

Countly metrics are auto-detected when init is called.


## How to use

Download the latest release [for x64](https://github.com/RealAshampoo/countly-sdk-c/releases/latest/download/countly_x64.zip)
or [for x86](https://github.com/RealAshampoo/countly-sdk-c/releases/latest/download/countly_x86.zip) .

Include the countly_c.h header in your application and link to the import library countly_c.lib
(or load the DLL dynamically if that is more convenient in your context).

All functions return an integer result constant (COUNTLY_C_OK, COUNTLY_C_GENERIC_ERROR, COUNTLY_C_NOT_STARTED)
to indicate success or failure.

Typical call order:

- Initialize with countly_c_init
- Set device ID with countly_c_setDeviceID
- Optional: Set flush interval with countly_c_setFlushIntervalSeconds
- Start with countly_c_start
- Send events and screen views with countly_c_recordEvent and countly_c_recordScreenView.
- When done: flush buffered data with countly_c_flush (recommended)
  Then stop and clean up with countly_c_end.

See documentation in countly_c.h for details.



