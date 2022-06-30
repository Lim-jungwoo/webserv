#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "./Location.hpp"

class	Server {

	private:
		std::string					_block;
		std::string					_host;		// host to listen to
		std::string					_port;		// port to open
		std::string					_name;		// name of server
		std::vector<std::string>	_errPages;	// there can be multiple error pages
		int							_clntSize;	// maximum size of client's body size
		std::string					_root;		// root directory of a server
		std::vector<Location>		_locations;	// location block to be used
		std::vector<std::string>	_methods;	// GET, POST, DELETE
		int							_redirect;	// if a response code is 3XX, inform to re-request to another [Location]
		bool						_autoindex;	// autoindex on / off
		std::vector<std::string>	_index;		// if a request is a directory, return this file (related with autoindex)

	public:
		// constructor, destructor, assignment operator overload
		Server ();
		Server (std::string block);
		Server (const Server &srv);
		~Server ();
		Server						&operator= (const Server &srv);

		// getters
		std::string					getBlock ();
		std::string					getHost ();
		std::string					getPort ();
		std::string					getName ();
		std::vector<std::string>	getErrPages ();
		int							getClntSize ();
		std::string					getRoot ();
		std::vector<Location>		getLocations ();
		std::vector<std::string>	getMethods ();
		int							getRedirect ();
		bool						getAutoindex ();
		std::vector<std::string>	getIndex ();
		
		// setters
		void						setBlock (std::string block);
		void						setHost (std::string host);
		void						setPort (std::string port);
		void						setName (std::string name);
		void						setErrPages (std::vector<std::string> pages);
		void						setClntSize (int size);
		void						setRoot (std::string root);
		void						addLocation (Location lc);
		void						setMethods (std::vector<std::string> methods);
		void						setRedirect (int redirection);
		void						setAutoindex (bool autoindex);
		void						setIndex (std::vector<std::string> index);

		int							parseAddress ();
		int							parseName ();
		int							parseErrPages ();
		int							parseClntSize ();
		int							parseRoot ();
		int							parseMethods ();
		int							parseAutoindex ();
		int							parseIndex ();

		int							parse ();

		std::vector<Location>		findMatchingLocations (std::string route);
		Location					selectLocation ();

};

# endif
