#include "logger.h"

void log(CString message, siSeverityType type)
{
	Application().LogMessage("[Naive RT] " + message, type);
}