// #include "../header/RequestHeader.hpp"

# include <cstdlib>
# include <iostream>

int	main()
{
	std::string	str = "1.9.17.9";
	size_t	sep = 0;
	std::string	substr;
	size_t	start = 0;
	for (int i = 3; i > -1; i--)
	{
		sep = str.find_first_of('.', sep);
		substr = str.substr(start, sep - start);
		std::cout << "start : " << start << ", sep : " << sep;
		std::cout << ", substr : " << substr << std::endl;
		sep++;
		start = sep;
	}
}