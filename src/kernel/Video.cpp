#include "Video.hpp"

namespace kernel {

  Video::Video() {
    screen_color = LightGrey << 8; // light grey text on black screen
    pos = 0; // top
    off = 0; // left.
    Videomem = ( unsigned short* ) 0x0000b8000; // beginning address of video memory

  }

  Video::~Video() {
  }

  void Video::clear() {
    _mutex.enter();

    for ( uint32 i = 0; i < ( width * height ); i++ ) {
      Videomem[ i ] = ' ' | screen_color; // fill screen with blanks
    }
    pos = 0; // set to default position, top left.
    off = 0;

    _mutex.leave();
  }

  void Video::move( uint32 x, uint32 y ) {
    if ( x < width && y < height ) {
      _mutex.enter();
      pos = x;
      off = width * y;
      _mutex.leave();
    }
  }

  void Video::position( uint32 *x, uint32 *y ) {
    _mutex.enter();
    *x = pos;
    *y = off / width;
    _mutex.leave();
  }

  Video& Video::operator<<( uchar* cp ) {
    while ( *cp != '\0' ) {
      put( *cp++ );
    }

    return *this;
  }

  Video& Video::operator<<( const char* cp ) {
    while ( *cp != '\0' ) {
      put( *cp++ );
    }

    return *this;
  }

  Video& Video::operator<<( uint32 i ) {
    char buff[ 10 ];
    int idx = 0;

    do {
      buff[ idx++ ] = ( i % 10 ) + '0';
      i /= 10;
    } while ( i > 0 );

    do {
      put( buff[ --idx ] );
    } while ( idx > 0 );

    return *this;
  }

  Video& Video::operator<<( int32 i ) {
    char buff[ 11 ];
    int idx = 0;

    if ( i < 0 ) {
      buff[ idx++ ] = '-';
      i *= -1;
    }

    do {
      buff[ idx++ ] = ( i % 10 ) + '0';
      i /= 10;
    } while ( i > 0 );

    do {
      put( buff[ --idx ] );
    } while ( idx > 0 );

    return *this;
  }

  Video& Video::operator<<( double d ) {
    char buff[ 11 ];
    int idx = 0;

    int i = d;

    if ( i < 0 ) {
      buff[ idx++ ] = '-';
      i *= -1;
    }

    do {
      buff[ idx++ ] = ( i % 10 ) + '0';
      i /= 10;
    } while ( i > 0 );

    do {
      put( buff[ --idx ] );
    } while ( idx > 0 );

    put( '.' );

    i = d;
    d -= i;

    do {
      d *= 10;
      i = d;
      buff[ idx++ ] = ( i % 10 ) + '0';
    } while ( i > 0 );

    for ( int i = 0; i <= idx; i++ ) {
      put( buff[ i ] );
    }

    return *this;
  }

  uint32 Video::write( void* data, uint32 length ) {
    char* Data = ( char* ) data;
    uint32 i;

    for ( i = 0; i < length; ++i ) {
      put( Data[ i ] );
    }

    return i;
  }

  void Video::hex( uint32 l ) {
    char b;
    int i = sizeof(uint32) << 1;

    put( '0' );
    put( 'X' );

    while ( i-- ) {
      b = 0x0f & ( l >> ( i << 2 ) );

      if ( b <= 9 ) {
        b += '0';
      }
      else {
        b += 'A' - 10;
      }

      put( b );
    }
  }

  void Video::hex( uint16 l ) {
    char b;
    int i = sizeof(uint16) << 1;

    put( '0' );
    put( 'X' );

    while ( i-- ) {
      b = 0x0f & ( l >> ( i << 2 ) );

      if ( b <= 9 ) {
        b += '0';
      }
      else {
        b += 'A' - 10;
      }

      put( b );
    }
  }

  void Video::hex( uint8 l ) {
    char b;
    int i = sizeof(uint8) << 1;

    put( '0' );
    put( 'X' );

    while ( i-- ) {
      b = 0x0f & ( l >> ( i << 2 ) );

      if ( b <= 9 ) {
        b += '0';
      }
      else {
        b += 'A' - 10;
      }

      put( b );
    }
  }

  void Video::bits( int i ) {
    int idx = sizeof(int) * 8;

    do {
      put( i & ( 1 << idx-- ) ? '1' : '0' );
    } while ( idx >= 0 );
  }

  void Video::put( char c ) {
    _mutex.enter();
    if ( pos >= width ) { // Wenn die Textausgabe den rechten...
      pos = 0;
      off += width;
    }

    if ( c == '\n' ) {
      pos = 0;
      off += width;
    }

    _mutex.leave();
    if ( off >= ( width * height ) ) { // ...oder den unteren Bildschirmrand erreicht, gibt es einen
      clear(); // Umbruch bzw. es wird aufgeräumt.
    }

    if ( c == '\n' ) {
      return;
    }

    _mutex.enter();
    Videomem[ off + pos ] = ( unsigned char ) c | screen_color; // Setzen des Zeichens und der Farbe in den Videospeicher
    pos++;
    _mutex.leave();
  }

  void Video::color( uint32 color ) {
    screen_color = color << 8;
  }

}
