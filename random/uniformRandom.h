#ifndef _UNIFORM_RANDOM_H_
#define _UNIFORM_RANDOM_H_
#include"random/randomBase.h"
#include"opencv2/opencv.hpp"

//均匀分布的随机数生成器
template<typename T>
class UniformRandom : public RandomBase<T>
{
public:
    const T min_;//最小值
    const T max_;//最大值
private:
    cv::RNG rng_;//来自opencv的随机数生成器
public:
    //生成一个随机数
    virtual T makeOneRandom();

    //构造函数
    UniformRandom(const T min, const T max)
        : RandomBase<T>(),min_(min),max_(max)
    {
        //初始化随机数生成器
        rng_=cv::RNG();
    }
};


template <typename T>
T UniformRandom<T>::makeOneRandom()
{
    //根据范围返回一个随机数
    return rng_.uniform(min_,max_);
}

#endif
