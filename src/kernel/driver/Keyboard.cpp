/**
 * Keyboard.cpp
 *
 * @since 07.10.2010
 * @author raup-tier
 */

#include "Keyboard.hpp"
#include <lib/std.hpp>
#include <kernel/System.hpp>

namespace kernel {

  namespace driver {

    const uint8 Keyboard::sc_to_kc[ 2 ][ 128 ] = { { 0, 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
        45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
        63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 84, 00, 00, 86, 87, 88, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
        00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
        00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00 },
        { 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 97, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 100, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 102, 103, 104, 00, 105, 00, 106, 00, 107, 108, 109,
            110, 111, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00 } };

    const char* Keyboard::lower_ascii[ 112 ] = { "(0)", "(1)", "1", "2", "3",
        "4", "5", "6", "7", "8", "9", "0", "(12)", "(13)", "<return>", "(15)",
        "q", "w", "e", "r", "t", "z", "u", "i", "o", "p", "ü", "+", "\n",
        "(29)", "a", "s", "d", "f", "g", "h", "j", "k", "l", "ö", "a", "(41)",
        "<left shift>", "#", "y", "x", "c", "v", "b", "n", "m", ",", ".", "-",
        "<right shift>", "(55)", "(56)", " ", "(58)", "(59)", "(60)", "(61)",
        "<f4>", "<f5>", "(64)", "(65)", "(66)", "(67)", "(68)", "(69)", "(70)",
        "(71)", "(72)", "(73)", "(74)", "(75)", "(76)", "(77)", "(78)", "(79)",
        "(80)", "(81)", "(82)", "(83)", "(84)", "(85)", "<", "(87)", "(88)",
        "(89)", "(90)", "(91)", "(92)", "(93)", "(94)", "(95)", "(96)", "(97)",
        "(98)", "(99)", "(100)", "(101)", "(102)", "(103)", "(104)", "(105)",
        "(106)", "(107)", "(108)", "(109)", "(110)" };

    const char* Keyboard::upper_ascii[ 112 ] = { "(0)", "(1)", "!", "\"", "§",
        "$", "%", "&", "/", "(", ")", "=", "(12)", "(13)", "<return>", "(15)",
        "Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P", "Ü", "*", "\n",
        "(29)", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Ö", "Ä", "(41)",
        "<left shift>", "'", "Y", "X", "C", "V", "B", "N", "M", ";", ":", "_",
        "<right shift>", "(55)", "(56)", " ", "(58)", "(59)", "(60)", "(61)",
        "<f4>", "<f5>", "(64)", "(65)", "(66)", "(67)", "(68)", "(69)", "(70)",
        "(71)", "(72)", "(73)", "(74)", "(75)", "(76)", "(77)", "(78)", "(79)",
        "(80)", "(81)", "(82)", "(83)", "(84)", "(85)", ">", "(87)", "(88)",
        "(89)", "(90)", "(91)", "(92)", "(93)", "(94)", "(95)", "(96)", "(97)",
        "(98)", "(99)", "(100)", "(101)", "(102)", "(103)", "(104)", "(105)",
        "(106)", "(107)", "(108)", "(109)", "(110)" };

    int Keyboard::translate_scancode( int a, int b ) {
      if ( a == 0 )
        return sc_to_kc[ 0 ][ b ];
      else if ( a == 1 )
        return sc_to_kc[ 1 ][ b ];
      else if ( b == 0x451d )
        return 119;
      else
        return 0x00;
    }

    bool Keyboard::call() {
      int scancode = 0;
      int keycode = 0;
      int break_code = 0;

      scancode = lib::inb( 0x60 );

      // Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
      // es kein e0 oder e1 fuer einen Extended-scancode ist
      if ( ( scancode & 0x80 ) != 0 and ( e1_code != 0 or scancode != 0xE1 )
          and ( e0_code == 1 or scancode != 0xE0 ) ) {
        break_code = 1;
        scancode = scancode & 0x7F;
      }
      if ( e0_code == 1 ) {
        if ( ( scancode == 0x2A ) or ( scancode == 0x36 ) ) {
          e0_code = 0;
        }
        else {
          keycode = translate_scancode( 1, scancode );
          e0_code = 0;
        }
      }

      else if ( e1_code == 2 ) {
        // Fertiger e1-Scancode
        // Zweiten Scancode in hoeherwertiges Byte packen
        e1_prev = e1_prev | ( scancode << 8 );
        keycode = translate_scancode( 2, e1_prev );
        e1_code = 0;
      }
      else if ( e1_code == 1 ) {
        // Erstes Byte fuer e1-Scancode
        e1_prev = scancode;
        e1_code += 1;
      }
      else if ( scancode == 0xE0 ) {
        // Anfang eines e0-Codes
        e0_code = 1;
      }
      else if ( scancode == 0xE1 ) {
        // Anfang eines e1-Codes
        e1_code = 1;
      }
      else {
        // Normaler Scancode
        keycode = translate_scancode( 0, scancode );
      }

      if ( keycode != 0 ) {
        // Keycodes an Anwendungen verteilen, die sich dafuer interessieren.

        event.keycode = keycode;

        if ( holded[ 42 ] || holded[ 54 ] )
          event.token = upper_ascii[ keycode ];
        else if ( holded[ 100 ] and keycode == 9 )
          event.token = "[";
        else if ( holded[ 100 ] and keycode == 10 )
          event.token = "]";
        else if ( holded[ 100 ] and keycode == 8 )
          event.token = "{";
        else if ( holded[ 100 ] and keycode == 11 )
          event.token = "}";
        else
          event.token = lower_ascii[ keycode ];

        event.break_code = break_code;

        system->video << event.token;

        for ( uint32 i = 0; i < listeners.size(); ++i ) {
          listeners.get( i )->event( &event );
        }

        holded[ keycode ] = !break_code;
      }

      return true;
    }

  }

}
