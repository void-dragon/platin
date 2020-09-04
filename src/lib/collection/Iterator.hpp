/**
 * Iterator.hpp
 *
 * @since 10.07.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef ITERATOR_HPP_
#define ITERATOR_HPP_

namespace lib {

  namespace collection {

    /**
     * An interface for an iterator.
     */
    template< class T > class Iterator {
      public:
        /**
         * Returns the Object of the current slot.
         */
        virtual T& operator*() = 0;

        /**
         * Goes to the next element.
         */
        virtual void operator++() = 0;

        /**
         * Indicates if the current element is a valid element.
         */
        virtual operator bool() = 0;

        virtual ~Iterator() {
        }
    };

  }

}

#endif /* ITERATOR_HPP_ */
