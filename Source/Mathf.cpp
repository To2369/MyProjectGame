#include "Mathf.h"
#include<stdlib.h>
// ���`��ԁi�P��̒l�j
float Mathf::Lerp(float a, float b, float t)
{
    return a * (1.0f - t) + (b * t);
}

//�w��͈͂̃����_���l���v�Z����
float Mathf::RandomRange(float min, float max)
{
    return rand() % (int)max - min;
}