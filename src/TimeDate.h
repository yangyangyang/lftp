/*
 * lftp - file transfer program
 *
 * Copyright (c) 1996-2012 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEDATE_H
#define TIMEDATE_H

#include <time.h>
#ifdef TM_IN_SYS_TIME
# include <sys/types.h>
# include <sys/time.h>
#endif

class time_tuple
{
   time_t sec;
   int msec;
   void add(time_t s,int ms);   // must be |ms|<1000

protected:
   time_t get_seconds()   const { return sec; }
   int get_milliseconds() const { return msec; }
   void normalize();
   void add_sec(long s) { sec+=s; }
   void add(double);
   void add(const time_tuple &o) { add(o.sec,o.msec); }
   void sub(const time_tuple &o) { add(-o.sec,-o.msec); }
   void sub(double o) { add(-o); }
   void set(time_t s,int ms) { sec=s; msec=ms; normalize(); }
   void set(const time_tuple &o) { sec=o.sec; msec=o.msec; }
   bool lt(const time_tuple &o) const { return sec<o.sec || (sec==o.sec && msec<o.msec); }
   double to_double() const;
};

class TimeDiff;

class Time : public time_tuple
{
public:
   Time();
   Time(time_t s,int ms=0)     { set(s,ms); }
   void Set(time_t s,int ms=0) { set(s,ms); }
   void SetToCurrentTime();

   time_t UnixTime() const { return get_seconds(); }
   int MilliSecond() const { return get_milliseconds(); }

   inline const Time& operator+=(const TimeDiff &o);
   inline const Time& operator-=(const TimeDiff &o);
   inline TimeDiff operator-(const Time &o) const;
   Time operator+(const TimeDiff &o) const { Time t(*this); t+=o; return t; }
   Time operator-(const TimeDiff &o) const { Time t(*this); t-=o; return t; }
   bool operator<(const Time &o) const { return this->lt(o); }
   bool operator>=(const Time &o) const { return !(*this<o); }

   operator time_t() const { return UnixTime(); }
   bool Passed(int s) const;
};

class TimeDate : public Time
{
   time_t local_time_unix;
   struct tm local_time;

   void set_local_time();

public:
   TimeDate() { SetToCurrentTime(); local_time_unix=0; }
   TimeDate(time_t s,int ms=0) : Time(s,ms) { local_time_unix=0; }

   operator const struct tm *() { set_local_time(); return &local_time; }
   operator const struct tm &() { set_local_time(); return local_time; }
   int Year()	     { set_local_time(); return local_time.tm_year+1900; }
   int Month()	     { set_local_time(); return local_time.tm_mon+1; }
   int DayOfMonth()  { set_local_time(); return local_time.tm_mday; }
   int Hour()	     { set_local_time(); return local_time.tm_hour; }
   int Minute()	     { set_local_time(); return local_time.tm_min; }
   int Second()	     { set_local_time(); return local_time.tm_sec; }

   // returns static string.
   const char *IsoDateTime();
};

// maybe it is better to make it double.
class TimeDiff : public time_tuple
{
public:
   void SetDiff(const Time&a,const Time&b) { this->set(a); sub(b); }
   TimeDiff() {}
   TimeDiff(const Time&a,const Time&b) { SetDiff(a,b); }
   TimeDiff(time_t s,int ms) { set(s,ms); }
   void Set(time_t s,int ms) { set(s,ms); }
   operator double() const { return to_double(); }
   void Set(double s);

   bool operator<(const TimeDiff &o) const { return this->lt(o); }
   bool operator>=(const TimeDiff &o) const { return !(*this<o); }
   bool operator<(int s) const { return get_seconds()<s; }
   bool operator>=(int s) const { return get_seconds()>=s; }

   const TimeDiff &operator-=(const TimeDiff &o) { sub(o); return *this; }
   const TimeDiff &operator+=(const TimeDiff &o) { add(o); return *this; }

   int MilliSeconds() const;
   time_t Seconds() const;

   static const TimeDiff& valueOf(double v);
};

inline TimeDiff Time::operator-(const Time &o) const { return TimeDiff(*this,o); }
inline const Time &Time::operator+=(const TimeDiff &o) { add(o); return *this; }
inline const Time &Time::operator-=(const TimeDiff &o) { sub(o); return *this; }

class TimeInterval : public TimeDiff
{
protected:
   bool infty;
public:
   TimeInterval() : TimeDiff(0,0) { infty=true; }
   TimeInterval(time_t s,int ms) : TimeDiff(s,ms) { infty=false; }
   TimeInterval(const TimeDiff &d) : TimeDiff(d) { infty=false; }
   void SetInfty(bool i=true) { infty=i; }
   bool IsInfty() const { return infty; }
   bool Finished(const Time &base) const;
   int GetTimeout(const Time &base) const;
   bool operator<(const TimeInterval &o) const { return infty<o.infty || lt(o); }
   bool operator>=(const TimeInterval &o) const { return !(*this<o); }
   bool operator<(int s) const { return !infty && get_seconds()<s; }
   bool operator>=(int s) const { return infty || get_seconds()>=s; }
   const char *toString(unsigned flags);
   enum {
      TO_STR_TRANSLATE=1,
      TO_STR_TERSE=2
   };
};

#define MINUTE (60)
#define HOUR   (60*MINUTE)
#define DAY    (24*HOUR)

#endif
