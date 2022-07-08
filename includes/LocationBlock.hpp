#ifndef __LOCATION_BLOCK_HPP__
# define __LOCATION_BLOCK_HPP__

# include "./Utils.hpp"

class	LocationBlock {

	private:
		std::string					_block;
		int							_mod;
		std::string					_uri;
		int							_clntSize;
		std::vector<std::string>	_methods;
		int							_redirect;
		std::string					_root;
		bool						_autoindex;
		std::vector<std::string>	_index;
		std::vector<LocationBlock>	_locations;

	public:
		// constructor, destructor, assignment operator overload
		LocationBlock ();
		LocationBlock (std::string block);
		LocationBlock (const LocationBlock &lb);
		~LocationBlock ();
		LocationBlock	&operator= (const LocationBlock &lb);

		// getter
		std::string					getBlock () const;
		int							getMod () const;
		std::string					getURI () const;
		int							getClntSize () const;
		std::vector<std::string>	getMethods () const;
		int							getRedirect () const;
		std::string					getRoot () const;
		bool						getAutoindex () const;
		std::vector<std::string>	getIndex () const;
		std::vector<LocationBlock>	getLocationBlocks () const;

		// setter
		void						setMod (int mod);
		void						setURI (std::string uri);
		void						setClntSize (int clntSize);
		void						setMethods (std::vector<std::string> methods);
		void						setRedirect (int redirection);
		void						setRoot (std::string root);
		void						setAutoindex (bool autoindex);
		void						setIndex (std::vector<std::string> index);
		void						addLocationBlock (LocationBlock lc);

		// parse
		int							parseModMatch ();
		int							parseClntSize ();
		int							parseMethods ();
		int							parseRoot ();
		int							parseAutoindex ();
		int							parseIndex ();

		int							parse ();
};

#endif
