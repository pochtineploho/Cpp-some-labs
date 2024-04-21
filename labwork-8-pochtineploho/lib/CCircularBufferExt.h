#ifndef LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFERExtEXT_H
#define LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFERExtEXT_H

#include "CCircularBuffer.h"
#include <iterator>
#include <iostream>

const size_t kResizeCoefficient = 2;

template<class ValueType, typename Allocator = std::allocator<ValueType>>
class CCircularBufferExt : public CCircularBuffer<ValueType, Allocator> {
public:
    using Buf = CCircularBuffer<ValueType, Allocator>;

    CCircularBufferExt();

    explicit CCircularBufferExt(size_t size);

    CCircularBufferExt(const CCircularBufferExt& copy);

    CCircularBufferExt(size_t quantity, ValueType element);

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    CCircularBufferExt(forward_iterator first, forward_iterator last);

    CCircularBufferExt(std::initializer_list<ValueType> list);

    ~CCircularBufferExt() = default;

    void push_front(const ValueType& element);

    void push_back(const ValueType& element);

private:
    void Construct(size_t size, size_t begin, size_t end, const ValueType& element = ValueType());

    void Deallocate();

    void Destruct();

    CircularIterator<ValueType> ForEmplace(CircularIterator<const ValueType> place, const ValueType& value);
};


template<class ValueType, typename Allocator>
void
CCircularBufferExt<ValueType, Allocator>::Construct(size_t size, size_t begin, size_t end, const ValueType& element) {
    Buf::capacity_ = size;
    Buf::buffer = Buf::allocator.allocate(Buf::capacity_);
    Buf::data_begin = begin;
    Buf::data_end = end;
}


template<class ValueType, typename Allocator>
void CCircularBufferExt<ValueType, Allocator>::Deallocate() {
    std::allocator_traits<Allocator>::deallocate(Buf::allocator, Buf::buffer, Buf::capacity_);
}

template<class ValueType, typename Allocator>
void CCircularBufferExt<ValueType, Allocator>::Destruct() {
    Buf::clear();
    Deallocate();
}


template<class ValueType, typename Allocator>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt() {
    Construct(1, 0, 0);
}

template<class ValueType, typename Allocator>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt(size_t size) {
    Construct(size + 1, 0, 0);
}
template<class ValueType, typename Allocator>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt(const CCircularBufferExt& copy) {
    Construct(copy.capacity_, 0, copy.size());
    size_t index = 0;
    for (CircularIterator<const ValueType> element = copy.cbegin(); element != copy.cend(); ++element) {
        std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + index, *element);
        ++index;
    }
}

template<class ValueType, typename Allocator>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt(size_t quantity, ValueType element) {
    Construct(quantity + 1, 0, quantity);
    for (int i = 0; i < Buf::capacity_ - 1; ++i) {
        std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + i, element);
    }
}

template<class ValueType, typename Allocator>
template<class forward_iterator, typename>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt(forward_iterator first, forward_iterator last) {
    if (first == last) {
        Construct(0, 0, 0);
    } else {
        Construct(last - first + 1, 0, last - first);
        size_t buffer_iterator = 0;
        for (auto tmp = first; tmp != last; ++tmp) {
            std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + buffer_iterator, *tmp);
            ++buffer_iterator;
        }
    }
}

template<class ValueType, typename Allocator>
CCircularBufferExt<ValueType, Allocator>::CCircularBufferExt(std::initializer_list<ValueType> list)
        : CCircularBufferExt(list.begin(), list.end()) {

}


template<class ValueType, typename Allocator>
void CCircularBufferExt<ValueType, Allocator>::push_front(const ValueType& element) {
    size_t index = Buf::data_begin;
    if (Buf::data_begin > 0) {
        index--;
    } else {
        index = Buf::capacity_ - 1;
    }
    if (index == Buf::data_end) {
        Buf::reserve(kResizeCoefficient * (Buf::capacity()));
    }
    std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + Buf::data_begin, element);
}

template<class ValueType, typename Allocator>
void CCircularBufferExt<ValueType, Allocator>::push_back(const ValueType& element) {
    if ((Buf::data_end + 1) % Buf::capacity_ == Buf::data_begin) {
        Buf::reserve(kResizeCoefficient * (Buf::capacity()));
    }
    std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + Buf::data_end, element);
    Buf::data_end = (Buf::data_end + 1) % Buf::capacity_;
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType>
CCircularBufferExt<ValueType, Allocator>::ForEmplace(CircularIterator<const ValueType> place, const ValueType& value) {
    size_t difference = place - Buf::cbegin();
    if ((Buf::data_end + 1) % Buf::capacity_ == Buf::data_begin) {
        Buf::reserve(kResizeCoefficient * (Buf::capacity_ - 1));
    }
    CircularIterator<ValueType> return_iterator = Buf::begin() + difference;
    CircularIterator<ValueType> tmp = Buf::end();
    std::allocator_traits<Allocator>::construct(Buf::allocator, Buf::buffer + Buf::data_end, Buf::back());
    while (tmp != return_iterator && tmp != Buf::begin()) {
        --tmp;
        *(tmp + 1) = *tmp;
    }
    *return_iterator = value;
    Buf::data_end = (Buf::data_end + 1) % Buf::capacity_;

    return return_iterator;
}

#endif
