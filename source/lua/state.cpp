#include "ircbot/lua/state.hpp"

namespace lua {

State::State() :
    m_state{luaL_newstate()} {
}

State::~State() {
  lua_close(m_state);
}

lua_State* State::operator()() {
  return m_state;
}

}
