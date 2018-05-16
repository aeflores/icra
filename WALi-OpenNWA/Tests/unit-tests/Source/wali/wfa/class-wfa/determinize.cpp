#include "gtest/gtest.h"
#include "wali/wfa/WFA.hpp"
#include "wali/wfa/State.hpp"
#include "wali/ShortestPathSemiring.hpp"
#include "wali/wfa/DeterminizeWeightGen.hpp"
#include "fixtures/StringWeight.hpp"

#include "fixtures.hpp"

#define NUM_ELEMENTS(array)  (sizeof(array)/sizeof((array)[0]))

using namespace wali::wfa;
using testing::StringWeight;

static const WFA fas[] = {
    LoopReject().wfa,
    LoopAccept().wfa,
    EvenAsEvenBs().wfa,
    EpsilonTransitionToAccepting().wfa,
    EpsilonFull().wfa,
    EpsilonTransitionToMiddleToAccepting().wfa,
    ADeterministic().wfa,
    EpsilonTransitionToMiddleToEpsilonToAccepting().wfa,
    AcceptAbOrAcNondet().wfa,
    AEpsilonEpsilonEpsilonA().wfa
};

static const unsigned num_fas = NUM_ELEMENTS(fas);

static const WFA fas_already_deterministic[] = {
    LoopReject().wfa,
    LoopAccept().wfa,
    EvenAsEvenBs().wfa,
    EpsilonFull().wfa,
    ADeterministic().wfa,
    AcceptAbOrAcDeterministic().wfa
};

static const unsigned num_fas_already_deterministic = NUM_ELEMENTS(fas_already_deterministic);


static const WFA fas_to_determinize_and_answers[4][3] = {
    // Original                                            Semi-det,                            det
    { EpsilonTransitionToAccepting().wfa,                  EpsilonSemiDeterministic().wfa,      EpsilonSemiDeterministic().wfa },
    { EpsilonTransitionToMiddleToAccepting().wfa,          ASemiDeterministic().wfa,            ADeterministic().wfa },
    { EpsilonTransitionToMiddleToEpsilonToAccepting().wfa, ASemiDeterministic().wfa,            ADeterministic().wfa },
    { AcceptAbOrAcNondet().wfa,                            AcceptAbOrAcSemiDeterministic().wfa, AcceptAbOrAcDeterministic().wfa }
};

static const unsigned num_fas_to_determinize = NUM_ELEMENTS(fas_to_determinize_and_answers);


namespace wali {
    namespace wfa {

        TEST(wali$wfa$$isIsomorphicTo, selfIsomorphismHolds)
        {
            LoopReject f;

            EXPECT_TRUE(f.wfa.isIsomorphicTo(f.wfa));
        }
        
        TEST(wali$wfa$$isIsomorphicTo, selfIsomorphismOfDifferentAutsFail)
        {
            LoopReject rej;
            LoopAccept acc;

            EXPECT_FALSE(acc.wfa.isIsomorphicTo(rej.wfa));
        }

        TEST(wali$wfa$$isIsomorphicTo, isomorphismChecksStateWeights)
        {
            sem_elem_t one = Reach(true).one();
            sem_elem_t zero = Reach(true).zero();
            
            Letters l;
            WFA w0, w1;

            w0.addState(l.a, zero);
            w1.addState(l.a, one);

            EXPECT_FALSE(w0.isIsomorphicTo(w1));
        }
        
        TEST(wali$wfa$$isIsomorphicTo, isomorphismChecksTransWeights)
        {
            sem_elem_t one = Reach(true).one();
            sem_elem_t zero = Reach(true).zero();
            
            Letters l;
            WFA w0, w1;

            w0.addState(l.a, zero);
            w1.addState(l.a, zero);

            w0.addTrans(l.a, l.a, l.a, zero);
            w1.addTrans(l.a, l.a, l.a, one);

            EXPECT_FALSE(w0.isIsomorphicTo(w1));
        }

        TEST(wali$wfa$$isIsomorphicTo, isomorphismIgnoresStateWeightsWhenCheckWeghtsIsFalse)
        {
            sem_elem_t one = Reach(true).one();
            sem_elem_t zero = Reach(true).zero();
            
            Letters l;
            WFA w0, w1;

            w0.addState(l.a, zero);
            w1.addState(l.a, one);

            EXPECT_TRUE(w0.isIsomorphicTo(w1, false));
        }
        
        TEST(wali$wfa$$isIsomorphicTo, isomorphismIgnoresTransWeightsWhenCheckWeghtsIsFalse)
        {
            sem_elem_t one = Reach(true).one();
            sem_elem_t zero = Reach(true).zero();
            
            Letters l;
            WFA w0, w1;

            w0.addState(l.a, zero);
            w1.addState(l.a, zero);

            w0.addTrans(l.a, l.a, l.a, zero);
            w1.addTrans(l.a, l.a, l.a, one);

            EXPECT_TRUE(w0.isIsomorphicTo(w1, false));
        }


        TEST(wali$wfa$$isIsomorphicTo, battery)
        {
            for (size_t i=0; i<num_fas; ++i) {
                for (size_t j=0; j<num_fas; ++j) {
                    std::stringstream ss;
                    ss << "Testing FAs " << i << " ~ " << j;
                    SCOPED_TRACE(ss.str());
                    
                    WFA left = fas[i];
                    WFA right = fas[j];

                    EXPECT_EQ(i == j, left.isIsomorphicTo(right));
                    EXPECT_EQ(i == j, left.isIsomorphicTo(right, true));
                    EXPECT_EQ(i == j, left.isIsomorphicTo(right, false));
                }
            }
        }

        TEST(wali$wfa$$isIsomorphicTo, loopRejectAndSimilar)
        {
            LoopReject f;
            WFA wfa;

            sem_elem_t one = Reach(true).one();
            sem_elem_t zero = Reach(true).zero();
                
            Letters l;
            Key state = getKey("different state");
            ASSERT_NE(state, f.state);

            wfa.addState(state, zero);
            wfa.setInitialState(state);
            wfa.addTrans(state, l.a, state, one);
            wfa.addTrans(state, l.b, state, one);
            wfa.addTrans(state, l.c, state, one);
            
            EXPECT_TRUE(f.wfa.isIsomorphicTo(wfa));
        }
        


        TEST(wali$wfa$$determinize, alreadyDeterministicBattery) {
            for (size_t i=0; i<num_fas_already_deterministic; ++i) {
                std::stringstream ss;
                ss << "Testing FA " << i;
                SCOPED_TRACE(ss.str());

                WFA orig = fas_already_deterministic[i];
                WFA det = orig.determinize();

                EXPECT_TRUE(orig.isIsomorphicTo(det, false));
            }
        }


        TEST(wali$wfa$$semideterminize, EpsilonTransitionToAccepting) {
            EpsilonTransitionToAccepting f;
            WFA wfa = f.wfa.semideterminize();

            EpsilonSemiDeterministic expected;

            EXPECT_TRUE(wfa.isIsomorphicTo(expected.wfa));
        }


        TEST(wali$wfa$$complete, DISABLED_epsilonSemi)
        {
            EpsilonSemiDeterministic orig;
            EpsilonFull expected;

            Letters l;
            std::set<Key> symbols;
            symbols.insert(l.a);
            symbols.insert(l.b);
            symbols.insert(l.c);

            orig.wfa.complete(symbols);

            EXPECT_TRUE(expected.wfa.isIsomorphicTo(orig.wfa));
        }


        TEST(wali$wfa$$semideterminize, battery)
        {
            for (size_t i=0; i<num_fas_to_determinize; ++i) {
                std::stringstream ss;
                ss << "Testing FA " << i;
                SCOPED_TRACE(ss.str());
                
                WFA input    = fas_to_determinize_and_answers[i][0];
                WFA expected = fas_to_determinize_and_answers[i][1];

                WFA det = input.semideterminize();

                EXPECT_TRUE(expected.isIsomorphicTo(det));

                if (!expected.isIsomorphicTo(det)) {
                    std::cerr << "\n\n=============================================================\n";
                    input.print(std::cerr << "Input:\n") << "\n";
                    expected.print(std::cerr << "Expected:\n") << "\n";
                    det.print(std::cerr << "Det:\n") << "\n";
                }
            }
        }
        
        TEST(wali$wfa$$determinize, battery)
        {
            for (size_t i=0; i<num_fas_to_determinize; ++i) {
                std::stringstream ss;
                ss << "Testing FA " << i;
                SCOPED_TRACE(ss.str());
                
                WFA input    = fas_to_determinize_and_answers[i][0];
                WFA expected = fas_to_determinize_and_answers[i][2];

                WFA det = input.determinize();

                EXPECT_TRUE(expected.isIsomorphicTo(det));
            }
        }




        struct TestLifter
            : public LiftCombineWeightGen
        {
            sem_elem_t liftWeight(WFA const & UNUSED_PARAMETER(original_wfa),
                                  Key source,
                                  Key symbol,
                                  Key target,
                                  sem_elem_t UNUSED_PARAMETER(weight)) const
            {
                std::stringstream ss;
                ss << key2str(source)
                   << " --" << key2str(symbol) << "--> "
                   << key2str(target);
                return new StringWeight(ss.str());
            }

            sem_elem_t liftAcceptWeight(WFA const & original_wfa,
                                        Key state,
                                        sem_elem_t UNUSED_PARAMETER(original_accept_weight)) const
            {
                std::stringstream ss;
                if (original_wfa.isFinalState(state)) {
                    ss << key2str(state);
                }
                return new StringWeight(ss.str());
            }

            sem_elem_t getOne(WFA const & UNUSED_PARAMETER(original_wfa)) const {
                return new StringWeight("");
            }
        };

#define ASSERT_CONTAINS(container, value) ASSERT_FALSE(container.end() == container.find(value))

        TEST(wali$wfa$$WFA$$semideterminize, weightGenAcceptingStates)
        {
            sem_elem_t reach_one = Reach(true).one();
            sem_elem_t reach_zero = Reach(true).zero();

            Letters letters;
            Key start = getKey("start"),
                acc1 = getKey("acc1"),
                acc2 = getKey("acc2"),
                rej = getKey("rej");

            //  --> start -----> ((acc1))
            //       |  |
            //       |  +------> ((acc2))
            //       |
            //       +---------> rej
            
            WFA nondet;

            nondet.addState(start, reach_zero);
            nondet.addState(acc1, reach_zero);
            nondet.addState(acc2, reach_zero);
            nondet.addState(rej, reach_zero);

            nondet.setInitialState(start);
            nondet.addFinalState(acc1);
            nondet.addFinalState(acc2);

            nondet.addTrans(start, letters.a, acc1, reach_one);
            nondet.addTrans(start, letters.a, acc2, reach_one);
            nondet.addTrans(start, letters.a, rej, reach_one);

            

            WFA det = nondet.semideterminize(TestLifter());

            // Sanity checks
            std::set<Key> start_set, fin_set;
            start_set.insert(start);
            fin_set.insert(acc1);
            fin_set.insert(acc2);
            fin_set.insert(rej);

            Key start_set_key = getKey(start_set),
                fin_set_key = getKey(fin_set);

            ASSERT_EQ(2u, det.getStates().size());
            ASSERT_CONTAINS(det.getStates(), start_set_key);
            ASSERT_CONTAINS(det.getStates(), fin_set_key);

            // Extract the transition, then the weight
            StringWeight * w = dynamic_cast<StringWeight*>(det.getState(fin_set_key)->acceptWeight().get_ptr());
            ASSERT_TRUE(w != NULL);

            // Test. This is maybe a fragile (though not as much as before)...
            EXPECT_EQ(" | acc1 | acc2", w->str);
        }

        TEST(wali$wfa$$WFA$$semideterminize, weightGenKnowsAboutEpsilonTransitions)
        {
            Letters letters;
            AEpsilonEpsilonEpsilonA nondet;
            // Add an extra transition for a better test: the machine can
            // take a transition without following all epsilons first.
            nondet.wfa.addTrans(nondet.ae, nondet.a, nondet.accept, nondet.one);
            nondet.wfa.addTrans(nondet.ae, letters.b, nondet.accept, nondet.one);
            WFA det = nondet.wfa.semideterminize(TestLifter());

            std::set<Key> start_set, mid_set, accept_set;
            start_set.insert(nondet.start);
            mid_set.insert(nondet.a);
            mid_set.insert(nondet.ae);
            mid_set.insert(nondet.aee);
            mid_set.insert(nondet.aeee);
            accept_set.insert(nondet.accept);

            Key start = getKey(start_set);
            Key mid = getKey(mid_set);
            Key accept = getKey(accept_set);

            // Sanity checks
            ASSERT_EQ(3u, det.getStates().size());
            ASSERT_CONTAINS(det.getStates(), start);
            ASSERT_CONTAINS(det.getStates(), mid);
            ASSERT_CONTAINS(det.getStates(), accept);

            // Extract the transitions
            Trans trans_start_mid, trans_mid_accept, trans_mid_accept_b;
            ASSERT_TRUE(det.find(start, nondet.a, mid, trans_start_mid));
            ASSERT_TRUE(det.find(mid, nondet.a, accept, trans_mid_accept));
            ASSERT_TRUE(det.find(mid, letters.b, accept, trans_mid_accept_b));

            // Extract the weights
            sem_elem_t se_start_mid = trans_start_mid.weight();
            sem_elem_t se_mid_accept = trans_mid_accept.weight();
            sem_elem_t se_mid_accept_b = trans_mid_accept_b.weight();

            StringWeight * w_start_mid = dynamic_cast<StringWeight*>(se_start_mid.get_ptr());
            StringWeight * w_mid_accept = dynamic_cast<StringWeight*>(se_mid_accept.get_ptr());
            StringWeight * w_mid_accept_b = dynamic_cast<StringWeight*>(se_mid_accept_b.get_ptr());

            ASSERT_TRUE(w_start_mid != NULL);
            ASSERT_TRUE(w_mid_accept != NULL);
            ASSERT_TRUE(w_mid_accept_b != NULL);

            // Finally, check that they are what we expect. These tests are a
            // bit fragile...
            EXPECT_EQ("start --a--> a | start --a--> ae | start --a--> aee | start --a--> aeee",
                      w_start_mid->str);
            EXPECT_EQ("ae --a--> accept | aeee --a--> accept", w_mid_accept->str);
            EXPECT_EQ("ae --b--> accept", w_mid_accept_b->str);
        }
        
        TEST(wali$wfa$$determinize, weightGen)
        {
            Letters l;
            AcceptAbOrAcNondet nondet;
            WFA det = nondet.wfa.semideterminize(TestLifter());

            std::set<Key> start_set, a_set, ab_set, ac_set;
            start_set.insert(nondet.start);
            a_set.insert(nondet.a_left);
            a_set.insert(nondet.a_top);
            ab_set.insert(nondet.ab);
            ac_set.insert(nondet.ac);

            Key start = getKey(start_set);
            Key a = getKey(a_set);
            Key ab = getKey(ab_set);
            Key ac = getKey(ac_set);

            // Just some sanity checks
            ASSERT_EQ(4u, det.getStates().size());
            ASSERT_CONTAINS(det.getStates(), start);
            ASSERT_CONTAINS(det.getStates(), a);
            ASSERT_CONTAINS(det.getStates(), ab);
            ASSERT_CONTAINS(det.getStates(), ac);

            // Now extract the transitions
            Trans trans_start_a, trans_a_ab, trans_a_ac;
            ASSERT_TRUE(det.find(start, l.a, a, trans_start_a));
            ASSERT_TRUE(det.find(a, l.b, ab, trans_a_ab));
            ASSERT_TRUE(det.find(a, l.c, ac, trans_a_ac));

            // Now extract the weights
            sem_elem_t se_start_a = trans_start_a.weight();
            sem_elem_t se_a_ab = trans_a_ab.weight();
            sem_elem_t se_a_ac = trans_a_ac.weight();

            StringWeight * weight_start_a = dynamic_cast<StringWeight*>(se_start_a.get_ptr());
            StringWeight * weight_a_ab = dynamic_cast<StringWeight*>(se_a_ab.get_ptr());
            StringWeight * weight_a_ac = dynamic_cast<StringWeight*>(se_a_ac.get_ptr());

            ASSERT_TRUE(weight_start_a != NULL);
            ASSERT_TRUE(weight_a_ab != NULL);
            ASSERT_TRUE(weight_a_ac != NULL);

            // Finally, check that they are what we expect.
            EXPECT_EQ("start --a--> a (left) | start --a--> a (top)", weight_start_a->str);
            EXPECT_EQ("a (top) --b--> ab", weight_a_ab->str);
            EXPECT_EQ("a (left) --c--> ac", weight_a_ac->str);
        }


        // FIXME: this isn't a test!
        TEST(wali$wfa$$semideterminize, zeroWeightPathDoesNotAccept)
        {
            //      zero
            //  -> A ---> (B)
            Key A = getKey("A");
            Key B = getKey("B");

            sem_elem_t zero = Reach(true).zero();

            WFA wfa;
            wfa.addState(A, zero);
            wfa.addState(B, zero);
            wfa.addTrans(A, A, B, zero);

            wfa.setInitialState(A);
            wfa.addFinalState(B);

            WFA det = wfa.semideterminize();

            wfa.print(std::cerr);
            det.print(std::cerr);

            WFA::Word w;
            w.push_back(A);

            std::cerr << "wfa accepts: " << wfa.isAcceptedWithNonzeroWeight(w) << "\n";
            std::cerr << "det accepts: " << det.isAcceptedWithNonzeroWeight(w) << "\n";
        }
        

        TEST(wali$wfa$$semideterminize, initialStateGetsLabeledWithTheCorrectKindOfWeight)
        {
            //      zero
            //  -> A ---> (B)
            Key A = getKey("A");
            Key B = getKey("B");

            sem_elem_t zero = Reach(true).zero();

            WFA wfa;
            wfa.addState(A, zero);
            wfa.addState(B, zero);
            wfa.addTrans(A, A, B, zero);

            wfa.setInitialState(A);
            wfa.addFinalState(B);

            WFA det = wfa.semideterminize(TestLifter());
            
            sem_elem_t initial_weight = det.getState(det.getInitialState())->weight();

            Reach * reach = dynamic_cast<Reach*>(initial_weight.get_ptr());
            StringWeight * str = dynamic_cast<StringWeight*>(initial_weight.get_ptr());

            EXPECT_EQ(NULL, reach);
            EXPECT_TRUE(str != NULL);
        }
        
        
    }
}
