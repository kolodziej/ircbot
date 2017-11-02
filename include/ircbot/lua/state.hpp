#ifndef _LUA_STATE_HPP
#define _LUA_STATE_HPP

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace lua {

class State {
 public:
  State();
  ~State();

  lua_State* operator()();

 private:
  lua_State* m_state;
};

}

#endif
