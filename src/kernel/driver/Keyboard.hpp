/**
 * Keyboard.hpp
 *
 * @since 07.10.2010
 * @author raup-tier
 */

#ifndef KEYBOARD_HPP_
#define KEYBOARD_HPP_

#include <lib/collection/Array.hpp>
#include <kernel/ISR.hpp>

namespace kernel {

  namespace driver {

    /**
     * The standard keyboard.
     */
    class Keyboard: public ISR {
      public:
        struct Event {
            const char* token;
            int keycode;
            int break_code;
        };

        struct EventListener {
            virtual void event( Event* e ) = 0;
        };

      protected:

        /// # scancodes from http://git.tyndur.org/?p=tyndur.git;a=blob;f=src/modules/kbc/sc2kc.c
        static const uint8 sc_to_kc[ 2 ][ 128 ];
        static const char* lower_ascii[ 112 ];
        static const char* upper_ascii[ 112 ];
        char holded[ 112 ];

        int e0_code;
        int e1_code;
        int e1_prev;

        Event event;

        int translate_scancode( int a, int b );
      public:

        lib::collection::Array< EventListener* > listeners;

        bool call();

        virtual ~Keyboard() {
        }
    };

  }

}

#endif /* KEYBOARD_HPP_ */
