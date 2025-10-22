
char **cgiEnv(const std::string& method, const std::string& query, const std::string& contentLength, const std::string& contentType,
                                     const std::string& scriptPath)
{

}

std::string cgiHandle()
{
	char **env = cgiEnv();
}