#pragma once

// fstring is a templated local-storage string with fixed capacity.

#include <concepts>
#include <cstring>
#include <string_view>

// str_subview definition below

namespace pg {

    /** Allows you to compare small portions of a str_view **/
    class str_subview {
    private:
        const char* data;
        uint32_t len;

    public:
        constexpr str_subview(const char* d, uint32_t l) : data(d), len(l) {}

        [[nodiscard]] constexpr uint32_t length() const { return len; }
        [[nodiscard]] constexpr const char* c_str() const { return data; }

        template<uint32_t N>
        bool operator==(const char (&str)[N]) const {
            if (len != N - 1) return false;
            return std::memcmp(data, str, len) == 0;
        }

        bool operator==(const char* str) const {
            for (uint32_t i = 0; i < len; ++i) {
                if (data[i] != str[i]) return false;
            }
            return str[len] == '\0';
        }

        bool operator==(const str_subview& other) const {
            if (len != other.len) return false;
            return std::memcmp(data, other.data, len) == 0;
        }
    };
}

/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */

// str_view definition below

namespace pg {
    /** Allows for memoryless setting of the precision parameter of an fstring **/
    template<uint8_t C>
    struct precision {
        static_assert(C <= 10, "Maximum float precision is 10 digits.");
        precision() = default;
    };

    /** Allows you to send an fstring along a buffer without template information. Very helpful**/
    class str_view final {
    private:
        /// Contains the data of the original fstring
        char* data;
        /// Contains the length of the original fstring
        uint32_t& len;
        /// Contains the capacity of the original fstring
        uint32_t cap;
        /// Contains precision information
        uint8_t prec;

    public:
        /// public constructor for str_view
        str_view(char* data, uint32_t& len, uint32_t cap);

        // **************
        // Utility functions
        // **************
        // <><><> Getters <><><>
        /// Returns the current length of the parent fstring
        [[nodiscard]] constexpr uint32_t length() const;
        /// Returns the capacity of the parent fstring
        [[nodiscard]] constexpr uint32_t capacity() const;
        /// Returns the data containing the parent fstring
        [[nodiscard]] constexpr const char* c_str() const;
        /// Createas a subview of the current view
        inline str_subview subview(uint32_t start, uint32_t sub_len) const;
        /// Emplaces a text at a position within the parent fstring.
        template <uint32_t N>
        void emplace(uint32_t index, const char (&str)[N]);
        ///  Emplaces a text at a position within the parent fstring.
        void emplace(uint32_t index, const char* str, uint32_t count);
        /// Finds a specific character starting from the provided position
        int find(char c, uint32_t pos = 0) const;

        // **************
        // Operator overloads
        // **************

        // <><><> Equality <><><>
        /// Returns the character at the requested position.
        char& operator[](uint32_t index) const;
        /// Checks if an fstring and a predefined const character are equal
        template<uint32_t N>
        bool operator==(const char (&str)[N]);
        /// Checks if an fst_view and a const char of undefined length are equal
        bool operator==(const char* str) const;

        // <><><> Piping Into String <><><>
        /// Adds a constant character with a literal into the parent fstring
        template<uint32_t N>
        str_view& operator<<(const char (&str)[N]);
        /// Adds a constant character of unknown length into the current fstring
        str_view& operator<<(const char* str);
        /// Adds an integer into the current fstring
        str_view& operator<<(int num);
        /// Converts from generic size to int
        str_view& operator<<(size_t num) { return *this << static_cast<int>(num); }
        /// Converts from generic type to int
        template<std::integral K>
        str_view& operator<<(K integer) { return *this << static_cast<int>(integer); }
        /// Lets us set the precision for floats
        template<uint8_t P>
        str_view& operator<<(precision<P> precision);
        /// Lets us accept floats as well
        str_view& operator<<(float num);
        /// Allows for doules as well
        str_view& operator<<(double num) { return *this << static_cast<float>(num); }
        /// Also characters, because why not
        str_view& operator<<(char c);
        /// Hexidecimals as well
        str_view& operator<<(const void*);
        /// Allows you to put in another str_view
        str_view& operator<<(const str_view &other);
        /// Also allows for booleans
        str_view& operator<<(bool b);

    };
}


/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */

// str_view definition below

#include <algorithm>
#include <stdexcept>
#include <charconv>

/** @brief Throws an exception for str_view errors */
static void fstr_view_throw_err(const char err[]) {
    throw std::runtime_error(err);
}

namespace pg {

// **************************************
// Construction
// **************************************
/** @brief Construct a str_view referencing an existing buffer
 *  @param data Pointer to the buffer
 *  @param len Reference to the current length
 *  @param cap Maximum capacity of the buffer (excluding null terminator)
 */
inline str_view::str_view(char* data, uint32_t& len, uint32_t cap)
    : data(data), len(len), cap(cap) {}

// **************************************
// Operations
// **************************************

constexpr uint32_t str_view::length() const { return len; }
constexpr uint32_t str_view::capacity() const { return cap; }

/** @brief Get C-style null-terminated string */
constexpr const char* str_view::c_str() const {
    if (len < cap) data[len] = '\0';
    return data;
}

/** @brief Get a subview of the string */
inline str_subview str_view::subview(uint32_t start, uint32_t sub_len) const {
    if (start + sub_len > len)
        fstr_view_throw_err("str_view::subview, out-of-bounds access");
    return {data + start, sub_len};
}

// **************************************
// Element Access
// **************************************
inline char& str_view::operator[](uint32_t index) const {
    if (index >= len)
        fstr_view_throw_err("str_view::operator[], access out of bounds memory.");
    return data[index];
}

// **************************************
// modification
// ***********************************
template <uint32_t N>
inline void str_view::emplace(uint32_t index, const char(&str)[N]) {
    if (index + (N - 1) > cap)
        fstr_view_throw_err("str_view::emplace, out-of-bounds memory");
    std::memcpy(data + index, str, N - 1);
    if (index + (N - 1) > len) len = index + (N - 1);
    data[len] = '\0';
}

// ReSharper disable once CppMemberFunctionMayBeConst
inline void str_view::emplace(const uint32_t index, const char* str, const uint32_t count) {
    if (!str) return;
    if (index + count > cap)
        fstr_view_throw_err("str_view::emplace, out-of-bounds memory");
    std::memcpy(data + index, str, count);
    if (index + count > len) len = index + count;
    data[len] = '\0';
}

inline int str_view::find(char c, uint32_t pos) const {
    while (pos < len) {
        if (data[pos] == c)
            return pos;
        pos++;
    }
    return -1;
}

// **************************************
// Equality tests
// **************************************
template<uint32_t N>
inline bool str_view::operator==(const char(&str)[N]) {
    if (len != N - 1) return false;
    return std::memcmp(data, str, len) == 0;
}

inline bool str_view::operator==(const char* str) const {
    for (uint32_t i = 0; i < len; ++i) {
        if (data[i] != str[i]) return false;
    }
    return str[len] == '\0';
}

// **************************************
// Append / stream operators
// **************************************
template<uint32_t N>
inline str_view& str_view::operator<<(const char(&str)[N]) {
    const uint32_t bytes = std::min<uint32_t>(N - 1, cap - len);
    std::memcpy(data + len, str, bytes);
    len += bytes;
    data[len] = '\0';
    return *this;
}

inline str_view& str_view::operator<<(const char* str) {
    if (!str) return (*this) << "nullptr";
    const uint32_t bytes = std::min<uint32_t>(std::strlen(str), cap - len);
    std::memcpy(data + len, str, bytes);
    len += bytes;
    data[len] = '\0';
    return *this;
}

inline str_view& str_view::operator<<(char c) {
    if (len < cap) data[len++] = c;
    data[len] = '\0';
    return *this;
}

inline str_view& str_view::operator<<(int num) {
    auto* start = data + len;
    auto* end = data + cap;
    auto res = std::to_chars(start, end, num);
    len += res.ptr - start;
    data[len] = '\0';
    return *this;
}

inline str_view& str_view::operator<<(float num) {
    auto* start = data + len;
    auto* end = data + cap;
    auto res = std::to_chars(start, end - 1, num, std::chars_format::fixed, prec);
    len += res.ptr - start;
    data[len] = '\0';
    return *this;
}

template<uint8_t P>
inline str_view& str_view::operator<<(precision<P>) {
    prec = P;
    return *this;
}

inline str_view& str_view::operator<<(bool b) {
    return *this << (b ? "true" : "false");
}

inline str_view& str_view::operator<<(const void* ptr) {
    if (!ptr) return (*this) << "nullptr";
    auto addr = reinterpret_cast<uintptr_t>(ptr);
    *this << "0x";
    auto* start = data + len;
    auto* end = data + cap;
    auto res = std::to_chars(start, end - 1, addr, 16);
    len += res.ptr - start;
    data[len] = '\0';
    return *this;
}

}



/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */

// fstring declaration below

namespace pg {

    /** @brief @code fstring@endcode is a custom string class that is much faster for comparison than std::string, but cannot be resized.
     * Create an fstring with @code fstring<size> varName@endcode.
     * @details @code fstring@endcode only has two member variables: @code uint_16 len@endcode and @code char arr[len]@endcode.
     * Note that this means the maximum length for any fstring is 65534 characters (+1 for null terminator)
     */
    template<uint32_t C>
    class fstring {
        static_assert(C >= 2 && C <= 65533, "fstring capacity must be at least 2, and less than 65533");
    private:
        /// Current length of the fstring
        uint32_t len;
        /// Array containing the information of the fstring
        char arr[C + 1];
        /// Current precision of the array
        uint8_t prec = 10;

    public:
        // **************
        // Constructors
        // **************
        /// Constructor for constant characters
        template<uint16_t N>
        constexpr fstring(const char (&str)[N]); // NOLINT(*-explicit-constructor)
        /// Constructor for constant character without length info
        constexpr fstring(const char* str); // NOLINT(*-explicit-constructor)
        /// Constructor for a singular character
        constexpr explicit fstring(char c);
        /// Nothing in the constructor
        constexpr fstring();

        // **************
        // Utility functions
        // **************
        // <><><> Getters <><><>
        /// Returns the current length of the fstring
        [[nodiscard]] constexpr uint32_t length() const;
        /// Returns the capacity of the fstring
        [[nodiscard]] static constexpr uint32_t capacity();
        /// Returns the data containing the fstring
        [[nodiscard]] const char* c_str();
        /// Returns the data in the fstring
        [[nodiscard]] char* data();
        /// Returns true if the fstring is empty
        [[nodiscard]] bool empty() const;
        // <><><> Modifiers <><><>
        /// Clears out all the data in the fstring
        void clear();
        /// Pops the entire fstring, setting length to 0 and returning a copy
        fstring pop();
        /// Wraps up a view for the current fstring
        str_view wrap();
        /// Resizes such that the fstring is of a certain size
        template<size_t N>
            requires(N <= C)
        void resize();

        // <><><> Emplacing <><><>
        /// Emplace text at a position within the string
        template<uint32_t N>
        void emplace(uint32_t index, const char (&str)[N]) requires(index + N - 1 <= C);
        /// Does the same but with a specified count
        void emplace(uint32_t index, const char *str,uint32_t count)requires(index + count <= C);

        /// Gets the substring with @code <N>@endcode elements at index @code uint32_t index@endcode
        /// @return @code fstring<N>@endcode, an fstring with @code N@endcode elements.
        template<uint32_t N>
        [[nodiscard]] constexpr fstring<N> substr(uint32_t index) const;

        // **************
        // Operator overloads
        // **************

        // <><><> Equality <><><>
        /// Returns the character at the requested position.
        constexpr char& operator[](uint32_t index);
        /// Checks if an fstring and a predefined const character array are equal
        template<uint32_t N>
        bool operator==(const char (&str)[N]) const;
        /// Checks if an fstring and a const char of undefined length are equal
        bool operator==(const char* str) const;

        // <><><> Piping Into String <><><>
        /// Adds another fstring into the current fstring
        template<uint32_t N>
        fstring<C>& operator<<(fstring<N>& other);
        /// Adds a constant character with a literal into the fstring
        template<uint32_t N>
        fstring<C>& operator<<(const char (&str)[N]);
        /// Adds a constant character of unknown length into the current fstring
        fstring<C>& operator<<(const char* str);
        /// Adds a string view into the current fstring
        fstring<C>& operator<<(std::string_view);

        // <><><> Specific types <><><>
        /// Adds an integer into the current fstring
        fstring<C>& operator<<(int num);
        /// Adds doubles into the current fstring. Static casts to float first.
        fstring<C>& operator<<(double num) { return *this << static_cast<float>(num); }
        /// Lets us set the precision for floats
        template<uint8_t P>
        fstring<C>& operator<<(precision<P> precision);
        /// Lets us accept floats as well
        fstring<C>& operator<<(float num);
        /// Lets us take unsigned integers
        template<typename T>
            requires(std::is_unsigned_v<T>)
        fstring<C>& operator<<(T num) { return *this << static_cast<int>(num); }
        /// Also characters, because why not
        fstring<C>& operator<<(char c);
        /// Hexadecimals as well
        fstring<C>& operator<<(const void*);
        /// Also boolean reading
        fstring<C>& operator<<(bool b);
    };

}

/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */
/* ******************************************************************** */

// fstring implementation below

static inline void fstring_throw_err(const char err[]) {
    throw std::runtime_error(err);
}

// ****************************** -------------------
// Constructors
// ****************************** -------------------

template<uint32_t C>
template<uint16_t N>
constexpr pg::fstring<C>::fstring(const char (&str)[N]) { // NOLINT(*-explicit-constructor, *-pro-type-member-init)
    // Gets the length of our string
    len = std::min<uint32_t>(N - 1, C - 1);
    // Allocate memory for array
    std::copy(str, str + len, arr);
    // Assign our null character
    arr[len] = '\0';
    // Assign our precision identifier
    prec = 10;
}
/// Constructor for constant character without length info
template<uint32_t C>
constexpr pg::fstring<C>::fstring(const char* str) { // NOLINT(*-pro-type-member-init)
    // If there's nothing there
    if (!str) {
        len = 0;
        arr[0] = '\0';
        return;
    }
    // Gets the lenght of our string
    len = std::min<uint32_t>(strlen(str), C - 1);
    // Allocate memory for array with builtin memcpy
    std::memcpy(arr, str, len);
    // Assign our null character
    arr[len] = '\0';
    // Assign our precision identifier
    prec = 10;
}

/// Constructor for a singular character
template<uint32_t C>
constexpr pg::fstring<C>::fstring(char c) { // NOLINT(*-pro-type-member-init)
    // Length equals one
    len = 1;
    // Assign the first value
    arr[0] = c;
    arr[1] = '\0';
    // Assign our precision identifier
    prec = 10;
}
/// Nothing in the constructor
template<uint32_t C>
constexpr pg::fstring<C>::fstring() { // NOLINT(*-pro-type-member-init)
    len = 0;
    arr[0] = '\0';
    // Assign our precision identifier
    prec = 10;
}

// ****************************** -------------------
// Utility functions
// ****************************** -------------------

template<uint32_t C>
constexpr uint32_t pg::fstring<C>::length() const {
    return len;
}

template<uint32_t C>
constexpr uint32_t pg::fstring<C>::capacity() {
    return C;
}

template<uint32_t C>
const char* pg::fstring<C>::c_str()  {
    arr[len] = '\0';
    return &arr[0];
}

template<uint32_t C>
char* pg::fstring<C>::data() {
    arr[len] = '\0';
    return &arr[0];
}

template<uint32_t C>
void pg::fstring<C>::clear() {
    len = 0;
}

template<uint32_t C>
pg::fstring<C> pg::fstring<C>::pop() {
    fstring<C> ret = *this;
    clear();
    return ret;
}

template<uint32_t C>
pg::str_view pg::fstring<C>::wrap() {
    return str_view(arr, len, C);
}

template<uint32_t C>
template<size_t N>
    requires(N <= C)
void pg::fstring<C>::resize() {
    len = N - 1;
}

template<uint32_t C>
bool pg::fstring<C>::empty() const {
    return len == 0;
}

template<uint32_t C> template <uint32_t N>
void pg::fstring<C>::emplace(uint32_t index, const char (&str)[N])
    requires(index + N - 1 <= C)
{
    memcpy(arr + index, str, N - 1);
    if (index + (N - 1) > len) {
        len = index + (N - 1);
        arr[len] = '\0';
    }
}

template<uint32_t C>
void pg::fstring<C>::emplace(uint32_t index, const char* str, uint32_t count)
    requires(index + count <= C)
{
    if (!str) return; // nothing to copy
    std::memcpy(arr + index, str, count);
    // Update length if we extended past current len
    if (index + count > len) {
        len = index + count;
        arr[len] = '\0';
    }
}

template<uint32_t C>
template<uint32_t N>
constexpr pg::fstring<N> pg::fstring<C>::substr(uint32_t index) const {
    if (N > C - index)
        fstring_throw_err("fstring<C>::substr, access out of bounds memory.");
    return fstring<N>(arr + index, std::min(len - index, N));
}

// ****************************** -------------------
// Operator overloads
// ****************************** -------------------

template<uint32_t C>
constexpr char& pg::fstring<C>::operator[](uint32_t index) {
    if (index >= len)
        fstring_throw_err("fstring<C>::operator[], access out of bounds memory.");
    return arr[index];
}

//<><><><><>
// Equality tests
// <><><><><>

template<uint32_t C>
template<uint32_t N>
bool pg::fstring<C>::operator==(const char(&str)[N]) const {
    if (len != N - 1) return false;
    return std::memcmp(arr, str, len) == 0;
}

template<uint32_t C>
bool pg::fstring<C>::operator==(const char *str) const{
    for (uint_fast32_t i = 0; i < len; i++) {
        if (arr[i] != str[i]) return false;
    }
    return str[len] == '\0';
}

template<uint32_t C>
template<uint32_t N>
pg::fstring<C>& pg::fstring<C>::operator<<(fstring<N> &other) {
    const uint32_t bytes = std::min<uint32_t>( other.length(), C - len);
    memcpy(arr + len, other.c_str(), bytes);
    len += bytes;
    return *this;
}

template<uint32_t C>
template<uint32_t N>
pg::fstring<C> & pg::fstring<C>::operator<<(const char(&str)[N]) {
    const uint32_t bytes = std::min<uint32_t>( N, C - len);
    memcpy(arr + len, str, bytes);
    len += bytes;
    return *this;
}

template<uint32_t C>
pg::fstring<C> & pg::fstring<C>::operator<<(const char *str) {
    if (!str) return (*this) << "null";
    const uint32_t bytes = std::min<uint32_t>( strlen(str), C - len);
    memcpy(arr + len, str, bytes);
    len += bytes;
    return *this;
}

template<uint32_t C>
pg::fstring<C>& pg::fstring<C>::operator<<(std::string_view v) {
    const uint32_t bytes = std::min<uint32_t>(v.length(), C - len);
    std::memcpy(arr + len, v.data(), bytes);
    len += bytes;
    return *this;
}

template<uint32_t C>
pg::fstring<C>& pg::fstring<C>::operator<<(const int num) {
    auto* start = arr + len;
    auto res = std::to_chars(start, arr + C, num);
    len += res.ptr - start;
    return *this;
}

template<uint32_t C>
template<uint8_t P>
pg::fstring<C> & pg::fstring<C>::operator<<(precision<P>) {
    arr[C + 1] = P;
    return *this;
}

template<uint32_t C>
pg::fstring<C> & pg::fstring<C>::operator<<(float num) {
    auto* start = arr + len;
    auto* end = arr + C;
    auto res = std::to_chars(start, end, num, std::chars_format::fixed, prec);
    if (res.ec == std::errc::value_too_large) {
        return *this;
    }
    len += res.ptr - start;
    return *this;
}

template<uint32_t C>
pg::fstring<C> & pg::fstring<C>::operator<<(char c) {
    if (len != C - 1)
        arr[len++] = c;
    return *this;
}

template<uint32_t C>
pg::fstring<C>& pg::fstring<C>::operator<<(const void* ptr) {
    if (!ptr) return (*this) << "nullptr";
    auto addr = reinterpret_cast<uintptr_t>(ptr);
    *this << "0x";
    auto* start = arr + len;
    auto* end = arr + C;
    auto res = std::to_chars(start, end, addr, 16);
    len += res.ptr - start;
    return *this;
}

template<uint32_t C>
pg::fstring<C> & pg::fstring<C>::operator<<(bool b) {
    return *this << (b ? "true" : "false");
}

inline pg::str_view& pg::str_view::operator<<(const pg::str_view &other) {
    const uint32_t bytes = std::min<uint32_t>( other.length(), cap - len);
    std::memmove(data + len, other.c_str(), bytes);
    len += bytes;
    return *this;
}

template<uint32_t N, uint32_t C>
pg::fstring<N+C> operator+(const pg::fstring<N> self, const pg::fstring<C>& other) {
    pg::fstring<N + C> ret;
    ret << self << other;
    return ret;
}