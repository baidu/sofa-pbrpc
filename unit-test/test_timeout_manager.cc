// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>
#include <gtest/gtest.h>
#include <sofa/pbrpc/timeout_manager.h>

using namespace ::sofa::pbrpc;

class TimeoutManagerTest: public ::testing::Test
{
protected:
    TimeoutManagerTest() {}
    virtual ~TimeoutManagerTest() {}
    virtual void SetUp() {
        // Called before every TEST_F(TimeoutManagerTest, *)
    }
    virtual void TearDown() {
        // Called after every TEST_F(TimeoutManagerTest, *)
    }
};

typedef std::pair<TimeoutManager::Id, TimeoutManager::Type> Event;
typedef std::vector<Event> EventVec;

static void SimpleCallback(EventVec* events,
        TimeoutManager::Id id, TimeoutManager::Type type)
{
    events->push_back(std::make_pair(id, type));
}
TEST_F(TimeoutManagerTest, Simple)
{
    TimeoutManager q;
    EventVec events;

    ASSERT_EQ(1u, q.add(100, NewExtClosure(&SimpleCallback, &events)));
    ASSERT_EQ(2u, q.add(200, NewExtClosure(&SimpleCallback, &events)));
    ASSERT_EQ(3u, q.add_repeating(200, NewPermanentExtClosure(&SimpleCallback, &events)));

    usleep(500000);

    ASSERT_EQ(4u, events.size());
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[0]);
    ASSERT_EQ(Event(2u, TimeoutManager::TIMEOUTED), events[1]);
    ASSERT_EQ(Event(3u, TimeoutManager::TIMEOUTED), events[2]);
    ASSERT_EQ(Event(3u, TimeoutManager::TIMEOUTED), events[3]);

    q.clear();

    ASSERT_EQ(5u, events.size());
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[0]);
    ASSERT_EQ(Event(2u, TimeoutManager::TIMEOUTED), events[1]);
    ASSERT_EQ(Event(3u, TimeoutManager::TIMEOUTED), events[2]);
    ASSERT_EQ(Event(3u, TimeoutManager::TIMEOUTED), events[3]);
    ASSERT_EQ(Event(3u, TimeoutManager::CLEARED),   events[4]);
}

static void EraseCallback(TimeoutManager* q, EventVec* events,
        TimeoutManager::Id id, TimeoutManager::Type type)
{
    events->push_back(std::make_pair(id, type));
    if (id == 2) {
        q->erase(1);
    }
}
TEST(TimeoutManager, Erase)
{
    TimeoutManager q;
    EventVec events;

    ASSERT_EQ(1u, q.add_repeating(200, NewPermanentExtClosure(&EraseCallback, &q, &events)));
    ASSERT_EQ(2u, q.add(500, NewExtClosure(&EraseCallback, &q, &events)));

    usleep(600000);

    ASSERT_EQ(4u, events.size());
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[0]);
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[1]);
    ASSERT_EQ(Event(2u, TimeoutManager::TIMEOUTED), events[2]);
    ASSERT_EQ(Event(1u, TimeoutManager::ERASED),    events[3]);

    q.clear();

    ASSERT_EQ(4u, events.size());
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[0]);
    ASSERT_EQ(Event(1u, TimeoutManager::TIMEOUTED), events[1]);
    ASSERT_EQ(Event(2u, TimeoutManager::TIMEOUTED), events[2]);
    ASSERT_EQ(Event(1u, TimeoutManager::ERASED),    events[3]);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
