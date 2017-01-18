// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/ext_closure.h>
#include <sofa/pbrpc/smart_ptr/smart_ptr.hpp>
#include <gtest/gtest.h>

using namespace ::sofa::pbrpc;

//   preArg
void test0() {
    printf("test0()\r\n");
}
void test1(char preA) {
    printf("test1(),preA=%c\r\n", preA);
}
void test2(char preA, short preB) {
    printf("test2(),preA=%c,preB=%d\r\n", preA, preB);
}
void test3(char preA, short preB, int preC) {
    printf("test3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
}

//   preArg_Arg(last one)
void test0_1(char a) {
    printf("test0_1(),a=%c\r\n", a);
}
void test1_1(char preA, short b) {
    printf("test1_1(),preA=%c,b=%d\r\n", preA, b);
}
void test2_1(char preA, short preB, int c) {
    printf("test2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
}

//   preArg_Arg(last)
void test0_2(char a, short b) {
    printf("test0_2(),a=%c,b=%d\r\n", a, b);
}
void test1_2(char preA, char a, int b) {
    printf("test1_2(),preA=%c,a=%c,b=%d\n", preA, a, b);
}

//   preArg_Arg(last)
void test0_3(char a, short b, int c) {
    printf("test0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
}

//----------ExtClosure---------
//       preArg
char Rtest0() {
    printf("Rtest0()\r\n");
    return 'a';
}
char Rtest1(char preA) {
    printf("Rtest1(),preA=%c\r\n", preA);
    return preA;
}
char Rtest2(char preA, short preB) {
    printf("Rtest2(),preA=%c,preB=%d\r\n", preA, preB);
    return preA;
}
char Rtest3(char preA, short preB, int preC) {
    printf("Rtest3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
    return preA;
}

//   preArg_Arg(last one)
short Rtest0_1(char a) {
    printf("Rtest0_1(),a=%c\r\n", a);
    return short(980);
}

short Rtest1_1(char preA, short b) {
    printf("Rtest1_1(),preA=%c,b=%d\r\n", preA, b);
    return b;
}

short Rtest2_1(char preA, short preB, int c) {
    printf("Rtest2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
    return preB;
}

//   preArg_Arg(last)
int Rtest0_2(char a, short b) {
    printf("Rtest0_2(),a=%c,b=%d\r\n", a, b);
    return int(9900);
}
int Rtest1_2(char preA, char a, int b) {
    printf("Rtest1_2(),preA=%c,a=%c,b=%d\r\n", preA, a, b);
    return b;
}

//   preArg_Arg(last)
unsigned int Rtest0_3(char a, short b, int c) {
    printf("Rtest0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
    return (unsigned int)c;
}

class CTest
{
public:
    CTest() {}
    virtual ~CTest() {}

    //       preArg
    void test0() {
        printf("test0()\r\n");
    }
    void test1(char preA) {
        printf("test1(),preA=%c\r\n", preA);
    }
    void test2(char preA, short preB) {
        printf("test2(),preA=%c,preB=%d\r\n", preA, preB);
    }
    void test3(char preA, short preB, int preC) {
        printf("test3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
    }

    //   preArg_Arg(last one)
    void test0_1(char a) {
        printf("test0_1(),a=%c\r\n", a);
    }
    void test1_1(char preA, short b) {
        printf("test1_1(),preA=%c,b=%d\r\n", preA, b);
    }
    void test2_1(char preA, short preB, int c) {
        printf("test2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
    }

    //   preArg_Arg(last)
    void test0_2(char a, short b) {
        printf("test0_2(),a=%c,b=%d\r\n", a, b);
    }
    void test1_2(char preA, char a, int b) {
        printf("test1_2(),preA=%c,a=%c,b=%d\n", preA, a, b);
    }

    //   preArg_Arg(last)
    void test0_3(char a, short b, int c) {
        printf("test0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
    }

    //----------ExtClosure---------
    //       preArg
    char Rtest0() {
        printf("Rtest0()\r\n");
        return 'a';
    }
    char Rtest1(char preA) {
        printf("Rtest1(),preA=%c\r\n", preA);
        return preA;
    }
    char Rtest2(char preA, short preB) {
        printf("Rtest2(),preA=%c,preB=%d\r\n", preA, preB);
        return preA;
    }
    char Rtest3(char preA, short preB, int preC) {
        printf("Rtest3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
        return preA;
    }

    //   preArg_Arg(last one)
    short Rtest0_1(char a) {
        printf("Rtest0_1(),a=%c\r\n", a);
        return short(980);
    }
    short Rtest1_1(char preA, short b) {
        printf("Rtest1_1(),preA=%c,b=%d\r\n", preA, b);
        return b;
    }
    short Rtest2_1(char preA, short preB, int c) {
        printf("Rtest2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
        return preB;
    }

    //   preArg_Arg(last)
    int Rtest0_2(char a, short b) {
        printf("Rtest0_2(),a=%c,b=%d\r\n", a, b);
        return int(9900);
    }
    int Rtest1_2(char preA, char a, int b) {
        printf("Rtest1_2(),preA=%c,a=%c,b=%d\r\n", preA, a, b);
        return b;
    }

    //   preArg_Arg(last)
    unsigned int Rtest0_3(char a, short b, int c) {
        printf("Rtest0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
        return (unsigned int) c;
    }
};
TEST(ExtClosure, ExtClosure)
{
    printf("\r\n==============test NewExtClosure===============\r\n");

    ASSERT_TRUE(true);

    // Closure0
    ExtClosure<void()> * pcb = NewExtClosure(test0);
    ASSERT_TRUE(pcb->IsSelfDelete());
    pcb->Run();

    pcb = NewExtClosure(test1, 'a');
    pcb->Run();

    pcb = NewExtClosure(test2, char('a'), short(456));
    pcb->Run();

    pcb = NewExtClosure(test3, char('a'), short(456), int(9800));
    pcb->Run();

    // Closure1
    ExtClosure<void(char)>* pcb0_1 = NewExtClosure(test0_1);
    pcb0_1->Run('a');

    ExtClosure<void(short)>* pcb1_1 = NewExtClosure(test1_1, 'a');
    pcb1_1->Run(780);

    ExtClosure<void(int)>* pcb2_1 = NewExtClosure(test2_1, char('a'), short(7800));
    pcb2_1->Run(9800);

    // Closure2
    ExtClosure<void(char, short)>* pcb0_2 = NewExtClosure(test0_2);
    pcb0_2->Run('a', 8900);

    ExtClosure<void(char, int)>* pcb1_2 = NewExtClosure(test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    ExtClosure<void(char, short, int)>* pcb0_3 = NewExtClosure(test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------ExtClosure------------
    printf("\r\n------ExtClosure------------\r\n");

    // Closure0
    ExtClosure<char()>* pRcb = NewExtClosure(Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(Rtest2, char('a'), short(456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(Rtest3, char('a'), short(456), int(9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    ExtClosure<short(char)>* pRcb0_1 = NewExtClosure(Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    ExtClosure<short(short)>* pRcb1_1 = NewExtClosure(Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    ExtClosure<short(int)>* pRcb2_1 = NewExtClosure(Rtest2_1, char('a'), short(7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    ExtClosure<int(char, short)>* pRcb0_2 = NewExtClosure(Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    ExtClosure<int(char, int)>* pRcb1_2 = NewExtClosure(Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    ExtClosure<unsigned int(char, short, int)>* pRcb0_3 = NewExtClosure(Rtest0_3);
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
}

TEST(ExtClosure, PermanentClosure)
{
    printf("\r\n==============test NewPermanentExtClosure===============\r\n");
    // Closure0
    ExtClosure<void()>* pcb = NewPermanentExtClosure(test0);
    ASSERT_FALSE(pcb->IsSelfDelete());
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(test1, 'a');
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(test2, char('a'), short(456));
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(test3, char('a'), short(456), int(9800));
    pcb->Run();
    delete pcb;

    // Closure1
    ExtClosure<void(char)>* pcb0_1 = NewPermanentExtClosure(test0_1);
    pcb0_1->Run('a');
    delete pcb0_1;

    ExtClosure<void(short)>* pcb1_1 = NewPermanentExtClosure(test1_1, 'a');
    pcb1_1->Run(780);
    delete pcb1_1;

    ExtClosure<void(int)>* pcb2_1 = NewPermanentExtClosure(test2_1, char('a'), short(7800));
    pcb2_1->Run(9800);
    delete pcb2_1;

    // Closure2
    ExtClosure<void(char, short)>* pcb0_2 = NewPermanentExtClosure(test0_2);
    pcb0_2->Run('a', 8900);
    delete pcb0_2;

    ExtClosure<void(char, int)>* pcb1_2 = NewPermanentExtClosure(test1_2, 'a');
    pcb1_2->Run('b', 780);
    delete pcb1_2;

    // Closure3
    ExtClosure<void(char, short, int)>* pcb0_3 = NewPermanentExtClosure(test0_3);
    pcb0_3->Run('a', 456, 78909);
    delete pcb0_3;

    //------ExtClosure------------
    printf("\r\n------ExtClosure------------\r\n");

    // Closure0
    ExtClosure<char()>* pRcb = NewPermanentExtClosure(Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);
    delete pRcb;

    pRcb = NewPermanentExtClosure(Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);
    delete pRcb;

    pRcb = NewPermanentExtClosure(Rtest2, char('a'), short(456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);
    delete pRcb;

    pRcb = NewPermanentExtClosure(Rtest3, char('a'), short(456), int(9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);
    delete pRcb;

    // Closure1
    ExtClosure<short(char)>* pRcb0_1 = NewPermanentExtClosure(Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);
    delete pRcb0_1;

    ExtClosure<short(short)>* pRcb1_1 = NewPermanentExtClosure(Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);
    delete pRcb1_1;

    ExtClosure<short(int)>* pRcb2_1 = NewPermanentExtClosure(Rtest2_1, char('a'), short(7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);
    delete pRcb2_1;

    // Closure2
    ExtClosure<int(char, short)>* pRcb0_2 = NewPermanentExtClosure(Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);
    delete pRcb0_2;

    ExtClosure<int(char, int)>* pRcb1_2 = NewPermanentExtClosure(Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);
    delete pRcb1_2;

    // Closure3
    ExtClosure<unsigned int(char, short, int)>* pRcb0_3 = NewPermanentExtClosure(Rtest0_3);
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
    delete pRcb0_3;
}

TEST(ExtClosure, ClassClosure)
{
    printf("\r\n==============test NewExtClosure(Class)===============\r\n");

    CTest obj;

    // Closure0
    ExtClosure<void()>* pcb = NewExtClosure(&obj, &CTest::test0);
    pcb->Run();

    pcb = NewExtClosure(&obj, &CTest::test1, 'a');
    pcb->Run();

    pcb = NewExtClosure(&obj, &CTest::test2, char('a'), short(456));
    pcb->Run();

    pcb = NewExtClosure(&obj, &CTest::test3, char('a'), short(456), int(9800));
    pcb->Run();

    // Closure1
    ExtClosure<void(char)>* pcb0_1 = NewExtClosure(&obj, &CTest::test0_1);
    pcb0_1->Run('a');

    ExtClosure<void(short)>* pcb1_1 = NewExtClosure(&obj, &CTest::test1_1, 'a');
    pcb1_1->Run(short(780));

    ExtClosure<void(int)>* pcb2_1 = NewExtClosure(&obj, &CTest::test2_1, char('a'), short(7800));
    pcb2_1->Run(9800);

    // Closure2
    ExtClosure<void(char, short)>* pcb0_2 = NewExtClosure(&obj, &CTest::test0_2);
    pcb0_2->Run('a', 8900);

    ExtClosure<void(char, int)>* pcb1_2 = NewExtClosure(&obj, &CTest::test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    ExtClosure<void(char, short, int)>* pcb0_3 = NewExtClosure(&obj, &CTest::test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------ExtClosure------------
    printf("\r\n------ExtClosure------------\r\n");

    // Closure0
    ExtClosure<char()>* pRcb = NewExtClosure(&obj, &CTest::Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(&obj, &CTest::Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(&obj, &CTest::Rtest2, char('a'), short(456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(&obj, &CTest::Rtest3, char('a'), short(456), int(9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    ExtClosure<short(char)>* pRcb0_1 = NewExtClosure(&obj, &CTest::Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    ExtClosure<short(short)>* pRcb1_1 = NewExtClosure(&obj, &CTest::Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    ExtClosure<short(int)>* pRcb2_1 = NewExtClosure(&obj, &CTest::Rtest2_1, char('a'), short(7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    ExtClosure<int(char, short)>* pRcb0_2 =
        NewExtClosure(&obj, &CTest::Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    ExtClosure<int(char, int)>* pRcb1_2 = NewExtClosure(&obj, &CTest::Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    ExtClosure<void(char, short, int)>* pRcb0_3 = NewExtClosure(&obj, &CTest::test0_3);
    pRcb0_3->Run('a', 456, 78909);
}

TEST(ExtClosure, ClassPermanentClosure)
{
    printf("\r\n==============test NewExtClosure(Class)===============\r\n");

    CTest obj;

    // Closure0
    ExtClosure<void()>* pcb = NewPermanentExtClosure(&obj, &CTest::test0);
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(&obj, &CTest::test1, 'a');
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(&obj, &CTest::test2, char('a'), short(456));
    pcb->Run();
    delete pcb;

    pcb = NewPermanentExtClosure(&obj, &CTest::test3, char('a'), short(456), int(9800));
    pcb->Run();
    delete pcb;

    // Closure1
    ExtClosure<void(char)>* pcb0_1 = NewPermanentExtClosure(&obj, &CTest::test0_1);
    pcb0_1->Run('a');
    delete pcb0_1;

    ExtClosure<void(short)>* pcb1_1 = NewPermanentExtClosure(&obj, &CTest::test1_1, 'a');
    pcb1_1->Run(short(780));

    ExtClosure<void(int)>* pcb2_1 =
        NewPermanentExtClosure(&obj, &CTest::test2_1, char('a'), short(7800));
    pcb2_1->Run(9800);

    // Closure2
    ExtClosure<void(char, short)>* pcb0_2 = NewPermanentExtClosure(&obj, &CTest::test0_2);
    pcb0_2->Run('a', 8900);

    ExtClosure<void(char, int)>* pcb1_2 = NewPermanentExtClosure(&obj, &CTest::test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    ExtClosure<void(char, short, int)>* pcb0_3 = NewPermanentExtClosure(&obj, &CTest::test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------ExtClosure------------
    printf("\r\n------ExtClosure------------\r\n");

    // Closure0
    ExtClosure<char()>* pRcb = NewPermanentExtClosure(&obj, &CTest::Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewPermanentExtClosure(&obj, &CTest::Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewPermanentExtClosure(&obj, &CTest::Rtest2, char('a'), short(456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewPermanentExtClosure(&obj, &CTest::Rtest3, char('a'), short(456), int(9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    ExtClosure<short(char)>* pRcb0_1 = NewPermanentExtClosure(&obj, &CTest::Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    ExtClosure<short(short)>* pRcb1_1 = NewPermanentExtClosure(&obj, &CTest::Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    ExtClosure<short(int)>* pRcb2_1 =
        NewPermanentExtClosure(&obj, &CTest::Rtest2_1, char('a'), short(7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    ExtClosure<int(char, short)>* pRcb0_2 =
        NewPermanentExtClosure(&obj, &CTest::Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    ExtClosure<int(char, int)>* pRcb1_2 = NewPermanentExtClosure(&obj, &CTest::Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    ExtClosure<unsigned int(char, short, int)>* pRcb0_3 = NewPermanentExtClosure(&obj, &CTest::Rtest0_3);
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
}

TEST(ExtClosure, ClassSharedPtrClosure)
{
    printf("\r\n==============test NewExtClosure(ClassPtr)===============\r\n");

    ::sofa::pbrpc::shared_ptr<CTest> obj(new CTest());

    // Closure0
    ExtClosure<void()>* pcb = NewExtClosure(obj, &CTest::test0);
    pcb->Run();

    pcb = NewExtClosure(obj, &CTest::test1, 'a');
    pcb->Run();

    const ::sofa::pbrpc::shared_ptr<CTest>& obj_const_ref = obj;
    pcb = NewExtClosure(obj_const_ref, &CTest::test1, 'a');
    pcb->Run();

    pcb = NewExtClosure(obj, &CTest::test2, char('a'), short(456));
    pcb->Run();

    pcb = NewExtClosure(obj, &CTest::test3, char('a'), short(456), int(9800));
    pcb->Run();

    // Closure1
    ExtClosure<void(char)>* pcb0_1 = NewExtClosure(obj, &CTest::test0_1);
    pcb0_1->Run('a');

    ExtClosure<void(short)>* pcb1_1 = NewExtClosure(obj, &CTest::test1_1, 'a');
    pcb1_1->Run(short(780));

    ExtClosure<void(int)>* pcb2_1 = NewExtClosure(obj, &CTest::test2_1, char('a'), short(7800));
    pcb2_1->Run(9800);

    // Closure2
    ExtClosure<void(char, short)>* pcb0_2 = NewExtClosure(obj, &CTest::test0_2);
    pcb0_2->Run('a', 8900);

    ExtClosure<void(char, int)>* pcb1_2 = NewExtClosure(obj, &CTest::test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    ExtClosure<void(char, short, int)>* pcb0_3 = NewExtClosure(obj, &CTest::test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------ExtClosure------------
    printf("\r\n------ExtClosure------------\r\n");

    // Closure0
    ExtClosure<char()>* pRcb = NewExtClosure(obj, &CTest::Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(obj, &CTest::Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(obj, &CTest::Rtest2, char('a'), short(456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewExtClosure(obj, &CTest::Rtest3, char('a'), short(456), int(9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    ExtClosure<short(char)>* pRcb0_1 = NewExtClosure(obj, &CTest::Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    ExtClosure<short(short)>* pRcb1_1 = NewExtClosure(obj, &CTest::Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    ExtClosure<short(int)>* pRcb2_1 = NewExtClosure(obj, &CTest::Rtest2_1, char('a'), short(7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    ExtClosure<int(char, short)>* pRcb0_2 =
        NewExtClosure(obj, &CTest::Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    ExtClosure<int(char, int)>* pRcb1_2 = NewExtClosure(obj, &CTest::Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    ExtClosure<void(char, short, int)>* pRcb0_3 = NewExtClosure(obj, &CTest::test0_3);
    pRcb0_3->Run('a', 456, 78909);
}

#if 0
class Performance : public testing::Test {};

const int kLoopCount = 1000000;

void NullFunction()
{
}

int AddFunction(int i)
{
    return i + 1;
}

class PTest1
{
public:
    int Add(int i)
    {
        return i + 2;
    }
};
class PTest2
{
public:
    int Sub(int i)
    {
        return i - 1;
    }
};
int PtrFunction(const ::sofa::pbrpc::shared_ptr<PTest1>& p1, const ::sofa::pbrpc::shared_ptr<PTest2>& p2, int i)
{
    i = p1->Add(i);
    i = p2->Sub(i);
    return i;
}

TEST_F(Performance, CxxFunction)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        NullFunction();
    }
}

TEST_F(Performance, Closure_null)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        ExtClosure<void()>* closure = NewExtClosure(NullFunction);
        closure->Run();
    }
}

TEST_F(Performance, Bind_null)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        boost::function<void()> func = boost::bind(NullFunction);
        func();
    }
}

TEST_F(Performance, Closure_add)
{
    for (int i = 0; i < kLoopCount;)
    {
        ExtClosure<int()>* closure = NewExtClosure(AddFunction, i);
        i = closure->Run();
    }
}

TEST_F(Performance, Bind_add)
{
    for (int i = 0; i < kLoopCount;)
    {
        boost::function<int()> func = boost::bind(AddFunction, i);
        i = func();
    }
}

TEST_F(Performance, Closure_add_with_pointer)
{
    ::sofa::pbrpc::shared_ptr<PTest1> p1(new PTest1());
    ::sofa::pbrpc::shared_ptr<PTest2> p2(new PTest2());
    for (int i = 0; i < kLoopCount;)
    {
        ExtClosure<int(int)>* closure = NewExtClosure(PtrFunction, p1, p2);
        i = closure->Run(i);
    }
}

TEST_F(Performance, Bind_add_with_pointer)
{
    ::sofa::pbrpc::shared_ptr<PTest1> p1(new PTest1());
    ::sofa::pbrpc::shared_ptr<PTest2> p2(new PTest2());
    for (int i = 0; i < kLoopCount;)
    {
        boost::function<int(int)> func = boost::bind(PtrFunction, p1, p2, _1);
        i = func(i);
    }
}

TEST_F(Performance, PermantClosure)
{
    ExtClosure<void()>* closure = NewPermanentExtClosure(&NullFunction);
    for (int i = 0; i < kLoopCount; ++i)
    {
        closure->Run();
    }
}

TEST_F(Performance, StdFunction)
{
    std::tr1::function<void ()> function = NullFunction;
    for (int i = 0; i < kLoopCount; ++i)
    {
        function();
    }
}

TEST_F(Performance, OnceStdFunction)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        std::tr1::function<void ()> function = NullFunction;
        function();
    }
}
#endif

class ClosureInheritTest : public testing::Test
{
public:
    int Foo() { return 42; }
};

TEST_F(ClosureInheritTest, Test)
{
    ExtClosure<int()>* closure = NewExtClosure(this, &ClosureInheritTest::Foo);
    EXPECT_EQ(42, closure->Run());
}

void RefTest(int& a, const int& b)
{
    printf("a = %d, b = %d\n", a, b);
    a = b;
}

TEST(ExtClosure, Reference)
{
    ExtClosure<void(int&, const int&)>* c1 = NewExtClosure(&RefTest);
    int a, b = 2;
    c1->Run(a, b);
    printf("a = %d, b = %d\n", a, b);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
