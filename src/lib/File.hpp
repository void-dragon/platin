/**
 * File.hpp
 *
 * @since 11.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef FILE_HPP_
#define FILE_HPP_

#include <lib/stream/In.hpp>
#include <lib/stream/Out.hpp>
#include <lib/String.hpp>
#include <lib/collection/Map.hpp>

namespace lib {

   /**
    * An interface for an abstract file.
    *
    * @since 11.02.2011
    * @date 11.02.2011
    * @author Arne Simon => email::[arne_simon@gmx.de]
    */
   class File: public stream::In, public stream::Out {
      public:
         static int32 nameCompare( String* &a, String* &b ) {
            return a->compare( b );
         }

         typedef collection::Map< String*, File*, &nameCompare > Files;

         /**
          * The size of the file.
          * @return The file size.
          */
         virtual uint64 size();

         /**
          * Reads a range of bytes from the file.
          *
          * @note Allocates a new block of memory for the data.
          *
          * @param start Starting offset in byte.
          * @param size Bytes to read.
          */
         virtual void* range( uint32 start, uint32 size );

         /**
          * Writes a range of bytes to the file.
          *
          * @param start Staring offset in byte.
          * @param size Bytes to read.
          * @param data The buffer from which the data will be read.
          */
         virtual void range( uint32 start, uint32 size, void* data );

         /**
          * Checks if the file has sub-files.
          */
         virtual bool hasFiles();

         /**
          * A collection of sub-files.
          * @return Sub-files.
          */
         virtual Files* files();

         virtual File* get( const char* name );

         // outsream

         uint32 write( void* data, uint32 length );

         File& operator<<( const char* cp );

         File& operator<<( uchar* cp );

         File& operator<<( uint32 i );

         File& operator<<( int32 i );

         File& operator<<( double i );

         // instream

         void read( void** data, uint32* length );

         uint32 read( void* data, uint32 length );

         File& operator>>( const char* &cp );

         File& operator>>( uchar* &cp );

         File& operator>>( uint32 &i );

         File& operator>>( int32 &i );

         File& operator>>( double &i );

         virtual ~File();
   };

}

#endif /* FILE_HPP_ */
