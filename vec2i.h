#ifndef VEC2IIH
#define VEC2IIH

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec2i
{
public:
    int e[2];
    
    vec2i(){}
    vec2i(int e0, int e1) { e[0] = e0, e[1] = e1; }
    inline int x() const { return e[0]; }
    inline int y() const { return e[1]; }

    inline const vec2i& operator+() const { return *this; }
    inline vec2i operator-() const { return vec2i(-e[0], -e[1]); }
    inline int operator[](int i) const { return e[i]; }
    inline int& operator[](int i) { return e[i]; }

    inline vec2i& operator +=(const vec2i &v2);
    inline vec2i& operator -=(const vec2i &v2);
    inline vec2i& operator *=(const vec2i &v2);
    inline vec2i& operator /=(const vec2i &v2);
    inline vec2i& operator *=(const int t);
    inline vec2i& operator /=(const int t);

    inline int length() const{ return sqrt(e[0]*e[0] + e[1]*e[1]); }
    inline int squared_length() const{ return e[0]*e[0] + e[1]*e[1]; }
};

inline std::istream& operator>>(std::istream &is, vec2i &t) {
    is >> t.e[0] >> t.e[1];
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec2i &t) {
    os << t.e[0] << " " << t.e[1];
    return os;
}

inline vec2i operator+(const vec2i &v1, const vec2i &v2) {
    return vec2i(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1]);
}

inline vec2i operator-(const vec2i &v1, const vec2i &v2) {
    return vec2i(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1]);
}

inline vec2i operator*(const vec2i &v1, const vec2i &v2) {
    return vec2i(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1]);
}

inline vec2i operator/(const vec2i &v1, const vec2i &v2) {
    return vec2i(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1]);
}

inline vec2i operator*(int t, const vec2i &v) {
    return vec2i(t*v.e[0], t*v.e[1]);
}

inline vec2i operator/(vec2i v, int t) {
    return vec2i(v.e[0]/t, v.e[1]/t);
}

inline vec2i operator*(const vec2i &v, int t) {
    return vec2i(t*v.e[0], t*v.e[1]);
}

inline int dot(const vec2i &v1, const vec2i &v2) {
    return v1.e[0] *v2.e[0] + v1.e[1] *v2.e[1];
}

inline vec2i& vec2i::operator+=(const vec2i &v){
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    return *this;
}

inline vec2i& vec2i::operator*=(const vec2i &v){
    e[0]  *= v.e[0];
    e[1]  *= v.e[1];
    return *this;
}

inline vec2i& vec2i::operator/=(const vec2i &v){
    e[0]  /= v.e[0];
    e[1]  /= v.e[1];
    return *this;
}

inline vec2i& vec2i::operator-=(const vec2i& v) {
    e[0]  -= v.e[0];
    e[1]  -= v.e[1];
    return *this;
}

inline vec2i& vec2i::operator*=(const int t) {
    e[0]  *= t;
    e[1]  *= t;
    return *this;
}

inline vec2i& vec2i::operator/=(const int t) {
    int k = 1.0/t;
    e[0]  *= k;
    e[1]  *= k;
    return *this;
}

inline vec2i unit_vector(vec2i v) {
    return v / v.length();
}

#endif