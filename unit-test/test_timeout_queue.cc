// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <vector>
#include <gtest/gtest.h>
#include <sofa/pbrpc/timeout_queue.h>

using namespace ::sofa::pbrpc;

class TimeoutQueueTest: public ::testing::Test
{
protected:
    TimeoutQueueTest() {}
    virtual ~TimeoutQueueTest() {}
    virtual void SetUp() {
        // Called before every TEST_F(TimeoutQueueTest, *)
    }
    virtual void TearDown() {
        // Called after every TEST_F(TimeoutQueueTest, *)
    }
};

typedef std::vector<uint64> EventVec;

static void SimpleCallback(
        EventVec* events, uint64 id, int64 /*now*/)
{
    events->push_back(id);
}
TEST_F(TimeoutQueueTest, Simple)
{
    EventVec events;
    TimeoutQueue q;

    ASSERT_EQ(1u, q.add(0, 10, NewExtClosure(
                    &SimpleCallback, &events)));
    ASSERT_EQ(2u, q.add(0, 11, NewExtClosure(
                    &SimpleCallback, &events)));
    ASSERT_EQ(3u, q.add_repeating(0, 9, NewPermanentExtClosure(
                    &SimpleCallback, &events)));

    ASSERT_TRUE(events.empty());
    ASSERT_EQ(21, q.run_once(12));  // now+9

    ASSERT_EQ(3u, events.size());
    ASSERT_EQ(3u, events[0]);
    ASSERT_EQ(1u, events[1]);
    ASSERT_EQ(2u, events[2]);

    events.clear();
    ASSERT_EQ(49, q.run_once(40));
    ASSERT_EQ(1u, events.size());
    ASSERT_EQ(3u, events[0]);
}

static void EraseCallback(
        TimeoutQueue* q, EventVec* events, uint64 id, int64 /*now*/)
{
    events->push_back(id);
    if (id == 2) {
        q->erase(1);
    }
}
TEST(TimeoutQueue, Erase)
{
    TimeoutQueue q;
    EventVec events;

    ASSERT_EQ(1u, q.add_repeating(0, 10, NewPermanentExtClosure(
                    &EraseCallback, &q, &events)));
    ASSERT_EQ(2u, q.add(0, 35, NewExtClosure(
                    &EraseCallback, &q, &events)));

    int64 now = 0;
    while (now < kint64max) {
        now = q.run_once(now);
    }

    ASSERT_EQ(4u, events.size());
    ASSERT_EQ(1u, events[0]);
    ASSERT_EQ(1u, events[1]);
    ASSERT_EQ(1u, events[2]);
    ASSERT_EQ(2u, events[3]);
}

static void RunOnceRepeatingCallback(
        TimeoutQueue* q, int* count, uint64 id, int64 /*now*/)
{
    if (++(*count) == 100) {
        ASSERT_TRUE(q->erase(id));
    }
}
TEST(TimeoutQueue, RunOnceRepeating)
{
    int count = 0;
    TimeoutQueue q;

    ASSERT_EQ(1u, q.add_repeating(0, 0, NewPermanentExtClosure(
                    &RunOnceRepeatingCallback, &q, &count)));

    ASSERT_EQ(0, q.run_once(0));
    ASSERT_EQ(1, count);
    ASSERT_EQ(0, q.run_once(0));
    ASSERT_EQ(2, count);
    ASSERT_EQ(kint64max, q.run_loop(0));
    ASSERT_EQ(100, count);
}

static void RunOnceRescheduleCallback(
        TimeoutQueue* q, int* count, uint64 id, int64 now)
{
    if (++(*count) < 100) {
        ASSERT_LT(id, q->add(now, 0, NewExtClosure(
                        &RunOnceRescheduleCallback, q, count)));
    }
}
TEST(TimeoutQueue, RunOnceReschedule)
{
    int count = 0;
    TimeoutQueue q;

    ASSERT_EQ(1u, q.add(0, 0, NewExtClosure(
                    &RunOnceRescheduleCallback, &q, &count)));

    ASSERT_EQ(0, q.run_once(0));
    ASSERT_EQ(1, count);
    ASSERT_EQ(0, q.run_once(0));
    ASSERT_EQ(2, count);
    ASSERT_EQ(kint64max, q.run_loop(0));
    ASSERT_EQ(100, count);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
