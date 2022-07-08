#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include "./ServerBlock.hpp"

class	Config {

	private:
		std::vector<ServerBlock>	_server;

	public:
		Config ();
		Config (Config &conf);
		~Config ();
		Config						&operator= (Config &conf);

		void						addServerBlock (ServerBlock serv);
		int							parse (std::string file);

		std::vector<ServerBlock>	findMatchingServerBlocks (std::string request, std::string *host, std::string *port) const;
		ServerBlock					selectServerBlock (std::string request) const;

};

#endif
