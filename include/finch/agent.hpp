#ifndef _FINCH_AGENT_HPP_
#define _FINCH_AGENT_HPP_

#include "node.hpp"
#include "program_state.hpp"

#include <vector>

namespace finch
{
  class agent
  {
  public:
    agent();
    agent(const node &program, const std::vector<double> &chromosomes = std::vector<double>());
    agent(const agent &rhs);
    
    bool is_valid() const;
    
    void set_program(const node &program);
    const node &program() const;
    void set_chromosomes(const std::vector<double> &chromosomes);
    const std::vector<double> &chromosomes() const;
    
    void set_final_state(const program_state &final_state);
    const program_state &final_state() const;
    bool is_final_state_set() const;
    
    agent &operator =(const agent &rhs);
    
  private:
    node _program;
    std::vector<double> _chromosomes;
    bool _final_state_set;
    program_state _final_state;
  };
}

#endif