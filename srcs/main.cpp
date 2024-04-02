#include "../includes/Response.hpp"

int main() {
	Response	r(200, "./index.html");

	std::cout << r.getFullReponse();
	
	return(0);
}
