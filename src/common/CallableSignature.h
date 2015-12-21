#ifndef INC_CALLABLE_SIGNATURE_H
#define INC_CALLABLE_SIGNATURE_H

namespace detail {

template <typename MemberFunction>
struct RemoveClass;

template <typename ReturnType, typename Class, typename... ArgumentTypes>
struct RemoveClass<ReturnType (Class::*)(ArgumentTypes...)>
{
	typedef ReturnType type(ArgumentTypes...);
};

template <typename ReturnType, typename Class, typename... ArgumentTypes>
struct RemoveClass<ReturnType (Class::*)(ArgumentTypes...) const>
{
	typedef ReturnType type(ArgumentTypes...);
};

} // namespace detail


template <typename Function, typename TargetClass = void>
struct CallableSignature
{
	typedef typename detail::RemoveClass<decltype(&Function::operator())>::type type;
};

template <typename ReturnType, typename... ArgumentTypes, typename TargetClass>
struct CallableSignature<ReturnType (*)(ArgumentTypes...), TargetClass>
{
	typedef ReturnType type(ArgumentTypes...);
};

template <typename ReturnType, typename Class, typename... ArgumentTypes, typename TargetClass>
struct CallableSignature<ReturnType (Class::*)(ArgumentTypes...), TargetClass>
{
	typedef ReturnType type(typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
};

template <typename ReturnType, typename Class, typename... ArgumentTypes, typename TargetClass>
struct CallableSignature<ReturnType (Class::*)(ArgumentTypes...) const, TargetClass>
{
	typedef ReturnType type(const typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
};

#endif // INC_CALLABLE_SIGNATURE_H
