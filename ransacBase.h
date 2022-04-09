#ifndef _RANSAC_BASE_H_
#define _RANSAC_BASE_H_
#include<vector>
#include<math.h>
#include"random/uniformRandom.h"
#include<omp.h>


//做ransac算法用的基类,dataT是数据的类型
template <typename DataType, typename Model>
class RansacBase
{

protected:
    unsigned int iterNum_;//总共需要迭代的次数
    const unsigned int minSample_;//生成一组模型最少需要几个数据

public:
    //基类的构造函数
    RansacBase(unsigned int minSample,//最小采样个数
        const double inRate,//假设的内点率
        const double expInRate //期望的一次采样全部为内点的概率
    ): minSample_(minSample)
    {
        //计算需要迭代的次数
        iterNum_=computeIterNum_(inRate,expInRate);
    };

    //开始执行ransac
    void doRansac();

private:
    //根据传入的好点概率，计算需要迭代的次数
    unsigned int computeIterNum_(const double inRate,//假设的内点率
        const double expInRate //期望的采样内点率
    );

    //获取某个模型对应的内点序列
    void listInlier(std::vector<int> &dstList,//获取到的目标内点序列
            const Model& getModel);

    //从找到的所有内点中，找到内点最多的一组内点并返回它的引用
    std::vector<int>& getMaxSize(std::vector<std::vector<int>> &inlierMap);
    
protected:
    //总共有多少个数据，纯虚函数，需要子类来实现
    virtual unsigned int dataSize_() const =0;

    //根据传入的标号获取一组数据用于构造模型
    virtual void getSubData(std::vector<DataType> &dstData,
            const std::vector<int> &dataId)=0;

    //根据传入的一组数据，生成一个模型
    virtual void makeModel(const std::vector<DataType> &data,
            Model &dstModel)=0;

    //保留最优秀的一组内点序列
    virtual void saveBestInlier(std::vector<int> &bestInlier)=0;

    //获取特定位置的数据,需要子类提供一个随机访问数据的方法
    virtual const DataType& getSpecificData(unsigned int local)=0;

    //数据的个数
    virtual unsigned int getDataAccount() const=0;

    //判断传入的点对当前的模型是否为内点
    virtual bool judgeInlier(const DataType &data,const Model &model)=0;

};



//根据传入的好点概率，计算需要迭代的次数
template <typename DataType, typename Model>
unsigned int RansacBase<DataType,Model>::computeIterNum_(const double inRate,//假设的内点率
    const double expInRate //期望的采样内点率
)
{
    //log(1-p)/log(1-w^n)
    return std::log(1-expInRate)/std::log(
        1-std::pow(inRate,minSample_)
    );
}

//获取某个模型对应的内点序列
template <typename DataType, typename Model>
void RansacBase<DataType,Model>::listInlier(
        std::vector<int> &dstList,//获取到的目标内点序列
        const Model& getModel)
{
    //获取数据的个数
    unsigned int dataAccount=getDataAccount();
    //遍历所有的数据序列
    for(unsigned int dataId=0;dataId<dataAccount;++dataId)
    {
        //获取当前位置的一组数据
        const DataType& tempData=getSpecificData(dataId);
        //判断当前的点是否为内点
        if(judgeInlier(tempData,getModel))
        {
            dstList.push_back(dataId);
        }
    }
}

//开始执行ransac
template <typename DataType, typename Model>
void RansacBase<DataType,Model>::doRansac()
{
    //获取整体数据的个数
    unsigned int dataSize=dataSize_();
    //定义一个均匀分布的随机数生成器
    UniformRandom<int> randomMaker(0,dataSize-1);
    //指定多线程的最大线程数
    omp_set_num_threads(16);
    //给每次采样都预先开辟一个存储内点的空间
    std::vector<std::vector<int>> inlierMap;
    inlierMap.resize(iterNum_);
    //并行循环采样
#ifdef USE_THREAD
#pragma omp parallel for
#endif
    for(unsigned int iterId=0;iterId<iterNum_;++iterId)
    {
        //根据生成一组模型所需的数据个数，随机选取数据
        std::vector<int> idList;
        randomMaker.makeDiffData(idList,minSample_);
        //根据选取到的标号，获取一组数据
        std::vector<DataType> subData;
        getSubData(subData,idList);
        //根据获取到的数据生成模型
        Model tempModel;
        makeModel(subData,tempModel);
        //记录当前模型对应的内点序列
        std::vector<int> &inlierList=inlierMap[iterId];
        listInlier(inlierList,tempModel);
    }
    //获取内点数最多的一组序列
    std::vector<int> &bestInlier=getMaxSize(inlierMap);
    //保留最多的内点序列
    saveBestInlier(bestInlier);
}

//从找到的所有内点中，找到内点最多的一组内点并返回它的引用
template <typename DataType, typename Model>
std::vector<int>& RansacBase<DataType,Model>::getMaxSize(
        std::vector<std::vector<int>> &inlierMap)
{
    //最大的标号
    int maxId=-1;
    //目前的最大值
    int maxSize=0;
    //遍历内点存储
    for(int i=0;i<inlierMap.size();++i)
    {
        //判断更大
        if(maxId==-1||inlierMap[i].size()>maxSize)
        {
            //记录最大位置
            maxId=i;
            maxSize=inlierMap[i].size();
        }
    }
    //返回最大位置的引用
    return inlierMap[maxId];
}



#endif
