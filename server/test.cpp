#include <string>
#include <cstdlib>
#include <limits>
#include <iostream>

#include <vector>

#include <sys/stat.h>

#include "../cgi/cgi.hpp"

int	main(int argc, char** argv)
{
	Cgi	cgi;

	cgi.initEnv();
	cgi.print_env();
}