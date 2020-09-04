/**
 * User.hpp
 *
 * @since 09.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef USER_HPP_
#define USER_HPP_

#include <lib/hash/Tiger.hpp>
#include <lib/String.hpp>

namespace kernel {

  class User {
    public:
      lib::String username;
      lib::hash::Tiger password;

      User();
      User( lib::String name );

      virtual ~User();
  };

}

#endif /* USER_HPP_ */
