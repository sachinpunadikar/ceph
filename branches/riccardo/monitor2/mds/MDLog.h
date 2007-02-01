// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */


#ifndef __MDLOG_H
#define __MDLOG_H

#include "include/types.h"
#include "include/Context.h"

#include <list>

//#include <ext/hash_map>
//using __gnu_cxx::hash_mapset;

class Journaler;
class LogEvent;
class MDS;

class Logger;

/*
namespace __gnu_cxx {
  template<> struct hash<LogEvent*> {
    size_t operator()(const LogEvent *p) const { 
      static hash<unsigned long> H;
      return H((unsigned long)p); 
    }
  };
}
*/

class MDLog {
 protected:
  MDS *mds;
  size_t num_events; // in events
  size_t max_events;

  int unflushed;

  inode_t log_inode;
  Journaler *journaler;

  
  //hash_map<LogEvent*>  trimming;       // events currently being trimmed
  map<off_t, LogEvent*> trimming;
  std::list<Context*>  trim_waiters;   // contexts waiting for trim
  bool                 trim_reading;

  bool waiting_for_read;
  friend class C_MDL_Reading;

  Logger *logger;
  
  list<Context*> waitfor_replay;

 public:
  MDLog(MDS *m);
  ~MDLog();
  
  void set_max_events(size_t max) { max_events = max; }
  size_t get_max_events() { return max_events; }
  size_t get_num_events() { return num_events + trimming.size(); }

  void submit_entry( LogEvent *e, Context *c = 0 );
  void wait_for_sync( Context *c );
  void flush();

  void trim(Context *c);
  void _did_read();
  void _trimmed(LogEvent *le);

  void reset();  // fresh, empty log! 
  void open(Context *onopen);
  void write_head(Context *onfinish);

  void replay(Context *onfinish);
  void _replay();
};

#endif
