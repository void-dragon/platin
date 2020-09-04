/**
 * SwapFS.hpp
 *
 * @since 19.07.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef SWAPFS_HPP_
#define SWAPFS_HPP_

namespace kernel {
  namespace driver {
    namespace filesystem {

      /**
       *
       */
      class SwapFS {
        public:
          SwapFS();
          virtual ~SwapFS();
      };

    } /* namespace filesystem */
  } /* namespace driver */
} /* namespace kernel */

#endif /* SWAPFS_HPP_ */
