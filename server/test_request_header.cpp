#include "../header/RequestHeader.hpp"

int	main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "please input test rn name\n";
		return (1);
	}
	RequestHeader	request_header;
	FILE*	file = fopen(argv[1], "r");
	fseek(file, 0, SEEK_END);
	size_t	file_size = ftell(file);
	char*	buf = new char[file_size + 1];
	memset(buf, 0, file_size + 1);
	fseek(file, 0, SEEK_SET);
	fread(buf, file_size, 1, file);
	
	std::string	str(buf);
	request_header.request_split(str);
	return (0);
}