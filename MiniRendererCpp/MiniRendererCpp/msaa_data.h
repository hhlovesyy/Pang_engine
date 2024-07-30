#pragma once
// msaa_data.h
#ifndef MSAA_DATA_H
#define MSAA_DATA_H

#include "geometry.h" // 确保包含vec4的定义

struct msaaData
{
    TGAColor color[4];
    bool mask[4];
};

#endif // MSAA_DATA_H