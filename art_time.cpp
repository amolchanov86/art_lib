#include "art_time.h"

namespace Art
{

double ArtClock()
{
    timespec cur_time;
    clock_gettime(CLOCK_MONOTONIC, &cur_time);
    long int mks = cur_time.tv_nsec / (long int) 1000;
    long int s2mks = (long int) cur_time.tv_sec * 1000000 + mks;
    double ss = (double) s2mks / 1000000.0;
//    printf("s2mks = %ld  mks = %ld s = %f\n", s2mks, mks, ss);
//    printf("%lld.%.9ld", (long long)cur_time.tv_sec, cur_time.tv_nsec);
    return ss;
}

Time::Time(){ updateTime(); }
Time::Time(const timespec& time_in){ time = time_in;}
Time::Time(long sec, long nsec)
{
  time.tv_sec = sec;
  time.tv_nsec = nsec;
}

/// Interfaces for getting the current time
void Time::now(timespec& cur_time)
{
  clock_gettime(CLOCK_REALTIME, &cur_time);
}

double Time::now()
{
  timespec cur_time;
  now(cur_time);
  return (double) cur_time.tv_sec + ( (double)cur_time.tv_nsec / (double)1000000000.0 );
}

/// Gets time and updates internal variables
void Time::updateTime()
{
  now(time);
}

/// Full current time current time can be found as:
/// updateTime(); ///< Updates the internal variable
/// double cur_time =  getMSec() + getMSec() + getNSec();

/// Time (s) with precision above 1000000 (s)
/// (doesn't update internal time by def)
double Time::getMSec( bool update )
{
  return (double)( (long) time.tv_sec / (long) 1000000  )* (double) 1000000;
}

/// Time (s) reminder of division by 1000000 and rounded to nearest int
/// (doesn't update internal time by def)
double Time::getSec( bool update )
{
  return (double)( (long) time.tv_sec % 1000000 );
}

/// nsec part of time converted to sec
double Time::getNSec( bool update )
{
  return (double) time.tv_nsec / (double)1000000000.0;
}

Time& Time::operator+(double rhs)
{

}

Time& Time::operator-(double rhs)
{

}

Time& Time::operator+(const Art::Time& rhs)
{

}

Time& Time::operator-(const Art::Time& rhs)
{

}

}
