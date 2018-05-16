#include "gtest/gtest.h"

#include <string>
#include <vector>

#include "wali/domains/KeyedSemElemSet.hpp"
#include "wali/SemElemPair.hpp"
#include "wali/ShortestPathSemiring.hpp"

#include "fixtures.hpp"

using std::string;
using namespace wali;
using namespace wali::domains;


namespace {
    PositionKey<int>* downi(sem_elem_t p) {
        return dynamic_cast<PositionKey<int>*>(p.get_ptr());
    }
    PositionKey<string>* downs(sem_elem_t p) {
        return dynamic_cast<PositionKey<string>*>(p.get_ptr());
    }
}


TEST(wali$domains$PositionKey$$extend, zeroExtendBlahGivesZero)
{
    PKFixtures f;

    sem_elem_t
        ai = f.i_empty->extend(f.i_empty),
        bi = f.i10->extend(f.i_empty),
        ci = f.i11->extend(f.i_empty),
        di = f.i_empty->extend(f.i10),
        ei = f.i_empty->extend(f.i_id),
        fi = f.i_id->extend(f.i_empty),
        as = f.s_empty->extend(f.s_empty),
        bs = f.s10->extend(f.s_empty),
        cs = f.s11->extend(f.s_empty),
        ds = f.s_empty->extend(f.s10),
        es = f.s_empty->extend(f.s_id),
        fs = f.s_id->extend(f.s_empty);

    EXPECT_TRUE(downi(ai)->isZero());
    EXPECT_TRUE(downi(bi)->isZero());
    EXPECT_TRUE(downi(ci)->isZero());
    EXPECT_TRUE(downi(di)->isZero());
    EXPECT_TRUE(downi(ei)->isZero());
    EXPECT_TRUE(downi(fi)->isZero());
    
    EXPECT_TRUE(downs(as)->isZero());
    EXPECT_TRUE(downs(bs)->isZero());
    EXPECT_TRUE(downs(cs)->isZero());
    EXPECT_TRUE(downs(ds)->isZero());
    EXPECT_TRUE(downs(es)->isZero());
    EXPECT_TRUE(downs(fs)->isZero());
}

TEST(wali$domains$PositionKey$$extend, oneExtendBlahGivesBlah)
{
    PKFixtures f;

    sem_elem_t
        ai = f.i_id->extend(f.i_id),
        bi = f.i10->extend(f.i_id),
        ci = f.i11->extend(f.i_id),
        di = f.i_id->extend(f.i10),
        as = f.s_id->extend(f.s_id),
        bs = f.s10->extend(f.s_id),
        cs = f.s11->extend(f.s_id),
        ds = f.s_id->extend(f.s10);

    EXPECT_TRUE(ai->equal(f.i_id));
    EXPECT_TRUE(bi->equal(f.i10));
    EXPECT_TRUE(ci->equal(f.i11));
    EXPECT_TRUE(di->equal(f.i10));
    
    EXPECT_TRUE(as->equal(f.s_id));
    EXPECT_TRUE(bs->equal(f.s10));
    EXPECT_TRUE(cs->equal(f.s11));
    EXPECT_TRUE(ds->equal(f.s10));
}

TEST(wali$domains$PositionKey$$extend, nonmatchesGiveZero)
{
    PKFixtures f;

    sem_elem_t
        ai = f.i_empty->extend(f.i11),
        bi = f.i10->extend(f.i11),
        ci = f.i11->extend(f.i00),
        as = f.s_empty->extend(f.s11),
        bs = f.s10->extend(f.s11),
        cs = f.s11->extend(f.s00);

    EXPECT_TRUE(downi(ai)->isZero());
    EXPECT_TRUE(downi(bi)->isZero());
    EXPECT_TRUE(downi(ci)->isZero());
    EXPECT_TRUE(downs(as)->isZero());
    EXPECT_TRUE(downs(bs)->isZero());
    EXPECT_TRUE(downs(cs)->isZero());
}

TEST(wali$domains$PositionKey$$extend, matchesGiveComposition)
{
    PKFixtures f;

    sem_elem_t
        ai = f.i00->extend(f.i00),
        bi = f.i00->extend(f.i01),
        ci = f.i10->extend(f.i01),
        di = f.i10->extend(f.i00),

        as = f.s00->extend(f.s00),
        bs = f.s00->extend(f.s01),
        cs = f.s10->extend(f.s01),
        ds = f.s10->extend(f.s00);

    EXPECT_TRUE(ai->equal(f.i00));
    EXPECT_TRUE(bi->equal(f.i01));
    EXPECT_TRUE(ci->equal(f.i11));
    EXPECT_TRUE(di->equal(f.i10));

    EXPECT_TRUE(as->equal(f.s00));
    EXPECT_TRUE(bs->equal(f.s01));
    EXPECT_TRUE(cs->equal(f.s11));
    EXPECT_TRUE(ds->equal(f.s10));
}



TEST(wali$domains$PositionKey$$PositionKey, constructors)
{
    PKFixtures f;
}


TEST(wali$domains$PositionKey$$equal, eachFixtureUnequal)
{
    PKLists lists;

    for (size_t i=0; i<lists.ints.size(); ++i) {
        for (size_t j=0; j<lists.ints.size(); ++j) {
            sem_elem_t
                left = lists.ints.at(i),
                right = lists.ints.at(j);
            EXPECT_EQ((i==j), left->equal(right));
        }
    }

    for (size_t i=0; i<lists.strings.size(); ++i) {
        for (size_t j=0; j<lists.strings.size(); ++j) {
            sem_elem_t
                left = lists.strings.at(i),
                right = lists.strings.at(j);
            EXPECT_EQ((i==j), left->equal(right));
        }
    }
}

TEST(wali$domains$PositionKey$$hash, eachFixtureHasDifferentHash)
{
    PKLists lists;

    for (size_t i=0; i<lists.ints.size(); ++i) {
        for (size_t j=0; j<lists.ints.size(); ++j) {
            sem_elem_t
                left = lists.ints.at(i),
                right = lists.ints.at(j);
            size_t
                left_hash = left->hash(),
                right_hash = right->hash();
            if (i == j) {
                EXPECT_EQ(left_hash, right_hash);
            }
            else {
                EXPECT_NE(left_hash, right_hash);
            }
        }
    }

    for (size_t i=0; i<lists.strings.size(); ++i) {
        for (size_t j=0; j<lists.strings.size(); ++j) {
            sem_elem_t
                left = lists.strings.at(i),
                right = lists.strings.at(j);
            size_t
                left_hash = left->hash(),
                right_hash = right->hash();
            if (i == j) {
                EXPECT_EQ(left_hash, right_hash);
            }
            else {
                EXPECT_NE(left_hash, right_hash);
            }
        }
    }
}



TEST(wali$domains$PositionKey$$containerLessThan, intsAreInCorrectOrder)
{
    PKLists lists;

    for (size_t i=0; i<lists.ints.size(); ++i) {
        for (size_t j=0; j<lists.ints.size(); ++j) {
            sem_elem_t
                left = lists.ints.at(i),
                right = lists.ints.at(j);
            EXPECT_EQ((i<j), left->containerLessThan(right));
        }
    }
}


TEST(wali$domains$PositionKey$$hash, differentElementsProduceDifferentHashes)
{
    PKLists lists;

    for (size_t i=0; i<lists.ints.size(); ++i) {
        for (size_t j=0; j<lists.ints.size(); ++j) {
            sem_elem_t
                left = lists.ints.at(i),
                right = lists.ints.at(j);
            if (i == j) {
                EXPECT_EQ(left->hash(), right->hash());
            }
            else {
                EXPECT_NE(left->hash(), right->hash());
            }
        }
    }

    for (size_t i=0; i<lists.strings.size(); ++i) {
        for (size_t j=0; j<lists.strings.size(); ++j) {
            sem_elem_t
                left = lists.strings.at(i),
                right = lists.strings.at(j);
            if (i == j) {
                EXPECT_EQ(left->hash(), right->hash());
            }
            else {
                EXPECT_NE(left->hash(), right->hash());
            }
        }
    }
}


TEST(wali$domains$PositionKey$$isZero, isZeroForFixture)
{
    PKFixtures f;
    EXPECT_TRUE(f.i_empty->isZero());
    EXPECT_TRUE(f.s_empty->isZero());

    EXPECT_FALSE(f.i_id->isZero());
    EXPECT_FALSE(f.i00->isZero());
    EXPECT_FALSE(f.i01->isZero());
    EXPECT_FALSE(f.i10->isZero());
    EXPECT_FALSE(f.i11->isZero());

    EXPECT_FALSE(f.s_id->isZero());
    EXPECT_FALSE(f.s00->isZero());
    EXPECT_FALSE(f.s01->isZero());
    EXPECT_FALSE(f.s10->isZero());
    EXPECT_FALSE(f.s11->isZero());
}


TEST(wali$domains$PositionKey$$isOne, isOneForFixture)
{
    PKFixtures f;

    EXPECT_TRUE(f.s_id->isOne());
    EXPECT_TRUE(f.s_id->isOne());    
    
    EXPECT_FALSE(f.i_empty->isOne());
    EXPECT_FALSE(f.i00->isOne());
    EXPECT_FALSE(f.i01->isOne());
    EXPECT_FALSE(f.i10->isOne());
    EXPECT_FALSE(f.i11->isOne());
    
    EXPECT_FALSE(f.s_empty->isOne());
    EXPECT_FALSE(f.s00->isOne());
    EXPECT_FALSE(f.s01->isOne());
    EXPECT_FALSE(f.s10->isOne());
    EXPECT_FALSE(f.s11->isOne());
}


TEST(wali$domains$PositionKey$$combine, combineAborts)
{
    PKFixtures f;
    EXPECT_DEATH({
            f.i_empty->combine(f.i_empty);
        },
        "");
}

TEST(wali$domains$PositionKey$$pre$and$post, testAccessors)
{
    PKFixtures f;
    EXPECT_EQ(0, f.i00->pre());
    EXPECT_EQ(0, f.i00->post());
    EXPECT_EQ(1, f.i10->pre());
    EXPECT_EQ("one", f.s11->post());

    EXPECT_EQ(1, f.i10copy->pre());
    EXPECT_EQ(0, f.i10copy->post());
}
