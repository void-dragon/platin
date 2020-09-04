#ifndef Video_HPP
#define Video_HPP

#include <cpp.hpp>
#include <lib/stream/Out.hpp>
#include <lib/sync/Mutex.hpp>

namespace kernel {

  /**
   * Basic output stream to console prompt.
   *
   */
  class Video: public lib::stream::Out {
    private:
      unsigned short* Videomem; // Zeiger auf den Videospeicher
      uint32 off; // Y-Position der Textausgabe, je volle Zeile +80
      uint32 pos; // X-Position der Textausgabe, ab Zeilenanfang
      uint32 screen_color; // FB/BG-Farbe
      static const uint32 width = 80;
      static const uint32 height = 24;
      lib::sync::Mutex _mutex;

    public:
      enum Color {
        Black,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Brown,
        LightGrey,
        DarkGrey,
        LightBlue,
        LightGreen,
        LightCyan,
        LightRed,
        LightMagenta,
        Yellow,
        White
      };

      Video();

      virtual ~Video();

      void clear();

      void move( uint32 x, uint32 y );

      void position( uint32 *x, uint32 *y );

      Video& operator<<( const char* cp );

      Video& operator<<( uchar* cp );

      Video& operator<<( uint32 i );

      Video& operator<<( int32 i );

      Video& operator<<( double i );

      void bits( int i );

      void hex( uint32 l );

      void hex( uint16 s );

      void hex( uint8 b );

      uint32 write( void* data, uint32 length );

      void put( char c );

      /**
       *
       */
      void color( uint32 color );

  };

}

#endif
