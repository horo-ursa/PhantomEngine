// simdMath.h
// Math library mostly for vectors, matrices, and quaternions

#pragma once

#include "engineMath.h"
#include <xmmintrin.h>
#include <smmintrin.h>
#include <cmath>


// SHUFFLER is like shuffle, but has easier to understand indices
#define _MM_SHUFFLER( xi, yi, zi, wi ) _MM_SHUFFLE( wi, zi, yi, xi )


// 3D Vector
class SIMDVector3
{
public:
    union
    {
        __m128 mVec;
        struct { float x, y, z; };
    };

    SIMDVector3()
    {
        mVec = _mm_setzero_ps();
    }

    explicit SIMDVector3(float inX, float inY, float inZ)
    {
        mVec = _mm_setr_ps(inX, inY, inZ, 0.0f);
    }

    explicit SIMDVector3(__m128 value)
    {
        mVec = value;
    }

    // Set all three components in one line
    void Set(float inX, float inY, float inZ)
    {
        mVec = _mm_setr_ps(inX, inY, inZ, 0.0f);
    }

    // Vector addition (a + b)
    friend SIMDVector3 operator+(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g;
        return SIMDVector3(_mm_add_ps(a.mVec, b.mVec));
    }

    // Vector subtraction (a - b)
    friend SIMDVector3 operator-(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_sub_ps(a.mVec, b.mVec));
    }

    // Component-wise multiplication
    friend SIMDVector3 operator*(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_mul_ps(a.mVec, b.mVec));
    }

    // Scalar multiplication
    friend SIMDVector3 operator*(const SIMDVector3& vec, float scalar)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_mul_ps(vec.mVec, _mm_set_ps1(scalar)));
    }

    // Scalar multiplication
    friend SIMDVector3 operator*(float scalar, const SIMDVector3& vec)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_mul_ps(_mm_set_ps1(scalar), vec.mVec));
    }

    // Scalar *=
    SIMDVector3& operator*=(float scalar)
    {
        // TODO Lab 01g
        mVec = _mm_mul_ps(mVec, _mm_set_ps1(scalar));
        return *this;
    }

    // Scalar division
    friend SIMDVector3 operator/(const SIMDVector3& vec, float scalar)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_mul_ps(vec.mVec, _mm_set_ps1(1 / scalar)));
    }

    // Scalar /=
    SIMDVector3& operator/=(float scalar)
    {
        // TODO Lab 01g
        mVec = _mm_mul_ps(mVec, _mm_set_ps1(1 / scalar));
        return *this;
    }

    // Vector +=
    SIMDVector3& operator+=(const SIMDVector3& right)
    {
        // TODO Lab 01g
        mVec = _mm_add_ps(mVec, right.mVec);
        return *this;
    }

    // Vector -=
    SIMDVector3& operator-=(const SIMDVector3& right)
    {
        // TODO Lab 01g
        mVec = _mm_sub_ps(mVec, right.mVec);
        return *this;
    }

    // Length squared of vector (as a vector)
    SIMDVector3 LengthSq_v() const
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_dp_ps(mVec, mVec, 0x77));
    }

    // Length squared of vector
    float LengthSq() const
    {
        // TODO Lab 01g
        return _mm_dp_ps(mVec, mVec, 0x77).m128_f32[0];
    }

    // Length of vector (as a vector)
    SIMDVector3 Length_v() const
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_sqrt_ps(LengthSq_v().mVec));
    }

    // Length of vector
    float Length() const
    {
        // TODO Lab 01g
        return sqrt(LengthSq());
    }

    // Normalize this vector
    void Normalize()
    {
        // TODO Lab 01g
        __m128 temp = _mm_dp_ps(mVec, mVec, 0x77);
        temp = _mm_rsqrt_ps(temp);
        mVec = _mm_mul_ps(mVec, temp);
    }

    // Normalize the provided vector
    friend SIMDVector3 Normalize(const SIMDVector3& vec)
    {
        // TODO Lab 01g
        __m128 temp = _mm_dp_ps(vec.mVec, vec.mVec, 0x77);
        temp = _mm_rsqrt_ps(temp);
        __m128 temp2 = _mm_mul_ps(vec.mVec, temp);
        return SIMDVector3(temp2);
    }

    // Dot product between two vectors (a dot b) (as a vector)
    friend SIMDVector3 Dot_v(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g
        return SIMDVector3(_mm_dp_ps(a.mVec, b.mVec, 0x77));
    }

    // Dot product between two vectors (a dot b)
    friend float Dot(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g
        return _mm_dp_ps(a.mVec, b.mVec, 0x77).m128_f32[0];
    }

    // Cross product between two vectors (a cross b)
    friend SIMDVector3 Cross(const SIMDVector3& a, const SIMDVector3& b)
    {
        // TODO Lab 01g
        SIMDVector3 tempA = SIMDVector3(_mm_shuffle_ps(a.mVec, a.mVec, _MM_SHUFFLER(1, 2, 0, 3)));
        SIMDVector3 tempB = SIMDVector3(_mm_shuffle_ps(b.mVec, b.mVec, _MM_SHUFFLER(2, 0, 1, 3)));
        SIMDVector3 result = tempA * tempB;
        tempA = SIMDVector3(_mm_shuffle_ps(a.mVec, a.mVec, _MM_SHUFFLER(2, 0, 1, 3)));
        tempB = SIMDVector3(_mm_shuffle_ps(b.mVec, b.mVec, _MM_SHUFFLER(1, 2, 0, 3)));
        tempA = tempA * tempB;
        result -= tempA;
        return result;
    }

    // Lerp from A to B by f
    friend SIMDVector3 Lerp(const SIMDVector3& a, const SIMDVector3& b, float f)
    {
        // TODO Lab 01g
        return SIMDVector3(a + f * (b - a));
    }

    static const SIMDVector3 Zero;
    static const SIMDVector3 One;
    static const SIMDVector3 UnitX;
    static const SIMDVector3 UnitY;
    static const SIMDVector3 UnitZ;

    static bool UnitTest();
};

// 4D Vector
class SIMDVector4
{
public:
    union
    {
        __m128 mVec;
        struct { float x, y, z, w; };
    };


    SIMDVector4()
    {
        mVec = _mm_setzero_ps();
    }

    explicit SIMDVector4(float inX, float inY, float inZ, float inW)
    {
        mVec = _mm_setr_ps(inX, inY, inZ, inW);
    }

    explicit SIMDVector4(__m128 value)
    {
        mVec = value;
    }

    // Set all four components in one line
    void Set(float inX, float inY, float inZ, float inW)
    {
        mVec = _mm_setr_ps(inX, inY, inZ, inW);
    }

    // Vector addition (a + b)
    friend SIMDVector4 operator+(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_add_ps(a.mVec, b.mVec));
    }

    // Vector subtraction (a - b)
    friend SIMDVector4 operator-(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_sub_ps(a.mVec, b.mVec));
    }

    // Component-wise multiplication
    friend SIMDVector4 operator*(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_mul_ps(a.mVec, b.mVec));
    }

    // Scalar multiplication
    friend SIMDVector4 operator*(const SIMDVector4& vec, float scalar)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_mul_ps(vec.mVec, _mm_set_ps1(scalar)));
    }

    // Scalar multiplication
    friend SIMDVector4 operator*(float scalar, const SIMDVector4& vec)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_mul_ps(vec.mVec, _mm_set_ps1(scalar)));
    }

    // Scalar *=
    SIMDVector4& operator*=(float scalar)
    {
        // TODO Lab 01h
        mVec = _mm_mul_ps(mVec, _mm_set_ps1(scalar));
        return *this;
    }

    // Scalar division
    friend SIMDVector4 operator/(const SIMDVector4& vec, float scalar)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_mul_ps(vec.mVec, _mm_set_ps1(1 / scalar)));
    }

    // Scalar /=
    SIMDVector4& operator/=(float scalar)
    {
        // TODO Lab 01h
        mVec = _mm_mul_ps(mVec, _mm_set_ps1(1 / scalar));
        return *this;
    }

    // Vector +=
    SIMDVector4& operator+=(const SIMDVector4& right)
    {
        // TODO Lab 01h
        mVec = _mm_add_ps(mVec, right.mVec);
        return *this;
    }

    // Vector -=
    SIMDVector4& operator-=(const SIMDVector4& right)
    {
        // TODO Lab 01h
        mVec = _mm_sub_ps(mVec, right.mVec);
        return *this;
    }

    // Length squared of vector (as a vector)
    SIMDVector4 LengthSq_v() const
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_dp_ps(mVec, mVec, 0xFF));
    }

    // Length squared of vector
    float LengthSq() const
    {
        // TODO Lab 01h
        return _mm_dp_ps(mVec, mVec, 0xFF).m128_f32[0];
    }

    // Length of vector (as a vector)
    SIMDVector4 Length_v() const
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_sqrt_ps(LengthSq_v().mVec));
    }

    // Length of vector
    float Length() const
    {
        // TODO Lab 01h
        return sqrt(LengthSq());
    }

    // Normalize this vector
    void Normalize()
    {
        // TODO Lab 01h
        __m128 temp = _mm_dp_ps(mVec, mVec, 0xFF);
        temp = _mm_rsqrt_ps(temp);
        mVec = _mm_mul_ps(mVec, temp);
    }

    // Normalize the provided vector
    friend SIMDVector4 Normalize(const SIMDVector4& vec)
    {
        // TODO Lab 01h
        __m128 temp = _mm_dp_ps(vec.mVec, vec.mVec, 0xFF);
        temp = _mm_rsqrt_ps(temp);
        __m128 temp2 = _mm_mul_ps(vec.mVec, temp);
        return SIMDVector4(temp2);
    }

    // Dot product between two vectors (a dot b) (as a vector)
    friend SIMDVector4 Dot_v(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        return SIMDVector4(_mm_dp_ps(a.mVec, b.mVec, 0xFF));
    }

    // Dot product between two vectors (a dot b)
    friend float Dot(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        return _mm_dp_ps(a.mVec, b.mVec, 0xFF).m128_f32[0];
    }

    // Cross product between two vectors (a cross b) (assumes w = 0)
    friend SIMDVector4 Cross(const SIMDVector4& a, const SIMDVector4& b)
    {
        // TODO Lab 01h
        SIMDVector4 tempA = SIMDVector4(_mm_shuffle_ps(a.mVec, a.mVec, _MM_SHUFFLER(1, 2, 0, 3)));
        SIMDVector4 tempB = SIMDVector4(_mm_shuffle_ps(b.mVec, b.mVec, _MM_SHUFFLER(2, 0, 1, 3)));
        SIMDVector4 result = tempA * tempB;
        tempA = SIMDVector4(_mm_shuffle_ps(a.mVec, a.mVec, _MM_SHUFFLER(2, 0, 1, 3)));
        tempB = SIMDVector4(_mm_shuffle_ps(b.mVec, b.mVec, _MM_SHUFFLER(1, 2, 0, 3)));
        tempA = tempA * tempB;
        result -= tempA;
        return result;
    }

    // Lerp from A to B by f
    friend SIMDVector4 Lerp(const SIMDVector4& a, const SIMDVector4& b, float f)
    {
        // TODO Lab 01h
        return SIMDVector4(a + f * (b - a));
    }

    static const SIMDVector4 Zero;
    static const SIMDVector4 One;
    static const SIMDVector4 UnitX;
    static const SIMDVector4 UnitY;
    static const SIMDVector4 UnitZ;
    static const SIMDVector4 UnitW;

    static bool UnitTest();
};

// 4x4 Matrix
class SIMDMatrix4
{
public:
    union
    {
        __m128 rows[4];
        float mat[4][4];
    };

    SIMDMatrix4()
    {
        rows[0] = _mm_setzero_ps();
        rows[1] = _mm_setzero_ps();
        rows[2] = _mm_setzero_ps();
        rows[3] = _mm_setzero_ps();
    }

    explicit SIMDMatrix4(const float inMat[4][4])
    {
        memcpy(mat, inMat, 16 * sizeof(float));
    }

    explicit SIMDMatrix4(const __m128 inRows[4])
    {
        rows[0] = inRows[0];
        rows[1] = inRows[1];
        rows[2] = inRows[2];
        rows[3] = inRows[3];
    }

    // Cast to a const float pointer
    const float* GetAsFloatPtr() const
    {
        return &mat[0][0];
    }

    // Matrix multiplication (a * b)
    friend SIMDMatrix4 operator*(const SIMDMatrix4& a, const SIMDMatrix4& b)
    {
        // transpose b
        __m128 bT[4];
        __m128 tmp0 = _mm_shuffle_ps(b.rows[0], b.rows[1], 0x44);
        __m128 tmp2 = _mm_shuffle_ps(b.rows[0], b.rows[1], 0xee);
        __m128 tmp1 = _mm_shuffle_ps(b.rows[2], b.rows[3], 0x44);
        __m128 tmp3 = _mm_shuffle_ps(b.rows[2], b.rows[3], 0xee);
        bT[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
        bT[1] = _mm_shuffle_ps(tmp0, tmp1, 0xdd);
        bT[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
        bT[3] = _mm_shuffle_ps(tmp2, tmp3, 0xdd);

        __m128 rows[4];
        for (int i = 0; i < 4; i++)
        {
            rows[i] = _mm_add_ps(
                _mm_add_ps(_mm_dp_ps(a.rows[i], bT[0], 0xF1),
                    _mm_dp_ps(a.rows[i], bT[1], 0xF2)
                ),
                _mm_add_ps(_mm_dp_ps(a.rows[i], bT[2], 0xF4),
                    _mm_dp_ps(a.rows[i], bT[3], 0xF8)
                )
            );
        }

        return SIMDMatrix4(rows);
    }

    SIMDMatrix4& operator*=(const SIMDMatrix4& right)
    {
        *this = *this * right;
        return *this;
    }

    // Invert the matrix - super slow
    void Invert();

    // Get the translation component of the matrix
    SIMDVector3 GetTranslation() const
    {
        return SIMDVector3(rows[3]);
    }

    // Get the X axis of the matrix (forward)
    SIMDVector3 GetXAxis() const
    {
        return Normalize(SIMDVector3(rows[0]));
    }

    // Get the Y axis of the matrix (left)
    SIMDVector3 GetYAxis() const
    {
        return Normalize(SIMDVector3(rows[1]));
    }

    // Get the Z axis of the matrix (up)
    SIMDVector3 GetZAxis() const
    {
        return Normalize(SIMDVector3(rows[2]));
    }

    // Extract the scale component from the matrix
    SIMDVector3 GetScale() const
    {
        __m128 x = _mm_dp_ps(rows[0], rows[0], 0x71);
        __m128 y = _mm_dp_ps(rows[1], rows[1], 0x72);
        __m128 z = _mm_dp_ps(rows[2], rows[2], 0x74);
        return SIMDVector3(_mm_sqrt_ps(_mm_add_ps(_mm_add_ps(x, y), z)));
    }

    // Transpose this matrix
    void Transpose()
    {
        _MM_TRANSPOSE4_PS(rows[0], rows[1], rows[2], rows[3]);
    }

    // Transpose the provided matrix
    friend SIMDMatrix4 Transpose(const SIMDMatrix4& inMat)
    {
        SIMDMatrix4 retVal = inMat;
        retVal.Transpose();
        return retVal;
    }

    // Create a scale matrix with x, y, and z scales
    static SIMDMatrix4 CreateScale(float xScale, float yScale, float zScale)
    {
        float temp[4][4] =
        {
            { xScale, 0.0f, 0.0f, 0.0f },
            { 0.0f, yScale, 0.0f, 0.0f },
            { 0.0f, 0.0f, zScale, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        return SIMDMatrix4(temp);
    }

    static SIMDMatrix4 CreateScale(const SIMDVector3& scaleVector)
    {
        return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
    }

    // Create a scale matrix with a uniform factor
    static SIMDMatrix4 CreateScale(float scale)
    {
        return CreateScale(scale, scale, scale);
    }

    // Rotation about x-axis
    static SIMDMatrix4 CreateRotationX(float theta)
    {
        float temp[4][4] =
        {
            { 1.0f, 0.0f, 0.0f , 0.0f },
            { 0.0f, cosf(theta), sinf(theta), 0.0f },
            { 0.0f, -sinf(theta), cosf(theta), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        return SIMDMatrix4(temp);
    }

    // Rotation about y-axis
    static SIMDMatrix4 CreateRotationY(float theta)
    {
        float temp[4][4] =
        {
            { cosf(theta), 0.0f, -sinf(theta), 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { sinf(theta), 0.0f, cosf(theta), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        return SIMDMatrix4(temp);
    }

    // Rotation about z-axis
    static SIMDMatrix4 CreateRotationZ(float theta)
    {
        float temp[4][4] =
        {
            { cosf(theta), sinf(theta), 0.0f, 0.0f },
            { -sinf(theta), cosf(theta), 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };
        return SIMDMatrix4(temp);
    }

    static SIMDMatrix4 CreateYawPitchRoll(float yaw, float pitch, float roll)
    {
        return SIMDMatrix4::CreateRotationZ(roll)
            * SIMDMatrix4::CreateRotationX(pitch)
            * SIMDMatrix4::CreateRotationY(yaw);
    }

    // Create a rotation matrix from a quaternion
    static SIMDMatrix4 CreateFromQuaternion(const Quaternion& q);

    static SIMDMatrix4 CreateTranslation(const SIMDVector3& trans)
    {
        float temp[4][4] =
        {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { trans.x, trans.y, trans.z, 1.0f }
        };
        return SIMDMatrix4(temp);
    }

    static SIMDMatrix4 CreateLookAt(const SIMDVector3& eye, const SIMDVector3& at, const SIMDVector3& up)
    {
        SIMDVector3 forward = Normalize(at - eye);
        SIMDVector3 left = Normalize(Cross(up, forward));
        SIMDVector3 newUp = Normalize(Cross(forward, left));

        float temp[4][4] =
        {
            { left.x, left.y, left.z, 0.0f },
            { newUp.x, newUp.y, newUp.z, 0.0f },
            { forward.x, forward.y, forward.z, 0.0f },
            { eye.x, eye.y, eye.z, 1.0f }
        };
        return SIMDMatrix4(temp);
    }

    static SIMDMatrix4 CreateOrtho(float width, float height, float nearf, float farf)
    {
        float temp[4][4] =
        {
            { 2.0f / width, 0.0f, 0.0f, 0.0f },
            { 0.0f, 2.0f / height, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f / (farf - nearf), 0.0f },
            { 0.0f, 0.0f, nearf / (nearf - farf), 1.0f }
        };
        return SIMDMatrix4(temp);
    }

    static SIMDMatrix4 CreatePerspectiveFOV(float fovY, float width, float height, float nearf, float farf)
    {
        float yScale = 1.0f / tanf(fovY / 2.0f);
        float xScale = yScale * height / width;
        float temp[4][4] =
        {
            { xScale, 0.0f, 0.0f, 0.0f },
            { 0.0f, yScale, 0.0f, 0.0f },
            { 0.0f, 0.0f, farf / (farf - nearf), 1.0f },
            { 0.0f, 0.0f, -nearf * farf / (farf - nearf), 0.0f }
        };
        return SIMDMatrix4(temp);
    }

    static const SIMDMatrix4 Identity;

    static bool UnitTest();
};

// Matt Whiting -- these are just copies of the floating point math.  
// Maybe someday I'll make pure SIMD versions of these...
// But not today.
inline SIMDVector3 Transform(const SIMDVector3& vec, const SIMDMatrix4& mat, float w = 1.0f)
{
    SIMDVector3 retVal;
    retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
        vec.z * mat.mat[2][0] + w * mat.mat[3][0];
    retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
        vec.z * mat.mat[2][1] + w * mat.mat[3][1];
    retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
        vec.z * mat.mat[2][2] + w * mat.mat[3][2];
    //ignore w since we aren't returning a new value for it...
    return retVal;
}
