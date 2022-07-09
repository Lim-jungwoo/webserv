#ifndef __SERVER_BLOCK_HPP__
# define __SERVER_BLOCK_HPP__

# include "./LocationBlock.hpp"

class	ServerBlock {

	private:
		std::string					_block;
		std::string					_host;
		std::string					_port;
		bool						_default;
		std::string					_name;
		std::vector<std::string>	_errPages;
		int							_clntSize;
		std::string					_root;
		std::vector<LocationBlock>	_locations;
		std::vector<std::string>	_methods;
		int							_redirect;
		bool						_autoindex;
		std::vector<std::string>	_index;

	public:
		// constructor, destructor, assignment operator overload
		ServerBlock ();
		ServerBlock (std::string block);
		ServerBlock (const ServerBlock &srv);
		~ServerBlock ();
		ServerBlock					&operator= (const ServerBlock &srv);

		// getter
		std::string					getBlock () const;
		std::string					getHost () const;
		std::string					getPort () const;
		bool						getDefault () const;
		std::string					getName () const;
		std::vector<std::string>	getErrPages () const;
		int							getClntSize () const;
		std::string					getRoot () const;
		std::vector<LocationBlock>	getLocationBlocks () const;
		std::vector<std::string>	getMethods () const;
		int							getRedirect () const;
		bool						getAutoindex () const;
		std::vector<std::string>	getIndex () const;
		
		// setter
		void						setBlock (std::string block);
		void						setHost (std::string host);
		void						setPort (std::string port);
		void						setDefault (bool dflt);
		void						setName (std::string name);
		void						setErrPages (std::vector<std::string> pages);
		void						setClntSize (int size);
		void						setRoot (std::string root);
		void						addLocationBlock (LocationBlock lc);
		void						setMethods (std::vector<std::string> methods);
		void						setRedirect (int redirection);
		void						setAutoindex (bool autoindex);
		void						setIndex (std::vector<std::string> index);

		// parse
		int							parseAddress ();
		int							parseName ();
		int							parseErrPages ();
		int							parseClntSize ();
		int							parseRoot ();
		int							parseMethods ();
		int							parseAutoindex ();
		int							parseIndex ();

		int							parse ();

		std::vector<LocationBlock>	findMatchingLocationBlocks (std::string requestURI) const;
		LocationBlock				selectLocationBlock (std::string requestURI) const;

};

# endif
