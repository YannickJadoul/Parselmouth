#ifndef INC_AUTO_THING_UTILS_H
#define INC_AUTO_THING_UTILS_H

#include "sys/Thing.h"
#undef I
#undef trace

#include <boost/python/make_function.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_value_policy.hpp>

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


	template <typename TargetClass, typename Function>
	struct FunctionSignature
	{
		typedef typename RemoveClass<decltype(&Function::operator())>::type type;
	};

	template <typename TargetClass, typename ReturnType, typename... ArgumentTypes>
	struct FunctionSignature<TargetClass, ReturnType (*)(ArgumentTypes...)>
	{
		typedef ReturnType type(ArgumentTypes...);
	};

	template <typename TargetClass, typename ReturnType, typename Class, typename... ArgumentTypes>
	struct FunctionSignature<TargetClass, ReturnType (Class::*)(ArgumentTypes...)>
	{
		typedef ReturnType type(typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
	};

	template <typename TargetClass, typename ReturnType, typename Class, typename... ArgumentTypes>
	struct FunctionSignature<TargetClass, ReturnType (Class::*)(ArgumentTypes...) const>
	{
		typedef ReturnType type(const typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
	};


	template <typename TargetClass, typename Function, typename SignatureType = typename FunctionSignature<TargetClass, Function>::type>
	class AutoThingFunction;

	template <typename TargetClass, typename Function, typename ThingType, typename... ArgumentTypes>
	class AutoThingFunction<TargetClass, Function, _Thing_auto<ThingType> (ArgumentTypes...)>
	{
	public:
		AutoThingFunction(Function &&wrapped) : m_wrapped(wrapped) {}
		AutoThingFunction(const Function &wrapped) : m_wrapped(wrapped) {}

		ThingType *operator()(ArgumentTypes... arguments) { return m_wrapped(std::forward<ArgumentTypes>(arguments)...).transfer(); }

	private:
		Function m_wrapped;
	};
}

template <typename TargetClass = std::nullptr_t, typename Function>
inline detail::AutoThingFunction<TargetClass, Function> returnsAutoThing(Function &&function) { return detail::AutoThingFunction<TargetClass, Function>(std::forward<Function>(function)); }

#endif // INC_AUTO_THING_UTILS_H
