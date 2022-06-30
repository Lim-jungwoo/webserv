#ifndef __CONFIG_HPP__
# define __CONFIG_HPP__

# include "./Server.hpp"

class	Config {

	private:
		std::vector<Server>				_server;

	public:
		Config ();
		Config (Config &conf);
		~Config ();
		Config						&operator= (Config &conf);

		void						addServer (Server serv);
		int							parse (std::string file);

		std::vector<Server>			findMatchingServers (std::string request, std::string *host, std::string *port);
		Server						selectServer (std::string request);

};

#endif
