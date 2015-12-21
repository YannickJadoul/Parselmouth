#ifndef INC_FUNCTOR_SIGNATURE_H
#define INC_FUNCTOR_SIGNATURE_H

#include <boost/mpl/erase.hpp>
#include <boost/mpl/vector.hpp>

namespace boost {
namespace python {
namespace detail {

template <class Functor>
struct functor_signature;

template <class Functor>
typename std::enable_if<std::is_member_function_pointer<decltype(&Functor::operator())>::value, typename functor_signature<Functor>::type>::type get_signature(Functor, void* = 0)
{
	return typename functor_signature<Functor>::type();
}

} // detail
} // python
} // boost

#include <boost/python/signature.hpp>

namespace boost {
namespace python {
namespace detail {

template <class Functor>
struct functor_signature
{
	typedef decltype(get_signature(&Functor::operator())) member_function_signature;
	typedef typename mpl::front<member_function_signature>::type return_type;
	typedef typename mpl::pop_front<member_function_signature>::type instance_and_arguments;
	typedef typename mpl::pop_front<instance_and_arguments>::type only_arguments;
	typedef typename mpl::push_front<only_arguments, return_type>::type type;
};

} // detail
} // python
} // boost

#endif // INC_FUNCTOR_SIGNATURE_H
