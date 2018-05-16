#ifndef WALI_TESTS_WALI_NWA_WFA_FIXTURES_HPP
#define WALI_TESTS_WALI_NWA_WFA_FIXTURES_HPP

#include "wali/Key.hpp"
#include "wali/wfa/WFA.hpp"
#include "wali/Reach.hpp"
#include "fixtures/Keys.hpp"
#include "fixtures/StringWeight.hpp"

namespace wali {
    namespace wfa {


        struct Letters
        {
            Key a, b, c;

            Letters()
                : a(getKey("a"))
                , b(getKey("b"))
                , c(getKey("c"))
            {}
        };

        struct Words
        {
            typedef WFA::Word Word;

            Word epsilon, a, b, c, aa, ab, ac;

            Words() {
                Letters l;

                a.push_back(l.a);

                b.push_back(l.b);

                c.push_back(l.c);

                aa.push_back(l.a);
                aa.push_back(l.a);

                ab.push_back(l.a);
                ab.push_back(l.b);

                ac.push_back(l.a);
                ac.push_back(l.c);
            }
        };

        struct LoopReject
        {
            // -->o--\               [anti-line-continuation]
            //    ^  | a, b, c
            //    |  |
            //    \--/
            WFA wfa;
            Key state;

            LoopReject() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Letters l;
                state = getKey("state");

                wfa.addState(state, zero);
                wfa.setInitialState(state);
                wfa.addTrans(state, l.a, state, one);
                wfa.addTrans(state, l.b, state, one);
                wfa.addTrans(state, l.c, state, one);
            }
        };

        struct LoopAccept
        {
            // -->(o)--\               [anti-line-continuation]
            //     ^   | a, b, c
            //     |   |
            //     \---/
            WFA wfa;
            Key state;

            LoopAccept()
                : wfa( LoopReject().wfa )
                , state( LoopReject().state )
            {
                wfa.addFinalState(state);
            }
        };

        struct EvenAsEvenBs
        {
            //            b
            // -->(ee) <-----> eo
            //     /\          /\                [anti-line-continuation]
            //   a |           | a
            //     \/          \/
            //     oe  <-----> oo
            //            b
            WFA wfa;

            EvenAsEvenBs() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key even_even = getKey("even_even");
                Key even_odd  = getKey("even_odd");
                Key odd_even  = getKey("odd_even");
                Key odd_odd   = getKey("odd_odd");
                Letters l;

                wfa.addState(even_even, zero);
                wfa.addState(even_odd, zero);
                wfa.addState(odd_even, zero);
                wfa.addState(odd_odd, zero);

                wfa.setInitialState(even_even);
                wfa.addFinalState(even_even);

                wfa.addTrans(even_even, l.a, odd_even,  one);
                wfa.addTrans(even_odd,  l.a, odd_odd,   one);
                wfa.addTrans(odd_even,  l.a, even_even, one);
                wfa.addTrans(odd_odd,   l.a, even_odd,  one);

                wfa.addTrans(even_even, l.b, even_odd,  one);
                wfa.addTrans(even_odd,  l.b, even_even, one);
                wfa.addTrans(odd_even,  l.b, odd_odd,   one);
                wfa.addTrans(odd_odd,   l.b, odd_even,  one);
            }
        };
        

        struct EpsilonTransitionToAccepting
        {
            //     eps
            // -->o-------->(o)
            WFA wfa;

            EpsilonTransitionToAccepting()
            {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key start = getKey("start");
                Key accept = getKey("accept");

                wfa.addState(start, zero);
                wfa.addState(accept, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, WALI_EPSILON, accept, one);
            }
        };

        struct EpsilonFull
        {
            //
            //         a,b,c
            // --> (o) ------> o --\             [anti-line-continuation]
            //                 ^   | a, b, c
            //                 |   |
            //                 \---/
            WFA wfa;

            EpsilonFull() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();

                Letters l;

                Key start = getKey("start");
                Key reject = getKey("reject");

                wfa.addState(start, zero);
                wfa.addState(reject, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(start);

                wfa.addTrans(start, l.a, reject, one);
                wfa.addTrans(start, l.b, reject, one);
                wfa.addTrans(start, l.c, reject, one);

                wfa.addTrans(reject, l.a, reject, one);
                wfa.addTrans(reject, l.b, reject, one);
                wfa.addTrans(reject, l.c, reject, one);
            }
        };
        
        struct EpsilonSemiDeterministic
        {
            // --> (o)
            WFA wfa;

            EpsilonSemiDeterministic() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();

                Letters l;

                Key start = getKey("start");

                wfa.addState(start, zero);
                wfa.setInitialState(start);
                wfa.addFinalState(start);
            }
        };
        

        struct EpsilonTransitionToMiddleToAccepting
        {
            //      eps     a
            // -->o------->o------->(o)
            WFA wfa;

            EpsilonTransitionToMiddleToAccepting() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key start = getKey("start");
                Key middle = getKey("middle");
                Key accept = getKey("accept");

                wfa.addState(start, zero);
                wfa.addState(middle, zero);
                wfa.addState(accept, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, WALI_EPSILON, middle, one);
                wfa.addTrans(middle, Letters().a, accept, one);
            }
        };

        struct ASemiDeterministic
        {
            //        a
            // --> o ----> (o)
            WFA wfa;

            ASemiDeterministic() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();

                Letters l;

                Key start = getKey("start");
                Key accept = getKey("accept");

                wfa.addState(start, zero);
                wfa.addState(accept, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, l.a, accept, one);
            }
        };

        struct ADeterministic
        {
            //
            //        a        a
            // --> o ----> (o) ----> o --\              [anti-line-continuation]
            //                       ^   | a
            //                       |   |
            //                       \---/
            WFA wfa;

            ADeterministic() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();

                Letters l;

                Key start = getKey("start");
                Key accept = getKey("accept");
                Key reject = getKey("reject");

                wfa.addState(start, zero);
                wfa.addState(accept, zero);
                wfa.addState(reject, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start,  l.a, accept, one);
                wfa.addTrans(accept, l.a, reject, one);
                wfa.addTrans(reject, l.a, reject, one);
            }
        };


        struct AFull
        {
            //
            //        a        a,b,c
            // --> o ----> (o) ----> o --\              [anti-line-continuation]
            //     |                 ^   | a, b, c
            //     | b,c             |   |
            //     \----------------/\---/
            WFA wfa;

            AFull() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();

                Letters l;

                Key start = getKey("start");
                Key accept = getKey("accept");
                Key reject = getKey("reject");

                wfa.addState(start, zero);
                wfa.addState(accept, zero);
                wfa.addState(reject, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, l.a, accept, one);
                wfa.addTrans(start, l.b, reject, one);
                wfa.addTrans(start, l.c, reject, one);

                wfa.addTrans(accept, l.a, reject, one);
                wfa.addTrans(accept, l.b, reject, one);
                wfa.addTrans(accept, l.c, reject, one);

                wfa.addTrans(reject, l.a, reject, one);
                wfa.addTrans(reject, l.b, reject, one);
                wfa.addTrans(reject, l.c, reject, one);
            }
        };


        struct EpsilonTransitionToMiddleToEpsilonToAccepting
        {
            //      eps     a     eps
            // -->o------->o---->o----->(o)
            WFA wfa;

            EpsilonTransitionToMiddleToEpsilonToAccepting() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key start = getKey("start");
                Key middle = getKey("middle");
                Key almost = getKey("almost");
                Key accept = getKey("accept");

                wfa.addState(start, zero);
                wfa.addState(middle, zero);
                wfa.addState(almost, zero);
                wfa.addState(accept, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, WALI_EPSILON, middle, one);
                wfa.addTrans(middle, Letters().a, almost, one);
                wfa.addTrans(almost, WALI_EPSILON, accept, one);
            }
        };

        struct AcceptAbOrAcNondet
        {
            // -->o--------->o-------->(o)
            //    |      a       b
            //    | a
            //   \/     c
            //    o--------->(o)
            WFA wfa;

            Key start, a_top, a_left, ab, ac;

            AcceptAbOrAcNondet() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                start = getKey("start");
                a_top = getKey("a (top)");
                a_left = getKey("a (left)");
                ab = getKey("ab");
                ac = getKey("ac");

                Letters l;

                wfa.addState(start, zero);
                wfa.addState(a_top, zero);
                wfa.addState(a_left, zero);
                wfa.addState(ab, zero);
                wfa.addState(ac, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(ab);
                wfa.addFinalState(ac);

                wfa.addTrans(start, l.a, a_top, one);
                wfa.addTrans(start, l.a, a_left, one);
                wfa.addTrans(a_top, l.b, ab, one);
                wfa.addTrans(a_left, l.c, ac, one);
            }
        };

        struct AcceptAbOrAcSemiDeterministic
        {
            //         a         b
            // -->o--------->o-------->(o)
            //               |
            //               |c
            //               V
            //              (o)
            WFA wfa;

            AcceptAbOrAcSemiDeterministic() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key start = getKey("start");
                Key a = getKey("a (state)");
                Key ab = getKey("ab");
                Key ac = getKey("ac");

                Letters l;

                wfa.addState(start, zero);
                wfa.addState(a, zero);
                wfa.addState(ab, zero);
                wfa.addState(ac, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(ab);
                wfa.addFinalState(ac);

                wfa.addTrans(start, l.a, a, one);

                wfa.addTrans(a, l.b, ab, one);
                wfa.addTrans(a, l.c, ac, one);

            }
        };

        struct AcceptAbOrAcDeterministic
        {
            //         a         b
            // -->o--------->o-------->(o)
            //    |         /|          |
            //    |       a/ |c         | a,b,c
            //    |b,c    /  V          |
            //    |      /  (o)         |
            //    |      |   | a,b,c    |
            //    V      |   |          |
            //    o <----<---<----------/
            //    |      |
            //    \------/
            //      a,b,c
            WFA wfa;

            AcceptAbOrAcDeterministic() {
                sem_elem_t one = Reach(true).one();
                sem_elem_t zero = Reach(true).zero();
                
                Key start = getKey("start");
                Key a = getKey("a (state)");
                Key ab = getKey("ab");
                Key ac = getKey("ac");
                Key reject = getKey("reject");

                Letters l;

                wfa.addState(start, zero);
                wfa.addState(a, zero);
                wfa.addState(ab, zero);
                wfa.addState(ac, zero);
                wfa.addState(reject, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(ab);
                wfa.addFinalState(ac);

                wfa.addTrans(start, l.a, a, one);
                wfa.addTrans(start, l.b, reject, one);
                wfa.addTrans(start, l.c, reject, one);

                wfa.addTrans(a, l.a, reject, one);
                wfa.addTrans(a, l.b, ab, one);
                wfa.addTrans(a, l.c, ac, one);

                wfa.addTrans(ab, l.a, reject, one);
                wfa.addTrans(ab, l.b, reject, one);
                wfa.addTrans(ab, l.c, reject, one);
                
                wfa.addTrans(ac, l.a, reject, one);
                wfa.addTrans(ac, l.b, reject, one);
                wfa.addTrans(ac, l.c, reject, one);
                
                wfa.addTrans(reject, l.a, reject, one);
                wfa.addTrans(reject, l.b, reject, one);
                wfa.addTrans(reject, l.c, reject, one);
            }
        };


        struct AEpsilonEpsilonEpsilonA
        {
            //     a     eps    eps    eps   a
            // -->o---->o----->o----->o---->o----->(o)
            //  start   a     ae     aee   aeee   accept
            WFA wfa;

            Key start;
            Key a;
            Key ae;
            Key aee;
            Key aeee;
            Key accept;
            
            sem_elem_t one;
            sem_elem_t zero;

            AEpsilonEpsilonEpsilonA() {
                one = Reach(true).one();
                zero = Reach(true).zero();
               
                start = getKey("start");
                a = getKey("a");
                ae = getKey("ae");
                aee = getKey("aee");
                aeee = getKey("aeee");
                accept = getKey("accept");

                Letters l;

                wfa.addState(start, zero);
                wfa.addState(a, zero);
                wfa.addState(ae, zero);
                wfa.addState(aee, zero);
                wfa.addState(aeee, zero);
                wfa.addState(accept, zero);

                wfa.setInitialState(start);
                wfa.addFinalState(accept);

                wfa.addTrans(start, l.a, a, one);
                wfa.addTrans(a, WALI_EPSILON, ae, one);
                wfa.addTrans(ae, WALI_EPSILON, aee, one);
                wfa.addTrans(aee, WALI_EPSILON, aeee, one);
                wfa.addTrans(aeee, l.a, accept, one);
            }
        };

      //   eps    eps    eps     
      // 1 ---> 2 ---> 3 ---> 4
      struct ChainedEpsilonTransitions
      {
	WFA wfa;
	testing::Keys keys;

	ChainedEpsilonTransitions(sem_elem_t first,
				  sem_elem_t second,
				  sem_elem_t third)
	{
	  sem_elem_t zero = first->zero();
        
	  wfa.addState(keys.st1, zero);
	  wfa.addState(keys.st2, zero);
	  wfa.addState(keys.st3, zero);
	  wfa.addState(keys.st4, zero);
	  wfa.setInitialState(keys.st2);

	  wfa.addTrans(keys.st1, WALI_EPSILON, keys.st2, first);
	  wfa.addTrans(keys.st2, WALI_EPSILON, keys.st3, second);
	  wfa.addTrans(keys.st3, WALI_EPSILON, keys.st4, third);
	}
      };


      //   eps     a
      // 1 ---> 2 ---> 3
      struct EpsTransThenNormal
      {
	WFA wfa;
	testing::Keys keys;

	EpsTransThenNormal(sem_elem_t first,
			   sem_elem_t second)
	{
	  sem_elem_t zero = first->zero();

	  wfa.addState(keys.st1, zero);
	  wfa.addState(keys.st2, zero);
	  wfa.addState(keys.st3, zero);
	  wfa.setInitialState(keys.st2);

	  wfa.addTrans(keys.st1, WALI_EPSILON, keys.st2, first);
	  wfa.addTrans(keys.st2, keys.a, keys.st3, second);
	}
      };

      //            eps
      //            ---> 3
      //    eps    /
      //  1 ---> 2
      //           \                          [anti-line-continuation]
      //            ---> 4
      //            eps
      struct Branching
      {
	WFA wfa;
	testing::Keys keys;

	Branching(sem_elem_t first,
		  sem_elem_t second,
		  sem_elem_t third)
	{
	  sem_elem_t zero = first->zero();
        
	  wfa.addState(keys.st1, zero);
	  wfa.addState(keys.st2, zero);
	  wfa.addState(keys.st3, zero);
	  wfa.addState(keys.st4, zero);
	  wfa.setInitialState(keys.st2);

	  wfa.addTrans(keys.st1, WALI_EPSILON, keys.st2, first);
	  wfa.addTrans(keys.st2, WALI_EPSILON, keys.st3, second);
	  wfa.addTrans(keys.st2, WALI_EPSILON, keys.st4, third);
	}
      };


      // --> 1
      struct SingleState
      {
	WFA wfa;
	testing::Keys keys;

	SingleState(sem_elem_t zero) {
	  wfa.addState(keys.st1, zero->zero());
	  wfa.setInitialState(keys.st1);
	}
      };


      struct BigEpsilonTest
      {
        WFA wfa, expected;

        Key a, b, c, d, e, f, g, h, i, j, k, l, sigma;

        BigEpsilonTest()
          : a(getKey("a"))
          , b(getKey("b"))
          , c(getKey("c"))
          , d(getKey("d"))
          , e(getKey("e"))
          , f(getKey("f"))
          , g(getKey("g"))
          , h(getKey("h"))
          , i(getKey("i"))
          , j(getKey("j"))
          , k(getKey("k"))
          , l(getKey("l"))
          , sigma(getKey("sigma"))
        {
          setupWfa();
          setupExpected();
        }

        void
        setupWfa()
        {
          using testing::StringWeight;
          StringWeight
            * ab = new StringWeight("a->b"),
            * bc = new StringWeight("b->c"),
            * cd = new StringWeight("c->d"),
            * de = new StringWeight("d->e"),
            * df = new StringWeight("d->f"),
            * eg = new StringWeight("e->g"),
            * gh = new StringWeight("g->h"),
            * fh = new StringWeight("f->h"),
            * hi = new StringWeight("h->i"),
            * hj = new StringWeight("h->j"),
            * ik = new StringWeight("i->k"),
            * jk = new StringWeight("j->k"),
            * kl = new StringWeight("k->l"),
            * ll = new StringWeight("l->l");
          sem_elem_t zero = ab->zero();
          Key EPSLN = WALI_EPSILON; // to make spacing prettier

          wfa.addState(a, zero);
          wfa.addState(b, zero);
          wfa.addState(c, zero);
          wfa.addState(d, zero);
          wfa.addState(e, zero);
          wfa.addState(f, zero);
          wfa.addState(g, zero);
          wfa.addState(h, zero);
          wfa.addState(i, zero);
          wfa.addState(j, zero);
          wfa.addState(k, zero);
          wfa.addState(l, zero);

          wfa.setInitialState(a);

          wfa.addTrans(a, sigma, b, ab);
          wfa.addTrans(b, EPSLN, c, bc);
          wfa.addTrans(c, EPSLN, d, cd);
          wfa.addTrans(d, EPSLN, e, de);
          wfa.addTrans(d, EPSLN, f, df);
          wfa.addTrans(e, EPSLN, g, eg);
          wfa.addTrans(g, sigma, h, gh);
          wfa.addTrans(f, EPSLN, h, fh);
          wfa.addTrans(h, sigma, i, hi);
          wfa.addTrans(h, sigma, j, hj);
          wfa.addTrans(i, EPSLN, k, ik);
          wfa.addTrans(j, EPSLN, k, jk);
          wfa.addTrans(k, EPSLN, l, kl);
          wfa.addTrans(l, EPSLN, l, ll);
        }

          //////////////////////////////////////////////////

        void
        setupExpected()
        {
          using testing::StringWeight;
          StringWeight
            * ad = new StringWeight("a->b b->c c->d"),
            * dg = new StringWeight("d->e e->g"),
            * dh = new StringWeight("d->f f->h"),
            * gh = new StringWeight("g->h"),
            * hl = new StringWeight("h->i i->k k->l | h->j j->k k->l"),
            * ll = new StringWeight("l->l");
          sem_elem_t zero = ad->zero();          
          Key EPSLN = WALI_EPSILON; // to make spacing prettier

          expected.addState(a, zero);
          expected.addState(d, zero);
          expected.addState(g, zero);
          expected.addState(h, zero);
          expected.addState(l, zero);

          expected.setInitialState(a);

          expected.addTrans(a, sigma, d, ad);
          expected.addTrans(d, EPSLN, g, dg);
          expected.addTrans(d, EPSLN, h, dh);
          expected.addTrans(g, sigma, h, gh);
          expected.addTrans(h, sigma, l, hl);
          expected.addTrans(l, EPSLN, l, ll);
        }
      };

    }
}


// Yo emacs!
// Local Variables:
//     c-file-style: "ellemtel"
//     c-basic-offset: 2
//     indent-tabs-mode: nil
// End:


#endif
