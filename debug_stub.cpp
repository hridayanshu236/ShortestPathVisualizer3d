// Debug stub for missing CRT symbols
#ifdef _DEBUG

#include <cstdarg>

extern "C" {
    // Stub for _CrtDbgReport
    int _imp__CrtDbgReport(
        int reportType,
        const char* filename,
        int linenumber,
        const char* moduleName,
        const char* format,
        ...
    ) {
        return 0;  // Do nothing - just return success
    }
}

#endif