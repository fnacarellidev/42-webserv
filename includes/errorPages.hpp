#pragma once

#define E400 "Your browser sent a request that this server could not understand."

#define E401 "This server could not verify that you are authorized to access " \
				"the document requested. Either you supplied the wrong credentials " \
				"(e.g., bad password), or your browser doesn't understand how to " \
				"supply the credentials required."

#define E403 "You don't have permission to access this resource on this server."

#define E404 "The requested URL was not found on this server."

#define E405 "The method specified in the request is not allowed for the " \
				"resource identified by the request URI."

#define E413 "The request was larger than the server is willing or able to " \
				"process."

#define E500 "The server encountered an internal error or misconfiguration " \
				"and was unable to complete your request."

#define E502 "The server, while acting as a gateway or proxy, received an " \
				"invalid response from the upstream server it accessed in attempting" \
				"to fulfill the request."

#define E503 "The server is currently unable to handle the request due to a " \
				"temporary overloading or maintenance of the server."
