#ifndef INC_COPYABLE_AUTO_THING_H
#define INC_COPYABLE_AUTO_THING_H

#include "common/MovingCopyable.h"

#include <boost/python/to_python_converter.hpp>

template <typename T>
using CopyableAutoThing = MovingCopyable<_Thing_auto<T>>;

template <typename T>
inline T *get_pointer(const _Thing_auto<T> &ptr)
{
	return ptr.get();
}

namespace boost {
namespace python {

template <class T>
struct pointee<_Thing_auto<T>>
{
	typedef T type;
};

template <class T>
struct pointee<CopyableAutoThing<T>>
{
	typedef T type;
};

} // namespace python
} // namespace boost


template <typename T>
struct auto_thing_converter : boost::python::to_python_converter<_Thing_auto<T>, auto_thing_converter<T>, true>
{
	typedef boost::python::objects::make_ptr_instance<T, boost::python::objects::pointer_holder<CopyableAutoThing<T>, T>> MakeInstance;

    static PyObject *convert(const _Thing_auto<T> &x)
    {
    	// Sorry, we'll need to steal this _Thing_auto, just like an auto_ptr is stolen.
    	// The only difference is that we cannot accept the _Thing_auto by value, because is respects move semantics and cannot be copied.
    	// So, if this were an auto_ptr, the const_cast would happen before the call to this convert function that would take it by value.
        return MakeInstance::execute(const_cast<_Thing_auto<T>&>(x));
    }

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    static PyTypeObject const *get_pytype() { return MakeInstance::get_pytype(); }
#endif
};

#endif // INC_COPYABLE_AUTO_THING_H
