#pragma once

#define ERR_400 "Your browser sent a request that this server could not understand."

#define ERR_401 "This server could not verify that you are authorized to access " \
				"the document requested. Either you supplied the wrong credentials " \
				"(e.g., bad password), or your browser doesn't understand how to " \
				"supply the credentials required."

#define ERR_403 "You don't have permission to access this resource on this server."

#define ERR_404 "The requested URL was not found on this server."

#define ERR_405 "The method specified in the request is not allowed for the " \
				"resource identified by the request URI."

#define ERR_500 "The server encountered an internal error or misconfiguration " \
				"and was unable to complete your request."

#define ERR_502 "The server, while acting as a gateway or proxy, received an " \
				"invalid response from the upstream server it accessed in attempting" \
				"to fulfill the request."

#define ERR_503 "The server is currently unable to handle the request due to a " \
				"temporary overloading or maintenance of the server."

