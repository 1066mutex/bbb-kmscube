
#include "Log.h"

#include <iostream>
#include <cstdio>
#include <stdarg.h>


// Function to log messages with variable arguments
// iType: An unsigned int representing the log type (not used in this implementation)
// szText: A format string, similar to printf
// ...: Variable number of arguments to be formatted into szText
void Log(unsigned int iType, const char* szText, ...) {
    // Declare a va_list to handle the variable arguments
    va_list argp;
    
    // Initialize the va_list to point to the first variable argument
    // The last named parameter before the ellipsis (...) must be passed to va_start
    va_start(argp, szText);
    
    // Buffer to store the formatted string
    char szBuffer[4096];
    
    // Use vsnprintf to safely format the string with variable arguments
    // 4094 leaves space for newline and null terminator
    vsnprintf(szBuffer, 4094, szText, argp);
    
    // Clean up the va_list
    va_end(argp);
    
    // Add a newline character at the end of the buffer
    szBuffer[4094] = '\n';
    
    // Ensure null-termination of the string
    szBuffer[4095] = 0;
    
    // Output the formatted string to console
    std::cout << szBuffer;
}
