/**
 * Cpp.hpp
 *
 * @since 29.12.2009
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef CPP_HPP_
#define CPP_HPP_

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned char uchar;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef void* address;

extern "C" void gcc_assert( bool b );
extern "C" void gcc_unreachable();

/**
 *  calls the constructor for globale/static objects.
 */
extern "C" void staticConstructors();
extern "C" void staticDestructors();

typedef void (*constructor)();

// Im Linkerskript definiert
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

#endif /* CPP_HPP_ */
