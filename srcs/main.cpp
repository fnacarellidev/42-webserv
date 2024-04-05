#include "../includes/Response.hpp"

int main(int argc, char **argv) {
	if (argc != 2)
		return (1);

	Response	r(200, argv[1]);

	std::cout << r.response();
	
	return (0);
}
