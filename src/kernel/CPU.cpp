/**
 * CPU.cpp
 *
 * @date 06.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "CPU.hpp"

namespace kernel {

  void CPU::Info::print( lib::stream::Out& o ) {
    uint32 a[ 4 ];

    lib::cpuid( 0x02, a );

    uint32 flags = a[ 3 ];

    if ( flags & FPU ) {
      o << "FPU ";
    }

    if ( flags & VME ) {
      o << "VME ";
    }

    if ( flags & DE ) {
      o << "DE ";
    }

    if ( flags & PSE ) {
      o << "PSE ";
    }

    if ( flags & TSC ) {
      o << "TSC ";
    }

    if ( flags & MSR ) {
      o << "MSR ";
    }

    if ( flags & PAE ) {
      o << "PAE ";
    }

    if ( flags & MCE ) {
      o << "MCE ";
    }

    if ( flags & CX8 ) {
      o << "CX8 ";
    }

    if ( flags & APIC ) {
      o << "APIC ";
    }

    if ( flags & SEP ) {
      o << "SEP ";
    }

    if ( flags & MTTR ) {
      o << "MTTR ";
    }

    if ( flags & PGE ) {
      o << "PGE ";
    }

    if ( flags & MCA ) {
      o << "MCA ";
    }

    if ( flags & CMOV ) {
      o << "CMOV ";
    }

    if ( flags & PAT ) {
      o << "PAT ";
    }

    if ( flags & PSE_36 ) {
      o << "PSE-36 ";
    }

    if ( flags & PSN ) {
      o << "PSN ";
    }

    if ( flags & CLFSH ) {
      o << "CLFSH ";
    }

    if ( flags & DS ) {
      o << "DS ";
    }

    if ( flags & ACPI ) {
      o << "ACPI ";
    }

    if ( flags & MMX ) {
      o << "MMX ";
    }

    if ( flags & FXSR ) {
      o << "FXSR ";
    }

    if ( flags & SSE ) {
      o << "SSE ";
    }

    if ( flags & SSE2 ) {
      o << "SSE2 ";
    }

    if ( flags & SS ) {
      o << "SS ";
    }

    if ( flags & HTT ) {
      o << "HTT ";
    }

    if ( flags & TM ) {
      o << "TM ";
    }

    if ( a[ 2 ] & 1 ) {
      o << "SSE3";
    }

    if ( a[ 2 ] & 1 << 3 ) {
      o << "MONITOR";
    }

    if ( a[ 2 ] & 1 << 5 ) {
      o << "VMX";
    }

    if ( a[ 2 ] & 1 << 9 ) {
      o << "SSSE3";
    }
  }

  CPU::CPU() {

  }

  CPU::~CPU() {
  }

}
