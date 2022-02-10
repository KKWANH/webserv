#ifndef TIMER_HPP
#define TIMER_HPP

#include <vector>
#include <sys/time.h>

class Timer{
    private:
        typedef struct s_Pair {
            void *obj;
            long genTime;
            long expTime;
        } Pair;
        std::vector<Pait> _timeList;
    public:
        Timer();
        ~Timer();
        void addObj(void* obj, int lifetime);
        void resetObj(void* obj, int lifetime);
        void delObj(void* obj, int (*del)(void*));
        void checkTimer(void(*del)(void*));
};

Timer::Timer(){
}
Timer::~Timer(){
}

void Timer::addObj(void* obj, int lifetime){
    struct timeval time;
    Pair pair;

    gettimeofday(&time, NULL);
    pair.obj = obj;
    pair.genTime = time.tv_sec;
    pair.expTime = time.tv_sec + long(lifetime);
    _timeList.push_back(pair);
}

void Timer::resetObj(void* obj, int lifetime){
    struct timeval time;
    std::vector<Pair>::iterator iter;
    gettimeofday(&time, NULL);
    for (iter = _timeList.begin(); iter != _timeList.end(); iter++){
        if ((*iter).obj == obj){
            (*iter).genTime = time.tv_sec;
            (*iter).expTime = time.tv_sec + long(lifetime);
            break;
        }
    }
}

void Timer::delObj(void* obj, void (*del)(void*)){
    std::vector<Pair>::iterator iter;
    for (iter = _timeList.begin(); iter != _timeList.end(); iter++){
        if ((*iter).obj == obj){
            del((*iter).obj);
            iter = _timerList.erase(iter);
            break;
        }
    }
}

void Timer::checkTimer(void (*del)(void*)){
    struct timeval time;
    gettimeofday(&time, NULL)
    std::vector<Pair>::iterator iter;
    for (iter = _timeList.begin(); iter != _timeList.end(); iter++){
        if ((*iter).expTime == time.tv_sec){
            del((*iter).obj);
            iter = _timerList.erase(iter);
            break;
        }
    };   
}


#endif