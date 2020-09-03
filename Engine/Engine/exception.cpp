#include "exception.h"

void Exception::printError() const
{
	fprintf(stdout, what());
	//if (getLine() > 0 && getFile() != nullptr)
	//{
	//	fprintf(stdout, "[%s] %s\n[LINE] %d\n[FILE] %s\n", getType(), getErrorString(), getLine(), getFile());
	//}
	//else
	//{
	//	fprintf(stdout, "[%s] %s\n]", getType(), getErrorString());
	//}
}

void Exception::logError() const
{
	errno_t err = freopen_s((FILE**)&fErrorLog, nErrorLogFile, "w", stderr);

	if (NULL != err || fErrorLog == NULL)
	{
		fprintf(stdout, "Could not log error in %s\n", nErrorLogFile);
		return;
	}

	// Log error in file
	fprintf(fErrorLog, what());
	fclose(fErrorLog);

	std::string s = "type " + std::string(nErrorLogFile);
	system(s.c_str());

	fprintf(stdout, "Successfully logged error in %s\n", nErrorLogFile);
}