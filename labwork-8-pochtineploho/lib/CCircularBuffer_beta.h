/*
    This realization of Circular Buffer is more memory efficient, but some std functions (like std::sort) don't work properly,
    because begin == end. In the new realization I will allocate extra sizeof(T) memory.
*/

#ifndef LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFER_BETA_H
#define LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFER_BETA_H

#include <iterator>
#include <iostream>

const size_t kDefaultBufferSize = 100;
const size_t kMaxContainerSize = std::string::npos;

template<class T>
class CircularAllocator : public std::allocator<T> {
public:
    T* allocate_memory(size_t size) {
        T* buffer;
        std::allocator<T> allocator;
        buffer = allocator.allocate(size);
        for (int i = 0; i < size; ++i) {
            std::construct_at(buffer + i, T());
        }
        return buffer;
    }

    void deallocate_memory(T* buffer, size_t size) {
        std::allocator<T> allocator;
        std::destroy_n(buffer, size);
        allocator.deallocate(buffer, size);
    }
};

template<class T, typename Allocator=CircularAllocator<T>>
class CCircularBuffer;

template<class T>
class CircularIterator : public std::iterator<std::input_iterator_tag, T> {
    friend CCircularBuffer<T>;
public:
    CircularIterator(const CircularIterator& iter) :
            pointer(iter.pointer),
            begin_pointer(iter.begin_pointer),
            end_pointer(iter.end_pointer),
            loop(iter.loop) {}

    CircularIterator& operator=(const CircularIterator& iter) {
        if (this == &iter) {
            return *this;
        }
        pointer = iter.pointer;
        begin_pointer = iter.begin_pointer;
        end_pointer = iter.end_pointer;
        loop = iter.loop;

        return *this;
    }

    bool operator==(CircularIterator const& rhs) const {
        return pointer == rhs.pointer && loop == rhs.loop;
    }

    bool operator==(T* rhs) const {
        return pointer == rhs;
    }

    bool operator!=(CircularIterator const& rhs) const {
        return pointer != rhs.pointer || (loop != rhs.loop);
    }

    CircularIterator& operator++() {
        if (pointer == end_pointer) {
            pointer = begin_pointer;
            loop = true;
        } else {
            ++pointer;
            loop = true;
        }
        return *this;
    }

    CircularIterator operator++(int) {
        if (pointer == end_pointer) {
            pointer = begin_pointer;
            loop = true;
        } else {
            ++pointer;
            loop = true;
        }
        return *this;
    }

    CircularIterator operator--() {
        if (pointer == begin_pointer) {
            pointer = end_pointer;
            loop = true;
        } else {
            --pointer;
            loop = true;
        }
        return *this;
    }

    CircularIterator operator--(int) {
        if (pointer == begin_pointer) {
            pointer = end_pointer;
            loop = true;
        } else {
            --pointer;
            loop = true;
        }
        return *this;
    }

    CircularIterator& operator+=(long long number) {
        typename std::iterator_traits<CircularIterator>::difference_type m = number;
        if (m >= 0) {
            while (m--) {
                ++*this;
            }
        } else {
            while (m++) {
                --*this;
            }
        }
        return *this;
    }

    CircularIterator& operator-=(long long number) {
        return *this += -number;
    }

    CircularIterator operator+(long long number) const {
        CircularIterator<T> tmp = *this;
        return tmp += number;
    }

    CircularIterator operator-(long long number) const {
        CircularIterator<T> tmp = *this;
        return tmp -= number;
    }

    long long operator-(const CircularIterator& rhs) const {
        long long tmp = pointer - rhs.pointer;
        return tmp;
    }

    T& operator[](long long number) {
        return *(*this + number);
    }

    bool operator<(const CircularIterator& rhs) const {
        return (*this - rhs > 0);
    }

    bool operator>(const CircularIterator& rhs) const {
        return (rhs < *this);
    }

    bool operator<=(const CircularIterator& rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const CircularIterator& rhs) const {
        return !(*this < rhs);
    }

    T& operator*() const {
        return *pointer;
    }

    explicit CircularIterator(T* point, const CCircularBuffer<T>& buffer, bool different) :
            begin_pointer(buffer.buffer),
            end_pointer(buffer.buffer + buffer.size_ - 1),
            pointer(point),
            loop(different) {}

    explicit CircularIterator(T* point, T* buffer, size_t size) :
            begin_pointer(buffer),
            end_pointer(buffer + size - 1),
            pointer(point),
            loop(true) {}

    T* where() {
        return pointer;
    }

    bool was() {
        return loop;
    }

    ~CircularIterator() = default;

private:
    T* begin_pointer;
    T* end_pointer;
    bool loop;

    T* pointer;
};

template<class T, typename Allocator>
class CCircularBuffer {
    friend CircularIterator<T>;
public:
    typedef CircularIterator<T> iterator;
    typedef CircularIterator<const T> const_iterator;

    CCircularBuffer();

    explicit CCircularBuffer(size_t size);

    CCircularBuffer(CCircularBuffer const& copy);

    CCircularBuffer& operator=(CCircularBuffer const& copy);

    CCircularBuffer(size_t quantity, T element);

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    CCircularBuffer(forward_iterator first, forward_iterator last);

    CCircularBuffer(std::initializer_list<T> list);

    CCircularBuffer& operator=(std::initializer_list<T> list);

    virtual ~CCircularBuffer();

    iterator begin();

    iterator end();

    const_iterator cbegin() const;

    const_iterator cend() const;

    bool operator==(const CCircularBuffer<T>& rhs) const;

    bool operator!=(const CCircularBuffer<T>& rhs) const;

    void swap(CCircularBuffer<T>& rhs);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t max_size() const;

    [[nodiscard]] bool empty() const;

    void clear();

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    void assign(forward_iterator first, forward_iterator last);

    void assign(std::initializer_list<T> list);

    void assign(size_t quantity, T element);

    const T& front() const;

    const T& back() const;

    iterator insert(const_iterator iter, std::initializer_list<T> list);

    template<class forward_iterator, typename = std::_RequireInputIter<forward_iterator>>
    iterator insert(const_iterator iter, forward_iterator first, forward_iterator last);

    iterator insert(const_iterator iter, size_t quantity, T element);

    iterator insert(const_iterator iter, T element);

    template<class ...TT>
    iterator emplace(iterator iter, TT&& ... element);

    template<class ...TT>
    void emplace_front(TT&& ... element);

    template<class ...TT>
    void emplace_back(TT&& ... element);

    virtual void push_front(const T& element);

    virtual void push_back(const T& element);

    void pop_front();

    void pop_back();

    void reserve(size_t quantity);

    void resize(size_t quantity);

    void resize(size_t quantity, const T& value);

    [[nodiscard]] size_t capacity() const;

    T& operator[](size_t index);

    T& operator[](size_t index) const;

    T& at(size_t index);

    template<typename TT>
    friend void swap(CCircularBuffer<TT>& lhs, CCircularBuffer<TT>& rhs);

protected:
    T* buffer;
    size_t size_{};
    size_t data_begin{};
    size_t data_end{};
    bool empty_{};
};

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::CCircularBuffer() {
    size_ = kDefaultBufferSize;
    Allocator allocator;
    buffer = allocator.allocate_memory(size_);
    data_begin = 0;
    data_end = 0;
    empty_ = true;
}


template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::CCircularBuffer(size_t size) {
    if (size == 0) {
        std::cerr << "Buffer size must be more than 0\n";
        size = kDefaultBufferSize;
    }
    size_ = size;
    empty_ = true;
    Allocator allocator;
    buffer = allocator.allocate_memory(size_);
    data_begin = 0;
    data_end = 0;
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>& CCircularBuffer<T, Allocator>::operator=(const CCircularBuffer& copy) {
    if (this != &copy) {
        Allocator allocator;
        allocator.deallocate_memory(buffer, size_);
        size_ = copy.size_;
        buffer = allocator.allocate_memory(size_);
        data_begin = copy.data_begin;
        data_end = copy.data_end;
        for (int i = 0; i < size_; ++i) {
            buffer[i] = copy.buffer[i];
        }
        empty_ = copy.empty_;
    }
    return *this;
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::CCircularBuffer(const CCircularBuffer& copy) {
    size_ = copy.size_;
    Allocator allocator;
    buffer = allocator.allocate_memory(size_);
    data_begin = copy.data_begin;
    data_end = copy.data_end;
    for (int i = 0; i < size_; ++i) {
        buffer[i] = copy.buffer[i];
    }
    empty_ = copy.empty_;
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::CCircularBuffer(size_t quantity, T element) {
    if (quantity == 0) {
        std::cerr << "Buffer size must be more than 0\n";
        quantity = kDefaultBufferSize;
    }
    size_ = quantity;
    empty_ = false;
    Allocator allocator;
    buffer = allocator.allocate_memory(size_);
    data_begin = 0;
    data_end = 0;
    std::fill(begin(), end(), element);
}

template<class T, typename Allocator>
template<class forward_iterator, typename>
CCircularBuffer<T, Allocator>::CCircularBuffer(forward_iterator first,
                                               forward_iterator last) {
    if (first == last) {
        empty_ = true;
        std::cerr << "Buffer size must be more than 0\n";
        buffer = nullptr;
        *this = CCircularBuffer<T>();
    } else {
        auto tmp = first;
        size_++;
        ++tmp;
        while (tmp != last) {
            ++tmp;
            size_++;
        }
        Allocator allocator;
        buffer = allocator.allocate_memory(size_);
        data_begin = 0;
        data_end = 0;
        for (int i = 0; i < size_; ++i) {
            buffer[i] = *first;
            ++first;
        }
        empty_ = false;
    }
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>& CCircularBuffer<T, Allocator>::operator=(std::initializer_list<T> list) {
    size_ = list.size();
    Allocator allocator;
    allocator.deallocate_memory(buffer, size_);
    *this = CCircularBuffer<T>(list);
    return *this;
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::CCircularBuffer(std::initializer_list<T> list) {
    size_ = list.size();
    if (list.size() == 0) {
        std::cerr << "Buffer size must be more than 0\n";
        size_ = kDefaultBufferSize;
    }
    Allocator allocator;
    buffer = allocator.allocate_memory(size_);
    data_begin = 0;
    data_end = 0;
    size_t i = 0;
    for (auto& value: list) {
        buffer[i] = value;
        i++;
    }
    empty_ = (list.size() == 0);
}

template<class T, typename Allocator>
CCircularBuffer<T, Allocator>::~CCircularBuffer() {
    Allocator allocator;
    allocator.deallocate_memory(buffer, size_);
}

template<class T, typename Allocator>
T& CCircularBuffer<T, Allocator>::operator[](size_t index) {
    index = index % size_;
    size_t real_index = (data_begin + index) % size_;
    if (empty_ || data_end > data_begin && data_end - data_begin < index ||
        data_begin > data_end && size_ - data_begin + data_end < index) {
        std::cerr << "Index out of range\n";
    }
    return buffer[real_index];
}

template<class T, typename Allocator>
T& CCircularBuffer<T, Allocator>::operator[](size_t index) const {
    index = index % size_;
    size_t real_index = (data_begin + index) % size_;
    if (empty_ || data_end > data_begin && data_end - data_begin < index ||
        data_begin > data_end && size_ - data_begin + data_end < index) {
        std::cerr << "Index out of range\n";
    }
    return buffer[real_index];
}

template<class T, typename Allocator>
T& CCircularBuffer<T, Allocator>::at(size_t index) {
    index = index % size_;
    size_t real_index = (data_begin + index) % size_;
    if (empty_ || data_end > data_begin && data_end - data_begin < index ||
        data_begin > data_end && size_ - data_begin + data_end < index) {
        throw std::out_of_range("Index out of range\n");
    }
    return buffer[real_index];
}

template<class T, typename Allocator>
CircularIterator<T> CCircularBuffer<T, Allocator>::begin() {
    return iterator(&buffer[data_begin], *this, empty_);
}

template<class T, typename Allocator>
CircularIterator<T> CCircularBuffer<T, Allocator>::end() {
    return iterator(&buffer[data_end], *this, true);
}

template<class T, typename Allocator>
CircularIterator<const T> CCircularBuffer<T, Allocator>::cbegin() const {
    return const_iterator(&buffer[data_begin], buffer, size_);
}

template<class T, typename Allocator>
CircularIterator<const T> CCircularBuffer<T, Allocator>::cend() const {
    return const_iterator(&buffer[data_end], buffer, size_);
}

template<class T, typename Allocator>
const T& CCircularBuffer<T, Allocator>::front() const {
    if (empty_) {
        std::cerr << "Index out of range\n";
        return buffer[0];
    }
    return buffer[data_begin];
}

template<class T, typename Allocator>
const T& CCircularBuffer<T, Allocator>::back() const {
    if (empty_) {
        std::cerr << "Index out of range\n";
        return buffer[0];
    }
    if (data_end == 0) {
        return buffer[size_ - 1];
    }
    return buffer[data_end - 1];
}

template<class T, typename Allocator>
bool CCircularBuffer<T, Allocator>::operator==(const CCircularBuffer<T>& rhs) const {
    if (size_ != rhs.size_)
        return false;
    auto tmp = data_begin;
    auto rhs_tmp = rhs.data_begin;
    while (tmp != data_end && rhs_tmp != rhs.data_end) {
        if (buffer[tmp] != rhs[rhs_tmp]) {
            return false;
        }
        if (tmp == size_ - 1) {
            tmp = 0;
        } else {
            tmp++;
        }
        if (rhs_tmp == size_ - 1) {
            rhs_tmp = 0;
        } else {
            rhs_tmp++;
        }
    }
    return true;
}

template<class T, typename Allocator>
bool CCircularBuffer<T, Allocator>::operator!=(const CCircularBuffer<T>& rhs) const {
    return !(*this == rhs);
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::swap(CCircularBuffer<T>& rhs) {
    std::swap(*this, rhs);
}

template<typename T>
void swap(CCircularBuffer<T>& lhs, CCircularBuffer<T>& rhs) {
    std::swap(lhs, rhs);
}

template<class T, typename Allocator>
size_t CCircularBuffer<T, Allocator>::size() const {
    if (empty_) {
        return 0;
    }
    if (data_end > data_begin) {
        return data_end - data_begin;
    }
    if (data_end == data_begin) {
        return size_;
    }
    return data_end + size_ - data_begin;

}

template<class T, typename Allocator>
size_t CCircularBuffer<T, Allocator>::max_size() const {
    return kMaxContainerSize;
}

template<class T, typename Allocator>
bool CCircularBuffer<T, Allocator>::empty() const {
    return empty_;
}

template<class T, typename Allocator>
CircularIterator<T>
CCircularBuffer<T, Allocator>::insert(CCircularBuffer::const_iterator iter, std::initializer_list<T> list) {
    auto new_buffer = CCircularBuffer<T>(*this);
    new_buffer.clear();
    iterator return_iterator = new_buffer.begin();
    iterator tmp = begin();
    const T* middle = iter.where();
    while (tmp.where() != middle || (tmp.where() == middle && tmp.was() < iter.was())) {
        new_buffer.push_back(*tmp);
        ++tmp;
        ++return_iterator;
    }
    for (auto& value: list) {
        new_buffer.push_back(value);

    }
    while (tmp != end()) {
        new_buffer.push_back(*tmp);
        ++tmp;
    }
    *this = new_buffer;
    return return_iterator;
}

template<class T, typename Allocator>
template<class forward_iterator, typename>
CircularIterator<T> CCircularBuffer<T, Allocator>::insert(CCircularBuffer::const_iterator iter, forward_iterator first,
                                                          forward_iterator last) {
    auto new_buffer = CCircularBuffer<T>(*this);
    new_buffer.clear();
    iterator return_iterator = new_buffer.begin();
    iterator tmp = begin();
    const T* middle = iter.where();
    while (tmp.where() != middle || (tmp.where() == middle && tmp.was() < iter.was())) {
        new_buffer.push_back(*tmp);
        ++tmp;
        ++return_iterator;
    }
    for (; first != last; ++first) {
        new_buffer.push_back(*first);
    }
    while (tmp != end()) {
        new_buffer.push_back(*tmp);
        ++tmp;
    }
    *this = new_buffer;
    return return_iterator;
}

template<class T, typename Allocator>
CircularIterator<T>
CCircularBuffer<T, Allocator>::insert(CCircularBuffer::const_iterator iter, size_t quantity, T element) {
    auto new_buffer = CCircularBuffer<T>(*this);
    new_buffer.clear();
    iterator return_iterator = new_buffer.begin();
    iterator tmp = begin();
    const T* middle = iter.where();
    while (tmp.where() != middle || (tmp.where() == middle && tmp.was() < iter.was())) {
        new_buffer.push_back(*tmp);
        ++tmp;
        ++return_iterator;
    }
    for (size_t i = 0; i < quantity; ++i) {
        new_buffer.push_back(element);
    }
    while (tmp != end()) {
        new_buffer.push_back(*tmp);
        ++tmp;
    }
    *this = new_buffer;
    return return_iterator;
}

template<class T, typename Allocator>
CircularIterator<T> CCircularBuffer<T, Allocator>::insert(CCircularBuffer::const_iterator iter, T element) {
    return insert(iter, 1, element);
}

template<class T, typename Allocator>
template<class... TT>
CircularIterator<T> CCircularBuffer<T, Allocator>::emplace(CCircularBuffer::iterator iter, TT&& ... element) {
    auto new_buffer = CCircularBuffer<T>(*this);
    new_buffer.clear();
    iterator return_iterator = new_buffer.begin();
    iterator tmp = begin();
    const T* middle = iter.where();
    while (tmp.where() != middle || (tmp.where() == middle && tmp.was() < iter.was())) {
        new_buffer.push_back(*tmp);
        ++tmp;
        ++return_iterator;
    }
    T value(std::forward<TT>(element)...);
    new_buffer.push_back(value);
    while (tmp != end()) {
        new_buffer.push_back(*tmp);
        ++tmp;
    }
    *this = new_buffer;
    return return_iterator;
}


template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::clear() {
    data_begin = 0;
    data_end = 0;
    empty_ = true;
}

template<class T, typename Allocator>
template<class forward_iterator, typename>
void CCircularBuffer<T, Allocator>::assign(forward_iterator first,
                                           forward_iterator last) {
    Allocator allocator;
    allocator.deallocate_memory(buffer, size_);
    if (first == last) {
        empty_ = true;
        buffer = nullptr;
        *this = CCircularBuffer<T>();
    } else {
        auto tmp = first;
        size_ = 0;
        size_++;
        ++tmp;
        while (tmp != last) {
            ++tmp;
            size_++;
        }
        buffer = allocator.allocate_memory(size_);
        data_begin = 0;
        data_end = 0;
        for (int i = 0; i < size_; ++i) {
            buffer[i] = *first;
            ++first;
        }
        empty_ = false;
    }
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::assign(std::initializer_list<T> list) {
    Allocator allocator;
    allocator.deallocate_memory(buffer, size_);
    if (list.size() == 0) {
        std::cerr << "Buffer size must be more than 0\n";
    }
    size_ = list.size();
    buffer = allocator.allocate(size_);
    for (int i = 0; i < size_; ++i) {
        std::construct_at(buffer + i, T());
    }
    data_begin = 0;
    data_end = 0;
    size_t i = 0;
    for (auto& value: list) {
        buffer[i] = value;
        i++;
    }
    empty_ = (list.size() == 0);
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::assign(size_t quantity, T element) {
    Allocator allocator;
    allocator.deallocate_memory(buffer, size_);
    *this = CCircularBuffer<T>(quantity, element);
}

template<class T, typename Allocator>
template<class ...TT>
void CCircularBuffer<T, Allocator>::emplace_front(TT&& ... element) {
    T value(std::forward<TT>(element)...);
    push_front(value);
}

template<class T, typename Allocator>
template<class ...TT>
void CCircularBuffer<T, Allocator>::emplace_back(TT&& ... element) {
    T value(std::forward<TT>(element)...);
    push_back(value);
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::push_front(const T& element) {
    if (data_begin > 0) {
        if (data_begin == data_end && !empty_) {
            data_end--;
        }
        buffer[data_begin - 1] = element;
        data_begin--;
    } else {
        if (data_begin == data_end && !empty_) {
            data_end = size_ - 1;
        }
        buffer[size_ - 1] = element;
        data_begin = size_ - 1;
    }
    empty_ = false;
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::push_back(const T& element) {
    buffer[data_end] = element;
    if (!empty_ && data_end == data_begin) {
        data_begin = (data_begin + 1) % size_;
    }
    data_end = (data_end + 1) % size_;
    empty_ = false;
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::pop_front() {
    if (!empty()) {
        data_begin = (data_begin + 1) % size_;
    }
    if (data_begin == data_end) {
        empty_ = true;
    }
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::pop_back() {
    if (!empty()) {
        if (data_end == 0) {
            data_end = size_ - 1;
        } else {
            data_end--;
        }
    }
    if (data_begin == data_end) {
        empty_ = true;
    }
}

template<class T, typename Allocator>
size_t CCircularBuffer<T, Allocator>::capacity() const {
    return size_;
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::reserve(size_t quantity) {
    if (quantity == 0) {
        std::cerr << "Buffer size must be more than 0\n";
        quantity = kDefaultBufferSize;
    }
    if (quantity == size_) {
        return;
    }
    CCircularBuffer<T> new_buffer;
    Allocator allocator;
    new_buffer.buffer = allocator.allocate_memory(quantity);
    new_buffer.size_ = quantity;
    new_buffer.data_begin = 0;
    new_buffer.data_end = 0;
    size_t element_counter = 0;
    for (auto tmp = begin(); tmp != end() && element_counter < quantity; ++tmp) {
        new_buffer.push_back(*tmp);
        element_counter++;
    }
    *this = new_buffer;
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::resize(size_t quantity, const T& value) {
    reserve(quantity);
    while (size() < capacity()) {
        push_back(value);
    }
}

template<class T, typename Allocator>
void CCircularBuffer<T, Allocator>::resize(size_t quantity) {
    resize(quantity, T());
}
#endif //LABWORK_8_POCHTINEPLOHO_CCIRCULARBUFFER_BETA_H
