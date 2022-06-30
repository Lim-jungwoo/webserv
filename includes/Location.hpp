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
<<<<<<< HEAD
		int							_mod;	// optional modifier
		std::string					_match;	// location match
		int							_clntSize;
		std::vector<std::string>	_methods;
		int							_redirect;
		std::string					_root;
		bool						_autoindex;
		std::vector<std::string>	_index;
		std::vector<Location>		_locations;
=======
		std::string					_mod;		// optional modifier
		std::string					_route;		// location match
		std::vector<std::string>	_methods;	// GET, POST, DELETE
		int							_redirect;	// if a response code is 302, inform to re-request to another [Location]
		bool						_dListing;	// autoindex on/off
		std::string					_default;	// if a request is a directory, return this file
>>>>>>> ffde9148b2142388403fd100328b992929e0602e

	public:
		// constructor, destructor, assignment operator overload
		Location ();
<<<<<<< HEAD
		Location (std::string block);
=======
>>>>>>> ffde9148b2142388403fd100328b992929e0602e
		Location (const Location &lb);
		~Location ();
		Location	&operator= (const Location &lb);

		// getter
		std::string					getBlock ();
<<<<<<< HEAD
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
=======
		std::string					getMod ();
		std::string					getRoute ();
		std::vector<std::string>	getMethods ();
		int							getRedirect ();
		bool						getDListing ();
		std::string					getDefault ();

		// setter
		void						setBlock (std::string block);
		void						setMod (std::string mod);
		void						setRoute (std::string route);
		void						setMethods (std::vector<std::string> methods);
		void						setRedirect (int redirection);
		void						setDListing (bool dListing);
		void						setDefault (std::string file);

		int							parseMethods ();
		int							parseDListing ();
		int							parseDefault ();
		void						parse ();
>>>>>>> ffde9148b2142388403fd100328b992929e0602e
};

#endif
