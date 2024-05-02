#include "../includes/Request.hpp"

bool shouldRunCgi(std::string filePath, std::vector<std::string> &allowedCgis) {
	size_t	pos = 0;

	pos = filePath.find_last_of('.');
	if (pos == std::string::npos)
		return (false);

	std::string fileExtension = filePath.substr(pos);

	for (size_t i = 0; i < allowedCgis.size(); ++i) {
		if (fileExtension == allowedCgis[i])
			return true;
	}

	return false;
}

char **getExecveArgs(std::string filePath, std::string fileExtension, std::string cgiParameter) {
	char **execveArgs;
	if (!cgiParameter.empty()) {
		execveArgs = (char **) std::calloc(4, sizeof(char *));
		execveArgs[2] = utils::strdup(cgiParameter.c_str());
	}
	else
		execveArgs = (char **) std::calloc(3, sizeof(char *));

	if (fileExtension == ".py")
		execveArgs[0] = utils::strdup("python3");
	else
		execveArgs[0] = utils::strdup("php");
	execveArgs[1] = utils::strdup(filePath.c_str());

	return execveArgs;
}

void runCgi(std::string filePath, int tmpFileFd, std::string cgiParameter) {
	std::string binPath;
	std::string fileExtension = filePath.substr(filePath.find_last_of('.'));

	if (fileExtension == ".py")
		binPath = "/usr/bin/python3";
	else
		binPath = "/usr/bin/php";

	int pid = fork();

	if (pid == -1) {
		perror("fork");
		throw std::runtime_error("fork");
	}
	else if (pid == 0) {
		char **args = getExecveArgs(filePath, fileExtension, cgiParameter);

		dup2(tmpFileFd, STDOUT_FILENO);
		dup2(tmpFileFd, STDERR_FILENO);
		if (execve(binPath.c_str(), args, environ) == -1) {
			perror("execve");
			throw std::runtime_error("execve");
		}
	}
	else {
		clock_t t = std::clock();
		int ret = 0;
		int	status = -1;

		while ((float)(std::clock() - t) / CLOCKS_PER_SEC < 5.0f) {
			ret = waitpid(pid, &status, WNOHANG);
			if (ret == -1 && status != -1) {
				perror("waitpid");
				throw std::runtime_error("waitpid");
			}
			else if (WIFEXITED(status) || WIFSIGNALED(status)) {
				close(tmpFileFd);
				return ;
			}
		}
		kill(pid, SIGKILL);
		close(tmpFileFd);
		std::cerr << "CGI timed out" << std::endl;
		throw std::runtime_error("TIMEOUT");
	}
}

std::string getCgiOutput(std::string filePath, int connectionFd, std::string cgiParameter) {
	std::string cgiOutput;
	std::string tmpFile(".response" + utils::toString(connectionFd));
	int tmpFileFd = open(tmpFile.c_str(), O_CREAT | O_RDWR, 0644);

	if (tmpFileFd == -1) {
		perror("open");
		throw std::runtime_error("open");
	}

	try {
		runCgi(filePath, tmpFileFd, cgiParameter);
		cgiOutput = utils::getFileContent(tmpFile);
		std::remove(tmpFile.c_str());
	} catch (std::exception &e) {
		std::remove(tmpFile.c_str());
		throw ;
	}

	return cgiOutput;
}

