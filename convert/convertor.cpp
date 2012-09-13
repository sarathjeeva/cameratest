#include "globals.h"
#include "convertor.h"
#include "convertor_registry.h"

void convertor_registry::register_convertor(convertor_factory* factory)
{
	con_map_t& the_map = get_convertor_map();
	if (the_map.find(factory->get_name()) != the_map.end()) {
		failure("Duplicate convertor \"%s\"!\n", factory->get_name().c_str());
	}
	the_map[factory->get_name()] = factory;
}

convertor* convertor_registry::get_convertor(unsigned int src_pixfmt, unsigned int dst_pixfmt, int width) 
{
	con_map_t& the_map = get_convertor_map();
	con_map_t::iterator iter = the_map.find();

	if (iter == the_map.end()) {
		return NULL;
	}

	return iter->second->get_convertor(width);
}

convertor_registry::con_map_t& convertor_registry::get_convertor_map() {
	static con_map_t* the_map = new con_map_t();
	return *the_map;
}

convertor_factory::convertor_factory(
		const std::string& name, 
		const std::string& documentation)
	: name_(name)
	, documentation_(documentation)
{
	convertor_registry::register_convertor(this);
}
