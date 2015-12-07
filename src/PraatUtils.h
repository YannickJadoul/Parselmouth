#ifndef INC_PRAAT_UTILS_H
#define INC_PRAAT_UTILS_H

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


	template <typename Function, typename TargetClass = void>
	struct FunctionSignature
	{
		typedef typename RemoveClass<decltype(&Function::operator())>::type type;
	};

	template <typename ReturnType, typename... ArgumentTypes, typename TargetClass>
	struct FunctionSignature<ReturnType (*)(ArgumentTypes...), TargetClass>
	{
		typedef ReturnType type(ArgumentTypes...);
	};

	template <typename ReturnType, typename Class, typename... ArgumentTypes, typename TargetClass>
	struct FunctionSignature<ReturnType (Class::*)(ArgumentTypes...), TargetClass>
	{
		typedef ReturnType type(typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
	};

	template <typename ReturnType, typename Class, typename... ArgumentTypes, typename TargetClass>
	struct FunctionSignature<ReturnType (Class::*)(ArgumentTypes...) const, TargetClass>
	{
		typedef ReturnType type(const typename std::conditional<std::is_base_of<Class, TargetClass>::value, TargetClass, Class>::type*, ArgumentTypes...);
	};


	template <typename Function, typename Signature>
	class AutoThingFunctionImpl;

	template <typename Function, typename ThingType, typename... ArgumentTypes>
	class AutoThingFunctionImpl<Function, _Thing_auto<ThingType> (ArgumentTypes...)>
	{
	public:
		AutoThingFunctionImpl(Function &&wrapped) : m_wrapped(wrapped) {}
		AutoThingFunctionImpl(const Function &wrapped) : m_wrapped(wrapped) {}

		ThingType *operator()(ArgumentTypes... arguments) { return m_wrapped(std::forward<ArgumentTypes>(arguments)...).transfer(); }

	private:
		Function m_wrapped;
	};

	template <typename TargetClass, typename Function>
	using AutoThingFunction = AutoThingFunctionImpl<Function, typename FunctionSignature<Function, TargetClass>::type>;
}

template <typename TargetClass = void, typename Function>
inline detail::AutoThingFunction<TargetClass, Function> returnsAutoThing(Function &&function)
{
	return detail::AutoThingFunction<TargetClass, Function>(std::forward<Function>(function));
}



class MelderInfoInterceptor
{
public:
	MelderInfoInterceptor() : m_string(), m_divertInfo(&m_string) {}
	std::string get() { return std::string(Melder_peek32to8(m_string.string)); }

private:
	autoMelderString m_string;
	autoMelderDivertInfo m_divertInfo;
};

#endif // INC_PRAAT_UTILS_H
