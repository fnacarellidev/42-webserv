#include "../../includes/utils.hpp"

time_t	convertTimeToGMT(time_t t) {
	struct tm	*gmtTime = gmtime(&t);
	return (mktime(gmtTime));
}

std::string	formatTimeString(time_t	time) {
	char	buffer[80];
	std::strftime(buffer, sizeof(buffer), "%c", localtime(&time));
	std::string strTime(buffer);
	strTime += " GMT";
	return (strTime);
}

std::string	getCurrentTimeInGMT() {
	time_t	now = convertTimeToGMT(time(0));
	return (formatTimeString(now));
}
