#pragma once
class Mathf
{
public:
    // 線形補間（単一の値）
    static float Lerp(float a, float b, float t);

    //指定範囲をランダム値を計算する
    static float RandomRange(float min, float max);
};