#pragma once
#include <Windows.h>
#include <cmath>

namespace Vector {
    // ��ά�����ṹ��
    struct Vec2 {
        float x;
        float y;

        // Ĭ�Ϲ��캯��
        Vec2(float x_ = 0.0f, float y_ = 0.0f) : x(x_), y(y_) {}

        // �ж��Ƿ�Ϊ������
        bool IsZero() const {
            return x == 0.0f && y == 0.0f;
        }

        // ������������
        float Length() const {
            return sqrt(x * x + y * y);
        }

        // ��һ������
        Vec2 Normalize() const {
            float len = Length();
            if (len == 0.0f) return Vec2(0.0f, 0.0f);
            return Vec2(x / len, y / len);
        }

        // ���
        float Dot(const Vec2& other) const {
            return x * other.x + y * other.y;
        }

        // ����
        float Distance(const Vec2& other) const {
            return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
        }
    };

    // ��ά�����ṹ��
    struct Vec3 {
        float x;
        float y;
        float z;

        // Ĭ�Ϲ��캯��
        Vec3(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) : x(x_), y(y_), z(z_) {}

        // �ж��Ƿ�Ϊ������
        bool IsZero() const {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        // ������������
        float Length() const {
            return sqrt(x * x + y * y + z * z);
        }

        // ��һ������
        Vec3 Normalize() const {
            float len = Length();
            if (len == 0.0f) return Vec3(0.0f, 0.0f, 0.0f);
            return Vec3(x / len, y / len, z / len);
        }

        // ���
        float Dot(const Vec3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        // ����
        float Distance(const Vec3& other) const {
            return sqrt((x - other.x) * (x - other.x) +
                (y - other.y) * (y - other.y) +
                (z - other.z) * (z - other.z));
        }
    };

    // ��ά�����ṹ��
    struct Vec4 {
        float x;
        float y;
        float z;
        float w;

        // Ĭ�Ϲ��캯��
        Vec4(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 0.0f)
            : x(x_), y(y_), z(z_), w(w_) {
        }

        // �ж��Ƿ�Ϊ������
        bool IsZero() const {
            return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
        }

        // ������������
        float Length() const {
            return sqrt(x * x + y * y + z * z + w * w);
        }

        // ��һ������
        Vec4 Normalize() const {
            float len = Length();
            if (len == 0.0f) return Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            return Vec4(x / len, y / len, z / len, w / len);
        }

        // ���
        float Dot(const Vec4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        // ����
        float Distance(const Vec4& other) const {
            return sqrt((x - other.x) * (x - other.x) +
                (y - other.y) * (y - other.y) +
                (z - other.z) * (z - other.z) +
                (w - other.w) * (w - other.w));
        }
    };

    // ���ڴ��ַ��ȡ��������
    Vec2 GetVec2(DWORD64 baseAddress);
    Vec3 GetVec3(DWORD64 baseAddress);
    Vec4 GetVec4(DWORD64 baseAddress);

    // �������������ڴ�������ʵ��
    Vec2 CreateVec2(float x, float y);
    Vec3 CreateVec3(float x, float y, float z);
    Vec4 CreateVec4(float x, float y, float z, float w);
}
