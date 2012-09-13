#ifndef CONVERTOR_REGISTRY_H
#define CONVERTOR_REGISTRY_H

#include <string>
#include <map>
#include "convertor_registry.h"

class convertor;

class convertor_factory {
public:
	convertor_factory(const std::string& name, 
					  unsigned int srcfmt, 
					  unsigned int dstfmt, 
					  const std::string& documentation);
	virtual ~convertor_factory() {}

	virtual convertor* get_convertor(int width) = 0;

	std::string get_name() { return name_; }
	std::string get_documentation() { return documentation_; }
	unsigned int get_srcfmt() { return srcfmt_; }
	unsigned int get_dstfmt() { return dstfmt_; }

private:
	std::string name_;
	std::string documentation_;
	unsigned int srcfmt_;
	unsigned int dstfmt_;
};

template <typename convertor_t>
class convertor_factory_impl : public convertor_factory {
public:
	convertor_factory_impl(const std::string& name, 
					  	   unsigned int srcfmt, 
					  	   unsigned int dstfmt, 
						   const std::string& documentation)
		: convertor_factory(name, srcfmt, dstfmt, documentation)
	{}

	virtual convertor* get_convertor(int width) {
		return new convertor_t(width);
	}
};

class convertor_registry {
public:
	static void register_convertor(convertor_factory* factory);		
	/**
	 * @param src_pixfmt, dst_pixfmt:	this value is V4L2_PIX_FMT_*
	 */
	static convertor* get_convertor(unsigned int src_pixfmt, 
									unsigned int dst_pixfmt, 
									int width);

	//typedef std::map<std::string, convertor_factory*> con_map_t;
	typedef std::pair<unsigned int, unsigned int> convertor_fmt_t;
	typedef std::map<convertor_fmt_t, convertor_factory*> con_map_t;
	static con_map_t& get_convertor_map();

private:
	convertor_registry();
	convertor_registry(const convertor_registry&);
};

#define REGISTER_CONVERTOR(name, srcfmt, dstfmt, doc)				\
	class name##_convertor_factory_impl						\
		: public convertor_factory_impl<name##_convertor>		\
	{															\
	public:														\
		name##_convertor_factory_impl()						\
			: convertor_factory_impl<name##_convertor>(		\
				#name, srcfmt, dstfmt, doc)										\
		{}														\
	};															\
	static name##_convertor_factory_impl _registerer;

#endif
