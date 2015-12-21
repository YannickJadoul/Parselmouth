#ifndef INC_CONSTRUCTOR_H
#define INC_CONSTRUCTOR_H

#include "common/CallableSignature.h"
#include "common/IndexSequence.h"
#include "common/MovingCopyable.h"

#include <boost/python/detail/python_type.hpp>

namespace detail {

template <typename Function, typename Signature, typename ConstructingClass, typename Holder>
class ConstructorImpl;

template <typename Function, typename ReturnType, typename... ArgumentTypes, typename ConstructingClass, typename Holder>
class ConstructorImpl<Function, ReturnType (ArgumentTypes...), ConstructingClass, Holder>
{
public:
	typedef boost::python::detail::python_class<ConstructingClass> UnconstructedObject;

	typedef typename std::remove_cv<typename boost::python::pointee<ReturnType>::type>::type ConstructingType;

	ConstructorImpl(Function &&wrapped) : m_wrapped(std::move(wrapped)) { UnconstructedObject::register_(); }

	void operator()(UnconstructedObject *self, ArgumentTypes... arguments)
	{
		typedef boost::python::objects::instance<Holder> InstanceType;

		auto constructed = m_wrapped(std::forward<ArgumentTypes>(arguments)...);
		void* memory = Holder::allocate(self, offsetof(InstanceType, storage), sizeof(Holder));
		try {
			(new (memory) Holder(std::move(constructed)))->install(self);
		}
		catch(...) {
			Holder::deallocate(self, memory);
			throw;
		}
	}

private:
	Function m_wrapped;
};

} // namespace detail


template <typename Function, typename... Extra>
class ConstructorVisitor : public boost::python::def_visitor<ConstructorVisitor<Function, Extra...>>
{
public:
	friend class boost::python::def_visitor_access;

	ConstructorVisitor(Function &&wrapped, Extra&&... extra) : m_wrapped(std::move(wrapped)), m_extra(std::forward<Extra>(extra)...) {}

    template <typename Class>
    void visit(Class &c) const
    {
    	typedef typename CallableSignature<Function>::type Signature;
    	typedef typename Class::wrapped_type WrappedType;
    	typedef typename Class::metadata::holder Holder;

    	def(c, "__init__", detail::ConstructorImpl<Function, Signature, WrappedType, Holder>(std::move(m_wrapped)));
    }

private:
    template <typename Class, typename... Args>
    void def(Class &c, Args&&... args) const
    {
    	def(IndexSequenceFor<Extra...>(), c, std::forward<Args>(args)...);
    }

    template <size_t... I, typename Class, typename... Args>
    void def(IndexSequence<I...>, Class &c, Args&&... args) const
    {
    	c.def(std::forward<Args>(args)..., std::move(std::get<I>(m_extra))...);
    }

	mutable Function m_wrapped;
	mutable std::tuple<typename std::remove_reference<Extra>::type...> m_extra;
};

template <typename Function, typename... Extra>
inline ConstructorVisitor<Function, Extra...> constructor(Function &&function, Extra&&... args)
{
	return ConstructorVisitor<Function, Extra...>(std::forward<Function>(function), std::forward<Extra>(args)...);
}


#endif // INC_CONSTRUCTOR_H
