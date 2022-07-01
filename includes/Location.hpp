#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

# include "./Utils.hpp"

/*
 * location optional_modifier location_route {
 * ...
 * }
 *
 * optional_modifier: (none), =, ~, ~*, ^~.
 * location_route: what Nginx should check the request URI against.
 */

class	Location {

	private:
		std::string					_block;
		int							_mod;	// optional modifier
		std::string					_match;	// location match
		int							_clntSize;
		std::vector<std::string>	_methods;
		int							_redirect;
		std::string					_root;
		bool						_autoindex;
		std::vector<std::string>	_index;
		std::vector<Location>		_locations;

	public:
		// constructor, destructor, assignment operator overload
		Location ();
		Location (std::string block);
		Location (const Location &lb);
		~Location ();
		Location	&operator= (const Location &lb);

		// getter
		std::string					getBlock ();
		int							getMod ();
		std::string					getMatch ();
		int							getClntSize ();
		std::vector<std::string>	getMethods ();
		int							getRedirect ();
		std::string					getRoot ();
		bool						getAutoindex ();
		std::vector<std::string>	getIndex ();
		std::vector<Location>		getLocations ();

		// setter
		void						setMod (int mod);
		void						setMatch (std::string match);
		void						setClntSize (int clntSize);
		void						setMethods (std::vector<std::string> methods);
		void						setRedirect (int redirection);
		void						setRoot (std::string root);
		void						setAutoindex (bool autoindex);
		void						setIndex (std::vector<std::string> index);
		void						addLocation (Location lc);

		int							parseModMatch ();
		int							parseClntSize ();
		int							parseMethods ();
		int							parseRoot ();
		int							parseAutoindex ();
		int							parseIndex ();

		int							parse ();
};

#endif
