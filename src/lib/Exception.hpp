/**
 * Exception.hpp
 *
 * @since 11.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef LIB_EXCEPTION_HPP_
#define LIB_EXCEPTION_HPP_

namespace lib {

  class Exception {
    public:
      static Exception buffer;

      const char* msg;
    public:

      static void throwing( const char* M );

      Exception();

      Exception( const char* M );

      virtual ~Exception();
  };

}

#endif /* LIB_EXCEPTION_HPP_ */
