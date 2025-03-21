#pragma once
#include <Windows.h>
#include <cmath>

namespace Vector {
    // ��ά�����ṹ��
    struct Vec2 {
        float x;
        float y;

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
            if (len == 0.0f) return { 0.0f, 0.0f };
            return { x / len, y / len };
        }

        // ���
        float Dot(const Vec2& other) const {
            return x * other.x + y * other.y;
        }
    };

    // ��ά�����ṹ��
    struct Vec3 {
        float x;
        float y;
        float z;

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
            if (len == 0.0f) return { 0.0f, 0.0f, 0.0f };
            return { x / len, y / len, z / len };
        }

        // ���
        float Dot(const Vec3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }
    };

    // ��ά�����ṹ��
    struct Vec4 {
        float x;
        float y;
        float z;
        float w;

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
            if (len == 0.0f) return { 0.0f, 0.0f, 0.0f, 0.0f };
            return { x / len, y / len, z / len, w / len };
        }

        // ���
        float Dot(const Vec4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }
    };

    // ���ڴ��ַ��ȡ��������
    Vec2 GetVec2(DWORD64 baseAddress);
    Vec3 GetVec3(DWORD64 baseAddress);
    Vec4 GetVec4(DWORD64 baseAddress);
}