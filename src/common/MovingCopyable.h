#ifndef INC_MOVING_COPYABLE_H
#define INC_MOVING_COPYABLE_H

template <class T>
class MovingCopyable : public T
{
public:
	template <typename... Args>	MovingCopyable<T>(Args&&... args) : T(std::forward<Args>(args)...) {}
	MovingCopyable<T>(T &other) : T(std::move(other)) {}
	MovingCopyable<T>(const T &other) = delete;
	MovingCopyable<T>(MovingCopyable<T> &other) : T(std::move(other)) {}
	MovingCopyable<T>(const MovingCopyable<T> &other) = delete;
	MovingCopyable<T>(T &&other) : T(std::move(other)) {}
	MovingCopyable<T>(MovingCopyable<T> &&) = default;
	MovingCopyable<T> &operator=(T &other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(const T &other) = delete;
	MovingCopyable<T> &operator=(MovingCopyable<T> &other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(const MovingCopyable<T> &other) = delete;
	MovingCopyable<T> &operator=(T &&other) { T::operator=(std::move(other)); return this; }
	MovingCopyable<T> &operator=(MovingCopyable<T> &&) = default;
};

#endif // INC_MOVING_COPYABLE_H
