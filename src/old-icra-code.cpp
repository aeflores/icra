#include "gtr/src/lang/gtr_config.h"

#include "tsl/cir/regexp/conc.1level.cir.hpp"
#include "tsl/analysis_components/src/reinterps/emul/conc_extern_phyla.hpp"
#include "tsl/analysis_components/src/reinterps/emul/conc_externs.hpp"
#include "tsl/analysis_components/src/reinterps/emul/concrete_base_type_interp.hpp"

////// The following LIBTSLRE files were originally included directly in NewtonCompare.cpp:
//////
//////   #include "tsl/analysis_components/src/reinterps/emul/conc_extern_types.cpp" // now conc_extern_phyla.cpp
//////   #include "tsl/analysis_components/src/reinterps/emul/conc_externs.cpp"
//////   #include "tsl/analysis_components/src/reinterps/emul/concrete_base_type_interp.cpp"
//////   #include "tsl/cir/regexp/conc.1level.cir.cpp"

//typedef CBTI BASETYPE;
//#include "tsl/analysis_components/src/utils/prettyprint/RegExpPrettyPrint.hpp"
/////////////////////////////////////////

#include "BplToPds.hpp"
#include "wali/domains/name_weight/nameWeight.hpp"

#include "wali/domains/tsl_weight/TSLWeight.hpp"

using namespace wali::domains::tsl_weight;
using namespace tsl_regexp;
using namespace wali::domains::name_weight;
using namespace wali::cprover;

typedef tsl_regexp::Conc1LevelRTG RTG;
typedef tsl_regexp::Conc1LevelCIR CIR;
typedef std::map<int,RTG::regExpRefPtr> tslRegExpMap;
typedef std::map<int,RTG::regExpTRefPtr> tslRegExpTMap;
typedef std::map<int,RTG::regExpTListRefPtr> tslDiffMap;
typedef std::map<int, RTG::regExpListRefPtr> tslUntensoredDiffMap;


// ----------------------------------------------------------------------------
//   Global map variables and their types:

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpRefPtr >,
    RTG::regExpTListRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, RTG::regExpTListRefPtr> > >
    TDiffHashMap;

TDiffHashMap TdiffMap;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpRefPtr >,
    RTG::regExpListRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, RTG::regExpListRefPtr> > >
    DiffHashMap;

DiffHashMap diffHMap;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpRefPtr >,
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr> > >
    EvalMap;

EvalMap EvalMap0;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpRefPtr >,
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr> > >
    EvalRMap;

EvalRMap EvalMap2;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpTRefPtr >,
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpTRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpTRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpTRefPtr >, CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr> > >
    EvalTMap;

EvalTMap EvalMapT;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpRefPtr >,
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr> > >
    StarMap;

typedef boost::unordered_map<
    MemoCacheKey1<RTG::regExpTRefPtr >,
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr,
    boost::hash<MemoCacheKey1<RTG::regExpTRefPtr > >,
    std::equal_to<MemoCacheKey1<RTG::regExpTRefPtr > >,
    std::allocator<std::pair<const MemoCacheKey1<RTG::regExpTRefPtr >, CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr> > >
    StarMapT;

StarMap newStarVal;
StarMapT newStarValT;
StarMap oldStarVal;
StarMapT oldStarValT;

// I'm using these maps as sets; the value in every map entry is "true".
// One could also use boost::unordered_set for this purpose, but that
//   would add another dependency.
typedef boost::unordered_map<
  MemoCacheKey1<RTG::regExpRefPtr >,
  bool,
  boost::hash<MemoCacheKey1<RTG::regExpRefPtr > >,
  std::equal_to<MemoCacheKey1<RTG::regExpRefPtr > >,
  std::allocator<std::pair<const MemoCacheKey1<RTG::regExpRefPtr >, bool > > >
  VisitedMap;

typedef boost::unordered_map<
  MemoCacheKey1<RTG::regExpTRefPtr >,
  bool,
  boost::hash<MemoCacheKey1<RTG::regExpTRefPtr > >,
  std::equal_to<MemoCacheKey1<RTG::regExpTRefPtr > >,
  std::allocator<std::pair<const MemoCacheKey1<RTG::regExpTRefPtr >, bool > > >
  VisitedMapT;

//map of regExps that have already been seen (should change to an unordered_map for speedup)
std::map<reg_exp_t, RTG::regExpRefPtr> regExpConversionMap; // This was formerly called "seen" and was local to convertToRegExp --JTB


// ----------------------------------------------------------------------------
// Stack frames needed by the non-recursive versions of convertToTSL and Eval and EvalT:

// A Stack frame which takes a reg_exp_t and has pointers to it's left and right children, used from
// Converting to a TSLRegExp
struct cFrame
{
    cFrame(reg_exp_t & e) : is_new(true), e(e), op(), left(), right() {}

    bool is_new;
    reg_exp_t e;
    int op;
    reg_exp_t left;
    reg_exp_t right;
};

// A Stack frame which takes a TSL regExpRefPtr and has pointers to it's left and right children
// Used for the differential function and evalRegExpAt0 and evalRegExp
struct dFrame
{
    dFrame(RTG::regExpRefPtr & e) : is_new(true), e(e), op(), left(), right(){}
    bool is_new;
    RTG::regExpRefPtr e;
    int op;
    RTG::regExpRefPtr left;
    RTG::regExpRefPtr right;
};

// A Stack frame which takes a TSL regExpTRefPtr and has pointers to it's left and right children, used for evalT
struct sFrame
{
    sFrame(RTG::regExpTRefPtr & e) : is_new(true), e(e), op(), left(), right(){}
    bool is_new;
    RTG::regExpTRefPtr e;
    int op;
    RTG::regExpTRefPtr left;
    RTG::regExpTRefPtr right;
};

// ----------------------------------------------------------------------------

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr mkSemElemWrapper(relation_t d) {
    wali::sem_elem_tensor_t s(d.get_ptr());
    return s;
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr mkSemElemWrapperFromSemElem(sem_elem_t d) {
    wali::sem_elem_tensor_t s(dynamic_cast<wali::SemElemTensor *>(d.get_ptr()));
    if (s == 0) {
        assert(0 && "Failed to cast a sem_elem to a sem_elem_tensor.");
    }
    return s;
}

relation_t mkRelation(CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr s) {
    relation_t r(dynamic_cast<Relation*>(s.v.get_ptr()));
    if (r == 0) {
        assert(0 && "Failed to cast a sem_elem_wrapper to a Relation.");
    }
    relation_t d(r); // This is unnecessary, right?
    return d;
}


// ----------------------------------------------------------------------------
// Implementations of LIBTSLRE functions:

// PART I: ORIGINALLY FROM conc_externs.cpp:

// TODO: In the following functions, consider whether or not:
//    (1) we need a separate case for the final evaluation that occurs outside
//       of the Newton loop
CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalKleeneSemElemT(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a, 
  const RTG::regExpTRefPtr & child)
{
    relation_t ar = mkRelation(a);
    if (inNewtonLoop) {
        relationpair_t ret;
        
        MemoCacheKey1<RTG::regExpTRefPtr > lookupKeyForStar(child);

        //std::cout << "*** Loop body regular expression is:" << std::endl;
        //tsl_regexp::regExpTPrettyPrint(child, std::cout); 
        //std::cout << std::endl;
        
        relation_t previousValue = 0;
        if (doWideningThisRound) {
            EvalTMap::const_iterator previousValueIterator = oldStarValT.find(lookupKeyForStar);
            if (previousValueIterator != oldStarValT.end()) {
                previousValue = mkRelation(previousValueIterator->second.v);
            }
        }
        
        ret = ar->alphaHatStar(previousValue);
        
        newStarValT.insert(std::make_pair(lookupKeyForStar, ret->first));
        return mkSemElemWrapper(ret->second);
    } else {
        CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
        relationpair_t ret;
        ret = ar->alphaHatStar();
        ans.v = ret->second.get_ptr();
        return ans;
    }
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalKleeneSemElem(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const RTG::regExpRefPtr & child)
{
    relation_t ar = mkRelation(a);
    if (inNewtonLoop) {
        relationpair_t ret;
        
        MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForStar(child);
        
        //std::cout << "*** Loop body regular expression is:" << std::endl;
        //tsl_regexp::regExpPrettyPrint(child, std::cout); 
        //std::cout << std::endl;
        
        relation_t previousValue = 0;
        if (doWideningThisRound) {
            EvalRMap::const_iterator previousValueIterator = oldStarVal.find(lookupKeyForStar);
            if (previousValueIterator != oldStarVal.end()) {
                previousValue = mkRelation(previousValueIterator->second.v);
            }
        }
        
        ret = ar->alphaHatStar(previousValue);
        
        newStarVal.insert(std::make_pair(lookupKeyForStar, ret->first));

        return mkSemElemWrapper(ret->second);
    } else {
        CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
        relationpair_t ret;
        ret = ar->alphaHatStar();
        ans.v = ret->second.get_ptr();
        return ans;
    }
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::evalVarSemElemT(const CBTI_INT32 & v)
{
    assert(false && "Attempted to evaluate a tensored variable.");
    //return CIR::getAssignment(v, globalAssignment);
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::evalVarSemElem(const CBTI_INT32 & v)
{
    return CIR::getAssignment(v, globalAssignment);
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::evalProjectSemElemT(
    const CBTI_INT32 & a, const CBTI_INT32 & b, const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & c)
{
    // Changed by Jason Breck on 2016_03_11 to only call Merge (as opposed to
    //   first calling detensor transpose)
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    relation_t cr = mkRelation(c);
    ans.v = cr->Merge(a.get_data(), b.get_data());
    return ans;
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::evalProjectSemElem(
    const CBTI_INT32 & a, const CBTI_INT32 & b, const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & c)
{
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    relation_t cr = mkRelation(c);
    ans.v = cr->Merge(a.get_data(), b.get_data());
    return ans;
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalTensorTranspose(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & b)
{
    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    //relation_t ar = mkRelation(a);
    //relation_t br = mkRelation(b);
    //relation_t updatedAv = ar->Transpose();
    //ans.v = updatedAv->Kronecker(br);
    //return ans;
    sem_elem_tensor_t updatedAv = a.v->transpose();
    sem_elem_tensor_t ans = updatedAv->tensor(b.v.get_ptr());
    return ans;
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalDotSemElemT(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & b)
{
    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    //relation_t ar = mkRelation(a);
    //relation_t br = mkRelation(b);
    //ans.v = ar->Compose(br);
    //return ans;
    return mkSemElemWrapperFromSemElem(a.v->extend(b.v.get_ptr()));
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalDotSemElem(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & b)
{
    //relation_t ar = mkRelation(a);
    //relation_t br = mkRelation(b);
    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    //ans.v = ar->Compose(br);
    //return ans;
    return mkSemElemWrapperFromSemElem(a.v->extend(b.v.get_ptr()));
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalPlusSemElemT(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & b)
{
    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    //relation_t ar = mkRelation(a);
    //relation_t br = mkRelation(b);
    //ans.v = ar->Union(br);
    //return ans;
    return mkSemElemWrapperFromSemElem(a.v->combine(b.v.get_ptr()));
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr  CONC_EXTERNS::evalPlusSemElem(
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a,
  const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & b)
{
    //relation_t ar = mkRelation(a);
    //relation_t br = mkRelation(b);
    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    //ans.v = ar->Union(br);
    //return ans;
    return mkSemElemWrapperFromSemElem(a.v->combine(b.v.get_ptr()));
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::getOneWt()
{
    return mkSemElemWrapper(Relation::getBaseOne());
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::getZeroWt()
{
    return mkSemElemWrapper(Relation::getBaseZero());
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::getOneTWt()
{
    return mkSemElemWrapper(Relation::getTensorOne());
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::getZeroTWt()
{
    return mkSemElemWrapper(Relation::getTensorZero());
}

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr CONC_EXTERNS::detensorTranspose(const CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr & a)
{
    //relation_t ar = mkRelation(a);
    //return ar->detensorTranspose();
    return a.v->detensorTranspose();
}

// PART II: ORIGINALLY FROM conc_extern_phyla.cpp:

CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr 
CONC_EXTERN_PHYLA::sem_elem_wrapper::random()
{
    // TODO: This may not be needed for regexp; random is only called from CreateRandomState
    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ans;
    return ans;
}

bool 
CONC_EXTERN_PHYLA::sem_elem_wrapper::operator==(const sem_elem_wrapper & a) const
{
    //relation_t ar = mkRelation(a);
    //relation_t vr = mkRelation(v);
    //bool retVal = vr->Equal(ar);
    //return retVal;
    return v->equal(a.v);
}

bool
CONC_EXTERN_PHYLA::sem_elem_wrapper::operator<(const sem_elem_wrapper & a) const
{
    return v<a.v;
}

bool
CONC_EXTERN_PHYLA::sem_elem_wrapper::operator>(const sem_elem_wrapper & a) const
{
    return a.v<v;
}

unsigned long CONC_EXTERN_PHYLA::sem_elem_wrapper::hashValue() const
{
    return v->hash();
}

std::ostream & CONC_EXTERN_PHYLA::sem_elem_wrapper::print(std::ostream & o) const
{
    return v->print(o);
}

std::ostream & CONC_EXTERN_PHYLA::sem_elem_wrapper::printIndented(std::ostream & o, unsigned int indent) const
{
    relation_t vt = mkRelation(v);
    return vt->printIndented(o, indent);
}

std::ostream & CONC_EXTERN_PHYLA::sem_elem_wrapper::concatName(std::ostream& o) {
    // TODO: This may not be needed for regexp; 
    return o;
}
std::ostream & CONC_EXTERN_PHYLA::sem_elem_wrapper::print_nodename(std::ostream & o) const {
    // TODO: This may not be needed for regexp; 
    return o;
}
std::ostream & CONC_EXTERN_PHYLA::sem_elem_wrapper::print_dag(std::ostream& o) const {
    // TODO: This may not be needed for regexp; 
    return o;
}

std::ostream & operator << (std::ostream& o, const CONC_EXTERN_PHYLA::sem_elem_wrapper & b)
{
  return b.print(o);
}

// End of implementations of LIBTSL functions.
// ----------------------------------------------------------------------------

/*
 *  A non recursive version of the Differential function that uses a stack
 *  It takes in a TSL regular expression and returns a Tensored differential
 *
 *  This is based on the recursive TDifferential in regExp.tsl and uses that function's hashtable
 *
 *  @param:  RTG::regExpRefPtr exp - The top level regular expression to be differentiated
 *  @return:  RTG::regExpTListRefPtr - A list of the tensored differentials with respect to
 *                                     all the variables in exp
 *
 *  Author: Emma Turetsky
 */
RTG::regExpTListRefPtr nonRecTDiff(RTG::regExpRefPtr exp)
{
    //This stack represents the worklist, as long as it's not empty, there are still
    //TSL regular expressions to be differentiated
    std::stack<dFrame> todo;
    std::map<RTG::regExpRefPtr, RTG::regExpTListRefPtr>::iterator it;

    //Push the top frame onto the stack
    todo.push(dFrame(exp));

    //While there are still 
    while (!todo.empty())
    {
        //Get the top stack frame
        dFrame & frame = todo.top();
        //If this is the first time we've seen this frame
        if (frame.is_new){  //First check if this regularexpression has been differentiated before, if so then no need to differentiate again
                            //Just return the differentiated regular expression and pop the frame off the stack
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHash(frame.e);
            TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHash);
            if (TDifferential___it != TdiffMap.end()) {
                todo.pop();
                continue;
            }
            frame.is_new = false;

            //Now determine what type of TSLRegExp this is - either a one with children (Kleene,Plus,Dot) or a leaf
            //If it's not a leaf, determine if the children have been differentiated before.  If they haven't, push them
            //on the stack.  If they have, differentiate the TSLRegExp and pop it from the stack
            if (CIR::isKleene(frame.e).get_data())
            {
                frame.op = 0;
                //Kleene only has one child
                RTG::regExpRefPtr child = CIR::getLChild(frame.e);
                frame.left = child;
                //Look it up in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashL(child);
                TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashL);
                //If it has been seen before
                if (TDifferential___it != TdiffMap.end())
                {
                    //Create the tensored differential for frame.e
                    RTG::regExpTListRefPtr ret;
                    RTG::regExpTRefPtr tmp = CIR::mkTensorTranspose(frame.e, frame.e);
                    ret = CIR::mapDotTOnRight(TDifferential___it->second, tmp);
                    //Add the resultant value to the hashmap and pop the stack frame
                    TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, ret));
                    todo.pop();
                    continue;
                }
                else
                {
                    //Push the child TSL RegExp onto the stackframe
                    todo.push(dFrame(child));
                    continue;
                }
            }
            else if (CIR::isDot(frame.e).get_data())
            {
                frame.op = 1;
                //Dot has a left and right child
                frame.left = CIR::getLChild(frame.e);
                frame.right = CIR::getRChild(frame.e);
                //Look up the left child in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashL(frame.left);
                TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashL);
                //The left child has been differentiated before
                if (TDifferential___it != TdiffMap.end())
                {
                    RTG::regExpTListRefPtr lch = TDifferential___it->second;
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashR(frame.right);
                    TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashR);
                    //If the right child has been differentiated before
                    if (TDifferential___it != TdiffMap.end())
                    {
                        //Create the tensored differential for frame.e
                        RTG::regExpTListRefPtr rch = TDifferential___it->second;
                        RTG::regExpTListRefPtr ret;
                        RTG::regExpTListRefPtr mma = CIR::mapDotTOnRight(lch, CIR::mkTensorTranspose(RTG::One::make(), frame.right));
                        RTG::regExpTListRefPtr mmb = CIR::mapDotTOnRight(rch, CIR::mkTensorTranspose(frame.left, RTG::One::make()));
                        ret = CIR::mapPlusT(mma, mmb);
                        //Add the resultant value to the hashmap and pop the stack frame
                        TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, ret));
                        todo.pop();
                        continue;
                    }
                    else  //The right child must be differentiated, so push it on to the stack frame
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
                else //The left child must be differentiated, so push it on to the stack frame
                {
                    todo.push(dFrame(frame.left));
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashR(frame.right);
                    TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashR);
                    //If the right child also must be differentiated, push it on to the stack frame
                    if (TDifferential___it == TdiffMap.end())
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
            }
            else if (CIR::isPlus(frame.e).get_data())
            {
                frame.op = 2;
                //Plus has a left and right child
                frame.left = CIR::getLChild(frame.e);
                frame.right = CIR::getRChild(frame.e);
                //Look up the left child in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashL(frame.left);
                TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashL);
                //The left child has been differentiated before
                if (TDifferential___it != TdiffMap.end())
                {
                    RTG::regExpTListRefPtr lch = TDifferential___it->second;
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashR(frame.right);
                    TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashR);
                    //If the right child has been differentiated before
                    if (TDifferential___it != TdiffMap.end())
                    {
                        //Create the tensored differential for frame.e
                        RTG::regExpTListRefPtr rch = TDifferential___it->second;
                        RTG::regExpTListRefPtr ret;
                        ret = CIR::mapPlusT(lch, rch);
                        //Add the resultant value to the hashmap and pop the stack frame
                        TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, ret));
                        todo.pop();
                        continue;
                    }
                    else //The right child must be differentiated, so push it on to the stack frame
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
                else //The left child must be differentiated, so push it on to the stack frame
                {
                    todo.push(dFrame(frame.left));
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashR(frame.right);
                    TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashR);
                    //If the right child also must be differentiated, push it on to the stack frame
                    if (TDifferential___it == TdiffMap.end())
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
            }
            else { 
                RTG::regExpTListRefPtr T_a1;
                switch (frame.e->GetClassId()) {
                case RTG::TSL_ID_Var: {
                    RTG::VarRefPtr t_T_c1_scast__1 = static_cast<RTG::Var*>(frame.e.get_ptr());
                    BASETYPE::INT32 e_Var_1 = t_T_c1_scast__1->Get_V();
                    RTG::regExpTRefPtr T_c4 = RTG::OneT::make(); // TSL-spec: line 825 of "regExp.tsl"
                    RTG::regExpTListRefPtr T_c5 = RTG::RegExpTListNull::make(); // TSL-spec: line 825 of "regExp.tsl"
                    RTG::regExpTListRefPtr T_c1 = RTG::RegExpTListCons::make(e_Var_1, T_c4, T_c5); // TSL-spec: line 825 of "regExp.tsl"
                    T_a1 = T_c1;
                    TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Zero: {
                    RTG::regExpTListRefPtr T_c1 = RTG::RegExpTListNull::make(); // TSL-spec: line 826 of "regExp.tsl"
                    T_a1 = T_c1;
                    TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_One: {
                    RTG::regExpTListRefPtr T_c1 = RTG::RegExpTListNull::make(); // TSL-spec: line 827 of "regExp.tsl"
                    T_a1 = T_c1;
                    TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Weight: {
                    RTG::WeightRefPtr t_T_c1_scast__1 = static_cast<RTG::Weight*>(frame.e.get_ptr());
                    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr e_Weight_1 = t_T_c1_scast__1->Get_weight();
                    RTG::regExpTListRefPtr T_c1 = RTG::RegExpTListNull::make(); // TSL-spec: line 828 of "regExp.tsl"
                    T_a1 = T_c1;
                    TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Project: {
                    RTG::ProjectRefPtr t_T_c1_scast__1 = static_cast<RTG::Project*>(frame.e.get_ptr());
                    BASETYPE::INT32 e_Project_1 = t_T_c1_scast__1->Get_MS();
                    BASETYPE::INT32 e_Project_2 = t_T_c1_scast__1->Get_MT();
                    RTG::regExpRefPtr e_Project_3 = t_T_c1_scast__1->Get_child();
                    frame.op = 3;
                    frame.left = e_Project_3;
                    todo.push(dFrame(frame.left));
                    continue;
                }
                }
                todo.pop();
                continue;
            }
        }
        else  //This stack fram has been seen before, this means it's children will have been differentiated already and is not a leaf
        {
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHash(frame.e);
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashL(frame.left);
            //Get the left child differentiated value (as all non leaves have a left child) and assign it to lch
            TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashL);
            RTG::regExpTListRefPtr lch = TDifferential___it->second;
            RTG::regExpTListRefPtr ret;
            if (frame.op == 0) //Kleene
            {
                //Create the tensored differential and insert it into the hash_map
                RTG::regExpTRefPtr tmp = CIR::mkTensorTranspose(frame.e, frame.e);
                ret = CIR::mapDotTOnRight(lch, tmp);
                TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, ret));
                todo.pop();
                continue;
            }
            else if (frame.op == 3) //Project
            {
                TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, lch));
                todo.pop();
                continue;
            }
            else  //Dot or Plus
            {
                //Both Dot and Plus have a right child, so look it up and assign it to rch
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForTDifferentialHashR(frame.right);
                //frame.right->print(std::cout) << std::endl;
                TDiffHashMap::const_iterator TDifferential___it = TdiffMap.find(lookupKeyForTDifferentialHashR);

                RTG::regExpTListRefPtr rch = TDifferential___it->second;
                RTG::regExpTListRefPtr ret;
                if (frame.op == 1) //Dot
                {
                    //Create the tensored Differential Value for Dot
                    RTG::regExpTListRefPtr mma = CIR::mapDotTOnRight(lch, CIR::mkTensorTranspose(RTG::One::make(), frame.right));
                    RTG::regExpTListRefPtr mmb = CIR::mapDotTOnRight(rch, CIR::mkTensorTranspose(frame.left, RTG::One::make()));
                    ret = CIR::mapPlusT(mma, mmb);
                }
                else
                {
                    //Create the tensored Differential Value for Plus
                    ret = CIR::mapPlusT(lch, rch);
                }
                //Insert the resultant value into the hashmap
                TdiffMap.insert(std::make_pair(lookupKeyForTDifferentialHash, ret));
                todo.pop();
                continue;
            }
        }
    }

    //This will start with a lookup on the hash_map for exp, which has been evaluated in the loop above, so will not re-evaluate using TDifferential
    MemoCacheKey1<RTG::regExpRefPtr > fin(exp);
    return TdiffMap[fin];
}

/*
 *  A non recursive version of the Differential function that uses a stack
 *  It takes in a TSL regular expression and returns a non-tensored differential
 *
 *  This is based on the recursive Differential in regExp.tsl and uses that function's hashtable
 *
 *  @param:  RTG::regExpRefPtr exp - The top level regular expression to be differentiated
 *  @return:  RTG::regExpListRefPtr - A list of the non-tensored differentials with respect to
 *                                     all the variables in exp
 *
 *  Author: Emma Turetsky
 */
RTG::regExpListRefPtr nonRecUntensoredDiff(RTG::regExpRefPtr exp)
{
    //This stack represents the worklist, as long as it's not empty, there are still
    //TSL regular expressions to be differentiated
    std::stack<dFrame> todo;
    std::map<RTG::regExpRefPtr, RTG::regExpListRefPtr>::iterator it;

    //Push the top frame onto the stack
    todo.push(dFrame(exp));

    while (!todo.empty())
    {
        //Get the top stack frame
        dFrame & frame = todo.top();
        //If this is the first time we've seen this frame
        if (frame.is_new){ //First check if this regularexpression has been differentiated before, if so then no need to differentiate again
                            //Just return the differentiated regular expression and pop the frame off the stack
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHash(frame.e);
            DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHash);
            if (Differential___it != diffHMap.end()) {
                todo.pop();
                continue;
            }
            frame.is_new = false;

            //Now determine what type of TSLRegExp this is - either a one with children (Kleene,Plus,Dot) or a leaf
            //If it's not a leaf, determine if the children have been differentiated before.  If they haven't, push them
            //on the stack.  If they have, differentiate the TSLRegExp and pop it from the stack
            if (CIR::isKleene(frame.e).get_data())
            {
                frame.op = 0;
                //Kleene only has one child
                RTG::regExpRefPtr child = CIR::getLChild(frame.e);
                frame.left = child;
                //Look it up in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashL(child);
                DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashL);
                //If it has been seen before
                if (Differential___it != diffHMap.end())
                {
                    //Create the tensored differential for frame.e
                    RTG::regExpListRefPtr ret;
                    ret = CIR::mapDotBothSides(frame.e, Differential___it->second);
                    //Add the resultant value to the hashmap and pop the stack frame
                    diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, ret));
                    todo.pop();
                    continue;
                }
                else //Push the child TSL RegExp onto the stackframe
                {
                    todo.push(dFrame(child));
                    continue;
                }
            }
            else if (CIR::isDot(frame.e).get_data())
            {
                frame.op = 1;
                //Dot has a left and right child
                frame.left = CIR::getLChild(frame.e);
                frame.right = CIR::getRChild(frame.e);
                //Look up the left child in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashL(frame.left);
                DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashL);
                //If the left child has been differentiated before
                if (Differential___it != diffHMap.end())
                {
                    RTG::regExpListRefPtr lch = Differential___it->second;
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashR(frame.right);
                    DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashR);
                    //If the right child has been differentiated before
                    if (Differential___it != diffHMap.end())
                    {
                        //Create the tensored differential for frame.e
                        RTG::regExpListRefPtr rch = Differential___it->second;
                        RTG::regExpListRefPtr ret;
                        RTG::regExpListRefPtr mma = CIR::mapDotOnRight(lch, frame.right);
                        RTG::regExpListRefPtr mmb = CIR::mapDotOnLeft(frame.left, rch);
                        ret = CIR::mapPlus(mma, mmb);
                        //Add the resultant value to the hashmap and pop the stack frame
                        diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, ret));
                        todo.pop();
                        continue;
                    }
                    else  //The right child must be differentiated, so push it on to the stack frame
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
                else //The left child must be differentiated, so push it on to the stack frame
                {
                    todo.push(dFrame(frame.left));
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashR(frame.right);
                    DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashR);
                    //If the right child also must be differentiated, push it on to the stack frame
                    if (Differential___it == diffHMap.end())
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
            }
            else if (CIR::isPlus(frame.e).get_data())
            {
                frame.op = 2;
                //Plus has a left and right child
                frame.left = CIR::getLChild(frame.e);
                frame.right = CIR::getRChild(frame.e);
                //Look up the left child in the hash table
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashL(frame.left);
                DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashL);
                //If the left child has been differentiated before
                if (Differential___it != diffHMap.end())
                {
                    RTG::regExpListRefPtr lch = Differential___it->second;
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashR(frame.right);
                    DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashR);
                    //If the right child has been differentiated before
                    if (Differential___it != diffHMap.end())
                    {
                        //Create the tensored differential for frame.e
                        RTG::regExpListRefPtr rch = Differential___it->second;
                        RTG::regExpListRefPtr ret;
                        ret = CIR::mapPlus(lch, rch);
                        //Add the resultant value to the hashmap and pop the stack frame
                        diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, ret));
                        todo.pop();
                        continue;
                    }
                    else //The right child must be differentiated, so push it on to the stack frame
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
                else //The left child must be differentiated, so push it on to the stack frame
                {
                    todo.push(dFrame(frame.left));
                    //Look up the right child in the hash table
                    MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashR(frame.right);
                    DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashR);
                    //If the right child also must be differentiated, push it on to the stack frame
                    if (Differential___it == diffHMap.end())
                    {
                        todo.push(dFrame(frame.right));
                        continue;
                    }
                }
            }
            else {
                //The value is a leaf, cheat and just call Differential.  This will add it to DifferentialHash (the hash map) and leverages
                //the existing TSL spec.
                //Pop the stack frame when done.
                RTG::regExpListRefPtr T_a1;

                switch (frame.e->GetClassId()) {
                case RTG::TSL_ID_Var: {
                    RTG::VarRefPtr t_T_c1_scast__1 = static_cast<RTG::Var*>(frame.e.get_ptr());
                    BASETYPE::INT32 e_Var_1 = t_T_c1_scast__1->Get_V();
                    RTG::regExpRefPtr T_c5 = RTG::One::make(); // TSL-spec: line 1018 of "regExp.tsl"
                    RTG::regExpRefPtr T_c6 = RTG::One::make(); // TSL-spec: line 1018 of "regExp.tsl"
                    RTG::rePairListRefPtr T_c7 = RTG::RePairListNull::make(); // TSL-spec: line 1018 of "regExp.tsl"
                    RTG::rePairListRefPtr T_c4 = RTG::RePairListCons::make(T_c5, T_c6, T_c7); // TSL-spec: line 1018 of "regExp.tsl"
                    RTG::regExpListRefPtr T_c8 = RTG::RegExpListNull::make(); // TSL-spec: line 1018 of "regExp.tsl"
                    RTG::regExpListRefPtr T_c1 = RTG::RegExpListCons::make(e_Var_1, T_c4, T_c8); // TSL-spec: line 1018 of "regExp.tsl"
                    T_a1 = T_c1;
                    diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Zero: {
                    RTG::regExpListRefPtr T_c1 = RTG::RegExpListNull::make(); // TSL-spec: line 1019 of "regExp.tsl"
                    T_a1 = T_c1;
                    diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_One: {
                    RTG::regExpListRefPtr T_c1 = RTG::RegExpListNull::make(); // TSL-spec: line 1020 of "regExp.tsl"
                    T_a1 = T_c1;
                    diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Weight: {
                    RTG::WeightRefPtr t_T_c1_scast__1 = static_cast<RTG::Weight*>(frame.e.get_ptr());
                    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr e_Weight_1 = t_T_c1_scast__1->Get_weight();
                    RTG::regExpListRefPtr T_c1 = RTG::RegExpListNull::make(); // TSL-spec: line 1021 of "regExp.tsl"
                    T_a1 = T_c1;
                    diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, T_a1));
                }
                    break;
                case RTG::TSL_ID_Project: {
                    RTG::ProjectRefPtr t_T_c1_scast__1 = static_cast<RTG::Project*>(frame.e.get_ptr());
                    RTG::regExpRefPtr e_Project_3 = t_T_c1_scast__1->Get_child();
                    frame.op = 3;
                    frame.left = e_Project_3;
                    todo.push(dFrame(frame.left));
                    continue;
                }
                }
                    todo.pop();
                    continue;
            }
        }
        else //This stack fram has been seen before, this means it's children will have been differentiated already and is not a leaf
        {
            //Get the left child differentiated value (as all non leaves have a left child) and assign it to lch
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHash(frame.e);
            MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashL(frame.left);
            DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashL);
            RTG::regExpListRefPtr lch = Differential___it->second;
            RTG::regExpListRefPtr ret;
            if (frame.op == 0) //Kleene
            {
                //Create the tensored differential and insert it into the hash_map
                ret = CIR::mapDotBothSides(frame.e, lch);
                diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, ret));
                todo.pop();
                continue;
            }
            else if (frame.op == 3)
            {
                diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, lch));
                todo.pop();
                continue;
            }
            else //Dot
            {
                //Both Dot and Plus have a right child, so look it up and assign it to rch
                MemoCacheKey1<RTG::regExpRefPtr > lookupKeyForDifferentialHashR(frame.right);
                //frame.right->print(std::cout) << std::endl;
                DiffHashMap::const_iterator Differential___it = diffHMap.find(lookupKeyForDifferentialHashR);

                RTG::regExpListRefPtr rch = Differential___it->second;
                RTG::regExpListRefPtr ret;
                if (frame.op == 1) //Dot
                {
                    //Create the tensored Differential Value for Dot
                    RTG::regExpListRefPtr mma = CIR::mapDotOnRight(lch, frame.right);
                    RTG::regExpListRefPtr mmb = CIR::mapDotOnLeft(frame.left, rch);
                    ret = CIR::mapPlus(mma, mmb);
                }
                else
                {
                    //Create the tensored Differential Value for Plus
                    ret = CIR::mapPlus(lch, rch);
                }
                //Insert the resultant value into the hashmap
                diffHMap.insert(std::make_pair(lookupKeyForDifferentialHash, ret));
                todo.pop();
                continue;
            }
        }
    }

    //This will start with a lookup on the hash_map for exp, which has been evaluated in the loop above, so will not re-evaluate using Differential
    MemoCacheKey1<RTG::regExpRefPtr > fin(exp);
    return diffHMap[fin];
}

// NOTE: this feature is not yet implemented
// If this variable is true, then during the procedure-summarization loop,
//   we only re-evaluate procedure summary regular expressions when some
//   variable that they depend on has changed, or we're on the first round.
bool useDependencyMap = true;

void findVariablesInRegExp(RTG::regExpRefPtr outerExpression,
                           std::set<int> &variables,
                           bool onlyIncludeFreeVariables,
                           bool verbose)
{
    std::stack<RTG::regExpRefPtr> todo;
    VisitedMap visited;
    todo.push(outerExpression);

    while (!todo.empty())
    {
        RTG::regExpRefPtr expr = todo.top(); 
        todo.pop();
        MemoCacheKey1<RTG::regExpRefPtr > lookupKey(expr);
        if (visited.find(lookupKey) != visited.end()) continue;
        visited.insert(std::make_pair(lookupKey, true));

        switch (expr->GetClassId()) 
        {
            case RTG::TSL_ID_Var: {
                RTG::VarRefPtr t_T_c1_scast__1 = static_cast<RTG::Var*>(expr.get_ptr());
                int var = t_T_c1_scast__1->Get_V().get_data();
                if (verbose && variables.find(var) == variables.end()) { 
                    std::cout << var << ";  "; 
                }
                variables.insert(var);
                break; }
            case RTG::TSL_ID_Project: {
                RTG::ProjectRefPtr t_T_b1_scast__1 = static_cast<RTG::Project*>(expr.get_ptr());
                todo.push(t_T_b1_scast__1->Get_child());
                break; }
            case RTG::TSL_ID_Dot: 
            case RTG::TSL_ID_Plus:
                todo.push(CIR::getRChild(expr));
                todo.push(CIR::getLChild(expr));
                break;
            case RTG::TSL_ID_Kleene:
                // Note: If onlyIncludeFreeVariables is true, we ignore variables under a Kleene star
                if (!onlyIncludeFreeVariables) { todo.push(CIR::getLChild(expr)); }
                break;
            case RTG::TSL_ID_Zero:
            case RTG::TSL_ID_One:
            case RTG::TSL_ID_Weight:
                break;
            default:
                std::cout << std::endl << "WARNING: Unrecognized regular expression constructor detected." << std::endl;
                useDependencyMap = false;
        }
    } 
}

void findVariablesInRegExpT(RTG::regExpTRefPtr outerExpression,
                            std::set<int> &variables,
                            bool onlyIncludeFreeVariables,
                            bool verbose)
{
    std::stack<RTG::regExpTRefPtr> todo;
    VisitedMapT visited;
    todo.push(outerExpression);

    while (!todo.empty())
    {
        RTG::regExpTRefPtr expr = todo.top(); 
        todo.pop();
        MemoCacheKey1<RTG::regExpTRefPtr > lookupKey(expr);
        if (visited.find(lookupKey) != visited.end()) continue;
        visited.insert(std::make_pair(lookupKey, true));

        switch (expr->GetClassId()) 
        {
            case RTG::TSL_ID_TensorTranspose: { 
                RTG::TensorTransposeRefPtr t_T_c1_scast__1 = 
                           static_cast<RTG::TensorTranspose*>(expr.get_ptr());
                findVariablesInRegExp(t_T_c1_scast__1->Get_lChild(), variables, onlyIncludeFreeVariables, verbose);
                findVariablesInRegExp(t_T_c1_scast__1->Get_rChild(), variables, onlyIncludeFreeVariables, verbose);
                break; }
            case RTG::TSL_ID_ProjectT: {
                RTG::ProjectTRefPtr t_T_b1_scast__1 = static_cast<RTG::ProjectT*>(expr.get_ptr());
                todo.push(t_T_b1_scast__1->Get_child());
                break; }
            case RTG::TSL_ID_DotT: 
            case RTG::TSL_ID_PlusT:
                todo.push(CIR::getRChildT(expr));
                todo.push(CIR::getLChildT(expr));
                break;
            case RTG::TSL_ID_KleeneT:
                // Note: If onlyIncludeFreeVariables is true, we ignore variables under a Kleene star
                if (!onlyIncludeFreeVariables) { todo.push(CIR::getLChildT(expr)); }
                break;
            case RTG::TSL_ID_ZeroT:
            case RTG::TSL_ID_OneT:
                break;
            case RTG::TSL_ID_VarT: {
                RTG::VarTRefPtr t_T_c1_scast__1 = static_cast<RTG::VarT*>(expr.get_ptr());
                int var = t_T_c1_scast__1->Get_VT().get_data();
                if (verbose && variables.find(var) == variables.end()) { 
                    std::cout << var << ";  "; 
                }
                // Currently, tensored variables are not used, so this code emits a warning
                std::cout << std::endl << "WARNING: Tensored variable detected." << std::endl;
                variables.insert(var);
                break; }
            default:
                std::cout << std::endl << "WARNING: Unrecognized regular expression constructor detected." << std::endl;
                useDependencyMap = false;
        }
    } 
}

//void topologicalSortAux(std::map<int, std::set<int> > &stratificationGraph,
//                        int node, 
//                        std::set<int> &visited, 
//                        std::set<int> &locallyVisited, 
//                        std::stack<int> &topologicalOrder,
//                        bool verbose) 
//{
//    visited.insert(node);
//    locallyVisited.insert(node);
//
//    std::set<int> &successors = stratificationGraph[node];
//
//    for(std::set<int>::iterator it = successors.begin(); 
//        it != successors.end(); ++it) 
//    {
//        if (locallyVisited.find(*it) != locallyVisited.end()) {
//            if (verbose) { 
//                std::cout << " CYCLE involving " << node << std::endl; 
//            }
//            assert(false && "Cyclic stratification graph.");
//        }
//        if (visited.find(*it) == visited.end()) {
//            topologicalSortAux(stratificationGraph, *it, visited, locallyVisited, topologicalOrder, verbose);
//        }
//    }
//
//    if (verbose) { std::cout << node << ";  "; }
//    topologicalOrder.push(node);
//    locallyVisited.erase(node);
//}
//
//void topologicalSort(std::map<int, std::set<int> > &stratificationGraph,
//                     std::stack<int> &topologicalOrder, bool verbose) 
//{
//    std::set<int> visited;
//    for(std::map<int, std::set<int> >::iterator it = stratificationGraph.begin();
//        it != stratificationGraph.end(); ++it)
//    {
//        std::set<int> locallyVisited;
//        if (visited.find(it->first) == visited.end()) {
//            topologicalSortAux(stratificationGraph, it->first, visited, locallyVisited, topologicalOrder, verbose);
//        }
//    }
//}

void tarjansSCCsAux(int var,
                    int &currentIndex,
                    std::map<int, int> &index,
                    std::map<int, int> &lowlink,
                    std::set<int> &onStack,
                    std::stack<int> &ancestors,
                    std::map<int, std::set<int> > &stratificationGraph,
                    std::stack<int> &topologicalOrder, 
                    bool verbose)
{
    index[var] = currentIndex;
    lowlink[var] = currentIndex;
    currentIndex++;
    ancestors.push(var);
    onStack.insert(var);

    std::set<int> &successors = stratificationGraph[var];

    for(std::set<int>::iterator it = successors.begin(); 
        it != successors.end(); ++it) 
    {
        int successor = *it;
        if (index.find(successor) == index.end()) {
            tarjansSCCsAux(successor, currentIndex, index, lowlink, onStack, ancestors,
                           stratificationGraph, topologicalOrder, verbose);
            lowlink[var] = min(lowlink[var], lowlink[successor]);
        } else if (onStack.find(successor) != onStack.end()) {
            lowlink[var] = min(lowlink[var], lowlink[successor]);
        }
        if (successor==var) {
            std::cout << std::endl << " *** CYCLE DETECTED: SELF-LOOP AT " 
                << var << " *** " << std::endl;
            assert(false && "Cyclic stratification graph.");
        }
    }

    if (lowlink[var] == index[var]) {
        if (var != ancestors.top()) {
            std::cout << std::endl << " *** CYCLE DETECTED: " << var;
            int ancestor; 
            do {
                ancestor = ancestors.top();
                ancestors.pop();
                std::cout << " <-- " << ancestor;
            } while(ancestor != var);

            std::cout << " *** " << std::endl << std::endl;
            assert(false && "Cyclic stratification graph.");
        }
        ancestors.pop();
        onStack.erase(var);
    }

    if (verbose) { std::cout << var << ";  "; }
    topologicalOrder.push(var);
}

void tarjansSCCs(std::map<int, std::set<int> > &stratificationGraph,
                 std::stack<int> &topologicalOrder, 
                 bool verbose)
{
    int currentIndex = 0;
    std::map<int, int> index;
    std::map<int, int> lowlink;
    std::set<int> onStack;
    std::stack<int> ancestors;
    for(std::map<int, std::set<int> >::iterator it = stratificationGraph.begin();
        it != stratificationGraph.end(); ++it)
    {
        int var = it->first;
        if (index.find(var) == index.end()) {
            tarjansSCCsAux(var, currentIndex, index, lowlink, onStack, ancestors,
                           stratificationGraph, topologicalOrder, verbose);
        }
    }
}

//void testStratificationGraphRoutines() {
//
//    std::map<int, std::set<int> > G;
//    std::stack<int> T;
//
//    std::cout << "TESTING Topological Order:" << std::endl;
//    { std::set<int> S; S.insert(45); S.insert(13); G[21] = S; }
//    { std::set<int> S; S.insert(21); S.insert(45); G[10] = S; }
//    { std::set<int> S; S.insert(21); S.insert(11); G[45] = S; }
//    topologicalSort(G, T, true);
//    std::cout << std::endl;
//}

/*
 *  Compute the number of successive rounds meeting the "simple termination
 *    condition" that we need to observe in the Newton loop before we can soundly
 *    terminate that loop.  (A round meets the "simple termination condition"
 *    if the abstract values of all its star bodies are equal to the values of
 *    star bodies on the previous round.)
 *
 *  We do this by building the "stratification graph" and returning the length 
 *    of the longest path through this graph.  Roughly, one round of the Newton 
 *    loop propagates changes in the value of a variable to the values of any 
 *    variables whose tensored regular expressions (directly) contain it.  
 *    Therefore, if there is a path of length K in this graph, then K rounds 
 *    may be required before a value is fully propagated to all downstream 
 *    variables.
 *
 *  If there is a cycle in the stratification graph, we abort immediately, 
 *    because our Newton loop cannot soundly process the equation system in
 *    that case.
 *
 *  An edge of the stratification graph exists from Xi to Xj just when 
 *    j \in free(i), using the definition of the free() function from 
 *    section 4.1 of the POPL 2016 submission.
 *
 *  Note that the smallest possible return value is 0, which should be
 *    interpreted to mean that no variable occurs free in any other 
 *    variable's regular expression.
 *
 *  @param:  tslRegExpTMap &tensoredRegExpMap - a map from reID to tensored regular expression
 *
 *  @return: int - the height of the stratification graph
 *         
 *  Author: Jason Breck
 */
int computeStratificationHeight(tslRegExpTMap &tensoredRegExpMap)
{
    std::map<int, std::set<int> > stratificationGraph;
    bool verbose = (newtonVerbosity >= NV_NEWTON_LOOP);
    int var;

    if (verbose) { std::cout << std::endl << "Variable Stratification Graph:" << std::endl << std::endl; }

    for (tslRegExpTMap::iterator varIt = tensoredRegExpMap.begin(); varIt != tensoredRegExpMap.end(); varIt++)
    {
        var = varIt->first;
        if (verbose) { std::cout << var << "-->" << std::endl << "  "; }
        std::set<int> &edges = stratificationGraph[var] = std::set<int>();
        findVariablesInRegExpT(varIt->second, edges, true, verbose); 
        if (verbose) { std::cout << std::endl; }
    }

    if (tensoredRegExpMap.size() == 0) { return 1; }

    if (verbose) { std::cout << std::endl << "Reverse Topological Order:" << std::endl << "  "; }
    std::stack<int> topologicalOrder;
    //topologicalSort(stratificationGraph, topologicalOrder, verbose); 
    tarjansSCCs(stratificationGraph, topologicalOrder, verbose); 
    if (verbose) { std::cout << std::endl << std::endl; }

    int stratificationHeight = 0;
    std::map<int, int> longestPathTo;
    while(topologicalOrder.size()) {
        int v = topologicalOrder.top(); 
        topologicalOrder.pop();
        int longestPathToV = longestPathTo[v];
        std::set<int> &successors = stratificationGraph[v];
        for(std::set<int>::iterator w = successors.begin(); w != successors.end(); ++w) 
        {
            // Remember, the default value of the longestPathTo map is 0!
            if (longestPathTo[*w] < longestPathToV + 1) {
                longestPathTo[*w] = longestPathToV + 1;
                if (stratificationHeight < longestPathToV + 1) {
                    stratificationHeight = longestPathToV + 1;
                }
            }
        }
    }

    if (verbose) { std::cout << "Stratification Height: " << stratificationHeight << std::endl; }

    return stratificationHeight;
}

/*
 *  A non recursive test to check the linearity of the untensored portiona of a differential
 *
 *  @param:  RTG::regExpRefPtr rExp - The top level regular expression to be looked at
 *  @return:  bool linear - returns true if the elements of tList pass the test
 *
 *  Author: Emma Turetsky
 */
bool checkLinearityHelperUntensored(RTG::regExpRefPtr rExp)
{
    std::stack<RTG::regExpRefPtr> workList;
    bool linearity = true;

    workList.push(rExp);
    while (!workList.empty() && linearity)
    {
        RTG::regExpRefPtr currentRE = workList.top();
        workList.pop();
        if (CIR::isKleene(currentRE).get_data())
        {
            RTG::regExpRefPtr child = CIR::getLChild(currentRE);
            workList.push(child);
        }
        else if (CIR::isPlus(currentRE).get_data())
        {
            RTG::regExpRefPtr left = CIR::getLChild(currentRE);
            RTG::regExpRefPtr right = CIR::getRChild(currentRE);
            workList.push(left);
            workList.push(right);
        }
        else if (CIR::isDot(currentRE).get_data())
        {
            RTG::regExpRefPtr left = CIR::getLChild(currentRE);
            RTG::regExpRefPtr right = CIR::getRChild(currentRE);
            workList.push(left);
            workList.push(right);
        }
        else if (CIR::isProject(currentRE).get_data())
        {
            RTG::regExpRefPtr child = CIR::getLChild(currentRE);
            workList.push(child);
        }
        else if (CIR::isVar(currentRE).get_data())
        {
            linearity = false;
        }
    }
    return linearity;

}

/*
 *  A non recursive test to check the linearity of tensored partial differential
 *
 *  @param:  RTG::regExpTRefPtr rExpT - The top level regular expression to be looked at
 *  @return:  bool linear - returns true if the elements of tList pass the test
 *
 *  Author: Emma Turetsky
 */
bool checkLinearityHelper(RTG::regExpTRefPtr rExpT)
{
    std::stack<RTG::regExpTRefPtr> workList;
    bool linearity = true;

    workList.push(rExpT);
    while (!workList.empty() && linearity)
    {
        RTG::regExpTRefPtr currentRET = workList.top();
        workList.pop();
        if (CIR::isKleeneT(currentRET).get_data())
        {
            RTG::regExpTRefPtr child = CIR::getLChildT(currentRET);
            workList.push(child);
        }
        else if (CIR::isDotT(currentRET).get_data())
        {
            RTG::regExpTRefPtr left = CIR::getLChildT(currentRET);
            RTG::regExpTRefPtr right = CIR::getRChildT(currentRET);
            workList.push(left);
            workList.push(right);
        }
        else if (CIR::isPlusT(currentRET).get_data())
        {
            RTG::regExpTRefPtr left = CIR::getLChildT(currentRET);
            RTG::regExpTRefPtr right = CIR::getRChildT(currentRET);
            workList.push(left);
            workList.push(right);
        }
        else if (CIR::isProjectT(currentRET).get_data()){
            RTG::regExpTRefPtr child = CIR::getLChildT(currentRET);
            workList.push(child);
        }
        else if (CIR::isTensorTranspose(currentRET).get_data())
        {
            RTG::TensorTransposeRefPtr t_T_c1_scast__1 = static_cast<RTG::TensorTranspose*>(currentRET.get_ptr());
            RTG::regExpRefPtr lch = t_T_c1_scast__1->Get_lChild();
            RTG::regExpRefPtr rch = t_T_c1_scast__1->Get_rChild();
            bool linL = checkLinearityHelperUntensored(lch);
            if (linL)
            {
                linearity = checkLinearityHelperUntensored(rch);
            }
            else
            {
                linearity = false;
            }
        }
    }
    return linearity;
}

/*
 *  A non recursive test to check the linearity of tensored differential
 *
 *  @param:  RTG::regExpTListRefPtr tList - The top level regularexpression list to be looked at
 *  @return:  bool linear - returns true if the elements of tList pass the test
 *
 *  Author: Emma Turetsky
 */
bool checkLinearity(RTG::regExpTListRefPtr tList)
{
    RTG::regExpTListRefPtr nextTList = tList;
    bool linearity = true;
    bool sentinal = true;
    while (sentinal && linearity)  //Run until we've failed the test or reach the end of the tList
    {
        RTG::regExpTListRefPtr currentTList = nextTList;
        if (CIR::isNullTList(currentTList).get_data()){ //Check to see if we've reached the end of the tList
            sentinal = false;
        }
        else
        {
            RTG::regExpTRefPtr rExpT = CIR::getRegExpTFromTList(tList);
            linearity = checkLinearityHelper(rExpT);  //check the linearity of the tensored regular expression
        }
        nextTList = CIR::getNextTList(currentTList);
    }
    
    return linearity;
}

/*
 * Create the tensored differentials from the regular expressions of nodes
 * with epsilon transitions and populate the map differentialMap.
 *
 * @params:  tslRegExpMap &diffMap - (reference to) a map from varID to its corresponding TSLRegExp
 *        tslDiffMap &differentialMap - (reference to) a map from
 *                                      varID to its list of TSL tensoredDifferentials
 *
 * @author:  Emma Turetsky 
 */
bool createDifferentials(tslRegExpMap &diffMap, tslDiffMap &differentialMap){
    bool linear = true;
    for (tslRegExpMap::iterator it = diffMap.begin(); it != diffMap.end(); ++it)
    {
       // std::cout << "EMap: " << it->first << ": ";
       // it->second.print(std::cout);
       // std::cout << std::endl;
        RTG::regExpTListRefPtr rtList = nonRecTDiff(it->second);
        if (linear)
        {
            linear = checkLinearity(rtList);
        }
        differentialMap[it->first] = rtList;
       // std::cout << "Diff: " << it->first << ": ";
       // rtList->print(std::cout);
       // std::cout << std::endl;
    }
    return linear;
}


/*
 *  convertToRegExpT
 *
 *  Convert a WALi NameWeight regexp to a tensored regular expression
 *
 *  This procedure implements Step (5) of Algorithm NPA-TP from TR-1825 (page 33)
 *  http://research.cs.wisc.edu/wpis/papers/tr1825.pdf
 *
 *  In the terms of that paper, this procedure is used to create the
 *  set of regular expressions that are used to evaluate the
 *  variables Z_i in the left-linear equation system over the vector
 *  of variables Z:
 *
 *    Z = (1^t tensor f(0)) combine_T D_T f|_nu(Z).       (*)
 *
 *  At top-level, the value of parameter "exp" is a WALi regular
 *  expression over constants of type NameWeight (for some variable
 *  Z_i). It will be converted into a tensored regular expression
 *  that expresses how to compute the solution for Z_i on a given
 *  Newton round.
 *
 *  Method. Recursively traverse exp:
 *
 *   o convert each regular-expression operator into the analogous
 *     tensored operator
 *
 *   o convert each leaf NameWeight(j,i) into the tensored regular
 *     expression RE_T[j,i], where RE_T[j,i] is the tensored regular
 *     expression in the i-th equation of equation-system (*)
 *
 *       Z_i = (1^t tensor f_i(0)) combine_T ... combine_T (Z_j *_T RE_T[j,i]) combine_T ...
 *
 *  params: reg_exp_t exp - a WALi NameWeight regular expression
 *          tslRegExpMap & regExpMap - map from varId to the TSL regExp representing it
 *          tslDiffMap & differentialMap - map from varId to list of partial differentials
 *                                         representing varId differentiated with
 *                                         respect to different variables
 *          mapBack - a map for associating different variables with the
 *                    appropriate variableID, needed because of the different node names
 *                    in the fwpds IGR - should try to eliminate this need if possible
 *
 *  return: RTG::regExpTRefPtr - the tensored TSLRegexp that
 *           represents exp converted in the manner described
 *           above. The return value at top-level expresses how to
 *           compute the solution for some variable Z_i on a given
 *           Newton round.
 *
 *  Author:  Emma Turetsky
 */
RTG::regExpTRefPtr convertToRegExpT(reg_exp_t exp, tslRegExpMap & regExpMap, tslDiffMap & differentialMap, std::map<int, std::pair<std::pair<int, int>, int> > & mapBack, map<std::pair<int, int>, std::pair<int, int> >  & mergeSrcMap)
{
    if (exp->isConstant()) // Encountered a NameWeight
                              // Create an appropriate tensored regular expression that reflects the NameWeight's
                              // role in the Z equation system
    {
        NameWeight * nw = static_cast<wali::domains::name_weight::NameWeight*>(exp->get_weight().get_ptr());
        int var = nw->getName1();
        int reID = nw->getName2();
        //std::cout << "Looking Up: " << reID << "," << var << std::endl;

        RTG::regExpTRefPtr d;
              // The Z equation-system has an equation of the form
              //    Z_reID = f_reId(0) combine_T ... combine_T (Z_var *_T RE_T[var,reID]) combine_T ...
        if (var == -1){ // Create f_reId(0): NameWeight(dummy, reID) stands for f_reId(0)
                              // in the FWPDS encoding of the Z equation-system's dependence graph
            if (reID == -1){  // Should never occur; return (1^t tensor 1)
                return RTG::OneT::make();
            }
            else { // From the regular expression r for reID, create (1^t tensor r(0))
                             // Look up the appropriate regexp for reID and evaluate at 0
                RTG::regExpRefPtr r = regExpMap[reID];
                //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = evalNonRecAt0(r);
                CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = CIR::evalRegExp(r);
                //std::cout << "***Output with evalNonRecAt0:" << std::endl;
                //newVal.v->print(std::cout);
                //std::cout << std::endl;
                //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr temp = CIR::evalRegExpAt0(r);
                //std::cout << "***Output with evalRegExpAt0:" << std::endl;
                //temp.v->print(std::cout);
                //std::cout << std::endl;
                //d = CIR::mkTensorTranspose(RTG::One::make(), RTG::Weight::make(newVal));
                d = CIR::mkTensorTranspose(RTG::One::make(), CIR::mkWeight(newVal));
            }
        }
        else { // Create the tensored regular expression RE_T[var,reID]
                     // that corresponds to the dependence of reID on var in
                     // the Z equation-system. That is, NameWeight(var,reID)
                     // represents RE_T[var,reID] in an equation in the Z equation-system
                     // of the form
                     //    Z_reID = f_reId(0) combine ... combine_T (Z_var *_T RE_T[var,reID]) combine_T ...
            d = CIR::getTFromRegList(differentialMap[reID], var);
        }

        // Create and return ProjectT(_, _, d) (with appropriate first and second components)
        int t1 = mapBack[reID].first.second;
        int t2 = mapBack[var].first.second;
        //std::cout << "newMerge: " << t1 << "," << t2 << std::endl;
        std::pair<int, int> mergePair = mergeSrcMap[std::pair<int, int>(t1, t2)];
        //return RTG::ProjectT::make(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), d);
        return CIR::mkProjectT(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), d);
    }
    else if (exp->isUpdatable()){  // This case is a placeholder; it will never occur due to
                                      // linear nature of the Z equation-system
              assert(false && "A representation of a tensored variable occurred in a nameweight\n");
        int node_no = exp->updatableNumber();
        //return RTG::VarT::make(CBTI::INT32(node_no));
        return CIR::mkVarT(CBTI::INT32(node_no));
    }
    else if (exp->isStar()){ // Convert a Kleene node to a KleeneT node
        list<reg_exp_t> children = exp->getChildren(); // The list of children of a Star operation has a only a single element
        RTG::regExpTRefPtr c = convertToRegExpT(children.front(), regExpMap, differentialMap, mapBack, mergeSrcMap);
        return CIR::mkKleeneT(c);
    }
    else if (exp->isExtend()){ // Convert a Dot node to a DotT node
        list<reg_exp_t> children = exp->getChildren();
        list<reg_exp_t>::iterator ch = children.begin();
        RTG::regExpTRefPtr lch = convertToRegExpT(*ch, regExpMap, differentialMap, mapBack, mergeSrcMap);
        ch++;
        RTG::regExpTRefPtr rch = convertToRegExpT(*ch, regExpMap, differentialMap, mapBack, mergeSrcMap);
        return CIR::mkDotT(lch, rch);
    }
    else if (exp->isCombine()){ // Convert a Plus node to a PlusT node
        list<reg_exp_t> children = exp->getChildren();
        list<reg_exp_t>::iterator ch = children.begin();
        RTG::regExpTRefPtr lch = convertToRegExpT(*ch, regExpMap, differentialMap, mapBack, mergeSrcMap);
        ch++;
        RTG::regExpTRefPtr rch = convertToRegExpT(*ch, regExpMap, differentialMap, mapBack, mergeSrcMap);
        return CIR::mkPlusT(lch, rch);
    }
} 

/*
 *  convertToRegExp
 *
 *  Convert regular expression reID to a TSL regular expression (RTG::regExpRefPtr)
 *
 *  @param: reID - the varID of the regular expressiosn
 *          exp - the expression to be converted
 *          varDependencies - a map from reID to the set of variabels it depends on that will be populated as we convert, used for the newton round later
 *          updateableMap - a map from updateable node to the intergraph node number it depends on (that node number is still associated with the inter graph)
 *          oMap - a map from the intergraph node number to the correct unique number for the tsl regexps
 *          mapBack - a map for associating different variables with the appropriate variableID, needed because of the different node names
 *                             in the fwpds IGR - should try to eliminate this need if possible]
 *          elapsedTime - Total time done on possible calculations in the conversion (due to simplification in MkDot
 *
 *   Author: Emma Turetsky
 */
RTG::regExpRefPtr convertToRegExp(int reID, reg_exp_t exp, std::map<int, reg_exp_t> & outNodeRegExpsMap, std::map<int, std::set<int> > & varDependencies, std::map<int, int> & updateableMap, std::map<int, int> & oMap, std::map<int, std::pair<std::pair<int, int>, int> > & mapBack, std::map<std::pair<int, int>, std::pair<int, int> > & mergeSrcMap, std::vector<int> & wl, std::set<int> & vl, double * elapsedTime, bool insertProjects = true)
{
    std::stack<cFrame> todo;
    std::map<reg_exp_t, RTG::regExpRefPtr>::iterator it;
#ifdef USE_IRE
    std::map<reg_exp_t, IRE::regExpRefPtr>::iterator IREit;
#endif
    wali::util::GoodTimer * tC = new wali::util::GoodTimer("temp");
    tC->stop();
    double extraTime = tC->total_time();

    reg_exp_t simpl_exp = exp;
    todo.push(cFrame(simpl_exp));
    std::set<int> vDep;
    while (!todo.empty())
    {
        cFrame & frame = todo.top();
        if (frame.is_new){ //We haven't seen the frame before
            it = regExpConversionMap.find(frame.e);
            if (it != regExpConversionMap.end())
            {
                todo.pop();
                continue;
            }
            frame.is_new = false;
            if (frame.e->isConstant())
            {
                if (frame.e->isOne()) {
                    regExpConversionMap[frame.e] = RTG::One::make();
#ifdef USE_IRE
                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkOne();
#endif
                } else if (frame.e->isZero()) {
                    regExpConversionMap[frame.e] = RTG::Zero::make();
#ifdef USE_IRE
                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkZero();
#endif
                } else {  //If the expresssion isn't a simple constant, make an external TSL wrapper around the constant and create a TSL weight
                    relation_t w = mkRelationFromSemElem(frame.e->get_weight());
                    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr wt = w;
                    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr wt = CONC_EXTERN_PHYLA::sem_elem_wrapper(w);
                    //regExpConversionMap[frame.e] = RTG::Weight::make(wt);
                    regExpConversionMap[frame.e] = CIR::mkWeight(wt);
                    //Added by Jason ---------------------------------
                    //std::cerr << "The RTG:regExpRefPtr (Weight) at address " << regExpConversionMap[frame.e].get_ptr() << "\n";
                    //std::cerr << "    was just made from the following weight:" << std::endl;
                    //wt.v->print(std::cerr);
                    //std::cerr << "\n" << std::endl;
                    //---------------------------------
                    //std::cerr << "+++++\n" << std::endl;
                    //wt.v->print(std::cerr);
                    //std::cerr << " @" << regExpConversionMap[frame.e].get_ptr() << "\n";
                    //---------------------------------
                    
#ifdef USE_IRE
                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkWeight(w);
#endif
                }
                todo.pop();
                continue;
            }
            else if (frame.e->isUpdatable())  //This means it's a call
            {
                //If the expression is updatable/is a variable
                //Then look up the node it depends on - and make a variable associated with it
                int node_no = frame.e->updatableNumber();
                //std::cout << "node_no: " << node_no;
                int mNum = oMap[updateableMap[node_no]];
                //std::cout << " mNum: " << mNum << std::endl;
                int t1 = mapBack[reID].first.second;
                int t2 = mapBack[mNum].first.second;
                std::pair<int, int> mergePair = mergeSrcMap[std::pair<int, int>(t1, t2)];
                if (vl.find(mNum) == vl.end())
                {
                    wl.push_back(mNum);
                    vl.insert(mNum);
                }
                vDep.insert(mNum);  //This regExp is dependent on the regExp represented by mNum
                //regExpConversionMap[frame.e] = insertProjects
                //                    ? RTG::Project::make(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), RTG::Var::make(CBTI::INT32(mNum)))
                //                    : RTG::Var::make(CBTI::INT32(mNum));
                regExpConversionMap[frame.e] = insertProjects
                                    ? CIR::mkProject(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), CIR::mkVar(CBTI::INT32(mNum)))
                                    : CIR::mkVar(CBTI::INT32(mNum));

#ifdef USE_IRE
                IREregExpConversionMap[frame.e] = insertProjects
                                    ? IRE::RegExp::mkVar(mNum)->mkProject()
                                    : IRE::RegExp::mkVar(mNum);
#endif
                todo.pop();
                continue;
            }
            //Determine if the children of this reg_exp_t have been seen before.  If so convert to the TSLRegExp, otherwise
            //push the children reg_exp_t onto the stack
            else if (frame.e->isStar())
            {
                frame.op = 0;
                reg_exp_t child = frame.e->getChildren().front();
                frame.left = child;
                it = regExpConversionMap.find(child);
                if (it != regExpConversionMap.end())
                {
                    tC->start();
                    RTG::regExpRefPtr ret = CIR::mkKleene(it->second);
                    //Added by Jason ---------------------------------
                    //std::cerr << "The RTG:regExpRefPtr at address " << ret.get_ptr() << "\n";
                    //std::cerr << "    was just made from the following loop body reg_exp_t:" << std::endl;
                    //frame.left.get_ptr()->print(std::cerr);
                    //std::cerr << "\n" << std::endl;
                    //---------------------------------
                    tC->stop();
                    regExpConversionMap[frame.e] = ret;

#ifdef USE_IRE
                    IREit = IREregExpConversionMap.find(child);
                    if (IREit != IREregExpConversionMap.end()) {
                        IREregExpConversionMap[frame.e] = IREit->second->mkKleene();
                    }
#endif
                    todo.pop();
                    continue;
                }
                else
                {
                    todo.push(cFrame(child));
                    continue;
                }
            }
            else
            {
                if (frame.e->isExtend())
                    frame.op = 1;
                else
                    frame.op = 2;

                list<reg_exp_t>::iterator ch = frame.e->children.begin();
                frame.left = *ch;
                ch++;
                frame.right = *ch;
                it = regExpConversionMap.find(frame.left);
                if (it != regExpConversionMap.end())
                {
                    RTG::regExpRefPtr lch = it->second;
                    it = regExpConversionMap.find(frame.right);
                    if (it != regExpConversionMap.end())
                    {
                        RTG::regExpRefPtr rch = it->second;
                        RTG::regExpRefPtr ret;
                        tC->start();
                        if (frame.op == 1)
                            ret = CIR::mkDot(lch, rch);
                        else
                            ret = CIR::mkPlus(lch, rch);
                        tC->stop();
                        regExpConversionMap[frame.e] = ret;

#ifdef USE_IRE
                        IREit = IREregExpConversionMap.find(frame.left); 
                        if (IREit != IREregExpConversionMap.end()) { IRE::regExpRefPtr IRElch = IREit->second;
                            IREit = IREregExpConversionMap.find(frame.right); 
                            if (IREit != IREregExpConversionMap.end()) { IRE::regExpRefPtr IRErch = IREit->second;
                                IRE::regExpRefPtr IREret;
                                if (frame.op == 1) {
                                    IREret = IRElch->mkDot(IRErch);
                                } else {
                                    IREret = IRElch->mkPlus(IRErch);
                                }
                                IREregExpConversionMap[frame.e] = IREret;
                            }
                        }
#endif
                        todo.pop();
                        continue;
                    }
                    else
                    {
                        todo.push(cFrame(frame.right));
                        continue;
                    }
                }
                else
                {
                    todo.push(cFrame(frame.left));
                    if (regExpConversionMap.find(frame.right) == regExpConversionMap.end())
                    {
                        todo.push(cFrame(frame.right));
                        continue;
                    }
                }
            }
        }
        else  //We've seen this reg_exp_t before, so it's children have been converted
        {
            //Look up the children and call the appropriate constructor for TSL
            RTG::regExpRefPtr lch = regExpConversionMap[frame.left];
            RTG::regExpRefPtr ret;

#ifdef USE_IRE
            IRE::regExpRefPtr IRElch = IREregExpConversionMap[frame.left];
            IRE::regExpRefPtr IREret;
#endif
            if (frame.op == 0)  //Kleene
            {
                tC->start();
                ret = CIR::mkKleene(lch);
                //Added by Jason ---------------------------------
                //std::cerr << "The RTG:regExpRefPtr at address " << ret.get_ptr() << "\n";
                //std::cerr << "    was just made from the following loop body reg_exp_t:" << std::endl;
                //frame.left.get_ptr()->print(std::cerr);
                //std::cerr << "\n" << std::endl;
                //---------------------------------
                tC->stop();

#ifdef USE_IRE
                IREret = IRElch->mkKleene();
#endif
            }
            else  //Dot or Plus
            {
                RTG::regExpRefPtr rch = regExpConversionMap[frame.right];

#ifdef USE_IRE
                IRE::regExpRefPtr IRErch = IREregExpConversionMap[frame.right];
#endif
                if (frame.op == 1)  //Dot
                {
                    tC->start();
                    ret = CIR::mkDot(lch, rch);
                    tC->stop();

#ifdef USE_IRE
                    IREret = IRElch->mkDot(IRErch);
#endif
                }
                else  //Plus
                {
                    tC->start();
                    ret = CIR::mkPlus(lch, rch);
                    tC->stop();

#ifdef USE_IRE
                    IREret = IRElch->mkPlus(IRErch);
#endif
                }
            }
            regExpConversionMap[frame.e] = ret;

#ifdef USE_IRE
            IREregExpConversionMap[frame.e] = IREret;
#endif

            todo.pop();
        }
    }

    varDependencies[reID] = vDep;   //Put the dependant variable set into varDependencies
    *elapsedTime = tC->total_time() - extraTime;

#ifdef USE_IRE
    std::cout << "CONVERTED a new-style (IRE) regular expression:" << std::endl;
    IREregExpConversionMap[simpl_exp]->print();
    std::cout << std::endl << std::endl;
#endif

    return regExpConversionMap[simpl_exp];
}

//{
//    std::stack<cFrame> todo;
//    std::map<reg_exp_t, RTG::regExpRefPtr>::iterator it;
//#ifdef USE_IRE
//    std::map<reg_exp_t, IRE::regExpRefPtr>::iterator IREit;
//#endif
//
//    reg_exp_t simpl_exp = exp;
//    todo.push(cFrame(simpl_exp));
//
//    while (!todo.empty())
//    {
//        cFrame & frame = todo.top();
//        if (frame.is_new){ //We haven't seen the frame before
//            it = regExpConversionMap.find(frame.e);
//            if (it != regExpConversionMap.end())
//            {
//                todo.pop();
//                continue;
//            }
//            frame.is_new = false;
//            if (frame.e->isConstant())
//            {
//                if (frame.e->isOne()) {
////                    regExpConversionMap[frame.e] = RTG::One::make();
//#ifdef USE_IRE
//                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkOne();
//#endif
//                } else if (frame.e->isZero()) {
////                    regExpConversionMap[frame.e] = RTG::Zero::make();
//#ifdef USE_IRE
//                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkZero();
//#endif
//                } else {  //If the expresssion isn't a simple constant, make an external TSL wrapper around the constant and create a TSL weight
////                    relation_t w = mkRelationFromSemElem(frame.e->get_weight());
//                    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr wt = w;
////                    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr wt = CONC_EXTERN_PHYLA::sem_elem_wrapper(w);
//                    //regExpConversionMap[frame.e] = RTG::Weight::make(wt);
////                    regExpConversionMap[frame.e] = CIR::mkWeight(wt);
//                    //Added by Jason ---------------------------------
//                    //std::cerr << "The RTG:regExpRefPtr (Weight) at address " << regExpConversionMap[frame.e].get_ptr() << "\n";
//                    //std::cerr << "    was just made from the following weight:" << std::endl;
//                    //wt.v->print(std::cerr);
//                    //std::cerr << "\n" << std::endl;
//                    //---------------------------------
//                    //std::cerr << "+++++\n" << std::endl;
//                    //wt.v->print(std::cerr);
//                    //std::cerr << " @" << regExpConversionMap[frame.e].get_ptr() << "\n";
//                    //---------------------------------
//                    
//#ifdef USE_IRE
//                    IREregExpConversionMap[frame.e] = IRE::RegExp::mkWeight(w);
//#endif
//                }
//                todo.pop();
//                continue;
//            }
//            else if (frame.e->isUpdatable())  //This means it's a call
//            {
//                //If the expression is updatable/is a variable
//                //Then look up the node it depends on - and make a variable associated with it
//                int node_no = frame.e->updatableNumber();
//                //std::cout << "node_no: " << node_no;
//                int mNum = oMap[updateableMap[node_no]];
//                //std::cout << " mNum: " << mNum << std::endl;
//
//#ifdef USE_IRE
//                IREregExpConversionMap[frame.e] = insertProjects
//                                    ? IRE::RegExp::mkVar(mNum)->mkProject()
//                                    : IRE::RegExp::mkVar(mNum);
//#endif
//                todo.pop();
//                continue;
//            }
//            //Determine if the children of this reg_exp_t have been seen before.  If so convert to the TSLRegExp, otherwise
//            //push the children reg_exp_t onto the stack
//            else if (frame.e->isStar())
//            {
//                frame.op = 0;
//                reg_exp_t child = frame.e->getChildren().front();
//                frame.left = child;
//                it = regExpConversionMap.find(child);
//                if (it != regExpConversionMap.end())
//                {
//                    tC->start();
//                    RTG::regExpRefPtr ret = CIR::mkKleene(it->second);
//                    //Added by Jason ---------------------------------
//                    //std::cerr << "The RTG:regExpRefPtr at address " << ret.get_ptr() << "\n";
//                    //std::cerr << "    was just made from the following loop body reg_exp_t:" << std::endl;
//                    //frame.left.get_ptr()->print(std::cerr);
//                    //std::cerr << "\n" << std::endl;
//                    //---------------------------------
//                    tC->stop();
//                    regExpConversionMap[frame.e] = ret;
//
//#ifdef USE_IRE
//                    IREit = IREregExpConversionMap.find(child);
//                    if (IREit != IREregExpConversionMap.end()) {
//                        IREregExpConversionMap[frame.e] = IREit->second->mkKleene();
//                    }
//#endif
//                    todo.pop();
//                    continue;
//                }
//                else
//                {
//                    todo.push(cFrame(child));
//                    continue;
//                }
//            }
//            else
//            {
//                if (frame.e->isExtend())
//                    frame.op = 1;
//                else
//                    frame.op = 2;
//
//                list<reg_exp_t>::iterator ch = frame.e->children.begin();
//                frame.left = *ch;
//                ch++;
//                frame.right = *ch;
//                it = regExpConversionMap.find(frame.left);
//                if (it != regExpConversionMap.end())
//                {
//                    RTG::regExpRefPtr lch = it->second;
//                    it = regExpConversionMap.find(frame.right);
//                    if (it != regExpConversionMap.end())
//                    {
//                        RTG::regExpRefPtr rch = it->second;
//                        RTG::regExpRefPtr ret;
//                        tC->start();
//                        if (frame.op == 1)
//                            ret = CIR::mkDot(lch, rch);
//                        else
//                            ret = CIR::mkPlus(lch, rch);
//                        tC->stop();
//                        regExpConversionMap[frame.e] = ret;
//
//#ifdef USE_IRE
//                        IREit = IREregExpConversionMap.find(frame.left); 
//                        if (IREit != IREregExpConversionMap.end()) { IRE::regExpRefPtr IRElch = IREit->second;
//                            IREit = IREregExpConversionMap.find(frame.right); 
//                            if (IREit != IREregExpConversionMap.end()) { IRE::regExpRefPtr IRErch = IREit->second;
//                                IRE::regExpRefPtr IREret;
//                                if (frame.op == 1) {
//                                    IREret = IRElch->mkDot(IRErch);
//                                } else {
//                                    IREret = IRElch->mkPlus(IRErch);
//                                }
//                                IREregExpConversionMap[frame.e] = IREret;
//                            }
//                        }
//#endif
//                        todo.pop();
//                        continue;
//                    }
//                    else
//                    {
//                        todo.push(cFrame(frame.right));
//                        continue;
//                    }
//                }
//                else
//                {
//                    todo.push(cFrame(frame.left));
//                    if (regExpConversionMap.find(frame.right) == regExpConversionMap.end())
//                    {
//                        todo.push(cFrame(frame.right));
//                        continue;
//                    }
//                }
//            }
//        }
//        else  //We've seen this reg_exp_t before, so it's children have been converted
//        {
//            //Look up the children and call the appropriate constructor for TSL
//            RTG::regExpRefPtr lch = regExpConversionMap[frame.left];
//            RTG::regExpRefPtr ret;
//
//#ifdef USE_IRE
//            IRE::regExpRefPtr IRElch = IREregExpConversionMap[frame.left];
//            IRE::regExpRefPtr IREret;
//#endif
//            if (frame.op == 0)  //Kleene
//            {
//                tC->start();
//                ret = CIR::mkKleene(lch);
//                //Added by Jason ---------------------------------
//                //std::cerr << "The RTG:regExpRefPtr at address " << ret.get_ptr() << "\n";
//                //std::cerr << "    was just made from the following loop body reg_exp_t:" << std::endl;
//                //frame.left.get_ptr()->print(std::cerr);
//                //std::cerr << "\n" << std::endl;
//                //---------------------------------
//                tC->stop();
//
//#ifdef USE_IRE
//                IREret = IRElch->mkKleene();
//#endif
//            }
//            else  //Dot or Plus
//            {
//                RTG::regExpRefPtr rch = regExpConversionMap[frame.right];
//
//#ifdef USE_IRE
//                IRE::regExpRefPtr IRErch = IREregExpConversionMap[frame.right];
//#endif
//                if (frame.op == 1)  //Dot
//                {
//                    tC->start();
//                    ret = CIR::mkDot(lch, rch);
//                    tC->stop();
//
//#ifdef USE_IRE
//                    IREret = IRElch->mkDot(IRErch);
//#endif
//                }
//                else  //Plus
//                {
//                    tC->start();
//                    ret = CIR::mkPlus(lch, rch);
//                    tC->stop();
//
//#ifdef USE_IRE
//                    IREret = IRElch->mkPlus(IRErch);
//#endif
//                }
//            }
//            regExpConversionMap[frame.e] = ret;
//
//#ifdef USE_IRE
//            IREregExpConversionMap[frame.e] = IREret;
//#endif
//
//            todo.pop();
//        }
//    }
//
//#ifdef USE_IRE
//    std::cout << "CONVERTED a new-style (IRE) regular expression:" << std::endl;
//    IREregExpConversionMap[simpl_exp]->print();
//    std::cout << std::endl << std::endl;
//#endif
//
//    return regExpConversionMap[simpl_exp];
//}
//#endif

/*
 *  computeVarDependenciesAndWorklists
 *
 *  Add to varDependencies[reID] the IDs of any variables that appear in the regular expression for reID; also, add them to wl and vl.
 *
 *  This function was created by copying convertToRegExp and removing all of the code that creates
 *    TSL regular expressions, only keeping the code that updates varDependencies and wl and vl.
 *    This function was needed after we changed the function cache (called regExpConversionMap now)
 *    for convertToRegExp so that that function cache was shared across calls; after that change,
 *    convertToRegExp cannot update varDependencies anymore.
 *
 *  @param: reID - the varID of the regular expressiosn
 *          exp - the expression to be converted
 *          varDependencies - a map from reID to the set of variabels it depends on that will be populated as we convert, used for the newton round later
 *          updateableMap - a map from updateable node to the intergraph node number it depends on (that node number is still associated with the inter graph)
 *          oMap - a map from the intergraph node number to the correct unique number for the tsl regexps
 *
 *   Author: Jason Breck
 */
void computeVarDependenciesAndWorklists(int reID, reg_exp_t exp, std::map<int, std::set<int> > & varDependencies, std::map<int, int> & updateableMap, std::map<int, int> & oMap, std::vector<int> & wl, std::set<int> & vl)
{
    //void computeVarDependencies(int reID, reg_exp_t exp, std::map<int, reg_exp_t> & outNodeRegExpsMap, std::map<int, std::set<int> > & varDependencies, std::map<int, int> & updateableMap, std::map<int, int> & oMap, std::map<int, std::pair<std::pair<int, int>, int> > & mapBack, std::map<std::pair<int, int>, std::pair<int, int> > & mergeSrcMap, std::vector<int> & wl, std::set<int> & vl, double * elapsedTime, bool insertProjects = true)
    std::stack<cFrame> todo;

    std::map<reg_exp_t, bool >::iterator it;
    std::map<reg_exp_t, bool > seen; 

    reg_exp_t simpl_exp = exp;
    todo.push(cFrame(simpl_exp));
    std::set<int> vDep;
    while (!todo.empty())
    {
        cFrame & frame = todo.top();
        if (frame.is_new){ // We haven't seen the frame before
            it = seen.find(frame.e);
            if (it != seen.end())
            {
                todo.pop();
                continue;
            }
            frame.is_new = false;
            if (frame.e->isConstant())
            {
                todo.pop();
                continue;
            }
            else if (frame.e->isUpdatable())  // This means it's a call
            {
                ////If the expression is updatable/is a variable
                ////Then look up the node it depends on - and make a variable associated with it
                int node_no = frame.e->updatableNumber();
                ////std::cout << "node_no: " << node_no;
                int mNum = oMap[updateableMap[node_no]];
                ////std::cout << " mNum: " << mNum << std::endl;
                //int t1 = mapBack[reID].first.second;
                //int t2 = mapBack[mNum].first.second;
                //std::pair<int, int> mergePair = mergeSrcMap[std::pair<int, int>(t1, t2)];
                if (vl.find(mNum) == vl.end())
                {
                    wl.push_back(mNum); // FIXME - jbreck - Does this need to be changed?
                    vl.insert(mNum);
                }
                vDep.insert(mNum);  //This regExp is dependent on the regExp represented by mNum
                ////seen[frame.e] = insertProjects
                ////                    ? RTG::Project::make(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), RTG::Var::make(CBTI::INT32(mNum)))
                ////                    : RTG::Var::make(CBTI::INT32(mNum));
                //seen[frame.e] = insertProjects
                //                    ? CIR::mkProject(CBTI::INT32(mergePair.first), CBTI::INT32(mergePair.second), CIR::mkVar(CBTI::INT32(mNum)))
                //                    : CIR::mkVar(CBTI::INT32(mNum));
                seen[frame.e] = true;
                todo.pop();
                continue;
            }
            //Determine if the children of this reg_exp_t have been seen before.  If so convert to the TSLRegExp, otherwise
            //push the children reg_exp_t onto the stack
            else if (frame.e->isStar())
            {
                frame.op = 0;
                reg_exp_t child = frame.e->getChildren().front();
                frame.left = child;
                it = seen.find(child);
                if (it != seen.end())
                {
                    seen[frame.e] = true;
                    todo.pop();
                    continue;
                }
                else
                {
                    todo.push(cFrame(child));
                    continue;
                }
            }
            else
            {
                if (frame.e->isExtend())
                    frame.op = 1;
                else
                    frame.op = 2;

                list<reg_exp_t>::iterator ch = frame.e->children.begin();
                frame.left = *ch;
                ch++;
                frame.right = *ch;
                it = seen.find(frame.left);
                if (it != seen.end())
                {
                    it = seen.find(frame.right);
                    if (it != seen.end())
                    {
                        seen[frame.e] = true;
                        todo.pop();
                        continue;
                    }
                    else
                    {
                        todo.push(cFrame(frame.right));
                        continue;
                    }
                }
                else
                {
                    todo.push(cFrame(frame.left));
                    if (seen.find(frame.right) == seen.end())
                    {
                        todo.push(cFrame(frame.right));
                        continue;
                    }
                }
            }
        }
        else 
        {
            seen[frame.e] = true;
            todo.pop();
        }
    }

    varDependencies[reID] = vDep;   // Put the dependent variable set into varDependencies
}
    
/*
 *  convertToTSLRegExps
 *
 *  Convert regular expression reg to a TSL regular expression, and
 *  put it in the map regExpMap, which maps from RegExp IDs to TSL
 *  regular expressions
 *
 *  outNodeRegExpMap - A map from reID to the wali reg_exp_t that represents it
 *  updateableMap - A map of updateable node numbers to the intergraph node they depend on
 *  oMap - a map of intergraph nodes to the ReID that represents them
 *  varDependencies - a map from reID to the set of variabels it depends on that will be populated as we convert, used for the newton round later
 *  mapBack - a map for associating different variables with the appropriate variableID, needed because of the different node names
 *
 *  Author:  Emma Turetsky
 */
double convertToTSLRegExps(int reg, std::map<int, reg_exp_t> & outNodeRegExpMap, tslRegExpMap & regExpMap, std::map<int, std::set<int> > & varDependencies, std::map<int, int> & updateableMap, std::map<int, int> & oMap, std::map<int, std::pair<std::pair<int, int>, int> > & mapBack, std::map<std::pair<int, int>, std::pair<int, int > >  & mergeSrcMap, std::vector<int> & wl, std::set<int> & vl
, bool insertProjects = true)
{
    //std::cout << "REID: " << reg << endl;
    //outNodeRegExpMap[reg]->print(std::cout) << std::endl;
    double evalTime = 0;
    RTG::regExpRefPtr rExp = convertToRegExp(reg, outNodeRegExpMap[reg], outNodeRegExpMap, varDependencies, updateableMap, oMap, mapBack, mergeSrcMap, wl, vl, &evalTime, insertProjects);

    computeVarDependenciesAndWorklists(reg, outNodeRegExpMap[reg], varDependencies, updateableMap, oMap, wl, vl);
    //rExp->print(std::cout) << std::endl;
    //std::cout << "src: " << mapBack[reg].first.first << "tgt: " << mapBack[reg].first.second << "stack: " << mapBack[reg].second << endl << endl;
    regExpMap[reg] = rExp;
    return evalTime;
}


/*
 *  convertToTSLRegExpsT
 *
 *  For each WALi NameWeight regexp in rList, convert it to a tensored regular expression
 *
 *  The wali regexp represents a differentiated value.  The weight represents a regexp in eMap
 *  differentiated with respect to a given variable (or not differentiated, depending) or is
 *  the representation of "one"
 *
 *
 *  params:  rList -  a map from reID to the appropriate NameWeight regExp
 *           tslRegExpMap & regExpMap - map from varId to the TSL regExpRepresenting it
 *           tslDiffMap & differentialMap - map from varId to list of partial differentials representing varId
 *           differentiated with respect to different variables
 *        mapBack - a map for associating different variables with the appropriate variableID, needed because of the different node names
 *                     in the fwpds IGR - should try to eliminate this need if possible
 *
 *  Author:  Emma Turetsky
 */
void convertToTSLRegExpsT(std::map<int, reg_exp_t> & rList, tslRegExpMap & regExpMap, tslRegExpTMap & tensoredRegExpMap, tslDiffMap & differentialMap, std::map<int, std::pair<std::pair<int, int>, int> > & mapBack, std::map<std::pair<int, int>, std::pair<int, int > >  & mergeSrcMap)
{
    for (std::map<int, reg_exp_t>::iterator it = rList.begin(); it != rList.end(); ++it)
    {
        if (it->first != -1){
            // std::cout << "Converting RegExp: " << it->first << std::endl;
            // it->second->print(std::cout);
            // std::cout << std::endl;
            RTG::regExpTRefPtr rExp = convertToRegExpT(it->second, regExpMap, differentialMap, mapBack, mergeSrcMap);
            //rExp->print(std::cout) << endl;
            tensoredRegExpMap[it->first] = rExp;
        }
    }
}

/*
 *  A method to print TSL RegExps
 */
void printTSLRegExps(tslRegExpMap rMap)
{
  for (tslRegExpMap::iterator it = rMap.begin(); it!=rMap.end(); ++it)
  {
    cout << it->first << ": ";
    it->second.print(cout);
    cout << "\n";
  }
}

/*
 *  A method to print TSL Differentials
 */
void printTSLDifferentials(tslDiffMap dMap)
{
  for (tslDiffMap::iterator it = dMap.begin(); it!=dMap.end(); ++it)
  {
    cout << it->first << ": ";
    it->second.print(cout);
    cout << "\n";
  }
}

/*
 *  A method to print TSL tensored RegExps
 */
void printTSLTExp(tslRegExpTMap tMap)
{
  for (tslRegExpTMap::iterator it = tMap.begin(); it!=tMap.end(); ++it)
  {
    cout << it->first << ": ";
    it->second.print(cout);
    cout << "\n";
  }
}

/*
 *  fwpdsFromDifferential
 *
 *  pds - the new fwpds that is being generated
 *  differentialMap - a map of differentials
 *  varDependencies - a map from an reID (a variable) to the set of
 *                    variables it depends on
 *
 *  This procedure implements Step (3) of Algorithm NPA-TP from TR-1825 (page 33)
 *  http://research.cs.wisc.edu/wpis/papers/tr1825.pdf
 *
 *  In the terms of that paper, we are creating the dependence graph
 *  G for the left-linear equation system over the vector Z:
 *
 *    Z = (1^t tensor f(0)) combine_T D_T f|_nu(Z).       (*)
 *
 *  In terms of the implementation, the procedure is really taking
 *  one encoding of G (namely, varDependencies + differentialMap),
 *  and converting it into another encoding of G. Procedure
 *  fwpdsFromDifferential returns an encoding of G as an FWPDS, which
 *  allows us to use the FWPDS operations that implement Tarjan's
 *  path-expression method to create an appropriate set of tensored
 *  regular expressions for the nodes of G (i.e., for the variables
 *  {Z_i}, where each Z_i corresponds to some procedure i).
 *
 *  However, the encoding is slightly indirect because we have not
 *  yet actually created the tensored regular expressions of the
 *  Z-equations.  (We have various auxiliary maps from which the
 *  appropriate tensored regular expressions will be created later.)
 *  Instead, we just create an FWPDS that represents the dependences
 *  of the Z system, along with "NameWeights" that stand for the
 *  (so-far still missing) tensored regular expressions.  (The
 *  NameWeights have enough information in them so that, using the
 *  aforementioned auxiliary data structures, the correct tensored
 *  regular expressions can be constructed later; see procedure
 *  convertToRegExpT.)
 *
 *  Because the Z system is left-linear, and hence regular,
 *  dependence graph G is a normal graph, and hence we only use
 *  Delta-1 rules.  In particular, if the equation system contains
 *  Z_i = ... combine_T (Z_j *_T RE_T[j,i]) combine_T ...,
 *
 *   o variable Z_i (corresponding to some procedure i) is represented by i
 *
 *   o the dependence of i on j is encoded by the Delta-1 rule
 *     (st1, j) -> (st1, i)
 *
 *   o the weight on such a rule is NameWeight(j,i), which stands
 *     for the tensored regular expression RE_T[j,i] that contributes
 *     to the i-th equation of equation-system (*)
 *
 *  In addition, G has additional Delta-1 rules of the form
 *
 *     (st1, dummy) -> (st1, i), with weight NameWeight(dummy,i).
 *
 *  Such a rule represents the equation with the constant right-hand side
 *
 *     Z_i = (1^t tensor f_i(0))
 *
 *  that appears in the i-th component of equation-system (*).
 *  
 *  Author: Emma Turetsky
 */
void fwpdsFromDifferential(FWPDS * pds, tslDiffMap & differentialMap, std::map<int, std::set<int> > & varDependencies)
{
    // For each component of the equation system
    for(tslDiffMap::iterator it = differentialMap.begin(); it!=differentialMap.end(); ++it)
    {
        int dummy = -1;
        int tgt = it->first;
        // Get the list of variable IDs on which this component of the equation system depends
        set<int> srcList = varDependencies[it->first];

        // For each variable src in srcList, create a Delta-1 rule (st1, src) -> (st1, tgt)
        // with weight NameWeight(src,tgt)
        for(set<int>::iterator i = srcList.begin(); i != srcList.end(); i++)
        {
            int src = *i;
            pds->add_rule(st1(), stk(src),st1(),stk(tgt), new NameWeight(src,tgt));
        }
        // Add the Delta-1 rule (st1, dummy) -> (st1, tgt), with weight NameWeight(dummy,tgt)
        pds->add_rule(st1(), stk(dummy), st1(), stk(tgt), new NameWeight(dummy,tgt));
    }
}

/*
 *  newtonLoop_NPATP
 *
 *  This procedure implements Steps (6)-(8) of Algorithm NPA-TP from TR-1825 (page 33)
 *  http://research.cs.wisc.edu/wpis/papers/tr1825.pdf
 *
 *  Perform Newton rounds until a fixed-point is reached
 *
 *  differentialMap - a map of differentials
 *  aList - an initial assignment for the variables
 *  varDependencies - a map from a variable (an reID) to the set of variables it depends on
 *  tensoredRegExpMap - a map from a variable (an reID) to the tensored regular expression
 *                      that defines its value
 *
 *  Author:  Emma Turetsky
 *  
 *  
 *  
 *  newVal = lambda var.bottom
 *  newStarval = empty map
 *  while true {
 *     NEWROUND:
 * oldval = newval
 * clear (newval)
 * oldstarval = newstarval
 * clear (newstarval)
 * 
 * for each variable v { 
 *     evaluete tensored re for v with the side effect of tabulating in newstarval the linear abstraction of each star body
 * }
 * for each star location s {
 *     if oldstarval(s) != newstarval(s)
 *         goto NEWROUND
 * }
 * break
 *  }
 */
#define WIDENING_DELAY 6
// Default values:
#define MAX_ROUNDS_FROM_BELOW 50
#define MAX_ROUNDS_FROM_ABOVE 4
void newtonLoop_NPATP(RTG::assignmentRefPtr & newVal, tslRegExpTMap & tensoredRegExpMap)
{
  int rnd = 0;
  bool newton = true;
  // A map of dependencies
  //RTG::assignmentRefPtr oldVal;
  tslDiffMap::iterator dIt;
  int pV = 0;
  tslRegExpTMap::iterator assignIt;
  
  newStarVal.clear();
  newStarValT.clear();

  RTG::_evalRegExpHash().clear();
  RTG::_evalTHash().clear();

  if (maxRnds < 0) {      //maxRnds is not set yet
      if (aboveBelowMode == NEWTON_FROM_BELOW)
          maxRnds = MAX_ROUNDS_FROM_BELOW;
      else if (aboveBelowMode == NEWTON_FROM_ABOVE)
          maxRnds = MAX_ROUNDS_FROM_ABOVE;
  }
  
  if (newtonVerbosity >= NV_NEWTON_LOOP) {
      // For debugging purposes, print all regular expressions before the first Newton round 
      std::cout << std::endl;
      for (assignIt = tensoredRegExpMap.begin(); assignIt != tensoredRegExpMap.end(); assignIt++)
      {
          std::cout << "Tensored regular expression for reID=" << assignIt->first << ": " << std::endl;
          tsl_regexp::regExpTPrettyPrint(assignIt->second, std::cout); 
          std::cout << std::endl << std::endl;
      }
  }

  int stratificationHeight = 1;
  int propagationRounds = 0;
  if (aboveBelowMode == NEWTON_FROM_BELOW) {
      stratificationHeight = computeStratificationHeight(tensoredRegExpMap);
  }

  // In Newton-from-below mode, we perform Newton rounds until convergence
  //   or until MAX_ROUNDS_FROM_BELOW; in Newton-from-above mode, we always 
  //   go through MAX_ROUNDS_FROM_ABOVE rounds.
  while (true){
NEWROUND:
      if (rnd >= maxRnds) {
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Maximum number of rounds reached. ------------------------------------------" << std::endl;
          }

          if (aboveBelowMode == NEWTON_FROM_BELOW) {
              assert(false && "In Newton-from-below mode, we must abort if we reach the maximum number of rounds.");
          }

          break;
      }

      if (newtonVerbosity >= NV_NEWTON_LOOP) {
          std::cout << "-------------------------------------------------------------------------------" << std::endl;
          std::cout << "Round " << rnd << ":" << std::endl;
      }
      rnd++;
          
      //oldVal = newVal;
      globalAssignment = newVal;
      newVal = CIR::initializeAssignment();
          
      oldStarVal = newStarVal;
      oldStarValT = newStarValT;
      newStarVal.clear();
      newStarValT.clear();

      inNewtonLoop = true;
      if (rnd >= WIDENING_DELAY) {
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Widening will be applied on this round." << std::endl;
          }
          doWideningThisRound = true;
      } else {
          doWideningThisRound = false;
      }

      // For each variable in the equation system, evaluate its tensored regular expression
      for (assignIt = tensoredRegExpMap.begin(); assignIt != tensoredRegExpMap.end(); assignIt++)
      {
          int var = assignIt->first;
          
          // Reevaluate the regular expression associated with variable var,
          //    using oldVal for the quantity \vec{nu} of Algorithm NPA-TP, and
          //    apply detensorTranspose
          //v = Tdetensor(evalT(Mmap[p],oldVal))
          
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Eval: " << assignIt->first << std::endl;
          }
          //assignIt->second.print(std::cout);
          //tsl_regexp::regExpTPrettyPrint(assignIt->second, std::cout);
          //std::cout << std::endl;
          
          //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newValue = evalTNonRec(assignIt->second, oldVal);
          CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newValue = CIR::evalT(assignIt->second);
          
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << std::endl << "Tensored Value: ";
              newValue.v->print(std::cout);
          }
          
          CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr rep = CONC_EXTERNS::detensorTranspose(newValue);
          
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << std::endl << "Detensored Val: ";
              rep.v->print(std::cout);
              std::cout << std::endl;
          }
          
          /*CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ret;
          
          ret = CONC_EXTERNS::evalDotSemElem(rep, CONC_EXTERNS::getOneWt());
          std::cout << std::endl << "After extend one: " << std::endl;
          ret.v->print(std::cout);
          std::cout << std::endl;*/

          // Insert <var,rep> into newVal
          newVal = CIR::updateAssignment(newVal, CBTI_INT32(var), rep);

      }
      
      //EvalMap2.clear();
      //EvalMapT.clear();
      RTG::_evalRegExpHash().clear();
      RTG::_evalTHash().clear();

      if (aboveBelowMode == NEWTON_FROM_ABOVE) 
          goto NEWROUND;

      // std::cout << "Beginning main-loop exit test:" << std::endl;
      // std::cout << "Old untensored star keys are:" << std::endl;
              // for(StarMap::const_iterator oldStar_it = oldStarVal.begin(); oldStar_it != oldStarVal.end(); ++oldStar_it)
      // {
      //  std::cout << "  Hash Key: " << hash_value((*oldStar_it).first) << std::endl;
      //  //std::cout << (*oldStar_it).second << std::endl; // segfault!
      // }
      // std::cout << "New untensored star keys are:" << std::endl;
              // for(StarMap::const_iterator newStar_it = newStarVal.begin(); newStar_it != newStarVal.end(); ++newStar_it)
      // {
      //  std::cout << "  Hash Key: " << hash_value((*newStar_it).first) << std::endl;
      //  //std::cout << (*oldStar_it).second << std::endl; // segfault!
      // }
      // std::cout << "End of untensored map keys." << std::endl;
      // Test to see whether all abstracted Kleene star bodies have converged
      for(StarMap::const_iterator newStar_it = newStarVal.begin(); newStar_it != newStarVal.end(); ++newStar_it) 
      {
          // std::cout << "  Checking an untensored Kleene star." << std::endl;
          StarMap::const_iterator oldStarValue = oldStarVal.find(newStar_it->first);
          if (oldStarValue == oldStarVal.end()) {
              // On the first round, we have no previous value to compare against
              // std::cout << "    Nothing to compare against: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          relation_t oldStarRelation = mkRelation(oldStarValue->second.v);
          relation_t newStarRelation = mkRelation(newStar_it->second.v);
          if (!newStarRelation->Equivalent(oldStarRelation)) {
              // std::cout << "    Inequivalent: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          // std::cout << "  Equivalent." << std::endl;
      }
      for(StarMapT::const_iterator newStar_it = newStarValT.begin(); newStar_it != newStarValT.end(); ++newStar_it) 
      {
          // std::cout << "  Checking a tensored Kleene star." << std::endl;
          StarMapT::const_iterator oldStarValue = oldStarValT.find(newStar_it->first);
          if (oldStarValue == oldStarValT.end()) {
              // On the first round, we have no previous value to compare against
              // std::cout << "    Nothing to compare against: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          relation_t oldStarRelation = mkRelation(oldStarValue->second.v);
          relation_t newStarRelation = mkRelation(newStar_it->second.v);
          if (!newStarRelation->Equivalent(oldStarRelation)) {
              // std::cout << "    Inequivalent: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          // std::cout << "  Equivalent." << std::endl;
      }
      if (propagationRounds >= stratificationHeight) {
          // std::cout << "  All stars are equivalent: exiting loop." << std::endl;
          break; // Exit the Newton loop because all abstracted Kleene star bodies have converged
      } else {
          propagationRounds++;
      }
  }
      
  if (newtonVerbosity >= NV_NEWTON_LOOP) {
      std::cout << std::endl << "NumRnds: " << rnd << std::endl;
  }

  if (testMode) {
      std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
      testFile << "__NUMRNDS " << rnd << std::endl;
      testFile.close();
  }
  
  inNewtonLoop = false;
}

void newtonLoop_GJ(RTG::assignmentRefPtr & newVal, 
                   IRE_Assignment & IREnewAssignment, 
                   tslRegExpMap & regExpsAfterIsolation, 
                   IRERegExpMap & IREregExpsAfterIsolation)
{
  int rnd = 0;
  tslRegExpMap::iterator assignIt;
  
  newStarVal.clear();
  newStarValT.clear();

//#ifdef USE_IRE
//  IREnewStarBodies.clear();
//#endif

  RTG::_evalRegExpHash().clear();
  RTG::_evalTHash().clear();

//#ifdef USE_IRE
//  IRE::clearEvalCaches();
//#endif

  if (maxRnds < 0) {      //maxRnds is not set yet
      if (aboveBelowMode == NEWTON_FROM_BELOW)
          maxRnds = MAX_ROUNDS_FROM_BELOW;
      else if (aboveBelowMode == NEWTON_FROM_ABOVE)
          maxRnds = MAX_ROUNDS_FROM_ABOVE;
  }
  
  if (newtonVerbosity >= NV_NEWTON_LOOP) {
      // For debugging purposes, print all regular expressions before the first Newton round 
      std::cout << "Before first Newton round: " << std::endl << std::endl;
      for (assignIt = regExpsAfterIsolation.begin(); assignIt != regExpsAfterIsolation.end(); assignIt++)
      {
          std::cout << "Regular expression for reID=" << assignIt->first << ": " << std::endl;
          tsl_regexp::regExpPrettyPrint(assignIt->second, std::cout); 
          std::cout << std::endl << std::endl;
      }
  }

  int stratificationHeight = 0;
  int propagationRounds = 0;
  //if (aboveBelowMode == NEWTON_FROM_BELOW) {
  //    stratificationHeight = computeStratificationHeight(tensoredRegExpMap);
  //}

  // In Newton-from-below mode, we perform Newton rounds until convergence
  //   or until MAX_ROUNDS_FROM_BELOW; in Newton-from-above mode, we always 
  //   go through MAX_ROUNDS_FROM_ABOVE rounds.
  while (true){
NEWROUND:
      if (rnd >= maxRnds) {
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Maximum number of rounds reached. ------------------------------------------" << std::endl;
          }

          if (aboveBelowMode == NEWTON_FROM_BELOW) {
              assert(false && "In Newton-from-below mode, we must abort if we reach the maximum number of rounds.");
          }

          break;
      }

      if (newtonVerbosity >= NV_NEWTON_LOOP) {
          std::cout << "-------------------------------------------------------------------------------" << std::endl;
          std::cout << "Round " << rnd << ":" << std::endl;
      }
      rnd++;
          
          
      //oldVal = newVal;
      globalAssignment = newVal;
      newVal = CIR::initializeAssignment();

//#ifdef USE_IRE
//      IREglobalAssignment = IREnewAssignment;
//      IREnewAssignment.clear();
//#endif
          
      oldStarVal = newStarVal;
      oldStarValT = newStarValT;
      newStarVal.clear();
      newStarValT.clear();

//#ifdef USE_IRE
//      IREoldStarBodies = IREnewStarBodies;
//      IREnewStarBodies.clear();
//#endif

      RTG::_evalRegExpHash().clear(); // Moved by Jason on 2018-04-21
      RTG::_evalTHash().clear();      // Moved by Jason on 2018-04-21

//#ifdef USE_IRE
//      IRE::clearEvalCaches();
//#endif

      inNewtonLoop = true;
      if (rnd >= WIDENING_DELAY) {
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Widening will be applied on this round." << std::endl;
          }
          doWideningThisRound = true;
      } else {
          doWideningThisRound = false;
      }

      if (newtonVerbosity >= NV_NEWTON_LOOP) {
          std::cout << std::endl << "Here is the current assignment of values to variables:" << std::endl;
          for (assignIt = regExpsAfterIsolation.begin(); assignIt != regExpsAfterIsolation.end(); assignIt++)
          {
              int var = assignIt->first;
              std::cout << "Value of variable " << assignIt->first << " is: " << std::endl;
              CIR::getAssignment(var, globalAssignment).print(std::cout);
              std::cout << std::endl << std::endl;

              std::cout << "Value of variable " << assignIt->first << " in the IRE assignment is: " << std::endl;
              if (IREglobalAssignment.count(var) > 0) {
                 IREglobalAssignment[var]->print(std::cout);
              } else { std::cout << "[Zero]" << std::endl; }
              std::cout << std::endl << std::endl;
          }
          std::cout << std::endl;
      }

      // For each variable in the equation system, evaluate its regular expression
      for (assignIt = regExpsAfterIsolation.begin(); assignIt != regExpsAfterIsolation.end(); assignIt++)
      {
          int var = assignIt->first;
          
          // Reevaluate the regular expression associated with variable var,
          //    using oldVal for the quantity \vec{nu} of Algorithm NPA-TP, and
          //    apply detensorTranspose
          //v = Tdetensor(evalT(Mmap[p],oldVal))
          
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "Eval: " << assignIt->first << std::endl;
          }
          //assignIt->second.print(std::cout);
          
          //std::cout << "\n  The regular expression for " << assignIt->first << " is: \n" << std::endl;
          //tsl_regexp::regExpPrettyPrint(assignIt->second, std::cout);
          //std::cout << std::endl;
          
          //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newValue = evalTNonRec(assignIt->second, oldVal);
          //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newValue = CIR::evalT(assignIt->second);
          CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newValue = CIR::evalRegExp(assignIt->second);

//#ifdef USE_IRE
//          if (newtonVerbosity >= NV_NEWTON_LOOP) {
//              std::cout << "Eval-IRE: " << assignIt->first << std::endl;
//          }
//          relation_t IREnewValue = IREregExpsAfterIsolation[var]->eval();
//#endif          
          ////////std::cout << std::endl << "Tensored Value: ";
          ////////newValue.v->print(std::cout);
          ////////
          ////////CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr rep = CONC_EXTERNS::detensorTranspose(newValue);
          ////////
          ////////std::cout << std::endl << "Detensored Val: ";
          if (newtonVerbosity >= NV_NEWTON_LOOP) {
              std::cout << "\n  The value on this round is: \n\n";
              newValue.v->print(std::cout);
              std::cout << std::endl << std::endl;

//#ifdef USE_IRE
//              std::cout << "\n  The IRE-evaluated value on this round is: \n\n";
//              IREnewValue->print(std::cout); 
//              std::cout << std::endl << std::endl;
//#endif
          }
          
          /*CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr ret;
          
          ret = CONC_EXTERNS::evalDotSemElem(rep, CONC_EXTERNS::getOneWt());
          std::cout << std::endl << "After extend one: " << std::endl;
          ret.v->print(std::cout);
          std::cout << std::endl;*/

          // Insert <var,newValue> into newVal
          newVal = CIR::updateAssignment(newVal, CBTI_INT32(var), newValue);
          //newVal = CIR::updateAssignment(newVal, CBTI_INT32(var), rep);
          
//#ifdef USE_IRE
//          IREnewAssignment[var] = IREnewValue;
//#endif
      }
      
      //EvalMap2.clear();
      //EvalMapT.clear();
      
      //RTG::_evalRegExpHash().clear(); // Commented out by Jason on 2018-04-21
      //RTG::_evalTHash().clear();      // Commented out by Jason on 2018-04-21

      if (aboveBelowMode == NEWTON_FROM_ABOVE) 
          goto NEWROUND;

//#ifdef USE_IRE
//      // Here is the main loop termination condition
//      if (newtonVerbosity >= NV_NEWTON_LOOP) {
//          std::cout << "    (IRE-tc) Checking termination condition." << std::endl;
//      }
//      for(IRE::StarMap::const_iterator starBody_it = IREnewStarBodies.begin(); starBody_it != IREnewStarBodies.end(); ++starBody_it) 
//      {
//          IRE::StarMap::const_iterator oldStarBody = IREoldStarBodies.find(starBody_it->first);
//          if (oldStarBody == IREoldStarBodies.end()) {
//              if (newtonVerbosity >= NV_NEWTON_LOOP) {
//                  std::cout << "    (IRE-tc)   >> First round; nothing to compare against; continuing loop." << std::endl;
//              }
//              ////goto NEWROUND;
//          } else {
//              if (!(starBody_it->second->Equivalent(oldStarBody->second))) {
//                  if (newtonVerbosity >= NV_NEWTON_LOOP) {
//                      std::cout << "    (IRE-tc)   >> Inequivalent; continuing loop." << std::endl;
//                  }
//                  ////goto NEWROUND;
//              } else {
//                  if (newtonVerbosity >= NV_NEWTON_LOOP) {
//                      std::cout << "    (IRE-tc)   >> Equivalent." << std::endl;
//                  }
//              }
//          }
//      }
//#endif

      // std::cout << "Beginning main-loop exit test:" << std::endl;
      // std::cout << "Old untensored star keys are:" << std::endl;
              // for(StarMap::const_iterator oldStar_it = oldStarVal.begin(); oldStar_it != oldStarVal.end(); ++oldStar_it)
      // {
      //  std::cout << "  Hash Key: " << hash_value((*oldStar_it).first) << std::endl;
      //  //std::cout << (*oldStar_it).second << std::endl; // segfault!
      // }
      // std::cout << "New untensored star keys are:" << std::endl;
              // for(StarMap::const_iterator newStar_it = newStarVal.begin(); newStar_it != newStarVal.end(); ++newStar_it)
      // {
      //  std::cout << "  Hash Key: " << hash_value((*newStar_it).first) << std::endl;
      //  //std::cout << (*oldStar_it).second << std::endl; // segfault!
      // }
      // std::cout << "End of untensored map keys." << std::endl;
      // Test to see whether all abstracted Kleene star bodies have converged
      for(StarMap::const_iterator newStar_it = newStarVal.begin(); newStar_it != newStarVal.end(); ++newStar_it) 
      {
          // std::cout << "  Checking an untensored Kleene star." << std::endl;
          StarMap::const_iterator oldStarValue = oldStarVal.find(newStar_it->first);
          if (oldStarValue == oldStarVal.end()) {
              // On the first round, we have no previous value to compare against
              // std::cout << "    Nothing to compare against: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          relation_t oldStarRelation = mkRelation(oldStarValue->second.v);
          relation_t newStarRelation = mkRelation(newStar_it->second.v);
          if (!newStarRelation->Equivalent(oldStarRelation)) {
              // std::cout << "    Inequivalent: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          // std::cout << "  Equivalent." << std::endl;
      }
      for(StarMapT::const_iterator newStar_it = newStarValT.begin(); newStar_it != newStarValT.end(); ++newStar_it) 
      {
          //std::cout << "  Checking a tensored Kleene star." << std::endl;
          //std::cout << "  Its new value is: " << std::endl;
          //newStar_it->second.v->printAbstract(std::cout);

          StarMapT::const_iterator oldStarValue = oldStarValT.find(newStar_it->first);
          if (oldStarValue == oldStarValT.end()) {
              // On the first round, we have no previous value to compare against
              // std::cout << "    Nothing to compare against: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          //std::cout << "\n  Its old value was: " << std::endl;
          //oldStarValue->second.v->printAbstract(std::cout);
          relation_t oldStarRelation = mkRelation(oldStarValue->second.v);
          relation_t newStarRelation = mkRelation(newStar_it->second.v);
          if (!newStarRelation->Equivalent(oldStarRelation)) {
              //std::cout << "\n    Inequivalent: continuing loop." << std::endl;
              propagationRounds = 0;
              goto NEWROUND;
          }
          //std::cout << "\n  Equivalent." << std::endl;
      }
      if (propagationRounds >= stratificationHeight) {
          //std::cout << "\n  All stars are equivalent: exiting loop." << std::endl;
          break; // Exit the Newton loop because all abstracted Kleene star bodies have converged
      } else {
          propagationRounds++;
      }
  }
      
  if (newtonVerbosity >= NV_NEWTON_LOOP) {
      std::cout << std::endl << "NumRnds: " << rnd << std::endl;
  }

  if (testMode) {
      std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
      testFile << "__NUMRNDS " << rnd << std::endl;
      testFile.close();
  }
  
  inNewtonLoop = false;
}









/*
 *  doNewtonSteps_NPATP
 *
 *  This procedure implements Algorithm NPA-TP from TR-1825 (page 33)
 *  http://research.cs.wisc.edu/wpis/papers/tr1825.pdf
 *
 *  Step 1 - Convert the program 'pg' into an fpds, whose rules
 *           encode the edges of the program's ICFG in the standard manner, e.g., see
 *           Fig. 3 of http://research.cs.wisc.edu/wpis/papers/fsttcs07.invited.pdf.
 *           A rule's weight represents the abstract transformer (in some abstract domain)
 *           of the corresponding ICFG edge.  The merge function on a call-rule (a.k.a.
 *           Delta-2 rule) must obey the properties given in Section 8 of TR-1825.
 *
 *  Step 2 - Apply Tarjan's path-expression algorithm to the CFG of each procedure of pg.
 *           Method: Perform poststar on the fpds to obtain the regular expressions associated with
 *           the outgoing nodes in the intra_graph associated with the fwpds.
 *           Each such node represents one procedure of pg.  Taken together, the
 *           regular expressions represent the equation system E = {Xj = Rhs_j(vec{X}) | Xj in vec{X}}.
 *
 *  Step 3 - Convert these regexps into TSL regular expressions and obtain the differentials
 *           with respect to the variables
 *
 *  Step 4 - Create a new fwpds using these differentials - run poststar on the fwpds in
 *           order to get new regular expressions representing the return points for Newton
 *
 *  Step 5 - Use the reg exps representing the differentials, the partial differentials, and the original
 *           tsl regular expressions to get the tensored regular expressions we need to run the newton rounds
 *
 *  Step 6 - Perform Newton rounds until a fixed-point is reached
 *
 *  Step 7 - Insert the new weights into the original outfa and perform the iterative path summary
 *           to determine if the error weight is reachable [Vestigial/optional]
 *
 *  Notes on the handling of Project and ProjectT:
 *  ---------------------------------------------
 *  The implementation plays a bit fast and loose with Project and ProjectT:
 *  o The original WALi regular expressions have no occurrences of Project
 *  o In Step 3
 *    - When the original WALi regular expressions are converted to TSL
 *      regular expressions, occurrences of Project are inserted (search
 *      for "Project::make")
 *    - However, when the differentials of the TSL regular expressions are created,
 *      all occurrences of Project are eliminated
 *  o In Step 5, we have a new set of regular expressions over nameWeights.
 *    A nameWeight corresponds to a right-hand-side constant in the Z equation system;
 *    it represents a regular expression over nu^(i) variables, and thus is a
 *    constant for a given round i, but varies from round to round. When these
 *    regular expressions are converted into tensored TSL regular expressions
 *    (convertToRegExpT), each regular expression r for a given nameWeight is
 *    turned into ProjectT(r) (search for "ProjectT::make").
 *
 *  Step 5 is justified because Lemma 8.9(2) allows all right-hand-side
 *  occurrences of ProjectT in the Z system of equations to be pushed down to the
 *  "constants" (see Eqn. (74)). The lemma ensures that the value computed for
 *  each Z_k is the same as it would have if ProjectT were applied to the
 *  entire right-hand side of Z_k's defining equation. By Theorems 8.5 and 8.14
 *  (also 8.13), the values correspond to the solution that we want.
 *  
 *  It may be worrisome that we have left out the occurrences of Project
 *  at occurrences of nu^(i) -- that is, because occurrences of Project
 *  were filtered out in Step 3, we have occurrences of nu^(i) instead of Project(nu^(i)).
 *  However, because the solution computed on each round for the Z equation system
 *  is equivalent to one with Project_T applied to each right-hand side, after
 *  we apply TDeTensor to obtain nu^(i+1), we have values equal to ones
 *  on which Project has been applied. (The paper does not have the following lemma
 *
 *     for all a in S_T, Project(TDeTensor(a)) = TDeTensor(Project_T(a)),
 *
 *  however, there is a proof of this lemma in TWR's notebook from 12/3/2015,
 *  orange notebook, p. 91.)
 *
 *  Author:  Emma Turetsky
 */

double doNewtonSteps_NPATP(WFA& outfa, wali::Key entry_key, FWPDS * originalPds = NULL, bool canPrune = true)
{ 
  globalAssignment = CIR::initializeAssignment(); // Create an assignment in which all variables are map to zero

  if (newtonVerbosity >= NV_NEWTON_STEPS) {
      cout << "#################################################################" << endl;
      cout << " Beginning Interprocedural Anaylsis (with regexp=TSL/without GJ) " << endl;
      if (aboveBelowMode == NEWTON_FROM_ABOVE) {
          cout << "     mode=NEWTON_FROM_ABOVE " << endl;
      }
      cout << "Step 1: =========================================================" << endl;
  }
  //if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "#################################################" << endl;
  //cout << "[Newton] Goal VIII: end-to-end newton_merge_notensor_fwpds run" << endl;

  
  //if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 1: =========================================================" << endl;
  //Step 1 - Convert the program 'pg' into an fpds where the weights are nwaobdds.
  FWPDS * fpds;
  if(originalPds != NULL)
    fpds = new FWPDS(*originalPds);
  else{
    assert(0 && "doNewtonSteps should not be called with originalPds == NULL");
    //fpds = new FWPDS();
    //con = pds_from_prog_with_newton_merge(fpds, pg);// ncon = pds_from_prog_nwa(fpds, pg);
  }

  //freopen("newtonMinTest.txt", "w", stdout);
  //fpds->print(std::cout) << endl;

  int dummy = -1;
  map<int, reg_exp_t> outNodeRegExpMap; // A map from a unique id of an outgoing node to the regular expression associated with it
  map<int, reg_exp_t> rNew; // A map from the node ids to the regexps representing the full differentials associated with that node id
  map<int, int> updateableMap;  //A map from an updateable node number to the id of the node it depends on 
  map<int, int> oMap;
  map<int, pair< pair<int, int>, int> > mapBack;  //A map from the node index to the struct of the <<src,tgt>,stack> - used to insert weights back into wfa
  map<pair<pair<int, int>, int>, int> transMap;
  vector<int> differentiatedList; //A list of nodes with where the differential is to be taken
  tslRegExpMap regExpMap;  //The map of all tsl regular expression
  tslRegExpMap diffMap;  //The map of tsl regular expressions to be differentiated (the program return points)
  tslDiffMap differentialMap;  //This is a map from regexp ids to the partial differentials assoicated with them
  tslRegExpTMap tensoredRegExpMap;  //A map from the regexpId to the tsl tensored differential representing it
  map<int, relation_t> finWeights;  //The map from node id to the final relational weights
  map<std::pair<int, int>, std::pair<int, int> > mergeSrcMap; //The map that keeps track of the src of calls on call instructions
  std::vector<int> wl;
  std::set<int> vl;
  double baseEvalTime = 0;
  map<int, std::set<int> > varDependencies;
  TdiffMap = TDiffHashMap();
  
  RTG::assignmentRefPtr aList;

  wali::set_verify_fwpds(false);
  
  fpds->useNewton(false);
  //fpds->add_rule(st1(),getKey(mainProc),st1(),fpds->get_theZero()->one());


  wali::util::GoodTimer * t = new wali::util::GoodTimer("temp");
  WFA fa;
  wali::Key acc = wali::getKeySpace()->getKey("accept");
  sem_elem_t temp = fpds->get_theZero()->one();
  fa.addTrans(getPdsState(),entry_key, acc, fpds->get_theZero()->one());
  fa.setInitialState(getPdsState());
  fa.addFinalState(acc);


  if (dump){
      fstream outfile("init_fa.dot", fstream::out);
      fa.print_dot(outfile, true);
      outfile.close();
  }
  if (dump){
      fstream outfile("init_fa.txt", fstream::out);
      fa.print(outfile);
      outfile.close();
  }

  if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 2: =========================================================" << endl;
  /* Step 2 - Perform poststar on the fpds get the regular expressions associated with
  *           the outgoing nodes in the intra_graph associated with the fwpds
  */
  // This function performs poststar on fpds w.r.t. fa, and populates the maps as described above
  // The Boolean indicates whether this is the first time the function is called and will generate unique ids as needed
  double curTime = t->total_time();
  t->stop();
  double t1 = fpds->getOutRegExps(fa, outfa, outNodeRegExpMap, updateableMap, oMap, mapBack, transMap, differentiatedList, mergeSrcMap);
  t->start();
  int testSize = mapBack.size();
  int testSize2 = transMap.size();

  // Find the key to the error state
  Key errKey = wali::getKeySpace()->getKey("error");
  Key iKey = wali::getKeySpace()->getKey("Unique State Name");
  Key pErrKey = wali::getKeySpace()->getKey(iKey, errKey);
  GenKeySource * esrc = new wali::wpds::GenKeySource(1, pErrKey);
  Key fKey = wali::getKeySpace()->getKey(esrc);
  bool prune = false;

  // If the error state is in the wfa, then prune, otherwise the error is not reachable  -- ETTODO (prune check is redundant)
  if (outfa.getState(fKey) != NULL)
  {
      prune = true;
  }
  if (prune || (!canPrune))
  {
              if(prune)
              {
      //Set the initial state to be the error state in the wfa and then prune
      outfa.setInitialState(fKey);
      outfa.prune();
      }
      t->stop();
      std::set<Key> faStates = outfa.getStates();
      std::set<Key>::iterator stateIter;
      int numTrans = 0;

      //Get the set of transitions on the remaining states and put that into the worklist
      for (stateIter = faStates.begin(); stateIter != faStates.end(); stateIter++)
      {
          TransSet & transSet = outfa.getState(*stateIter)->getTransSet();
          TransSet::iterator transIt;
          for (transIt = transSet.begin(); transIt != transSet.end(); transIt++)
          {
              ITrans * tr = *transIt;
              int tSrc = tr->from();
              int tTgt = tr->to();
              int tStack = tr->stack();
              int transReg = transMap[std::make_pair(std::make_pair(tSrc, tTgt), tStack)];
              wl.push_back(transReg);
              vl.insert(transReg);
              numTrans++;
          }
      }
      if (dump){
          cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
          fstream outfile("inter_outfa.dot", fstream::out);
          outfa.print_dot(outfile, true);
          outfile.close();
      } if (dump){
          cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
          fstream outfile("inter_outfa.txt", fstream::out);
          outfa.print(outfile);
          outfile.close();
      }

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 3: =========================================================" << endl;
      /*Step 3 - Convert these regexps into TSL regular expressions and get the partial differentials
      *          with respect their variables
      */

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "[Newton] converting to TSL" << endl;
      while (!wl.empty())
      {
          int wlSzie = wl.size();
          //convertToTSLRegExp
          //if in conversion an updatable node is hit
          //add that regexp to wl if not already in visitedList
          int rToConvert = wl.back();
          //std::cout << rToConvert << endl;
          wl.pop_back();
          baseEvalTime += convertToTSLRegExps(rToConvert, outNodeRegExpMap, regExpMap, varDependencies, updateableMap, oMap, mapBack, mergeSrcMap, wl, vl, false);
      }
      //std::cout << "ESIZE: " << E.size() << std::endl;
      //std::cout << "DSIZE: " << differentiatedList.size() << std::endl;

      // Create a map from unique IDs to tsl regular expressions with variables
      // Currently doing this by iterating through the regular expressions and copying
      // the tsl regular expression whose ids match the nodes in the differentiatedMap
      for (vector<int>::iterator eit = differentiatedList.begin(); eit != differentiatedList.end(); eit++)
      {
          // std::cout << "D: " << *eit << endl;
              RTG::regExpRefPtr tmpReg = regExpMap[(*eit)];
              if (tmpReg != NULL)
              {
                  diffMap[(*eit)] = regExpMap[(*eit)];
                  //tsl_regexp::regExpPrettyPrint(tmpReg, std::cout);
              }
          //  std::cout << "RE: " << *eit;
          //  std::cout << " ";
          //  E[(*eit)].print(std::cout) << std::endl;
          // This is used in debugging to compare the epsilon transitions with those generated by non-newton methods
          // to make sure they match
#if defined(NEWTON_DEBUG)
          int src = mapBack[(*eit)].first.first;
          int tgt = mapBack[(*eit)].first.second;
          int stack = mapBack[(*eit)].second;
          globalEpsilon.insert(std::make_pair(std::make_pair(src,tgt),stack));
#endif
      }
      t->start();

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << std::endl << "[Newton] Creating Differentials" << std::endl;
      //Created the differentials
      double t2 = 0;
      if (diffMap.size() != 0)
      {
          bool linear = createDifferentials(diffMap, differentialMap);
          if (linear)
          {
              std::cout << "linear";
              std::cout << std::endl;
          }
          else
          {
              std::cout << "NonLin";
              std::cout << std::endl;
          }
          tslDiffMap::iterator it3;

          if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 4: =========================================================" << endl;
          /* Step 4 - Create a new fwpds using these partial differentials - run poststar on the fwpds in
          *           order to get the full differentials representing the values of the program return points
          */
          //Now we create new fwpds using these differentials, this fwpds has the weight type of NameWeight
          FWPDS * fnew = new FWPDS();
          fwpdsFromDifferential(fnew, differentialMap, varDependencies);

          //Now create another finite automaton
          WFA fa2;
          wali::Key acc2 = wali::getKeySpace()->getKey("accept2");
          fa2.addTrans(getPdsState(), stk(dummy), acc2, fnew->get_theZero()->one());
          fa2.setInitialState(getPdsState());
          fa2.addFinalState(acc2);
          WFA outfa2;

          //Get the regexps generated by running poststar on the new fwpds, these are the ones we will use in the Newton Round
          t->stop();
          t2 = fnew->getOutRegExpsSimple(fa2, outfa2, rNew);
          t->start();
          if (dump){
              cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
              fstream outfile("inter_outfa2.dot", fstream::out);
              outfa2.print_dot(outfile, true);
              outfile.close();
          } if (dump){
              cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
              fstream outfile("inter_outfa2.txt", fstream::out);
              outfa2.print(outfile);
              outfile.close();
          }

          if (dump){
              cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
              fstream outfile("fa2.dot", fstream::out);
              fa2.print_dot(outfile, true);
              outfile.close();
          } if (dump){
              cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
              fstream outfile("fa2.txt", fstream::out);
              fa2.print(outfile);
              outfile.close();
          }

          if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 5: =========================================================" << endl;
          /* Step 5 - Use the reg exps representing the differentials, the partial differentials, and the original
          *           tsl regular expressions to get the tensored regular expressions we need to run the newton rounds
          */
          convertToTSLRegExpsT(rNew, regExpMap, tensoredRegExpMap, differentialMap, mapBack, mergeSrcMap);
          
          if (aboveBelowMode == NEWTON_FROM_BELOW) {
              aList = CIR::initializeAssignment();
              
              for (tslRegExpMap::iterator it = regExpMap.begin(); it != regExpMap.end(); ++it)
              {
                  RTG::regExpRefPtr r = it->second;
                  //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = evalNonRecAt0(r);
                  CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = CIR::evalRegExp(r);

                  // Insert <it->first,newVal> into aList
                  aList = CIR::updateAssignment(aList, CBTI_INT32(it->first), newVal);
              }
          } else if (aboveBelowMode == NEWTON_FROM_ABOVE) {
              CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr topVal = mkSemElemWrapper(Relation::getBaseTop());
              aList = CIR::mkConstantAssignment(topVal);
          } else {
              assert(false && "Unrecognized running mode.");
          }

          if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 6: =========================================================" << endl;
          /* Step 6 - Perform Newton rounds until a fixed-point is reached */
          if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "[Newton] Running Newton" << endl;
          newtonLoop_NPATP(aList, tensoredRegExpMap);

          //Using the final weights from Newton, evaluate the tslRegExps to get the final weights
          //evalRegExps(aList);


          /* Step 7 - Insert the new weights into the original outfa and perform the iterative path summary in order to
          *           determine if the error weight is reachable
          */
      }
      else  //There are no variables to be differentiated
      {
          std::cout << "NonRec";
          std::cout << std::endl;
          aList = CIR::initializeAssignment();
          //evalRegExps(aList);
          t2 = 0;
          if (testMode) {
              std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
              testFile << "__NUMRNDS 0" << std::endl;
              testFile.close();
          }   
      }
      t->stop();

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 7: =========================================================" << endl;
      globalAssignment = aList;
      //Map the evaluated weights back to the transitions the regexps came from
      for (stateIter = faStates.begin(); stateIter != faStates.end(); stateIter++)
      {
          TransSet & transSet = outfa.getState(*stateIter)->getTransSet();
          TransSet::iterator transIt;
          for (transIt = transSet.begin(); transIt != transSet.end(); transIt++)
          {
              ITrans * tt = *transIt;
              int tSrc = tt->from();
              int tTgt = tt->to();
              int tStack = tt->stack();
              int transReg = transMap[std::make_pair(std::make_pair(tSrc, tTgt), tStack)];
              t->start();
              //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr w = evalRegExpFinNonRec(regExpMap[transReg], aList);
              CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr w = CIR::evalRegExp(regExpMap[transReg]);
              t->stop();
              //std::cout << "OutWeight: " << tSrc << "," << tTgt << "," << tStack << ":";
              //w.v->print(std::cout) << endl;
              tt->setWeight(w.v);
          }
      }
      /*cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
      fstream outfile("newton_outfa.dot", fstream::out);
      outfa.print_dot(outfile, true);
      outfile.close();*/

      //Perform the final path summary
      t->start();
      outfa.path_summary_tarjan_fwpds(true, true);
      //State * initS = outfa.getState(outfa.getInitialState());
      //if (initS == NULL)
      //{
      //  cout << "[Newton] FWPDS ==> error not reachable" << endl;
      //}
      //else
      //{
      //  sem_elem_t fWt = outfa.getState(outfa.getInitialState())->weight();
      //  if (fWt->equal(fWt->zero()))
      //  {
      //      cout << "[Newton] FWPDS ==> error not reachable" << endl;
      //  }
      //  else{
      //      cout << "[Newton] FWPDS ==> error reachable" << endl;
      //  }
      //}

      t->stop();
      double tTime = t->total_time() + t1 + t2 + baseEvalTime;
      if (newtonVerbosity >= NV_NEWTON_STEPS) {
          std::cout << std::endl << "[Newton] Time taken by: Newton: " << std::endl << std::endl;
          cout << tTime << endl;
      }
      
      if (testMode) {
          std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
          testFile << "__TIME " << tTime << std::endl;
          testFile.close();
      }
      
      return tTime;
  }
  else  //There is no error state
  {
      cout << "[Newton] FWPDS ==> error not reachable" << endl;
      double tTime = t->total_time() + t1;;
      std::cout << "[Newton] Time taken by: Newton: ";
      cout << tTime << endl;
      std::cout << "NonRec";
      std::cout << std::endl;
      
      if (testMode) {
          std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
          testFile << "__TIME " << tTime << std::endl;
          testFile.close();
      }
      
      return tTime;
  }
}

double doNewtonSteps_GJ(WFA& outfa, wali::Key entry_key, FWPDS * originalPds = NULL, bool canPrune = true)
{ 
  if (newtonVerbosity >= NV_NEWTON_STEPS) {
      cout << "#################################################################" << endl;
      cout << "           Beginning Interprocedural Anaylsis (with regexp=TSL)  " << endl;
      cout << "Step 1: =========================================================" << endl;
  }
  globalAssignment = CIR::initializeAssignment(); // Create an assignment in which all variables are map to zero
  //Step 1 - Convert the program 'pg' into an fpds where the weights are nwaobdds.
  FWPDS * fpds;
  if(originalPds != NULL)
    fpds = new FWPDS(*originalPds);
  else{
    assert(0 && "doNewtonSteps should not be called with originalPds == NULL");
    //fpds = new FWPDS();
    //con = pds_from_prog_with_newton_merge(fpds, pg);// ncon = pds_from_prog_nwa(fpds, pg);
  }

  //freopen("newtonMinTest.txt", "w", stdout);
  //fpds->print(std::cout) << endl;

  int dummy = -1;
  map<int, reg_exp_t> outNodeRegExpMap; // A map from a unique id of an outgoing node to the regular expression associated with it
  map<int, reg_exp_t> rNew; // A map from the node ids to the regexps representing the full differentials associated with that node id
  map<int, int> updateableMap;  //A map from an updateable node number to the id of the node it depends on 
  map<int, int> oMap;
  map<int, pair< pair<int, int>, int> > mapBack;  //A map from the node index to the struct of the <<src,tgt>,stack> - used to insert weights back into wfa
  map<pair<pair<int, int>, int>, int> transMap;
  vector<int> variableIDs; //A list of nodes with where the differential is to be taken

  IRERegExpMap IREregExpMap;  //The map of all IRE regular expressions

  tslRegExpMap regExpMap;  //The map of all tsl regular expression
  tslRegExpMap diffMap;  //The map of tsl regular expressions to be differentiated (the program return points)
  tslDiffMap differentialMap;  //This is a map from regexp ids to the partial differentials assoicated with them
  tslRegExpTMap tensoredRegExpMap;  //A map from the regexpId to the tsl tensored differential representing it
  map<int, relation_t> finWeights;  //The map from node id to the final relational weights
  map<std::pair<int, int>, std::pair<int, int> > mergeSrcMap; //The map that keeps track of the src of calls on call instructions
  std::vector<int> wl;
  std::set<int> vl;
  double baseEvalTime = 0;
  map<int, std::set<int> > varDependencies;
  TdiffMap = TDiffHashMap();
  
  RTG::assignmentRefPtr aList;

  IRE_Assignment IREaList;

  wali::set_verify_fwpds(false);
  
  fpds->useNewton(false);
  //fpds->add_rule(st1(),getKey(mainProc),st1(),fpds->get_theZero()->one());


  wali::util::GoodTimer * t = new wali::util::GoodTimer("temp");
  WFA fa;
  wali::Key acc = wali::getKeySpace()->getKey("accept");
  sem_elem_t temp = fpds->get_theZero()->one();
  fa.addTrans(getPdsState(),entry_key, acc, fpds->get_theZero()->one());
  fa.setInitialState(getPdsState());
  fa.addFinalState(acc);


  if (dump){
      fstream outfile("init_fa.dot", fstream::out);
      fa.print_dot(outfile, true);
      outfile.close();
  }
  if (dump){
      fstream outfile("init_fa.txt", fstream::out);
      fa.print(outfile);
      outfile.close();
  }

  if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 2: =========================================================" << endl;
  /* Step 2 - Perform poststar on the fpds get the regular expressions associated with
  *           the outgoing nodes in the intra_graph associated with the fwpds
  */
  // This function performs poststar on fpds w.r.t. fa, and populates the maps as described above
  // The Boolean indicates whether this is the first time the function is called and will generate unique ids as needed
  double curTime = t->total_time();
  t->stop();
  double t1 = fpds->getOutRegExps(fa, outfa, outNodeRegExpMap, updateableMap, oMap, mapBack, transMap, variableIDs, mergeSrcMap);
  t->start();
  int testSize = mapBack.size();
  int testSize2 = transMap.size();

  // Find the key to the error state
  Key errKey = wali::getKeySpace()->getKey("error");
  Key iKey = wali::getKeySpace()->getKey("Unique State Name");
  Key pErrKey = wali::getKeySpace()->getKey(iKey, errKey);
  GenKeySource * esrc = new wali::wpds::GenKeySource(1, pErrKey);
  Key fKey = wali::getKeySpace()->getKey(esrc);
  bool prune = false;

  // If the error state is in the wfa, then prune, otherwise the error is not reachable  -- ETTODO (prune check is redundant)
  if (outfa.getState(fKey) != NULL)
  {
      prune = true;
  }
  if (prune || (!canPrune))
  {
              if(prune)
              {
      //Set the initial state to be the error state in the wfa and then prune
      outfa.setInitialState(fKey);
      outfa.prune();
      }
      t->stop();
      std::set<Key> faStates = outfa.getStates();
      std::set<Key>::iterator stateIter;
      int numTrans = 0;

      //Get the set of transitions on the remaining states and put that into the worklist
      for (stateIter = faStates.begin(); stateIter != faStates.end(); stateIter++)
      {
          TransSet & transSet = outfa.getState(*stateIter)->getTransSet();
          TransSet::iterator transIt;
          for (transIt = transSet.begin(); transIt != transSet.end(); transIt++)
          {
              ITrans * tr = *transIt;
              int tSrc = tr->from();
              int tTgt = tr->to();
              int tStack = tr->stack();
              int transReg = transMap[std::make_pair(std::make_pair(tSrc, tTgt), tStack)];
              wl.push_back(transReg);
              vl.insert(transReg);
              numTrans++;
          }
      }
      //if (dump){
      //  cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
      //  fstream outfile("inter_outfa.dot", fstream::out);
      //  outfa.print_dot(outfile, true);
      //  outfile.close();
      //} if (dump){
      //  cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
      //  fstream outfile("inter_outfa.txt", fstream::out);
      //  outfa.print(outfile);
      //  outfile.close();
      //}

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 3: =========================================================" << endl;
      /*Step 3 - Convert these regexps into TSL regular expressions and get the partial differentials
      *          with respect their variables
      */

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "[Newton] converting to TSL" << endl;
      while (!wl.empty())
      {
          int wlSzie = wl.size();
          //convertToTSLRegExp
          //if in conversion an updatable node is hit
          //add that regexp to wl if not already in visitedList
          int rToConvert = wl.back();
          //std::cout << rToConvert << endl;
          wl.pop_back();

          bool insertProjects = false;
//#ifdef USE_IRE
//// XXX HERE XXX Remove the next line!
//          IREregExpMap[rToConvert] = convertToICRARegExp(outNodeRegExpMap[rToConvert], updateableMap, oMap, insertProjects);
//// XXX HERE XXX
//#endif
          // This converted regular expression goes into regExpMap
          //bool insertProjects = true;
          baseEvalTime += convertToTSLRegExps(rToConvert, outNodeRegExpMap, regExpMap, varDependencies, updateableMap, oMap, mapBack, mergeSrcMap, wl, vl, insertProjects);

#ifdef USE_IRE
          IREregExpMap[rToConvert] = IREregExpConversionMap[outNodeRegExpMap[rToConvert]];
#endif

          //if (newtonVerbosity >= NV_EVERYTHING) {
          //    cout << "  There are " << wl.size() << " regular expressions remaining to be converted." << endl;
          //}
      }
      //std::cout << "ESIZE: " << E.size() << std::endl;
      //std::cout << "DSIZE: " << variableIDs.size() << std::endl;

      //// Create a map from unique IDs to tsl regular expressions with variables
      //// Currently doing this by iterating through the regular expressions and copying
      //// the tsl regular expression whose ids match the nodes in the differentiatedMap
      //for (vector<int>::iterator eit = variableIDs.begin(); eit != variableIDs.end(); eit++)
      //{
      //    // std::cout << "D: " << *eit << endl;
      //        RTG::regExpRefPtr tmpReg = regExpMap[(*eit)];
      //        if (tmpReg != NULL)
      //        {
      //            diffMap[(*eit)] = regExpMap[(*eit)];
      //              //tsl_regexp::regExpPrettyPrint(tmpReg, std::cout);
      //        }
      //    //  std::cout << "RE: " << *eit;
      //    //  std::cout << " ";
      //    //  E[(*eit)].print(std::cout) << std::endl;
      //    // This is used in debugging to compare the epsilon transitions with those generated by non-newton methods
      //    // to make sure they match
      //#if defined(NEWTON_DEBUG)
      //    int src = mapBack[(*eit)].first.first;
      //    int tgt = mapBack[(*eit)].first.second;
      //    int stack = mapBack[(*eit)].second;
      //    globalEpsilon.insert(std::make_pair(std::make_pair(src,tgt),stack));
      //#endif
      //}
      //t->start();

      //cout << std::endl << "[Newton] Creating Differentials" << std::endl;
      ////Created the differentials
      //double t2 = 0;
      //if (diffMap.size() != 0)
      //{
      //  bool linear = createDifferentials(diffMap, differentialMap);
      //  if (linear)
      //  {
      //      std::cout << "linear";
      //      std::cout << std::endl;
      //  }
      //  else
      //  {
      //      std::cout << "NonLin";
      //      std::cout << std::endl;
      //  }
      //  tslDiffMap::iterator it3;

      //    cout << "Step 4: =========================================================" << endl;
      //  /* Step 4 - Create a new fwpds using these partial differentials - run poststar on the fwpds in
      //  *           order to get the full differentials representing the values of the program return points
      //  */
      //  //Now we create new fwpds using these differentials, this fwpds has the weight type of NameWeight
      //  FWPDS * fnew = new FWPDS();
      //  fwpdsFromDifferential(fnew, differentialMap, varDependencies);

      //  //Now create another finite automaton
      //  WFA fa2;
      //  wali::Key acc2 = wali::getKeySpace()->getKey("accept2");
      //  fa2.addTrans(getPdsState(), stk(dummy), acc2, fnew->get_theZero()->one());
      //  fa2.setInitialState(getPdsState());
      //  fa2.addFinalState(acc2);
      //  WFA outfa2;

      //  //Get the regexps generated by running poststar on the new fwpds, these are the ones we will use in the Newton Round
      //  t->stop();
      //  t2 = fnew->getOutRegExpsSimple(fa2, outfa2, rNew);
      t->start();
      //  if (dump){
      //      cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
      //      fstream outfile("inter_outfa2.dot", fstream::out);
      //      outfa2.print_dot(outfile, true);
      //      outfile.close();
      //  } if (dump){
      //      cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
      //      fstream outfile("inter_outfa2.txt", fstream::out);
      //      outfa2.print(outfile);
      //      outfile.close();
      //  }

      //  if (dump){
      //      cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
      //      fstream outfile("fa2.dot", fstream::out);
      //      fa2.print_dot(outfile, true);
      //      outfile.close();
      //  } if (dump){
      //      cout << "[Newton] Dumping the output automaton to final_outfa.txt" << endl;
      //      fstream outfile("fa2.txt", fstream::out);
      //      fa2.print(outfile);
      //      outfile.close();
      //  }

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 4: =========================================================" << endl;
      // Perform Gaussian elimination

      if (newtonVerbosity >= NV_GAUSS) {
          std::cout << "Contents of variableIDs: " << std::endl;
          for (vector<int>::iterator eit = variableIDs.begin(); eit != variableIDs.end(); eit++)
          {
              std::cout << *eit << "  "; 
          }
          std::cout << std::endl;
          std::cout << "Keys of regExpMap: " << std::endl;
          for (tslRegExpMap::iterator it = regExpMap.begin(); it != regExpMap.end(); ++it)
          {
              std::cout << it->first << "  "; 
          }
          std::cout << std::endl << std::endl;
          for (vector<int>::iterator eit = variableIDs.begin(); eit != variableIDs.end(); eit++)
          {
              std::cout << "Regular expression in regExpMap for reID=" << *eit << ": " << std::endl;
              tsl_regexp::regExpPrettyPrint(regExpMap[*eit], std::cout); 
              std::cout << std::endl << std::endl;

#ifdef USE_IRE
              std::cout << "New-style (IRE) regular expression in IREregExpMap for reID=" << *eit << ": " << std::endl;
              IREregExpMap[*eit]->print(); 
              std::cout << std::endl << std::endl;
#endif
          }
      }

      std::sort(variableIDs.begin(), variableIDs.end());

      // FIXME: I don't really need two different maps here...
      //   But, for debugging, I think it will be useful.
      tslRegExpMap regExpsBeforeIsolation;
      tslRegExpMap regExpsAfterIsolation;

      IRERegExpMap IREregExpsBeforeIsolation;
      IRERegExpMap IREregExpsAfterIsolation;

      if (newtonVerbosity >= NV_GAUSS) {
          std::cout << std::endl << "Performing Gaussian Elimination." << std::endl << std::endl;
      }

      for (vector<int>::iterator varIt = variableIDs.begin(); varIt != variableIDs.end(); varIt++)
      {
          int i = *varIt;
          //regExpsBeforeIsolation[i] = regExpMap[i];
          regExpsBeforeIsolation[i] = CIR::mkProject(0,0,regExpMap[i]); // FIXME: 0,0 are dummy values, but we don't use them anyway

#ifdef USE_IRE
          IREregExpsBeforeIsolation[i] = IREregExpMap[i]->mkProject();
#endif
      }

      for (vector<int>::iterator varIt = variableIDs.begin(); varIt != variableIDs.end(); varIt++)
      {
          int i = *varIt;

          if (newtonVerbosity >= NV_GAUSS) {
              std::cout << std::endl << "  ------------------------------ " << std::endl;
              std::cout << "Working on variable " << i << std::endl;
              std::cout << "  The regexp for " << i << " just before isolating it:" << std::endl << std::endl;
              tsl_regexp::regExpPrettyPrint(regExpsBeforeIsolation[i], std::cout); 

#ifdef USE_IRE
              std::cout << "  New-style (IRE) regular expression for " << i << " just before isolating it: " << std::endl << std::endl;
              IREregExpsBeforeIsolation[i]->print(); 
              std::cout << std::endl << std::endl;
#endif
          }

          RTG::regExpRefPtr iRHS = CIR::isolate(i, regExpsBeforeIsolation[i]);

#ifdef USE_IRE
          IRE::regExpRefPtr IREiRHS = IREregExpsBeforeIsolation[i]->isolate(i);
#endif
          if (newtonVerbosity >= NV_GAUSS) {
              std::cout << std::endl << std::endl << "  The regexp for " << i << " just after isolating it:" << std::endl << std::endl;
              tsl_regexp::regExpPrettyPrint(iRHS, std::cout); 
              std::cout << std::endl;
              std::cout << std::endl << "  ------------------------------ " << std::endl;
          }

          regExpsAfterIsolation[i] = iRHS;

#ifdef USE_IRE
          IREregExpsAfterIsolation[i] = IREiRHS;
#endif
          for (vector<int>::iterator varIt2 = variableIDs.begin(); varIt2 != variableIDs.end(); varIt2++)
          {
              int j = *varIt2;

              if (j < i) {
                  if (newtonVerbosity >= NV_GAUSS) {
                      std::cout << std::endl << "  ''''''''''''''''''''''''''''''''''(A) " << std::endl;
                      std::cout << " Substituting in for " << i << " in the RHS for variable " << j << std::endl;
                      std::cout << "  The regexp for " << j << " just before substituting in for " << i << ":\n" << std::endl;
                      tsl_regexp::regExpPrettyPrint(regExpsAfterIsolation[j], std::cout); 
                  }

                  regExpsAfterIsolation[j] = CIR::substFree(iRHS, i, regExpsAfterIsolation[j]);

#ifdef USE_IRE
                  IREregExpsAfterIsolation[j] = IREregExpsAfterIsolation[j]->substFree(i, IREiRHS);
#endif
                  if (newtonVerbosity >= NV_GAUSS) {
                      std::cout << std::endl << "  The regexp for " << j << " just after substituting in for " << i << ":\n" << std::endl;
                      tsl_regexp::regExpPrettyPrint(regExpsAfterIsolation[j], std::cout); 
                      std::cout << std::endl << "  '''''''''''''''''''''''''''''''''' " << std::endl;
                  }
              } else if (j > i) {
                  if (newtonVerbosity >= NV_GAUSS) {
                      std::cout << std::endl << "  ''''''''''''''''''''''''''''''''''(B) " << std::endl;
                      std::cout << " Substituting in for " << i << " in the RHS for variable " << j << std::endl;
                      std::cout << "  The regexp for " << j << " just before substituting in for " << i << ":\n" << std::endl;
                      tsl_regexp::regExpPrettyPrint(regExpsBeforeIsolation[j], std::cout); 
                  }

                  regExpsBeforeIsolation[j] = CIR::substFree(iRHS, i, regExpsBeforeIsolation[j]);

#ifdef USE_IRE
                  IREregExpsBeforeIsolation[j] = IREregExpsBeforeIsolation[j]->substFree(i, IREiRHS);
#endif
                  if (newtonVerbosity >= NV_GAUSS) {
                      std::cout << std::endl << "  The regexp for " << j << " just after substituting in for " << i << ":\n" << std::endl;
                      tsl_regexp::regExpPrettyPrint(regExpsBeforeIsolation[j], std::cout); 
                      std::cout << std::endl;
                      std::cout << std::endl << "  '''''''''''''''''''''''''''''''''' " << std::endl;
                  }
              }

          }

      }

      if (newtonVerbosity >= NV_GAUSS) {
          std::cout << std::endl << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  " << std::endl << std::endl;
          std::cout << std::endl << "Finished Gaussian Elimination." << std::endl << std::endl;
          for (vector<int>::iterator eit = variableIDs.begin(); eit != variableIDs.end(); eit++)
          {
              std::cout << "Regular expression in regExpsAfterIsolation for reID=" << *eit << ": " << std::endl;
              tsl_regexp::regExpPrettyPrint(regExpsAfterIsolation[*eit], std::cout); 
              std::cout << std::endl << std::endl;

#ifdef USE_IRE
              std::cout << "New-style (IRE) regular expression in IREregExpsAfterIsolation for reID=" << *eit << ": " << std::endl;
              IREregExpsAfterIsolation[*eit]->print(); 
              std::cout << std::endl << std::endl;
#endif
          }
      }
      
      //convertToTSLRegExpsT(rNew, regExpMap, tensoredRegExpMap, differentialMap, mapBack, mergeSrcMap);
      
      if (aboveBelowMode == NEWTON_FROM_BELOW) {
          aList = CIR::initializeAssignment(); 
          //for (tslRegExpMap::iterator it = regExpMap.begin(); it != regExpMap.end(); ++it)
          //{
          //    RTG::regExpRefPtr r = it->second;
          //    //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = evalNonRecAt0(r);
          //    CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr newVal = CIR::evalRegExp(r);

          //    // Insert <it->first,newVal> into aList
          //    aList = CIR::updateAssignment(aList, CBTI_INT32(it->first), newVal);
          //}

          IREaList.clear();

      } else if (aboveBelowMode == NEWTON_FROM_ABOVE) {
          assert(false && "We should not use aboveBelowMode NEWTON_FROM_ABOVE in newtonLoop_GJ");
          CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr topVal = mkSemElemWrapper(Relation::getBaseTop());
          aList = CIR::mkConstantAssignment(topVal);
      } 
      // else {
      //    assert(false && "Unrecognized running mode.");
      //}

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "Step 5: =========================================================" << endl;
      /* Step 6 - Perform Newton rounds until a fixed-point is reached */
      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "[Newton] Running Newton" << endl;
      newtonLoop_GJ(aList, IREaList, regExpsAfterIsolation, IREregExpsAfterIsolation);

      //assert(false && "The rest of this function is not yet implemented!"); 

      //Using the final weights from Newton, evaluate the tslRegExps to get the final weights
      //evalRegExps(aList);


      /* Step 7 - Insert the new weights into the original outfa and perform the iterative path summary in order to
      *           determine if the error weight is reachable
      */
      //}
      //else  //There are no variables to be differentiated
      //{
      //  nonRec = true;
      //  std::cout << "NonRec";
      //  std::cout << std::endl;
      //  aList = CIR::initializeAssignment();
      //  //evalRegExps(aList);
      //  //t2 = 0;
      //  if (testMode) {
      //      std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
      //      testFile << "__NUMRNDS 0" << std::endl;
      //      testFile.close();
      //  }   
      //}
      t->stop();

      if (newtonVerbosity >= NV_NEWTON_STEPS) cout << "\nStep 6: =========================================================" << endl;
      globalAssignment = aList;
      //Map the evaluated weights back to the transitions the regexps came from
      for (stateIter = faStates.begin(); stateIter != faStates.end(); stateIter++)
      {
          TransSet & transSet = outfa.getState(*stateIter)->getTransSet();
          TransSet::iterator transIt;
          for (transIt = transSet.begin(); transIt != transSet.end(); transIt++)
          {
              ITrans * tt = *transIt;
              int tSrc = tt->from();
              int tTgt = tt->to();
              int tStack = tt->stack();
              int transReg = transMap[std::make_pair(std::make_pair(tSrc, tTgt), tStack)];
              t->start();
              //CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr w = evalRegExpFinNonRec(regExpMap[transReg], aList);
              CONC_EXTERN_PHYLA::sem_elem_wrapperRefPtr w = CIR::evalRegExp(regExpMap[transReg]);
              t->stop();
              //std::cout << "OutWeight: " << tSrc << "," << tTgt << "," << tStack << ":";
              //w.v->print(std::cout) << endl;
              tt->setWeight(w.v);
          }
      }
      /*cout << "[Newton] Dumping the output automaton in dot format to outfa.dot" << endl;
      fstream outfile("newton_outfa.dot", fstream::out);
      outfa.print_dot(outfile, true);
      outfile.close();*/

      //Perform the final path summary
      t->start();
      outfa.path_summary_tarjan_fwpds(true, true);
      //State * initS = outfa.getState(outfa.getInitialState());
      //if (initS == NULL)
      //{
      //  cout << "[Newton] FWPDS ==> error not reachable" << endl;
      //}
      //else
      //{
      //  sem_elem_t fWt = outfa.getState(outfa.getInitialState())->weight();
      //  if (fWt->equal(fWt->zero()))
      //  {
      //      cout << "[Newton] FWPDS ==> error not reachable" << endl;
      //  }
      //  else{
      //      cout << "[Newton] FWPDS ==> error reachable" << endl;
      //  }
      //}

      t->stop();
      double tTime = t->total_time() + t1 + baseEvalTime;
      //double tTime = t->total_time() + t1 + t2 + baseEvalTime;
      if (newtonVerbosity >= NV_NEWTON_STEPS) { 
          std::cout << "\n[Newton] Time taken by: Newton: " << std::endl;
          cout << tTime << "\n" << endl;
      }
      
      if (testMode) {
          std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
          testFile << "__TIME " << tTime << std::endl;
          testFile.close();
      }
      
      return tTime;
  }
  else  //There is no error state
  {
      cout << "[Newton] FWPDS ==> error not reachable" << endl;
      double tTime = t->total_time() + t1;;
      std::cout << "\n[Newton] Time taken by: Newton: \n";
      cout << tTime << endl;
      std::cout << "NonRec";
      std::cout << std::endl;
      
      if (testMode) {
          std::fstream testFile(testFileName.c_str(), std::fstream::out | std::fstream::app);
          testFile << "__TIME " << tTime << std::endl;
          testFile.close();
      }
      
      return tTime;
  }
}





//void testICRARegExp() {
//    std::cout << "Testing ICRA regexp functionality:" << std::endl;
//    static value * testFunction = NULL;
//    if (testFunction == NULL) {
//        testFunction = caml_named_value("try_icra_regexp");
//    }
//    value v = caml_callback(*testFunction, Val_unit);
//    std::cout << "  The dummy value I saw was: " << Int_val(v) << std::endl;
//}


// Old code for manipulating procedure summaries

void printProcedureNameFromNode(int nodeNumber, ostream& out) {
    CAMLparam0();
    CAMLlocal1(sval);
    value * proc_func = caml_named_value("procedure_of_vertex_callback");
    sval = caml_callback(*proc_func, Val_int(nodeNumber));
    out << String_val(sval);
    CAMLreturn0;
}

// Print summaries of all procedures
//   Return the procedure summary of the main procedure, if we were able to identify it.
relation_t printProcedureSummaries(WFA& outfaNewton) {

    if (newtonVerbosity >= NV_SUMMARIES) {
        std::cout << "================================================" << std::endl;
        std::cout << "Procedure Summaries" << std::endl << std::endl;
    }
    
    ofstream smtout;
    if (doSmtlibOutput) { smtout.open("smtlib_output.smt2"); }

    relation_t mainProcedureSummary = NULL;

    // Set exit_transitions to the set of all transitions in outfaNewton
    // of the form (st1,WALI_EPSILON,<st1,e>), where e is an entry node
    wali::wfa::TransSet exit_transitions;
    exit_transitions = outfaNewton.match(st1(), WALI_EPSILON);
    for(wali::wfa::TransSet::iterator tsit = exit_transitions.begin(); tsit != exit_transitions.end(); tsit++)
    {
        if (newtonVerbosity >= NV_SUMMARIES) { std::cout << "------------------------------------------------" << std::endl; }

        bool foundMain = false;

        if (newtonVerbosity >= NV_SUMMARIES) std::cout << "Procedure summary for ";
        if (doSmtlibOutput) { smtout << "; Procedure summary for "; }

        // First, we want to print the procedure name; for that, we need to find
        //   the procedure's entry vertex, so we can call printProcedureNameFromNode.
        // After our various automaton manipulations, the following code seems to be
        //   what's required to obtain the procedure's entry vertex key from the
        //   exit transition whose weight is the procedure summary.
        wali::Key toKey = (*tsit)->to();
        wali::ref_ptr<wali::KeySource> ks = wali::getKeySource(toKey);
        wali::ref_ptr<wali::wpds::GenKeySource> gks = dynamic_cast<wali::wpds::GenKeySource *>(ks.get_ptr());
        if (gks != NULL) {
            wali::ref_ptr<wali::KeySource> gksks = wali::getKeySource(gks->getKey());
            wali::ref_ptr<wali::KeyPairSource> kps = dynamic_cast<wali::KeyPairSource *>(gksks.get_ptr());
            if (kps != NULL) {
                wali::ref_ptr<wali::KeySource> kpsks = wali::getKeySource(kps->second());
                wali::ref_ptr<wali::IntSource> is = dynamic_cast<wali::IntSource *>(kpsks.get_ptr());
                if (is != NULL) {
                    int entryVertex = is->getInt();
                    if (newtonVerbosity >= NV_SUMMARIES) printProcedureNameFromNode(entryVertex, std::cout);
                    if (doSmtlibOutput) { smtout << "'"; printProcedureNameFromNode(entryVertex, smtout); smtout << "'" << std::endl; }
                    if (newtonVerbosity >= NV_SUMMARIES) std::cout << std::endl;
                } else {
                    if (newtonVerbosity >= NV_SUMMARIES) std::cout << "an unknown procedure.  This shouldn't happen.  Case 1." << std::endl;
                    if (doSmtlibOutput) { smtout << "an unknown procedure.  This shouldn't happen.  Case 1." << std::endl; }
                }
            } else {
                if (newtonVerbosity >= NV_SUMMARIES) std::cout << "an unknown procedure.  This shouldn't happen.  Case 2." << std::endl;
                if (doSmtlibOutput) { smtout << "an unknown procedure.  This shouldn't happen.  Case 2." << std::endl; }
            }
        } else {
            if (newtonVerbosity >= NV_SUMMARIES) std::cout << "main (assumed)" << std::endl;
            if (doSmtlibOutput) { smtout << "'main' (assumed)" << std::endl; }
            foundMain = true;
        }

        // Finally, we can print the procedure summary itself:
        if (newtonVerbosity >= NV_SUMMARIES) std::cout << std::endl;
        relation_t nval = mkRelationFromSemElem((*tsit)->weight());
        if (foundMain) { mainProcedureSummary = nval; }
        if (newtonVerbosity >= NV_SUMMARIES) {
            nval->print(std::cout);
            std::cout << std::endl << std::endl;
        }

        if (doSmtlibOutput) {
            nval->printSmtlib(smtout);
            smtout << std::endl;
        }
    }

    if (doSmtlibOutput) { smtout.close(); }

    return mainProcedureSummary;

}

    //std::cout << "Testing New Regular Expression System:" << std::endl;
    //std::cout << "Zero plus Zero (without simplification) is:" << std::endl;
    //IRE::regExpRefPtr r1 = IRE::RegExp::mkZero();
    //IRE::regExpRefPtr r2 = IRE::RegExp::mkZero();
    //IRE::regExpRefPtr r = r1->mkPlus(r2);
    //r->print();
    //std::cout << "Zero plus Main_summary (without simplification) is:" << std::endl;
    //IRE::regExpRefPtr r1a = IRE::RegExp::mkZero();
    //IRE::regExpRefPtr r2a = IRE::RegExp::mkWeight(program->main->summary);
    //IRE::regExpRefPtr ra = r1a->mkPlus(r2a);
    //ra->print();
    //std::cout << "End of Test" << std::endl;
    //std::cout << "Now let's do a cache experiment:" << std::endl;
    //IRE::clearEvalCaches();
    //ra->eval();
    //ra->eval();
    //std::cout << "End of Experiment" << std::endl;

