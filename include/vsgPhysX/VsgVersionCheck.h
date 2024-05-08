#pragma once

#include <vsg/core/Version.h>

#ifndef VSG_API_VERSION_LESS

#    define VSG_API_VERSION_LESS(major, minor, patch) ((VSG_VERSION_MAJOR < major) || ((VSG_VERSION_MAJOR == major) && ((VSG_VERSION_MINOR < minor) || ((VSG_VERSION_MINOR == minor) && VSG_VERSION_PATCH < patch))))
#    define VSG_API_VERSION_GREATER_EQUAL(major, minor, patch) ((VSG_VERSION_MAJOR > major) || ((VSG_VERSION_MAJOR == major) && ((VSG_VERSION_MINOR > minor) || ((VSG_VERSION_MINOR == minor) && VSG_VERSION_PATCH >= patch))))

#endif
