#ifndef INC_INDEX_SEQUENCE_H
#define INC_INDEX_SEQUENCE_H

template <size_t... I>
struct IndexSequence
{
	template<size_t N>
	using append = IndexSequence<I..., N>;
};

namespace detail {

template <size_t N>
struct MakeIndexSequenceImpl
{
	typedef typename MakeIndexSequenceImpl<N-1>::type::template append<N-1> type;
};

template <>
struct MakeIndexSequenceImpl<0>
{
	typedef IndexSequence<> type;
};

} // namespace detail

template <size_t N>
using MakeIndexSequence = typename detail::MakeIndexSequenceImpl<N>::type;

template <typename... Args>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Args)>;

#endif // INC_INDEX_SEQUENCE_H
