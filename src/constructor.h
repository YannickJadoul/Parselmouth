#ifndef INC_CONSTRUCTOR_H
#define INC_CONSTRUCTOR_H

#include "PraatUtils.h"

#include <boost/python/detail/python_type.hpp>

namespace detail {

template <typename T>
inline T *get_pointer(const std::unique_ptr<T> &p)
{
	return p.get();
}

template <typename Function, typename Signature>
class ConstructorImpl;

template <typename Function, typename ReturnType, typename... ArgumentTypes>
class ConstructorImpl<Function, ReturnType (ArgumentTypes...)>
{
public:
	template <class T>
	class MovingCopyable : public T
	{
	public:
		template <typename... Args>	MovingCopyable<T>(Args&&... args) : T(std::forward<Args>(args)...) {}
		MovingCopyable<T>(MovingCopyable<T> &other) : T(std::move(other)) {}
		MovingCopyable<T>(const MovingCopyable<T> &other) = delete;
		MovingCopyable<T>(MovingCopyable<T> &&) = default;
		MovingCopyable<T> &operator=(MovingCopyable<T> &other) { T::operator=(std::move(other)); return this; }
		MovingCopyable<T> &operator=(const MovingCopyable<T> &other) = delete;
		MovingCopyable<T> &operator=(MovingCopyable<T> &&) = default;
	};

	typedef typename std::remove_cv<typename boost::python::pointee<ReturnType>::type>::type ConstructingType;
	typedef boost::python::detail::python_class<ConstructingType> UnconstructedObject;

	ConstructorImpl(Function &&wrapped) : m_wrapped(wrapped) { UnconstructedObject::register_(); }
	ConstructorImpl(const Function &wrapped) : m_wrapped(wrapped) { UnconstructedObject::register_(); }

	void operator()(UnconstructedObject *self, ArgumentTypes... arguments)
	{
		auto constructed = m_wrapped(std::forward<ArgumentTypes>(arguments)...);
		dispatch(self, std::move(constructed), std::is_pointer<ReturnType>());
	}

	private:
	template <class U>
	void dispatch(UnconstructedObject *self, U* x, std::true_type) const
	{
		MovingCopyable<std::unique_ptr<U>> owner(x);
		dispatch(self, owner, std::false_type());
	}

	template <class Ptr>
	void dispatch(UnconstructedObject *self, Ptr x, std::false_type) const
	{
		typedef typename boost::python::pointee<Ptr>::type value_type;
		typedef boost::python::objects::pointer_holder<Ptr,value_type> holder;
		typedef boost::python::objects::instance<holder> instance_t;

		void* memory = holder::allocate(self, offsetof(instance_t, storage), sizeof(holder));
		try {
			(new (memory) holder(x))->install(self);
		}
		catch(...) {
			holder::deallocate(self, memory);
			throw;
		}
	}

	private:
	Function m_wrapped;
};

template <typename TargetClass, typename Function>
using Constructor = ConstructorImpl<Function, typename FunctionSignature<Function, TargetClass>::type>;

} // namespace detail

template <typename TargetClass = void, typename Function>
inline detail::Constructor<TargetClass, Function> constructor(Function &&function)
{
	return detail::Constructor<TargetClass, Function>(std::forward<Function>(function));
}

#endif // INC_CONSTRUCTOR_H
