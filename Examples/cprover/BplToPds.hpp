#ifndef __CPROVER_BPL_TO_PDS_HPP_
#define __CPROVER_BPL_TO_PDS_HPP_

#include "wali/HashMap.hpp"

#include "wali/wpds/WPDS.hpp"
#include "wali/wpds/fwpds/FWPDS.hpp"
#include "wali/wpds/Rule.hpp"
#include "wali/wpds/RuleFunctor.hpp"

#ifdef USE_NWAOBDD
#include "../turetsky/svn-repository/NWAOBDDRel.hpp"
#else
#include "wali/domains/binrel/BinRel.hpp"
#include "wali/domains/binrel/ProgramBddContext.hpp"
#endif

extern "C" {
#include "ast.h"
}

namespace wali 
{
  namespace cprover 
  {
    namespace details 
    {
      namespace resolve_details
      {
        struct hash_str 
        {
          size_t operator() (const char * const & str) const 
          {
            const char * c = str;
            size_t sum = 0;
            while(*c != '\0'){
              sum += *c;
              c++;
            }
            return sum;
          };
        };
        struct str_equal 
        {
          bool operator () (const char * c1, const char * c2)
          {
            if(c1 == NULL && c2 == NULL)
              return true;
            if(c1 == NULL || c2 == NULL)
              return false;
            while(*c1 != '\0' && *c2 != '\0'){
              if(*c1 != *c2)
                return false;
              c1++;
              c2++;
            }
            if(*c1 != '\0' || *c2 != '\0')
              return false;
            return true;
          };
        };
        struct hash_stmt_ptr
        {
          size_t operator() (const stmt * const & s) const 
          {
            return (size_t) s; 
          };
        };
        struct stmt_ptr_equal
        {
          bool operator() (const stmt * s1, const stmt * s2) const
          {
            return s1 == s2;
          };
        };

        typedef wali::HashMap<const char *, stmt *, hash_str, str_equal> str_stmt_ptr_hash_map;
        typedef wali::HashMap<const char *, proc *, hash_str, str_equal> str_proc_ptr_hash_map;
        //IMP: This hashmap owns the stmt_list, but not the stmt in the key, or those in the list.
        //On clearing, we must delete the stmt_list objects, but nothing deeper.
        typedef wali::HashMap<stmt *, stmt_list *, hash_stmt_ptr, stmt_ptr_equal> stmt_ptr_stmt_list_ptr_hash_map;
        typedef wali::HashMap<stmt *, proc *, hash_stmt_ptr, stmt_ptr_equal> stmt_ptr_proc_ptr_hash_map;

        // must use this to delete a hashmap of type stmt_ptr_stmt_list_ptr_hash_map
        void clear_stmt_ptr_stmt_list_ptr_hash_map(stmt_ptr_stmt_list_ptr_hash_map& m);

        void map_label_to_stmt(str_stmt_ptr_hash_map& m, const proc * p);
        void map_name_to_proc(str_proc_ptr_hash_map& m, const prog * pg);
        void map_goto_to_targets(stmt_ptr_stmt_list_ptr_hash_map& mout, str_stmt_ptr_hash_map& min, const proc * p);
        void map_call_to_callee(stmt_ptr_proc_ptr_hash_map& mout, str_proc_ptr_hash_map& min, const proc * p);
      }


#ifdef USE_NWAOBDD
      wali::domains::nwaobddrel::NWAOBDDContext * dump_pds_from_prog_nwa(wpds::WPDS * pds, prog * pg, bool first);
#else
      wali::domains::binrel::BddContext * dump_pds_from_prog(wpds::WPDS * pds, prog * pg);
#endif

#ifdef USE_NWAOBDD

	  void dump_pds_from_proc(
		  wpds::WPDS * pds,
		  proc * p,
		  domains::nwaobddrel::NWAOBDDContext * con,
		  const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets,
		  const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee);


	  void dump_pds_from_stmt(
		  wpds::WPDS * pds,
		  stmt * s,
		  domains::nwaobddrel::NWAOBDDContext * con,
		  const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets,
		  const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee,
		  const char * f,
		  stmt * ns);

	  void dump_pds_from_stmt_list(
		  wpds::WPDS * pds,
		  stmt_list * sl,
		  domains::nwaobddrel::NWAOBDDContext * con,
		  const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets,
		  const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee,
		  const char * f,
		  stmt * es);
#else
         void dump_pds_from_proc(
		  wpds::WPDS * pds,
		  proc * p,
		  domains::binrel::ProgramBddContext * con,
		  const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets,
		  std::map<string, int> & localVars,
          const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee);
      void dump_pds_from_stmt(
          wpds::WPDS * pds, 
          stmt * s, 
          domains::binrel::ProgramBddContext * con, 
          const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets,
          const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee,
		  std::map<string, int> & localVars,
          const char * f,
          stmt * ns);

      void dump_pds_from_stmt_list(
          wpds::WPDS * pds, 
          stmt_list * sl, 
          domains::binrel::ProgramBddContext * con, 
          const resolve_details::stmt_ptr_stmt_list_ptr_hash_map& goto_to_targets, 
          const resolve_details::stmt_ptr_proc_ptr_hash_map& call_to_callee, 
		  std::map<string, int> & localVars,
          const char * f, 
          stmt * es);
#endif
    }

#ifdef USE_NWAOBDD

	// Dumps the program as a PDS into pds. pds must be preallocated.
	// Returns the vocabulary generated when creating the pds.
	wali::domains::nwaobddrel::NWAOBDDContext * pds_from_prog_nwa(wpds::WPDS * pds, prog * pg, bool first);
	// Same, but generates MergeFns of type wali::domains::binrel::BinRelMeetMerge
	wali::domains::nwaobddrel::NWAOBDDContext * pds_from_prog_with_meet_merge_nwa(wpds::ewpds::EWPDS * pds, prog * pg, bool first = true);
	// Same, but generates MergeFns of type wali::domains::binrel::BinRelTensorMerge
	wali::domains::nwaobddrel::NWAOBDDContext * pds_from_prog_with_tensor_merge_nwa(wpds::ewpds::EWPDS * pds, prog * pg, bool first = true);
	wali::domains::nwaobddrel::NWAOBDDContext * pds_from_prog_with_newton_merge_nwa(wpds::ewpds::EWPDS * pds, prog * pg, bool first = true);

	wali::domains::nwaobddrel::NWAOBDDContext * havocLocalsNWA(wpds::WPDS * pds, prog * pg, domains::nwaobddrel::NWAOBDDContext * con);
	wali::domains::nwaobddrel::NWAOBDDContext * pds_from_prog_nwa(wpds::WPDS * pds, prog * pg, bool first);
#else
    // Parses the program in file fname and generates the PDS in pds. pds must be preallocated.
    // Returns the vocabulary generated when parsing the program.
    wali::domains::binrel::BddContext * read_prog(wpds::WPDS * pds, const char * fname, bool dbg = false);
    prog * parse_prog(const char * fname);

    // Dumps the program as a PDS into pds. pds must be preallocated.
    // Returns the vocabulary generated when creating the pds.
    wali::domains::binrel::BddContext * pds_from_prog(wpds::WPDS * pds, prog * pg);
    // Same, but generates MergeFns of type wali::domains::binrel::BinRelMeetMerge
    wali::domains::binrel::BddContext * pds_from_prog_with_meet_merge(wpds::ewpds::EWPDS * pds, prog * pg);
    // Same, but generates MergeFns of type wali::domains::binrel::BinRelTensorMerge
    wali::domains::binrel::BddContext * pds_from_prog_with_tensor_merge(wpds::ewpds::EWPDS * pds, prog * pg);
	wali::domains::binrel::BddContext * pds_from_prog_with_newton_merge(wpds::ewpds::EWPDS * pds, prog * pg);

    wali::domains::binrel::BddContext * havocLocals(wpds::WPDS * pds, prog * pg, domains::binrel::ProgramBddContext * con, std::map<string,int> & localVars);
    #endif
    void print_prog_stats(prog * pg);

    // Must be called to fix fall-through returns *before* dumping PDS
    void make_void_returns_explicit(prog * pg);
    // Must be called after make_void_returns_explicit and before instrument_asserts
    void instrument_enforce(prog * pg);
    // Must be called to fix assert statements, *before* resolving the program.
    void instrument_asserts(prog * pg, const char * errLbl = "error");
    // Must be called to add stuff to handle values returned from procedures correctly
    void instrument_call_return(prog * pg);
    // May be called any time.
    void remove_skip(prog * pg);
    //Splice non-goto statement into program and give it the successing statements labels
    void splice_nongo_into_stmt_list(stmt_list** prev, stmt_list* new_sl, stmt_list* to_come_after);

    wali::Key getEntryStk(const prog * pg, const char * procname);
    wali::Key getPdsState();
    wali::Key getErrStk(const prog * pg);
  } 
}





#endif //#ifndef __CPROVER_BPL_TO_PDS_HPP_

