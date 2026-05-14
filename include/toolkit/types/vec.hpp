#pragma once
#include <cmath>

/* Created by Gordie Novak on 4/5/26.
 * Purpose: 
 * Holds all the vector types */

namespace pig {

    // -------------------------- //
    struct vec2 {
        union { float x, w; };
        union { float y, h; };

        constexpr vec2() = default;
        constexpr vec2(float x, float y = {}) : x(x), y(y) {}

        [[nodiscard]] constexpr vec2 unit() const; ///< Returns the unit vector. Will not error on zero magnitude.
        [[nodiscard]] constexpr float mag() const; ///< Returns the magnitude of the vector.
        [[nodiscard]] constexpr vec2 square() const; ///< Returns the magnitude squared of the vector.
        [[nodiscard]] constexpr float dot(const vec2&) const;

        constexpr void operator+=(const vec2&);
        constexpr void operator-=(const vec2&);
        constexpr void operator*=(float);
        constexpr void operator/=(float);
    };

    // -------------------------- //
    struct vec3 {
        union { float x, r; };
        union { float y, g; };
        union { float z, b; };

        constexpr vec3() = default;
        constexpr vec3(float x, float y = {}, float z = {}) : x(x), y(y), z(z) {}

        [[nodiscard]] constexpr vec3 unit() const; ///< Returns the unit vector. Will not error on zero magnitude.
        [[nodiscard]] constexpr float mag() const; ///< Returns the magnitude of the vector.
        [[nodiscard]] constexpr vec3 cross(const vec3&) const;
        [[nodiscard]] constexpr vec3 square() const;
        [[nodiscard]] constexpr float dot(const vec3&) const;

        constexpr void operator+=(const vec3&);
        constexpr void operator-=(const vec3&);
        constexpr void operator*=(float);
        constexpr void operator/=(float);
    };

    // -------------------------- //
    struct vec4 {
        union { float x, r; };
        union { float y, g; };
        union { float z, b; };
        union { float w, a; };

        constexpr vec4() = default;
        constexpr vec4(float x, float y = {}, float z = {}, float w = {}) : x(x), y(y), z(z), w(w) {}

        [[nodiscard]] constexpr vec4 unit() const; ///< Returns the unit vector. Will not error on zero magnitude.
        [[nodiscard]] constexpr float mag() const; ///< Returns the magnitude of the vector.
        [[nodiscard]] constexpr vec4 square() const; ///< Returns the magnitude squared of the vector.
        [[nodiscard]] constexpr float dot(const vec4&) const;

        constexpr void operator+=(const vec4&);
        constexpr void operator-=(const vec4&);
        constexpr void operator*=(float);
        constexpr void operator/=(float);
    };


    // -------- vec2 operations --------- //
    [[nodiscard]] constexpr vec2 unit(const vec2&);
    [[nodiscard]] constexpr float mag(const vec2&);
    [[nodiscard]] constexpr vec2 square(const vec2&);
    [[nodiscard]] constexpr float dot(const vec2&, const vec2&);
    [[nodiscard]] constexpr vec2 NDC(const vec2&);
    [[nodiscard]] constexpr vec2 operator+(const vec2&, const vec2&);
    [[nodiscard]] constexpr vec2 operator-(const vec2&, const vec2&);
    [[nodiscard]] constexpr vec2 operator*(const vec2&, float);
    [[nodiscard]] constexpr vec2 operator/(const vec2&, float);


    // -------- vec3 operations --------- //
    [[nodiscard]] constexpr vec3 unit(const vec3&);
    [[nodiscard]] constexpr float mag(const vec3&);
    [[nodiscard]] constexpr vec3 cross(const vec3&, const vec3&);
    [[nodiscard]] constexpr vec3 square(const vec3&);
    [[nodiscard]] constexpr float dot(const vec3&, const vec3&);
    [[nodiscard]] constexpr vec3 operator+(const vec3& lhs, const vec3& rhs);
    [[nodiscard]] constexpr vec3 operator-(const vec3& lhs, const vec3& rhs);
    [[nodiscard]] constexpr vec3 operator*(const vec3& lhs, float f);
    [[nodiscard]] constexpr vec3 operator/(const vec3& lhs, float f);

    // -------- vec4 operations --------- //
    [[nodiscard]] constexpr vec4 unit(const vec4&);
    [[nodiscard]] constexpr float mag(const vec4&);
    [[nodiscard]] constexpr vec4 square(const vec4&);
    [[nodiscard]] constexpr float dot(const vec4&, const vec4&);

    [[nodiscard]] constexpr vec4 operator+(const vec4& lhs, const vec4& rhs);
    [[nodiscard]] constexpr vec4 operator-(const vec4& lhs, const vec4& rhs);
    [[nodiscard]] constexpr vec4 operator*(const vec4& lhs, float f);
    [[nodiscard]] constexpr vec4 operator/(const vec4& lhs, float f);
}





// implementation

namespace pig {
    // vec2

constexpr vec2 vec2::unit() const {
    return *this/sqrtf((x*x) + (y*y));
}

constexpr float vec2::mag() const {
    return sqrtf((x*x) + (y*y));
}

constexpr vec2 vec2::square() const {
    return {x*x, y*y};
}

constexpr float vec2::dot(const vec2& v) const {
    return x*v.x + y*v.y;
}

constexpr void vec2::operator+=(const vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
}
constexpr void vec2::operator-=(const vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
}
constexpr void vec2::operator*=(const float f) {
    x *= f;
    y *= f;
}
constexpr void vec2::operator/=(float f) {
    x /= f;
    y /= f;
}

constexpr vec2 unit(const vec2& v) {
    return v/sqrtf((v.x*v.x) + (v.y*v.y));
}

constexpr float mag(const vec2& v) {
    return sqrtf((v.x*v.x) + (v.y*v.y));
}

constexpr vec2 square(const vec2& v) {
    return v.square();
}

constexpr float dot(const vec2& v, const vec2& w) { return v.dot(w); }

constexpr vec2 NDC(const vec2& v) {
    vec2 norm = unit(v);
    return (norm - vec2{0.5f, 0.5f})*2.f;
}

constexpr vec2 operator+(const vec2& lhs, const vec2& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
constexpr vec2 operator-(const vec2& lhs, const vec2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
constexpr vec2 operator*(const vec2& lhs, const float f) {
    return {lhs.x * f, lhs.y * f};
}
constexpr vec2 operator/(const vec2& lhs, const float f) {
    return {lhs.x / f, lhs.y / f};
}


// vec3

constexpr vec3 vec3::unit() const {
    return *this/((x*x)+(y*y)+z*z);
}

constexpr float vec3::mag() const {
    return sqrtf((x*x) + (y*y) + (z*z));
}

constexpr vec3 vec3::cross(const vec3& v) const {
    return {y*v.z - z*v.y, -x*v.z - z*v.x, x*v.y - y*v.x};
}

constexpr vec3 vec3::square() const {
    return {x*x, y*y, z*z};
}

constexpr float vec3::dot(const vec3&) const {
    return x*x + y*y + z*z;
}

constexpr void vec3::operator+=(const vec3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}
constexpr void vec3::operator-=(const vec3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}
constexpr void vec3::operator*=(const float f) {
    x *= f;
    y *= f;
    z *= f;
}
constexpr void vec3::operator/=(float f) {
    x /= f;
    y /= f;
    z /= f;
}

constexpr vec3 unit(const vec3& v) {
    return v.unit();
}

constexpr float mag(const vec3& v) {
    return v.mag();
}

constexpr vec3 cross(const vec3& lhs, const vec3& rhs) {
    return lhs.cross(rhs);
}

constexpr vec3 square(const vec3& v) {
    return v.square();
}

constexpr float dot(const vec3& v, const vec3& w) {
    return v.dot(w);
}



constexpr vec3 operator+(const vec3& lhs, const vec3& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}
constexpr vec3 operator-(const vec3& lhs, const vec3& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}
constexpr vec3 operator*(const vec3& lhs, const float f) {
    return {lhs.x * f, lhs.y * f, lhs.z * f};
}
constexpr vec3 operator/(const vec3& lhs, const float f) {
    return {lhs.x / f, lhs.y / f, lhs.z / f};
}



// vec4

constexpr vec4 vec4::unit() const {
    return *this/((x*x)+(y*y)+(z*z)+(w*w));
}

constexpr float vec4::mag() const {
    return sqrtf((x*x) + (y*y) + (z*z) + (w*w));
}

constexpr vec4 vec4::square() const {
    return {x*x, y*y, z*z, w*w};
}

constexpr float vec4::dot(const vec4&) const {
    return x*x + y*y + z*z + w*w;
}

constexpr void vec4::operator+=(const vec4& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
}
constexpr void vec4::operator-=(const vec4& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
}
constexpr void vec4::operator*=(const float f) {
    x *= f;
    y *= f;
    z *= f;
    w *= f;
}
constexpr void vec4::operator/=(float f) {
    x /= f;
    y /= f;
    z /= f;
    w /= f;
}

constexpr vec4 unit(const vec4& v) {
    return v.unit();
}

constexpr float mag(const vec4& v) {
    return v.mag();
}

constexpr vec4 square(const vec4& v) {
    return v.square();
}

constexpr float dot(const vec4& v, const vec4& w) {
    return v.dot(w);
}

constexpr vec4 operator+(const vec4& lhs, const vec4& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}
constexpr vec4 operator-(const vec4& lhs, const vec4& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
}
constexpr vec4 operator*(const vec4& lhs, const float f) {
    return {lhs.x * f, lhs.y * f, lhs.z * f, lhs.w * f};
}
constexpr vec4 operator/(const vec4& lhs, const float f) {
    return {lhs.x / f, lhs.y / f, lhs.z / f, lhs.w / f};
}

    
}