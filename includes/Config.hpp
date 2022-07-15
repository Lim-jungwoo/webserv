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

		std::vector<ServerBlock>	getServerBlocks ();

		void						addServerBlock (ServerBlock serv);

		int							parse (std::string file);
};

#endif
