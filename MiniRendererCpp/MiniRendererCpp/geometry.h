#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec {
    double data[n] = { 0 };
    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    double ret = 0;
    for (int i = n; i--; ret += lhs[i] * rhs[i]);
    return ret;
}

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] += rhs[i]);
    return ret;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] -= rhs[i]);
    return ret;
}

template<int n> vec<n> operator*(const double& rhs, const vec<n>& lhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

template<int n> vec<n> operator*(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

template<int n> vec<n> operator/(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] /= rhs);
    return ret;
}

/*
这个函数的作用是将一个 n2 维向量嵌入到一个 n1 维向量中。
如果 n1 大于 n2，那么在嵌入后的向量中，除了前 n2 个元素与原始向量相同，剩余的元素会用 fill 参数进行填充。返回的结果是一个 n1 维向量。
*/
template<int n1, int n2> vec<n1> embed(const vec<n2>& v, double fill = 1) {
    vec<n1> ret;
    for (int i = n1; i--; ret[i] = (i < n2 ? v[i] : fill));
    return ret;
}

template<int n1, int n2> vec<n1> proj(const vec<n2>& v) {
    vec<n1> ret;
    for (int i = n1; i--; ret[i] = v[i]);
    return ret;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
    vec<2> normalized() { return (*this) / norm(); }
    //构造函数，提供x和y值作为输入
    vec<2>(double x, double y) : x(x), y(y) {}
    //默认构造函数
    vec<2>() = default;
};

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double norm2() const { return *this * *this; }
    double norm()  const { return std::sqrt(norm2()); }
    vec<3> normalized() { return (*this) / norm(); }
    //构造函数，提供x、y和z值作为输入
    vec<3>(double x, double y, double z) : x(x), y(y), z(z) {}
    //默认构造函数
    vec<3>() = default;
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;
vec3 cross(const vec3& v1, const vec3& v2);

template<int n> struct dt;

template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows] = { {} };

    vec<ncols>& operator[] (const int idx) { assert(idx >= 0 && idx < nrows); return rows[idx]; }
    const vec<ncols>& operator[] (const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }

    vec<nrows> col(const int idx) const {
        assert(idx >= 0 && idx < ncols);
        vec<nrows> ret;
        for (int i = nrows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    void set_col(const int idx, const vec<nrows>& v) {
        assert(idx >= 0 && idx < ncols);
        for (int i = nrows; i--; rows[i][idx] = v[i]);
    }

    static mat<nrows, ncols> identity() {
        mat<nrows, ncols> ret;
        for (int i = nrows; i--; )
            for (int j = ncols; j--; ret[i][j] = (i == j));
        return ret;
    }

    double det() const {
        return dt<ncols>::det(*this);
    }

    mat<nrows - 1, ncols - 1> get_minor(const int row, const int col) const {
        mat<nrows - 1, ncols - 1> ret;
        for (int i = nrows - 1; i--; )
            for (int j = ncols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    double cofactor(const int row, const int col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    mat<nrows, ncols> adjugate() const {
        mat<nrows, ncols> ret;
        for (int i = nrows; i--; )
            for (int j = ncols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    mat<nrows, ncols> invert_transpose() const {
        mat<nrows, ncols> ret = adjugate();
        return ret / (ret[0] * rows[0]);
    }

    mat<nrows, ncols> invert() const {
        return invert_transpose().transpose();
    }

    mat<ncols, nrows> transpose() const {
        mat<ncols, nrows> ret;
        for (int i = ncols; i--; ret[i] = this->col(i));
        return ret;
    }
};

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows, ncols>& lhs, const vec<ncols>& rhs) {
    vec<nrows> ret;
    for (int i = nrows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<int R1, int C1, int C2>mat<R1, C2> operator*(const mat<R1, C1>& lhs, const mat<C1, C2>& rhs) {
    mat<R1, C2> result;
    for (int i = R1; i--; )
        for (int j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator*(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] * val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator/(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] / val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator+(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] + rhs[i][j]);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator-(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] - rhs[i][j]);
    return result;
}

template<int nrows, int ncols> std::ostream& operator<<(std::ostream& out, const mat<nrows, ncols>& m) {
    for (int i = 0; i < nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct dt {
    static double det(const mat<n, n>& src) {
        double ret = 0;
        for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<> struct dt<1> {
    static double det(const mat<1, 1>& src) {
        return src[0][0];
    }
};

