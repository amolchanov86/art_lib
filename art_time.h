#ifndef ART_TIME_H
#define ART_TIME_H

#include <time.h>
#include <stdio.h>
//#include <chrono>
//using namespace std::chrono;

namespace Art {

double ArtClock();

template<class T>
class valTracker
{
public:
    valTracker(double callback_interval_in, const T& val_in = T() )
    {
        val_prev = val_in;
        callback_interval = callback_interval_in;
//        last_time = high_resolution_clock::now();
        last_time = Art::ArtClock();
    }
    virtual ~valTracker() {}


    /// Call this function if comparing absolute values
    void tick(const T& cur_val)
    {
        //        clock_t cur_time;
//        double time_gone = (double) (cur_time - last_time) / (double) CLOCKS_PER_SEC;
//        duration<double> time_gone = duration_cast<duration<double> >(cur_time - last_time);\

        double cur_time = Art::ArtClock();
        double time_gone = cur_time - last_time;
        if( !compare(cur_val, val_prev) && time_gone >= callback_interval )
        {
            last_time = cur_time;
            callBack();
        }
    }

    /// Call this function if addition happening
    void tickAdd(const T& add_val)
    {
//        high_resolution_clock::time_point cur_time;
        double cur_time = Art::ArtClock();
        val_accum = val_accum + add_val;
//        double time_gone = (double) (cur_time - last_time) / (double) CLOCKS_PER_SEC;
        double time_gone = cur_time - last_time;
        if( !compare(val_accum, val_prev) && time_gone >= callback_interval )
        {
//            printf("time gone = %f \n", time_gone);
            val_prev = val_accum;
            last_time = cur_time;
            callBack();
        }
    }

    /// Setting how often one should check
    bool setCallbackInterval( double callback_interval_in )
    {
        if(callback_interval_in > 0)
        {
            callback_interval = callback_interval_in;
            return true;
        }
        else
        {
            return false;
        }
    }


protected:
    bool getCallbackInterval() const {return callback_interval;}

    /// callBack for the case
    virtual void callBack()
    {
        printf("Callback triggered ... \n");
    }

    /// Comparsion for overloading
    virtual bool compare(const T& cur_val, const T& prev_val)
    {
        return prev_val == cur_val;
    }

    static const double callback_interval_def = 1; ///< default callback interval = 1s

    T val_prev;
    T val_accum;
    double callback_interval;
//        high_resolution_clock::time_point last_time;
    double last_time;
};

class Time
{
public:
  Time();
  Time(const timespec& time_in);
  Time(long sec, long nsec);

  /// Gets time and updates internal variables
  void updateTime();
  static double now();
  static void now(timespec& cur_time);

  /// Full current time current time can be found as:
  /// updateTime(); ///< Updates the internal variable
  /// double cur_time =  getSec() + getMSec() + getNSec();

  /// Time (s) with precision above 1000000 (s)
  /// (doesn't update internal time by def)
  double getMSec( bool update = false  );

  /// Time (s) reminder of division by 1000000 and rounded to nearest int
  /// (doesn't update internal time by def)
  double getSec( bool update = false  );
  /// nsec part of time converted to sec
  double getNSec( bool update = false  );

  /// Operators
  Time& operator+(double rhs);
  Time& operator-(double rhs);
  Time& operator+(const Art::Time& rhs);
  Time& operator-(const Art::Time& rhs);

  timespec time;
};

}

#endif // ART_TIME_H
