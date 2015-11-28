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
			typename std::enable_if<std::is_member_function_pointer<decltype(&Functor::operator())>::value, typename functor_signature<Functor>::type>::type get_signature(Functor &, void* = 0)
			{
				return typename functor_signature<Functor>::type();
			}
		}
	}
}

#include <boost/python/signature.hpp>

namespace boost {
	namespace python {
		namespace detail {
			template <class Functor>
			struct functor_signature
			{
				typedef decltype(get_signature(&Functor::operator())) member_function_signature;
				typedef typename mpl::advance<typename mpl::begin<member_function_signature>::type, mpl::int_<1>>::type instance_argument_iterator;
				typedef typename mpl::erase<member_function_signature, instance_argument_iterator>::type type;
			};
		}
	}
}

#endif // INC_FUNCTOR_SIGNATURE_H
