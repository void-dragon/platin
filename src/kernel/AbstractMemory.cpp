/**
 * AbstractMemory.cpp
 *
 * @date 12.05.2012
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "AbstractMemory.hpp"
#include <lib/Exception.hpp>

namespace kernel {

  void AbstractMemory::node_put( Area* node ) {
    if ( node_addr_pointer > 1000 ) {
      lib::Exception::throwing( "to many stack nodes" );
    }

    node_addr_pointer++;
    uint32 pid = node_addr_pointer % 1024;

    node_addr_stack[ pid ] = ( uint32 ) node;
  }

  AbstractMemory::Area* AbstractMemory::node_get() {

    if ( node_addr_pointer == 0 ) {
      lib::Exception::throwing( "out of stack nodes" );
    }

    uint32 pid = node_addr_pointer % 1024;
    node_addr_pointer--;

    return ( Area* ) node_addr_stack[ pid ];
  }

  void AbstractMemory::init( Area* a ) {
    a->key = 0;
    a->val = 0;
    a->level = 1;
    a->left = 0;
    a->right = 0;
  }

  void AbstractMemory::clear( Area* a ) {
    a->level = 1;
    a->left = 0;
    a->right = 0;
  }

  AbstractMemory::AbstractMemory() {
    node_addr_stack = 0;
    node_addr_pointer = 0;
    node_last_block = 0;
    memsize = 0;
    memused = 0;
  }

  AbstractMemory::~AbstractMemory() {
  }

}
