/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "rw_lock_generic.h"

#include "condition_variable_wrapper.h"
#include "critical_section_wrapper.h"

namespace webrtc {
//基于读写独占，写写独占，读读共享，优先提供写
RWLockGeneric::RWLockGeneric()
    : readers_active_(0),
      writer_active_(false),
      readers_waiting_(0),
      writers_waiting_(0) {
  critical_section_ = CriticalSectionWrapper::CreateCriticalSection();
  read_condition_ = ConditionVariableWrapper::CreateConditionVariable();
  write_condition_ = ConditionVariableWrapper::CreateConditionVariable();
}

RWLockGeneric::~RWLockGeneric() {
  delete write_condition_;
  delete read_condition_;
  delete critical_section_;
}

void RWLockGeneric::AcquireLockExclusive() //独占锁获得
{
  CriticalSectionScoped cs(critical_section_);
  //不判断写的等待者，可能引起超车的可能，但是其实也没什么关系，便于循环的判断
  if (writer_active_ || readers_active_ > 0)//如果已经正在写,或者还有读的操作则等待
  {
    ++writers_waiting_;//等待写操作加一
    while (writer_active_ || readers_active_ > 0) //等到不再写并且没有正在读操作
	{
      write_condition_->SleepCS(*critical_section_);
    }
    --writers_waiting_;//我们可以去操作了,等待写操作减一
  }
  writer_active_ = true;//获得独占锁
}

void RWLockGeneric::ReleaseLockExclusive() //独占锁释放
{
  CriticalSectionScoped cs(critical_section_);
  writer_active_ = false;//释放独占锁
  if (writers_waiting_ > 0) //如果有等待写的操作，则先唤醒写的操作
  {
    write_condition_->Wake();
  } 
  else if (readers_waiting_ > 0) //如果没有写的操作，但是有等待读的操作，则唤醒所有等待读的操作
  {
    read_condition_->WakeAll();
  }
}

void RWLockGeneric::AcquireLockShared() //共享锁获得
{
  CriticalSectionScoped cs(critical_section_);
  if (writer_active_ || writers_waiting_ > 0) //如果正在写操作，或者有等待的写操作
  {
    ++readers_waiting_;//等待读操作加一

    while (writer_active_ || writers_waiting_ > 0) //等到不再写，并且没有等待写操作
	{
      read_condition_->SleepCS(*critical_section_);
    }
    --readers_waiting_;//我们可以去读了，等待读操作减一
  }
  ++readers_active_;//增加读操作
}

void RWLockGeneric::ReleaseLockShared() //共享锁释放
{
  CriticalSectionScoped cs(critical_section_);
  --readers_active_;//减少正在读操作
  if (readers_active_ == 0 && writers_waiting_ > 0) //如果没有正在读操作，并且有等待写操作，唤醒写操作
  {
    write_condition_->Wake();
  }
}

}  // namespace webrtc
