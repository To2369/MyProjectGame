#include "Mathf.h"

// 線形補間（単一の値）
float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}