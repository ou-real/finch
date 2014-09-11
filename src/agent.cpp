#include <finch/agent.hpp>

using namespace finch;

agent::agent()
  : _program(null_type)
  , _chromosomes(std::vector<double>())
  , _final_state_set(false)
{
  
}

agent::agent(const node &program, const std::vector<double> &chromosomes)
  : _program(program)
  , _chromosomes(chromosomes)
  , _final_state_set(false)
{
}

agent::agent(const agent &rhs)
  : _program(rhs._program)
  , _chromosomes(rhs._chromosomes)
  , _final_state_set(rhs._final_state_set)
  , _final_state(rhs._final_state)
{
}

bool agent::is_valid() const
{
  return _program.is_valid();
}

void agent::set_program(const node &program)
{
  _program = program;
}

const node &agent::program() const
{
  return _program;
}

void agent::set_chromosomes(const std::vector<double> &chromosomes)
{
  _chromosomes = chromosomes;
}

const std::vector<double> &agent::chromosomes() const
{
  return _chromosomes;
}

void agent::set_final_state(const program_state &final_state)
{
  _final_state_set = true;
  _final_state = final_state;
}

const program_state &agent::final_state() const
{
  return _final_state;
}

bool agent::is_final_state_set() const
{
  return _final_state_set;
}

agent &agent::operator =(const agent &rhs)
{
  _program = rhs._program;
  _chromosomes = rhs._chromosomes;
  _final_state_set = rhs._final_state_set;
  _final_state = rhs._final_state;
  return *this;
}
