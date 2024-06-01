#ifndef VECTOR3DD_H
#define VECTOR3DD_H

#include <cassert>
#include <cmath>

//based on Qt's Vector3DD, but with double precision to eliminate runtime conversions

class Vector3DD
{
public:
    Vector3DD() {
        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
    };
    Vector3DD(double x, double y, double z){
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    static Vector3DD isotropic_from_random(double a1_uniform_0_1, double a2_uniform_0_1){
        double phi = 2. * M_PI * a1_uniform_0_1;
        double cosTh = 1. - 2. * a2_uniform_0_1;
        double sinTh = 2. * sqrt(a2_uniform_0_1*(1.-a2_uniform_0_1));
        return Vector3DD(sinTh * cos(phi), sinTh * sin(phi), cosTh);
    }
    double &operator[](unsigned i){
        assert(i < 3);
        return v[i];
    }
    double operator[](unsigned i) const {
        assert(i < 3);
        return v[i];
    }

    double length() const;
    double lengthSquared() const;

    Vector3DD normalized() const;
    void normalize();

    inline Vector3DD &operator+=(const Vector3DD &vector);
    inline Vector3DD &operator-=(const Vector3DD &vector);
    inline Vector3DD &operator*=(double factor);
    inline Vector3DD &operator*=(const Vector3DD& vector);
    inline Vector3DD &operator/=(double divisor);
    inline Vector3DD &operator/=(const Vector3DD &vector);

    static double dotProduct(const Vector3DD& v1, const Vector3DD& v2); //In Qt 6 convert to inline and constexpr
    static Vector3DD crossProduct(const Vector3DD& v1, const Vector3DD& v2); //in Qt 6 convert to inline and constexpr


    friend inline bool operator==(const Vector3DD &v1, const Vector3DD &v2);
    friend inline bool operator!=(const Vector3DD &v1, const Vector3DD &v2);
    friend inline const Vector3DD operator+(const Vector3DD &v1, const Vector3DD &v2);
    friend inline const Vector3DD operator-(const Vector3DD &v1, const Vector3DD &v2);
    friend inline const Vector3DD operator*(double factor, const Vector3DD &vector);
    friend inline const Vector3DD operator*(const Vector3DD &vector, double factor);
    friend const Vector3DD operator*(const Vector3DD &v1, const Vector3DD& v2);
    friend inline const Vector3DD operator-(const Vector3DD &vector);
    friend inline const Vector3DD operator/(const Vector3DD &vector, double divisor);
    friend inline const Vector3DD operator/(const Vector3DD &vector, const Vector3DD &divisor);
private:
    double v[3];
};


inline bool operator==(const Vector3DD &a, const Vector3DD &b)
{
    return a.v[0] == b.v[0] && a.v[1] == b.v[1] && a.v[2] == b.v[2];
}

inline bool operator!=(const Vector3DD &a, const Vector3DD &b)
{
    return a.v[0] != b.v[0] || a.v[1] != b.v[1] || a.v[2] != b.v[2];
}


inline const Vector3DD operator+(const Vector3DD &a, const Vector3DD &b)
{
    return Vector3DD(a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2]);
}

inline const Vector3DD operator-(const Vector3DD &a, const Vector3DD &b)
{
    return Vector3DD(a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2]);
}

inline const Vector3DD operator*(double factor, const Vector3DD &vector)
{
    return Vector3DD(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor);
}

inline const Vector3DD operator*(const Vector3DD &vector, double factor)
{
    return Vector3DD(vector.v[0] * factor, vector.v[1] * factor, vector.v[2] * factor);
}

inline const Vector3DD operator*(const Vector3DD &a, const Vector3DD& b)
{
    return Vector3DD(a.v[0] * b.v[0], a.v[1] * b.v[1], a.v[2] * b.v[2]);
}

inline const Vector3DD operator-(const Vector3DD &vector)
{
    return Vector3DD(-vector.v[0], -vector.v[1], -vector.v[2]);
}

inline const Vector3DD operator/(const Vector3DD &vector, double divisor)
{
    return Vector3DD(vector.v[0] / divisor, vector.v[1] / divisor, vector.v[2] / divisor);
}

inline const Vector3DD operator/(const Vector3DD &vector, const Vector3DD &divisor)
{
    return Vector3DD(vector.v[0] / divisor.v[0], vector.v[1] / divisor.v[1], vector.v[2] / divisor.v[2]);
}

inline double Vector3DD::length() const {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}
inline double Vector3DD::lengthSquared() const {
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

inline Vector3DD Vector3DD::normalized() const {
    return *this / length();
}

inline void Vector3DD::normalize() {
    *this /= length();
}

inline Vector3DD &Vector3DD::operator+=(const Vector3DD &vector){
    v[0] += vector.v[0];
    v[1] += vector.v[1];
    v[2] += vector.v[2];
    return *this;
}
inline Vector3DD &Vector3DD::operator-=(const Vector3DD &vector){
    v[0] -= vector.v[0];
    v[1] -= vector.v[1];
    v[2] -= vector.v[2];
    return *this;
}
inline Vector3DD &Vector3DD::operator*=(double factor){
    v[0] *= factor;
    v[1] *= factor;
    v[2] *= factor;
    return *this;
}
inline Vector3DD &Vector3DD::operator*=(const Vector3DD& vector) {
    v[0] *= vector.v[0];
    v[1] *= vector.v[1];
    v[2] *= vector.v[2];
    return *this;
}
inline Vector3DD &Vector3DD::operator/=(double divisor) {
    v[0] /= divisor;
    v[1] /= divisor;
    v[2] /= divisor;
    return *this;
}
inline Vector3DD &Vector3DD::operator/=(const Vector3DD &vector) {
    v[0] /= vector.v[0];
    v[1] /= vector.v[1];
    v[2] /= vector.v[2];
    return *this;
}

inline double Vector3DD::dotProduct(const Vector3DD& a, const Vector3DD& b) {
    return a.v[0]*b.v[0] + a.v[1]*b.v[1] + a.v[2]*b.v[2];
}

inline Vector3DD Vector3DD::crossProduct(const Vector3DD& a, const Vector3DD& b) {
    return Vector3DD(a.v[1]*b.v[2] - a.v[2]*b.v[1], a.v[2]*b.v[0] - a.v[0]*b.v[2], a.v[0]*b.v[1] - a.v[1]*b.v[0]);
}


#endif // VECTOR3DD_H
