#ifndef wN_binrel_BINRELMANAGER_GUARD
#define wN_binrel_BINRELMANAGER_GUARD 1

// ::wali::domains::binrel
#include "BinRel.hpp"
// ::wali
#include "wali/ref_ptr.hpp"
#include "wali/Countable.hpp"

#include "buddy/fdd.h"

#include "boost/serialization/access.hpp"

#define BINREL_I_WANT_MINUS_TIMES_AND_DIV_EVEN_THOUGH_THEY_CAN_BE_EXPONENTIALLY_SLOW
#define BINREL_ALWAYS_BUILD_SLOW_ADDER 0
#define BINREL_ALLOW_BUILD_SLOW_ADDER 1

namespace wali
{
  namespace domains
  {
    namespace binrel
    {

      // Forward declare class to define the ref_ptr to ProgramBddContext. 
      class ProgramBddContext;
      typedef ref_ptr< ProgramBddContext > program_bdd_context_t;

      /**
       * @see BddContext
       * All BinRel objects are built in a BddContext.
       * ProgramBddContext is a BddContext with utility functions to create the
       * bdds that can then be encapsulated in a BinRel as a semiring object.
       * The target clients of ProgramBddContext are program analysis tools --
       * it provides ways to build up transformers for program expressions,
       * assignments and assumes.
       *
       * Partial list of functions, grouped by usage:
       * (1) Constructors
       * @see Constructors of BddContext
       * Among other tasks, these intitialize buddy.
       *
       * (2) addBoolVar/addIntVar/setBoolVar/setIntVar variants.
       * A ProgramBddContext has a vocabulary that is intended to correspond to
       * program variables.
       * A base BddContext also has a vocabulary, that is used for pretty
       * printing, and can be used as logical variables.  These functions are
       * used to setup the vocabulary.
       * @see BddContext variants of these functions
       *
       * (3) From, NonDet, Const, Plus, Minus, Times, Div
       * Expression builders. From creates an expression for an rval. NonDet is
       * non-deterministic choice.
       * The bdd returned by these functions *must* (unless you really know
       * what you're doing) be used as an argument to Assign or Assume
       *
       * (4) Assign, Assume
       * Transformer builders. Given lval/rval etc, they build the transformer
       * bdds for assignment and assume statements. Note that Assume takes two
       * expression bdds e1 and e2 as input and returns the transformer for 
       * (e1 == e2)
       *
       * (5) setPre, unsetPre, setPost, unsetPost
       * More low level functions to set the value of a certain variable. These
       * quntify out the rest of the bdd, so to compose them together, you need
       * to take conjunction / disjunction yourself.
       * These functions are meant for testing / experimenting.
       * If these functions are all you need, you should probably be using
       * BddContext, not the more heavy-weight ProgramBddContext.
       **/
      class ProgramBddContext : public BddContext
      {
        public:

          /**
           * Initialize the ProgramBddContext. 
           * @param [bddMemSize] the memory size buddy should use. Use 0 for default.
           * @param [cacheSize] the memory size buddy should use. Use 0 for default.
           **/
          ProgramBddContext(int bddMemSize=0, int cacheSize=0);
          ProgramBddContext(const ProgramBddContext& other);
          virtual ProgramBddContext& operator = (const ProgramBddContext& other);

          ProgramBddContext(const std::map<std::string, int>& vars, int bddMemSize = 0, int cacheSize = 0);
          ProgramBddContext(const std::vector<std::map<std::string, int> >& vars, int bddMemSize = 0, int cacheSize = 0);
        
          //This is left undefined. We need this declaration so that the compiler doesn't think we've
          //hidden the user defined operator = for base class BddContext. We leave it undefined so that 
          //you get a link error if you try to call this.
          //virtual ProgramBddContext& operator = (const BddContext& other);
          ~ProgramBddContext();

          /** Add a boolean variable to the vocabulary with the name 'name' **/
          virtual void addBoolVar(std::string name);
          /** Add a int variable to the vocabulary with the name 'name'. The integer can take values
           * between 0...size-1. **/
          virtual void addIntVar(std::string name, unsigned size);

          /**
           * Add multiple int variables with the given sizes.
           * This function should not be used after addBoolVar/addIntVar.
           * This should be faster than multiple calls to addIntVar.
           **/
          virtual void setIntVars(const std::map<std::string, int>& vars);
          virtual void setIntVars(const std::vector<std::map<std::string, int> >& vars);
        private:
          /**
           * 1) Create bdd space for extra variables.
           * This is pulled out as a different function because both forms of 
           * setIntVars need to use this, but slightly differently.
           * 2) Create bdds chached here. Pulled out for the same reason.
           **/
          void createExtraVars();
          virtual void setupCachedBdds(); 
        public: 
          std::ostream& print(std::ostream& o) const;

          // ////////////////Create a expression for the variable var///////////////////////
          bdd From(std::string var) const;
          // ////////////////Non deterministic expression///////////////////////////////////
          bdd NonDet() const;
          // ////////////////Boolen Expression Generators///////////////////////////////////
          bdd True() const;
          bdd False() const;

          bdd And(bdd lexpr, bdd rexpr) const;
          bdd Or(bdd lexpr, bdd rexpr) const;
          bdd Not(bdd expr) const;

          // //////////////Integer Expression Generators///////////////////////////////////
          bdd Const(unsigned val) const;

          bdd Plus(bdd lexpr, bdd rexpr) const;
#ifdef BINREL_I_WANT_MINUS_TIMES_AND_DIV_EVEN_THOUGH_THEY_CAN_BE_EXPONENTIALLY_SLOW
          bdd Minus(bdd lexpr, bdd rexpr) const;
          bdd Times(bdd lexpr, bdd rexpr) const;
          bdd Div(bdd lexpr, bdd rexpr) const;
#endif

          // //////////////Statement Generators/////////////////////////////////////////////
          // Statements can not be composed.
          bdd HavocVar(std::string var, bdd prev) const;
	  bdd Assign(std::string var, bdd expr) const;
          bdd AssignGen(std::string var, bdd expr, bdd aType) const;
	  bdd AssignTrue(std::string var, bdd expr) const;
	  bdd Assume(bdd expr1, bdd expr2) const;
          bdd BaseID() const;

          // //////////////Generates a random bdd///////////////////////////////////////////
          bdd tGetRandomTransformer(bool isTensored = false, unsigned seed=0) const;

        public:
          // Very useful for testing. Just set variables in the bdd
          // setPre("x") will return a bdd 
          //      *
          // x: 1/ \0
          //    T   F
          // unSet, and set(var,val) do what you'd expect.
          // [uns/s]etPost do the same for post vocabularies. 
          // No tensored plies are used.
          bdd setPre(std::string var) const;
          bdd unsetPre(std::string var) const;
          bdd setPre(std::string var, unsigned val) const;
          bdd setPost(std::string var) const;
          bdd unsetPost(std::string var) const;
          bdd setPost(std::string var, unsigned val) const;

          // //////////////////////////////////////////////////
          // Print out a trace of operations used to build a BddContext
          // object. Later, use such a trace to magically recreate the 
          // BddContext object.
        //public:
          void printHistory(std::ostream& o) const;
          static ProgramBddContext * buildFromHistory(std::istream& in);
        private:
          typedef enum {SET_INT_VECT, SET_INT_MAP, ADD_INT, ADD_BOOL} CreationFunction;
          typedef struct 
          {
            friend class boost::serialization::access;
            CreationFunction cf;
            std::vector< std::map<std::string, int> > arg;
            template<class Archive>            
            void serialize(Archive &ar, const unsigned int file_version);
            std::ostream& print(std::ostream& o) const;
          }CreationCall;
          typedef std::vector< CreationCall > CreationHistory;
          CreationHistory creationHistory;

        private:
          // This is the bdd index of the size info in each bdd
          unsigned sizeInfo;
          // This is the maximum size a register can have
          unsigned maxSize; 
          // These store indices for the "base" only bdd levels that we use for manipulation
          bddinfo_t regAInfo, regBInfo;
		 
          bdd bddAnd() const;
          bdd bddOr() const;
          bdd bddNot() const;
          bdd bddPlus(unsigned in1Size, unsigned in2Size) const;
          bdd bddMinus(unsigned in1Size, unsigned in2Size) const;
          bdd bddTimes(unsigned in1Size, unsigned in2Size) const;
          bdd bddDiv(unsigned in1Size, unsigned in2Size) const;
          
          //Helper functions
          unsigned getRegSize(bdd forThis) const;
          bdd applyBinOp(bdd lexpr, bdd rexpr, bdd op) const;
          bdd applyUnOp(bdd expr, bdd op) const;

          // cache identity. We don't want to create this multiple times
          // also the move for assume
          bdd baseId;
          BddPairPtr baseLhs2Rhs;
      };
    } // namespace binrel
  } //namespace domains
} //namespace wali

// Yo, Emacs!
// Local Variables:
//   c-file-style: "ellemtel"
//   c-basic-offset: 2
// End:

#endif //wN_binrel_BINRELMANAGER_GUARD

