#ifndef _RANDOM_BASE_H_
#define _RANDOM_BASE_H_
#include<vector>
#include<map>

template<typename T>
class RandomBase
{
protected:
    //基类的构造函数
    RandomBase()
    {
        
    }

public:
    //生成一个随机数
    virtual T makeOneRandom()=0;

    //生成dataNum个不同的随机数，存储在dstData里面
    void makeDiffData(std::vector<T> &dstData,unsigned int dataNum);
};



//生成dataNum个不同的随机数，存储在dstData里面
template<typename T>
void RandomBase<T>::makeDiffData(std::vector<T> &dstData,unsigned int dataNum)
{
    //给向量预先开辟空间
    dstData.reserve(dataNum);
    //新建数字哈希表
    std::map<T,bool> dataMap;
    //循环获取数字
    while(true)
    {
        //判断结束条件
        if(dataNum==0) break;
        //随机获取一个数字
        T tempData=makeOneRandom();
        //判断数字是否被获取过
        if(dataMap[tempData]==false)
        {
            //记录获取过的数字
            dataMap[tempData]=true;
            dstData.push_back(tempData);
            //待记录的数字个数更改
            dataNum--;
        }
    }
}


#endif
