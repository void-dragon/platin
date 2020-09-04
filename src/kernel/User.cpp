/**
 * User.cpp
 *
 * @since 09.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "User.hpp"

namespace kernel {

  User::User() {

  }

  User::User( lib::String name )
      : username( name ) {

  }

  User::~User() {
  }

}
