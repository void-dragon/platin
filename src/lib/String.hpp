/**
 * String.hpp
 *
 * @since 10.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef STRING_HPP_
#define STRING_HPP_

#include <cpp.hpp>
#include <lib/hash/Tiger.hpp>

namespace lib {

  /**
   * The representation of a string.
   */
  class String {
    protected:
      bool _deleteOnDestroy;
      uint32 _length;
      uint32 _size;
      char* _data;

      void grow( uint32 n );

    public:

      String();

      String( const char* Data );

      /**
       *
       * @param Data A zero terminated string.
       */
      String( char* Data );

      /**
       * Uses a copy of the given string.
       *
       * @param Data The array it self.
       * @param Size The number of elements.
       */
      String( char* Data, uint32 Size );

      char& operator[]( uint32 idx ) {
        return _data[ idx ];
      }

      /**
       * Get character at index.
       */
      char get( uint32 idx ) {
        return _data[ idx ];
      }

      void operator+=( const char* str );

      /**
       * Adds a new element.
       *
       * @param v The new element.
       */
      void operator+=( char v );

      /**
       * Adds an array of new elements.
       *
       * @param a Array of elements which will be added.
       */
      void operator+=( String* a );

      bool operator==( String* a );

      bool operator==( const char* str );

      /**
       * Adds a new element.
       *
       * @param v The new element.
       */
      void add( char v );

      void add( char* a, uint32 len );

      /**
       * Adds an array of new elements.
       *
       * @param a Array of elements which will be added.
       */
      void add( String* a );

      int32 compare( char* data, uint32 len );

      int32 compare( String* A );

      /**
       * The number of fields the array has allocated in the system.
       */
      uint32 capacity() const {
        return _size;
      }

      void set( char* Data, uint32 Len );

      /**
       * The length of the actual data.
       */
      uint32 size() {
        return _length;
      }

      /**
       * Returns a new array which references to a part of the original array.
       *
       * @param start The index of the first element of the new array.
       * @param end The index of the last element of the new array.
       * @return A pointer to a new string.
       */
      String* sub( uint32 start, uint32 end ) {
        return new String( _data + start, end );
      }

      char* ptr() const {
        return _data;
      }

      hash::Tiger hash();

      operator int() {
        return ( int ) _data;
      }

      operator const char*() {
        return _data;
      }

      operator char*() {
        return _data;
      }

      virtual ~String();
  };

}

#endif /* STRING_HPP_ */
