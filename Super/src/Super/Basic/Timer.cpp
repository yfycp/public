﻿#include "Timer.h"

#include "SuperTime.h"

//用条件变量实现的sleep 精度更高，唤醒更及时
#include <mutex>
#include <condition_variable>

static inline void TimerSleep(unsigned int millisecond)
{
    Thread::sleep_for(millisecond);   //或sleep(0) 
}

//time in ms 
static inline double currTime()
{
    return SuperTime::getStartupToNowInms();
}

//time in ns 
static inline uint64_t currTimens()
{
    return SuperTime::getStartupToNowInns();
}

class SleepByCondition
{
public:
    SleepByCondition(){}
    ~SleepByCondition(){}
    void sleep_for(unsigned int milliseconds)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::milliseconds(milliseconds));//== std::cv_status::timeout;
    }
private:
    std::condition_variable cv;
    std::mutex mtx;
};

NAME_SPACE_MAIN_BEGIN

//static void RemovTimer(int timerID,std::vector<TimerInfo>& ListTimer)
//{
//    std::vector<TimerInfo>::iterator it;
//    for (it = ListTimer.begin(); it!=ListTimer.end();)
//    {
//        if (timerID==(*it).timerID)
//        {
//            it=ListTimer.erase(it);
//            break;
//        }
//        else
//        {
//            ++it;
//        }
//    }
//}

//新思路无用的用尾部替换后移除尾部 new 2020-05-02
//可能会造成原尾部的定时器要到下一轮循环才有机会运行，但是对于定时器应用来说影响不大
//void RemovTimer(int timerID,std::vector<TimerInfoV2>& ListTimer)
void RemovTimer(int timerID,std::vector<TimerInfo>& ListTimer)
{
    for (size_t n=0;n<ListTimer.size();n++)
    {
        TimerInfo& itemInfo=ListTimer[n];
        if (timerID==itemInfo.timerID)
        {
            ListTimer[n]=ListTimer.back();   //替换为尾部
            ListTimer.pop_back();                //移除尾部
        }
    }
}


////old为了避免动态内存申请,同时传入临时交换结构体
//void RemovTimer(int timerID,std::vector<TimerInfoV2>& ListTimer,std::vector<TimerInfoV2>& ListTmp)
//{
//    //若频繁增删,可能有些低效,优化
//
//#if 1
//    size_t invalidCnt=0;   //统计失效定时器
//    for (size_t n=0;n<ListTimer.size();n++)
//    {
//        TimerInfoV2& itemInfo=ListTimer[n];
//        if (timerID==itemInfo.timerID)
//        {
//            itemInfo.bContinue=false;
//            //invalidCnt++;
//            //break;
//        }
//        if (itemInfo.bContinue==false)   //每次删除,遍历所有,统计失效定时器
//        {
//            invalidCnt++;
//        }
//    }
//
//    //失效数量大于一半,重新移位
//    if (invalidCnt>=ListTimer.size()/2)
//    {
//        ListTmp=ListTimer;    //拷贝到临时空间
//        ListTimer.clear();
//        for (size_t n=0;n<ListTmp.size();n++)
//        {
//            const TimerInfoV2& itemInfo=ListTmp[n];
//            if (itemInfo.bContinue==true)   //
//            {
//                ListTimer.push_back(itemInfo);
//            }
//        }
//    }
//
//#else
//    std::vector<TimerInfoV2>::iterator it;
//    for (it = ListTimer.begin(); it!=ListTimer.end();)
//    {
//        if (timerID==(*it).timerID)
//        {
//            it=ListTimer.erase(it);
//            break;
//        }
//        else
//        {
//            ++it;
//        }
//    }
//#endif
//}


void TimerInfo::initTime()
{
    alreadyCount=0;
    begin_time=currTime();            //定时器开始时绝对时间
    calltime=begin_time+peroid;   //第一次回调到期时间
}

inline void TimerInfo::updataNextTime()
{
    alreadyCount++;
    double curr=currTime();
    //本次在回调函数里面超时了，超时次数统计或时间累加有需要待续。
    //double currUsedtime=curr-(calltime-peroid);    //当前时间减去回调前时间
    double currUsedtime=curr-in_time;    //当前时间减去回调前时间
    if (currUsedtime>peroid)
    {
        ::printf("Timer warning: timeout at callback fun! id:%d timeUsed:%f\n",timerID,currUsedtime);
        //上一次调用卡住了之后,暂时通过修正回调次数来纠正,否则会影响此后的精度,或改用其他机制,待续

        //uint64_t theoryCallCount=(curr-begin_time)/peroid;   //理论应该回调次数
        //alreadyCount=theoryCallCount;
        //
    }

    uint64_t theoryCallCount=(curr-begin_time)/peroid;   //理论应该回调次数
    if ((alreadyCount+2)<theoryCallCount)  //偏差
    {
          ::printf("Timer adjust realCnt:%u theory:%u\n",alreadyCount,theoryCallCount);
          alreadyCount=theoryCallCount;
    }

    //每次从绝对起始时间开始计算，否则随着时间积累误差会越来越大
    calltime=begin_time+(alreadyCount+1)*peroid;   //下次回调到期时间点
    //多个定时器情况，一个延时影响其他的校正待续。
}

void TimerInfo::setPhaseOffet(float offset_ms)
{
    //对浮点数取模,小于一个周期的相位调整才有意义
    float offset=fmod(offset_ms,peroid);
    begin_time+=offset;

    PreDprintf("offset:%f nbtime:%f\n",offset,begin_time);
    //没有必要 回调完本次之后会调用updataNextTime()
    //calltime=begin_time+(alreadyCount+1)*peroid;   //计算下次回调到期时间点
}


int Timer::AddTimer(double PeriodInms,TimerCallFun pCbFun,void* pUser,void* pData,uint64_t times)
{
     TimerInfo tmInfo;
     tmInfo.peroid=PeriodInms;
     tmInfo.timerCount=times;
     tmInfo.pCbFun=pCbFun;
     tmInfo.pUser=pUser;
     tmInfo.pData=pData;
     return AddTimer(tmInfo);
}


int Timer::AddTimer(TimerInfo& tmInfo)
{
    //小于0的时间周期没有意义，时间周期太小无法实现
    if (tmInfo.peroid<1)
    {
        tmInfo.peroid=1;
    }
    //运行0次没有实际意义，改为1
    if (tmInfo.timerCount==0)
    {
        tmInfo.timerCount=1;
    }
    tmInfo.timerID=TimerIDbegin++;
    tmInfo.alreadyCount=0;
    ListTimer.push_back(tmInfo);

    PreDprintf("Period:%.16f ms id:%u\n",tmInfo.peroid,tmInfo.timerID);

    return tmInfo.timerID;
}

void Timer::RemovTimer(int timerID)
{
    ::RemovTimer(timerID,ListTimer);
}


void Timer::CleanTimer()
{
    ListTimer.clear();
}


void Timer::TimerEvent(int id,void* pTimerInfo,void* pUser,void* pData)
{
    double timeNow=currTime();
    PreDprintf("TimerID:%d Timers:%u timeNow:%f ms\n",id,((TimerInfo*)pTimerInfo)->alreadyCount,timeNow);

    static SuperTime tm("Timer::TimeEvent");
    tm.printReCallTime(24);
}


//#if 1
////为了避免回调耗时过长或卡住恢复后的快速追赶，校正计数器，或者在添加辅助变量，记录丢失的次数 待续
//uint64_t theoryCallCount=timeNow/Tinfo.peroid;   //理论应该回调次数
//if (theoryCallCount>(Tinfo.alreadyCount+10))
//{
//    printf("校正 theoryCallCount:%u Tinfo.alreadyCount:%u\n",theoryCallCount,Tinfo.alreadyCount);
//    Tinfo.alreadyCount=theoryCallCount;
//}
//#endif


void Timer::TimerThread::run()
{
    PRINT_FUN_NAME

    //拷贝一份定时器信息，可以保证定时器正在运行时候，外部各种set不会有线程安全问题
    //每次重启后会用新的定时器信息去运行,同时可以保证本次运行被remove之后，下次重新运行功能一样
    std::vector<TimerInfo> VTimer=ParentTimer->ListTimer;

    //定时器列表为空,直接退出
    if (VTimer.empty())
    {
        return;
    }

    //每次重新运行，把已经运行次数复位为0,非必要
    std::vector<TimerInfo>::iterator it;
    for (size_t n=0; n<VTimer.size();n++)
    {
        TimerInfo& Tinfo=VTimer[n];
        Tinfo.alreadyCount=0;
        Tinfo.initTime();
    }

    //double timeBegin=currTime();
    SleepByCondition Sp;  //高精度及时唤醒的延时方法

    while (getContinueRun())
    {
        //先用简单遍历，有空改时间轮或最小堆来管理定时器信息。每次只和最接近的比较。待续 20200401 

        for (size_t n=0; n<VTimer.size();n++)
        {
            TimerInfo& Tinfo=VTimer[n];
            double timeDiff=Tinfo.calltime-currTime();
            if (timeDiff>0.1)  //与理论时间差距1ms ,则调用定时回调函数
            {
                continue;
            }

            int retCallFun=0;
            Tinfo.in_time=currTime();
            if (Tinfo.pCbFun!=NULL)
            {
                retCallFun=Tinfo.pCbFun(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
            }
            else
            {
                ParentTimer->TimerEvent(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
            }

            //printf("theoryCallCount:%u Tinfo.alreadyCount:%u\n",theoryCallCount,Tinfo.alreadyCount);
            Tinfo.updataNextTime();

            //移除某定时器条件:
            //1).定时器回调函数返回值非0
            //2).对于非永久运行的定时器，运行次数已到
            if ((retCallFun!=0)||(Tinfo.timerCount!=UINT64_MAX&&Tinfo.alreadyCount>=Tinfo.timerCount))     
            {
                ::RemovTimer(Tinfo.timerID,VTimer);//
                //若移除已经运行完成的定时器后,定时器列表为空,直接退出
                if (VTimer.empty())
                {
                    return;
                }
            }
        }

        //Thread::sleep_for(1);   //or sleep(0) 
        Sp.sleep_for(1);             //faster
    }
}


void MediaTimer::SetTimer(double PeriodInms,TimerCallFun pCbFun,void* pUser,void* pData,uint64_t times)
{
    //小于0的时间周期没有意义，时间周期太小无法实现
    if (PeriodInms<1)
    {
        PeriodInms=1;
    }

    //运行0次没有实际意义，改为1
    if (times==0)
    {
        times=1;
    }

    TimerInfo Tinfo;
    Tinfo.peroid=PeriodInms;
    //Tinfo.timerID=TimerIDbegin++;
    Tinfo.timerCount=times;
    Tinfo.alreadyCount=0;
    Tinfo.pCbFun=pCbFun;
    Tinfo.pUser=pUser;
    Tinfo.pData=pData;
    m_TimerInfo=Tinfo;

    PreDprintf("Period:%.16f ms id:%u\n",PeriodInms,Tinfo.timerID);
}


void MediaTimer::TimerEvent(int id,void* pTimerInfo,void* pUser,void* pData)
{
    double timeNow=currTime();

    PreDprintf("TimerID:%d Timers:%u timeNow:%f ms\n",id,((TimerInfo*)pTimerInfo)->alreadyCount,timeNow);

    static SuperTime tm("Timer::TimeEvent");
    tm.printReCallTime(24);
}


void MediaTimer::TimerThread::run()
{
    PRINT_FUN_NAME

    //拷贝一份定时器信息，可以保证定时器正在运行时候，外部各种set不会有线程安全问题
    //每次重启后会用新的定时器信息去运行,同时可以保证本次运行结束后，下次重新运行功能一样
    TimerInfo Tinfo=ParentTimer->m_TimerInfo;
    Tinfo.initTime();

    SleepByCondition Sp;  //高精度及时唤醒的延时方法

    while (getContinueRun())
    {
        double timeDiff=Tinfo.calltime-currTime();
        if (timeDiff>2)                        //差距比较大用系统延时
        {
            //Thread::sleep_for(1);        //or sleep(0)
            //Sp.sleep_for(1);                //faster
            Sp.sleep_for(timeDiff/2.0); //为了减轻负荷用sleep延时剩余时间的一半。逐步逼近
            continue;
        }
        if (timeDiff>1)   //不延时,轮询 //与理论时间差距小于1ms ,则调用定时回调函数
        {
            Sp.sleep_for(1);
            continue;
        }

        int retCallFun=0;
        Tinfo.in_time=currTime();
        if (Tinfo.pCbFun!=NULL)
        {
            retCallFun=Tinfo.pCbFun(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
        }
        else
        {
            ParentTimer->TimerEvent(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
        }
        Tinfo.updataNextTime();

        //停止定时器条件:
        //1).定时器回调函数返回值非0
        //2).对于非永久运行的定时器，运行次数已到
        if ((retCallFun!=0)||(Tinfo.timerCount!=UINT64_MAX&&Tinfo.alreadyCount>=Tinfo.timerCount))     
        {
            return;
        }
    }
}



inline TimerDynamic& GetTimerDynamic()
{
    //全局单例访问接口
    static TimerDynamic gTimerDynamic;
    return gTimerDynamic;
}


int TimerDynamic::AddTimer(double PeriodInms,TimerCallFun pCbFun,void* pUser,void* pData,uint64_t times)
{
    //小于0的时间周期没有意义，时间周期太小无法实现
    if (PeriodInms<1)
    {
        PeriodInms=1;
    }

    //运行0次没有实际意义，改为1
    if (times==0)
    {
        times=1;
    }

    //如果定时器线程没有在工作,则启动它
    if (!isTimerRuning())
    {
        StartTimer();
    }

    Message msg;
    msg.type=Msg_AddTimer;
    char* pBufMsg=msg.buf;

    //直接往消息结构填充数据
    TimerInfo& Tinfo=*(TimerInfo*)pBufMsg;
    Tinfo.peroid=PeriodInms;
    Tinfo.timerID=TimerIDbegin++;
    Tinfo.timerCount=times;
    Tinfo.alreadyCount=0;
    Tinfo.pCbFun=pCbFun;
    Tinfo.pUser=pUser;
    Tinfo.pData=pData;
    //Tinfo.begin_time=currTime();  //记住定时器插入时间
    Tinfo.bContinue=true;

    PreDprintf("Period:%.16f ms id:%u\n",PeriodInms,Tinfo.timerID);

    bool ret=timerThread.m_msgQueue.pushMsg(msg,false);
    //一些特殊情况的考虑,Timer后台线程将要退出的瞬间要添加定时器，导致没有实际运行。
    //确认机制或延时判断,待续TODO:

    return Tinfo.timerID;
}

void TimerDynamic::StopTimer(int timerID)
{
    Message msg;
    msg.type=Msg_AddTimer;
    char* pData=msg.buf;
    *(int*)pData=timerID;

    timerThread.m_msgQueue.pushMsg(msg,false);
}




void TimerDynamic::TimerEvent(int id,void* pTimerInfo,void* pUser,void* pData)
{
    double timeNow=currTime();

    PreDprintf("TimerID:%d Timers:%u timeNow:%f ms\n",id,((TimerInfo*)pTimerInfo)->alreadyCount,timeNow);

    static SuperTime tm("Timer::TimeEvent");
    tm.printReCallTime(24);
}


void TimerDynamic::TimerThread::run()
{
    PRINT_FUN_NAME

        ////为了启动效率，放类内
        //std::vector<TimerInfoV2> ListTimer;
        //ListTimer.reserve(100);

        //std::vector<TimerInfoV2> ListTmp;
        //ListTmp.reserve(100);

     SleepByCondition Sp;  //高精度及时唤醒的延时方法
    while (getContinueRun())
    {
        Message msg;
        if (m_msgQueue.getMsg(msg,0U))
        {
            switch (msg.type)
            {
            case Msg_AddTimer:
                {
                    TimerInfo& Tinfo=*(TimerInfo*)msg.buf;

                    Tinfo.initTime();   //收到新增定时器,在此刷新开始时间
                    ListTimer.push_back(Tinfo);    //添加定时器
                }
                break;
            case Msg_RemoveTimer:
                {
                    int timerID=*(int*)msg.buf;
                    RemovTimer(timerID,ListTimer);
                }
                break;
            default:
                {
                }
                break;
            }
        }

        //若定时器列表为空,直接退出
        if (ListTimer.empty())
        {
            return;
        }

        for (size_t n=0; n<ListTimer.size();n++)
        {
            TimerInfo& Tinfo=ListTimer[n];
            if (!Tinfo.bContinue)  //无效定时器，待清理
            {
                continue;
            }

            double timeDiff=Tinfo.calltime-currTime();
            if (timeDiff>1)  //与理论时间差距1ms ,则调用定时回调函数
            {
                continue;
            }

            int retCallFun=0;
            Tinfo.in_time=currTime();
            if (Tinfo.pCbFun!=NULL)
            {
                retCallFun=Tinfo.pCbFun(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
            }
            else
            {
                ParentTimer->TimerEvent(Tinfo.timerID,&Tinfo,Tinfo.pUser,Tinfo.pData);
            }
            //printf("theoryCallCount:%u Tinfo.alreadyCount:%u\n",theoryCallCount,Tinfo.alreadyCount);
            Tinfo.updataNextTime();


            //移除某定时器条件:
            //1).定时器回调函数返回值非0
            //2).对于非永久运行的定时器，运行次数已到
            if ((retCallFun!=0)||(Tinfo.timerCount!=UINT64_MAX&&Tinfo.alreadyCount>=Tinfo.timerCount))     
            {
                RemovTimer(Tinfo.timerID,ListTimer);//
                //若移除已经运行完成的定时器后,定时器列表为空,直接退出
                if (ListTimer.empty())
                {
                    return;
                }
            }
        }

        //Thread::sleep_for(1);   //or sleep(0) 
        Sp.sleep_for(1);             //faster
    }
}


class TimerTest:public Timer
{
public:
    TimerTest()
    {
        SetTimer(500);
        //timerID1=AddTimer(500.0);
        //timerID2=AddTimer((unsigned int)1000,5);
        StartTimer();
    }
    ~TimerTest()
    {

    }
    void TimerEvent(int id,void* pTimerInfo)
    {
        return Timer::TimerEvent(id,pTimerInfo);
    }
    int timerID1;
    int timerID2;
};



#ifdef UNIT_TEST
int Test_Timer()
{
    TimerTest tm;
    while (1)
    {
        Thread::sleep_for(10);
    }
}
#endif



NAME_SPACE_MAIN_END
