#include "../includes/Response.hpp"

#define ERROR_PAGES "/home/reinan/Documents/42-Projects/42-webserv/test-files/error_pages"

int main(int argc, char **argv) {
	if (argc == 2)
	{
		Response	r(200, argv[1]);
		return (1);
	}

	// Response	badRequest(400, ERROR_PAGES"/bad_request.html");
	// Response	unauthorized(401, ERROR_PAGES"/unauthorized.html");
	// Response	forbidden(403, ERROR_PAGES"/forbidden.html");
	// Response	notFound(404, ERROR_PAGES"/not_found.html");

	Response	r1(400);
	Response	r2(401);
	Response	r3(403);
	Response	r4(404);
	Response	r5(405);
	Response	r6(500);

	std::cout << r1.response() << "\n\n";
	std::cout << r2.response() << "\n\n";
	std::cout << r3.response() << "\n\n";
	std::cout << r4.response() << "\n\n";
	std::cout << r5.response() << "\n\n";
	std::cout << r6.response() << "\n\n";
	return (0);
}
