#pragma once
// msaa_data.h
#ifndef MSAA_DATA_H
#define MSAA_DATA_H

#include "geometry.h" // ȷ������vec4�Ķ���

struct msaaData
{
    TGAColor color[4];
    bool mask[4];
};

#endif // MSAA_DATA_H