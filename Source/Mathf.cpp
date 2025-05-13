#include "Mathf.h"
#include<stdlib.h>
// 線形補間（単一の値）
float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

//指定範囲のランダム値を計算する
float Mathf::RandomRange(float min, float max)
{
    return rand() % (int)max - min;
}