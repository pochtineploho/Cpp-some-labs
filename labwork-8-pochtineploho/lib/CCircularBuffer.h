#ifndef LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFER_H
#define LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFER_H

#include <iterator>
#include <iostream>

const size_t kDefaultBufferSize = 101;

template<class ValueType, typename Allocator=std::allocator<ValueType>>
class CCircularBuffer;

template<class ValueType>
class CircularIterator : public std::iterator<std::input_iterator_tag, ValueType> {
    friend CCircularBuffer<ValueType>;
public:
    CircularIterator(const CircularIterator& iter) :
            pointer(iter.pointer),
            begin_pointer(iter.begin_pointer),
            end_pointer(iter.end_pointer) {}

    CircularIterator& operator=(const CircularIterator& iter) {
        if (this == &iter) {
            return *this;
        }
        pointer = iter.pointer;
        begin_pointer = iter.begin_pointer;
        end_pointer = iter.end_pointer;

        return *this;
    }

    bool operator==(CircularIterator const& rhs) const {
        return pointer == rhs.pointer;
    }

    bool operator==(ValueType* rhs) const {
        return pointer == rhs;
    }

    bool operator!=(CircularIterator const& rhs) const {
        return pointer != rhs.pointer;
    }

    CircularIterator& operator++() {
        if (pointer == end_pointer) {
            pointer = begin_pointer;
        } else {
            ++pointer;
        }
        return *this;
    }

    CircularIterator& operator++() const {
        return ++*this;
    }

    CircularIterator operator++(int) {
        return ++*this;
    }

    CircularIterator operator++(int) const {
        return ++*this;
    }

    CircularIterator operator--() {
        if (pointer == begin_pointer) {
            pointer = end_pointer;
        } else {
            --pointer;
        }
        return *this;
    }

    CircularIterator operator--() const {
        return --*this;
    }

    CircularIterator operator--(int) {
        return --*this;
    }

    CircularIterator operator--(int) const {
        return --*this;
    }

    CircularIterator& operator+=(long long number) {
        number = number % (end_pointer - begin_pointer);
        if (pointer + number <= end_pointer && pointer + number >= begin_pointer) {
            pointer += number;
        } else {
            if (number < 0) {
                pointer = end_pointer + ((pointer - begin_pointer + 1) + number);
            } else {
                pointer = begin_pointer + ((pointer + number - 1) - end_pointer);
            }
        }
        return *this;
    }

    CircularIterator& operator-=(long long number) {
        return *this += -number;
    }

    CircularIterator operator+(long long number) {
        CircularIterator<ValueType> tmp = *this;
        return tmp += number;
    }

    CircularIterator operator-(long long number) {
        CircularIterator<ValueType> tmp = *this;
        return tmp -= number;
    }

    std::iter_difference_t<CircularIterator> operator-(const CircularIterator& rhs) {
        std::iter_difference_t<CircularIterator> tmp;
        if (pointer >= rhs.pointer) {
            tmp = pointer - rhs.pointer;
        } else {
            tmp = (rhs.pointer - begin_pointer) + (end_pointer - pointer);
        }
        return tmp;
    }

    ValueType& operator[](long long number) {
        return *(*this + number);
    }

    bool operator<(const CircularIterator& rhs) {
        return (*this - rhs > 0);
    }

    bool operator>(const CircularIterator& rhs) {
        return (rhs < *this);
    }

    bool operator<=(const CircularIterator& rhs) {
        return !(rhs < *this);
    }

    bool operator>=(const CircularIterator& rhs) {
        return !(*this < rhs);
    }

    ValueType& operator*() {
        return *pointer;
    }

    explicit CircularIterator(ValueType* point, const CCircularBuffer<ValueType>& buffer) :
            begin_pointer(buffer.buffer),
            end_pointer(buffer.buffer + buffer.capacity_ - 1),
            pointer(point) {}

    explicit CircularIterator(ValueType* point, ValueType* buffer, size_t size) :
            begin_pointer(buffer),
            end_pointer(buffer + size - 1),
            pointer(point) {}

    ValueType* where() {
        return pointer;
    }

    ~CircularIterator() = default;

private:
    ValueType* begin_pointer;
    ValueType* end_pointer;
    ValueType* pointer;
};


template<class ValueType, typename Allocator>
class CCircularBuffer {
    friend CircularIterator<ValueType>;
public:
    typedef CircularIterator<ValueType> iterator;
    typedef CircularIterator<const ValueType> const_iterator;

    CCircularBuffer();

    explicit CCircularBuffer(size_t size);

    CCircularBuffer(const CCircularBuffer& copy);

    CCircularBuffer& operator=(CCircularBuffer const& copy);

    CCircularBuffer(size_t quantity, ValueType element);

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    CCircularBuffer(forward_iterator first, forward_iterator last);

    CCircularBuffer(std::initializer_list<ValueType> list);

    CCircularBuffer& operator=(std::initializer_list<ValueType> list);

    virtual ~CCircularBuffer();

    iterator begin();

    iterator end();

    const_iterator cbegin() const;

    const_iterator cend() const;

    bool operator==(const CCircularBuffer<ValueType>& rhs) const;

    bool operator!=(const CCircularBuffer<ValueType>& rhs) const;

    void swap(CCircularBuffer<ValueType>& rhs);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t max_size() const;

    [[nodiscard]] bool empty() const;

    void clear();

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    void assign(forward_iterator first, forward_iterator last);

    void assign(std::initializer_list<ValueType> list);

    void assign(size_t quantity, ValueType element);

    const ValueType& front() const;

    const ValueType& back() const;

    iterator insert(const_iterator position, std::initializer_list<ValueType> list);

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    iterator insert(const_iterator position, forward_iterator first, forward_iterator last);

    iterator insert(const_iterator position, size_t quantity, ValueType element);

    iterator insert(const_iterator position, ValueType element);

    template<class ...VValueType>
    iterator emplace(const_iterator place, VValueType&& ... element);

    template<class ...VValueType>
    void emplace_front(VValueType&& ... element);

    template<class ...VValueType>
    void emplace_back(VValueType&& ... element);

    iterator erase(const_iterator first, const_iterator last);

    iterator erase(const_iterator position);

    virtual void push_front(const ValueType& element);

    virtual void push_back(const ValueType& element);

    void pop_front();

    void pop_back();

    void reserve(size_t quantity);

    void resize(size_t quantity, const ValueType& value = ValueType());

    [[nodiscard]] size_t capacity() const;

    ValueType& operator[](size_t index);

    ValueType& operator[](size_t index) const;

    ValueType& at(size_t index);

    template<typename VValueType>
    friend void swap(CCircularBuffer<VValueType>& lhs, CCircularBuffer<VValueType>& rhs);

protected:
    ValueType* buffer = nullptr;
    size_t capacity_{};
    size_t data_begin{};
    size_t data_end{};
    Allocator allocator;

    virtual void Construct(size_t size, size_t begin, size_t end);

    virtual void Deallocate();

    virtual void Destruct();

    virtual iterator ForEmplace(const_iterator place, const ValueType& value);
};

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::Construct(size_t size, size_t begin, size_t end) {
    if (size <= 1) {
        std::cerr << "Buffer size must be more than 0\n";
        size = kDefaultBufferSize;
    }
    capacity_ = size;
    buffer = allocator.allocate(capacity_);
    data_begin = begin;
    data_end = end;
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::Deallocate() {
    std::allocator_traits<Allocator>::deallocate(allocator, buffer, capacity_);
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::Destruct() {
    clear();
    Deallocate();
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer() {
    Construct(kDefaultBufferSize, 0, 0);
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer(size_t size) {
    Construct(size + 1, 0, 0);
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>& CCircularBuffer<ValueType, Allocator>::operator=(const CCircularBuffer& copy) {
    if (this != &copy) {
        Destruct();
        Construct(copy.capacity_, 0, copy.size());
        size_t index = 0;
        for (const_iterator element = copy.cbegin(); element != copy.cend(); ++element) {
            std::allocator_traits<Allocator>::construct(allocator, buffer + index, *element);
            ++index;
        }
    }
    return *this;
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer(const CCircularBuffer& copy) {
    Construct(copy.capacity_, 0, copy.size());
    size_t index = 0;
    for (const_iterator element = copy.cbegin(); element != copy.cend(); ++element) {
        std::allocator_traits<Allocator>::construct(allocator, buffer + index, *element);
        ++index;
    }
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer(size_t quantity, ValueType element) {
    Construct(quantity + 1, 0, quantity);
    for (int i = 0; i < capacity_ - 1; ++i) {
        std::allocator_traits<Allocator>::construct(allocator, buffer + i, element);
    }
}

template<class ValueType, typename Allocator>
template<class forward_iterator, typename>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer(forward_iterator first,
                                                       forward_iterator last) {
    if (first == last) {
        Construct(0, 0, 0);
    } else {
        Construct(last - first + 1, 0, last - first);
        size_t buffer_iterator = 0;
        for (auto tmp = first; tmp != last; ++tmp) {
            std::allocator_traits<Allocator>::construct(allocator, buffer + buffer_iterator, *tmp);
            ++buffer_iterator;
        }
    }
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>&
CCircularBuffer<ValueType, Allocator>::operator=(std::initializer_list<ValueType> list) {
    assign(list);

    return *this;
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::CCircularBuffer(std::initializer_list<ValueType> list) : CCircularBuffer(
        list.begin(), list.end()) {
}

template<class ValueType, typename Allocator>
CCircularBuffer<ValueType, Allocator>::~CCircularBuffer() {
    Destruct();
}

template<class ValueType, typename Allocator>
ValueType& CCircularBuffer<ValueType, Allocator>::operator[](size_t index) {
    if (empty() || index % capacity() >= size()) {
        std::cerr << "Buffer is empty\n";
    }
    return begin()[index];
}

template<class ValueType, typename Allocator>
ValueType& CCircularBuffer<ValueType, Allocator>::operator[](size_t index) const {
    if (empty() || index % capacity() >= size()) {
        std::cerr << "Buffer is empty\n";
    }
    return begin()[index];
}

template<class ValueType, typename Allocator>
ValueType& CCircularBuffer<ValueType, Allocator>::at(size_t index) {
    if (empty() || index % capacity() >= size()) {
        throw std::out_of_range("Buffer is empty");
    }
    return begin()[index];
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType> CCircularBuffer<ValueType, Allocator>::begin() {
    return iterator(buffer + data_begin, *this);
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType> CCircularBuffer<ValueType, Allocator>::end() {
    return iterator(buffer + data_end, *this);
}

template<class ValueType, typename Allocator>
CircularIterator<const ValueType> CCircularBuffer<ValueType, Allocator>::cbegin() const {
    return const_iterator(buffer + data_begin, buffer, capacity_);
}

template<class ValueType, typename Allocator>
CircularIterator<const ValueType> CCircularBuffer<ValueType, Allocator>::cend() const {
    return const_iterator(buffer + data_end, buffer, capacity_);
}

template<class ValueType, typename Allocator>
const ValueType& CCircularBuffer<ValueType, Allocator>::front() const {
    if (empty()) {
        std::cerr << "Index out of range\n";
    }
    return buffer[data_begin];
}

template<class ValueType, typename Allocator>
const ValueType& CCircularBuffer<ValueType, Allocator>::back() const {
    if (empty()) {
        std::cerr << "Index out of range\n";
    }
    if (data_end == 0) {
        return buffer[capacity_ - 1];
    }
    return buffer[data_end - 1];
}

template<class ValueType, typename Allocator>
bool CCircularBuffer<ValueType, Allocator>::operator==(const CCircularBuffer<ValueType>& rhs) const {
    if (size() != rhs.size()) {
        return false;
    }
    const_iterator iter_right = rhs.cbegin();
    for (auto iter_left = cbegin(); iter_left != cend(); ++iter_left) {
        if (*iter_left != *iter_right) {
            return false;
        }
        ++iter_right;
    }
    return true;
}

template<class ValueType, typename Allocator>
bool CCircularBuffer<ValueType, Allocator>::operator!=(const CCircularBuffer<ValueType>& rhs) const {
    return !(*this == rhs);
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::swap(CCircularBuffer<ValueType>& rhs) {
    std::swap(*this, rhs);
}

template<typename ValueType>
void swap(CCircularBuffer<ValueType>& lhs, CCircularBuffer<ValueType>& rhs) {
    std::swap(lhs, rhs);
}

template<class ValueType, typename Allocator>
size_t CCircularBuffer<ValueType, Allocator>::size() const {
    if (data_end > data_begin) {
        return data_end - data_begin;
    }
    if (data_end == data_begin) {
        return 0;
    }
    return data_end + capacity_ - data_begin;
}

template<class ValueType, typename Allocator>
size_t CCircularBuffer<ValueType, Allocator>::max_size() const {
    return allocator.max_size();
}

template<class ValueType, typename Allocator>
bool CCircularBuffer<ValueType, Allocator>::empty() const {
    return data_begin == data_end;
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::insert(CCircularBuffer::const_iterator position,
                                              std::initializer_list<ValueType> list) {
    return insert(position, list.begin(), list.end());
}

template<class ValueType, typename Allocator>
template<class forward_iterator, typename>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::insert(CCircularBuffer::const_iterator position, forward_iterator first,
                                              forward_iterator last) {
    size_t return_iterator = position - cbegin();
    size_t quantity = last - first;
    if (quantity == 0) {
        return begin() + return_iterator;
    }
    size_t ending_size = cend() - position;
    size_t beginning_size = position - cbegin();
    if (quantity + size() > capacity()) {
        CCircularBuffer<ValueType> new_buffer;
        new_buffer.Construct(quantity + size() + 1, 0, 0);
        for (auto tmp = cbegin(); tmp != position; ++tmp) {
            new_buffer.push_back(*tmp);
        }
        for (auto tmp = first; tmp != last; ++tmp) {
            new_buffer.push_back(*tmp);
        }
        for (auto tmp = position; tmp != cend(); ++tmp) {
            new_buffer.push_back(*tmp);
        }
        swap(new_buffer);
        new_buffer.Destruct();

        return begin() + return_iterator;
    }
    if (beginning_size <= ending_size) {
        iterator return_it = begin() + return_iterator;
        size_t construct_times = 0;
        iterator tmp = begin();
        if (quantity > data_begin) {
            data_begin = capacity_ - (quantity - data_begin);
        } else {
            data_begin -= quantity;
        }
        for (int i = 0; i < beginning_size; ++i) {
            if (construct_times < quantity) {
                std::allocator_traits<Allocator>::construct(allocator, buffer + (data_begin + i) % capacity_, *tmp);
                construct_times++;
                ++tmp;
            } else {
                ++tmp;
                buffer[data_begin] = *tmp;
            }
        }
        iterator input = begin() + return_iterator;
        for (auto it = first; it != last; ++it) {
            if (construct_times < quantity) {
                std::allocator_traits<Allocator>::construct(allocator,
                                                            buffer +
                                                            (data_begin + return_iterator + it - first) % capacity_,
                                                            *it);
                construct_times++;
            } else {
                buffer[(data_begin + return_iterator + it - first) % capacity_] = *it;
            }
        }
        return return_it;
    } else {
        size_t construct_times = 0;
        size_t index = data_end + quantity;
        iterator tmp = end();
        for (int i = 0; i < ending_size + 1; ++i) {
            if (construct_times < quantity) {
                construct_times++;
                std::allocator_traits<Allocator>::construct(allocator, buffer + index, *tmp);
                --tmp;
                --index;
            } else {
                buffer[data_begin + index] = *tmp;
                --tmp;
                --index;
            }
        }
        iterator input = begin() + return_iterator;
        for (auto it = first; it != last; ++it) {
            *input = *it;
            ++input;
        }
        data_end += quantity;

        return begin() + return_iterator;
    }
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::insert(CCircularBuffer::const_iterator position, size_t quantity,
                                              ValueType element) {
    size_t return_iterator = position - cbegin();
    if (quantity == 0) {
        return begin() + return_iterator;
    }
    size_t ending_size = cend() - position;
    size_t beginning_size = position - cbegin();
    if (quantity + size() > capacity()) {
        CCircularBuffer<ValueType> new_buffer;
        new_buffer.Construct(quantity + size() + 1, 0, 0);
        for (auto tmp = cbegin(); tmp != position; ++tmp) {
            new_buffer.push_back(*tmp);
        }
        for (size_t i = 0; i < quantity; ++i) {
            new_buffer.push_back(element);
        }
        for (auto tmp = position; tmp != cend(); ++tmp) {
            new_buffer.push_back(*tmp);
        }
        swap(new_buffer);
        new_buffer.Destruct();

        return begin() + return_iterator;
    }
    if (beginning_size <= ending_size) {
        iterator return_it = begin() + return_iterator;
        size_t construct_times = 0;
        iterator tmp = begin();
        if (quantity > data_begin) {
            data_begin = capacity_ - (quantity - data_begin);
        } else {
            data_begin -= quantity;
        }
        for (int i = 0; i < beginning_size; ++i) {
            if (construct_times < quantity) {
                std::allocator_traits<Allocator>::construct(allocator, buffer + (data_begin + i) % capacity_, *tmp);
                construct_times++;
                ++tmp;
            } else {
                ++tmp;
                buffer[data_begin] = *tmp;
            }
        }
        iterator input = begin() + return_iterator;
        for (size_t i = 0; i < quantity; ++i) {
            if (construct_times < quantity) {
                std::allocator_traits<Allocator>::construct(allocator,
                                                            buffer + (data_begin + return_iterator + i) % capacity_,
                                                            element);
                construct_times++;
            } else {
                buffer[(data_begin + return_iterator + i) % capacity_] = element;
            }
        }
        return return_it;
    } else {
        size_t construct_times = 0;
        size_t index = data_end + quantity;
        iterator tmp = end();
        for (int i = 0; i < ending_size + 1; ++i) {
            if (construct_times < quantity) {
                construct_times++;
                std::allocator_traits<Allocator>::construct(allocator, buffer + index, *tmp);
                --tmp;
                --index;
            } else {
                buffer[data_begin + index] = *tmp;
                --tmp;
                --index;
            }
        }
        iterator input = begin() + return_iterator;
        for (size_t i = 0; i < quantity; ++i) {
            *input = element;
            ++input;
        }
        data_end += quantity;

        return begin() + return_iterator;
    }
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::insert(CCircularBuffer::const_iterator position, ValueType element) {
    return emplace(position, element);
}

template<class ValueType, typename Allocator>
template<class... VValueType>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::emplace(CCircularBuffer::const_iterator place, VValueType&& ... element) {
    ValueType value(std::forward<VValueType>(element)...);

    return ForEmplace(place, value);
}


template<class ValueType, typename Allocator>
CircularIterator<ValueType> CCircularBuffer<ValueType, Allocator>::ForEmplace(CCircularBuffer::const_iterator place, const ValueType& value) {
    iterator return_iterator = begin() + (place - cbegin());
    if (size() == capacity()) {
        pop_front();
    }
    iterator tmp = end();
    std::allocator_traits<Allocator>::construct(allocator, buffer + data_end, back());
    while (tmp != return_iterator && tmp != begin()) {
        --tmp;
        *(tmp + 1) = *tmp;
    }
    *return_iterator = value;
    data_end = (data_end + 1) % capacity_;
    if (data_begin == data_end) {
        std::allocator_traits<Allocator>::destroy(allocator, buffer + data_end);
        data_begin = (data_begin + 1) % capacity_;
    }

    return return_iterator;
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::clear() {
    for (int i = data_begin; i != data_end; i = (i + 1) % capacity_) {
        std::allocator_traits<Allocator>::destroy(allocator, buffer + i);
    }
    data_begin = 0;
    data_end = 0;
}

template<class ValueType, typename Allocator>
template<class forward_iterator, typename>
void CCircularBuffer<ValueType, Allocator>::assign(forward_iterator first,
                                                   forward_iterator last) {
    Destruct();
    if (first == last) {
        Construct(0, 0, 0);
    } else {
        Construct(last - first + 1, 0, last - first);
        size_t buffer_iterator = 0;
        for (auto tmp = first; tmp != last; ++tmp) {
            std::allocator_traits<Allocator>::construct(allocator, buffer + buffer_iterator, *tmp);
            ++buffer_iterator;
        }
    }
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::assign(std::initializer_list<ValueType> list) {
    assign(list.begin(), list.end());
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::assign(size_t quantity, ValueType element) {
    Destruct();
    Construct(quantity + 1, 0, quantity);
    for (int i = 0; i < capacity_ - 1; ++i) {
        std::construct_at(buffer + i, element);
    }
}

template<class ValueType, typename Allocator>
template<class ...VValueType>
void CCircularBuffer<ValueType, Allocator>::emplace_front(VValueType&& ... element) {
    ValueType value(std::forward<VValueType>(element)...);
    push_front(value);
}

template<class ValueType, typename Allocator>
template<class ...VValueType>
void CCircularBuffer<ValueType, Allocator>::emplace_back(VValueType&& ... element) {
    ValueType value(std::forward<VValueType>(element)...);
    push_back(value);
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::push_front(const ValueType& element) {
    if (data_begin > 0) {
        data_begin--;
    } else {
        data_begin = capacity_ - 1;
    }
    std::allocator_traits<Allocator>::construct(allocator, buffer + data_begin, element);
    if (data_end == data_begin) {
        if (data_end == 0) {
            data_end = capacity_ - 1;
        } else {
            data_end--;
            std::allocator_traits<Allocator>::destroy(allocator, buffer + data_end);
        }
    }
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::push_back(const ValueType& element) {
    std::allocator_traits<Allocator>::construct(allocator, buffer + data_end, element);
    data_end = (data_end + 1) % capacity_;
    if (data_end == data_begin) {
        std::allocator_traits<Allocator>::destroy(allocator, buffer + data_end);
        data_begin = (data_begin + 1) % capacity_;
    }
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::pop_front() {
    if (!empty()) {
        std::allocator_traits<Allocator>::destroy(allocator, buffer + data_begin);
        data_begin = (data_begin + 1) % capacity_;
    } else {
        std::cerr << "Can not pop empty buffer\n";
    }
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::pop_back() {
    if (!empty()) {
        if (data_end == 0) {
            data_end = capacity_ - 1;
        } else {
            data_end--;
        }
        std::allocator_traits<Allocator>::destroy(allocator, buffer + data_end);
    } else {
        std::cerr << "Can not pop empty buffer\n";
    }
}

template<class ValueType, typename Allocator>
size_t CCircularBuffer<ValueType, Allocator>::capacity() const {
    return capacity_ - 1;
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::reserve(size_t quantity) {
    if (quantity == 0) {
        std::cerr << "Buffer size must be more than 0\n";
        quantity = kDefaultBufferSize;
    }
    if (quantity == capacity_ - 1) {
        return;
    }
    CCircularBuffer<ValueType> new_buffer;
    new_buffer.Construct(quantity + 1, 0, 0);
    size_t element_counter = 0;
    for (auto tmp = begin(); tmp != end() && element_counter < quantity; ++tmp) {
        new_buffer.push_back(*tmp);
        element_counter++;
    }
    swap(new_buffer);
}

template<class ValueType, typename Allocator>
void CCircularBuffer<ValueType, Allocator>::resize(size_t quantity, const ValueType& value) {
    reserve(quantity);
    while (size() < capacity()) {
        push_back(value);
    }
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType>
CCircularBuffer<ValueType, Allocator>::erase(CCircularBuffer::const_iterator first,
                                             CCircularBuffer::const_iterator last) {
    size_t return_iterator = 0;
    const_iterator tmp = cbegin();
    while (tmp != first) {
        ++tmp;
        ++return_iterator;
    }
    ++return_iterator;
    iterator copy = begin() + return_iterator + (last - first) - 1;
    iterator rewrite = begin() + (tmp - cbegin());
    while (copy != end()) {
        *rewrite = *copy;
        std::allocator_traits<Allocator>::destroy(allocator, buffer + return_iterator + (last - first) - 1);
        ++copy;
        ++rewrite;
    }
    data_end -= last - first;

    return begin() + return_iterator;
}

template<class ValueType, typename Allocator>
CircularIterator<ValueType> CCircularBuffer<ValueType, Allocator>::erase(CCircularBuffer::const_iterator position) {
    return erase(position, position + 1);
}


#endif
