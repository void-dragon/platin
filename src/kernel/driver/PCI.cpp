/**
 * PCI.cpp
 *
 * @since 29.03.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "PCI.hpp"
#include <kernel/System.hpp>
#include <lib/std.hpp>
#include <lib/Exception.hpp>

namespace kernel {

  namespace driver {

    void PCI::ConfigBlock::read( uint32 bus, uint32 device, uint32 func ) {
      uint32 address;

      for ( uint8 offset = 0; offset < 16; offset++ ) {
        address = ( bus << 16 ) | ( device << 11 ) | ( func << 8 ) | offset << 2 | ( ( uint32 ) 0x80000000 );

        // write out the address
        lib::out( config_address, address );

        // read data
        data[ offset ] = lib::in( config_data );
      }

    }

    void PCI::ConfigBlock::write( uint32 bus, uint32 device, uint32 func ) {
      uint32 address;

      for ( uint8 offset = 0; offset < 16; offset++ ) {
        address = ( bus << 16 ) | ( device << 11 ) | ( func << 8 ) | offset << 2 | ( ( uint32 ) 0x80000000 );

        // write out the address
        lib::out( config_address, address );

        // write data
        lib::out( config_data, data[ offset ] );
      }

    }

    bool PCI::ConfigBlock::isValid( uint32 bus, uint32 device, uint32 func ) {
      uint32 address;
      uint32 tmp = 0;

      address = ( bus << 16 ) | ( device << 11 ) | ( func << 8 ) | ( ( uint32 ) 0x80000000 ); // calculate address

      // write out the address
      lib::out( config_address, address );

      tmp = lib::in( config_data ); // read data

      return ( tmp & 0xffff ) != 0xffff; // if 0xffff then its not valid
    }

    PCI::Device::Device( uint8 B, uint8 D, uint8 F )
        : bus( B ), device( D ), func( F ) {
      config.read( bus, device, func );
    }

    PCI::PCI() {

    }

    void PCI::scan( bool print ) {
      Device* d;

      // 8-bit for bus ids => 256 possible busses !!
      // but it seems unlikely that we ever see more than 10 busses...very unlikely!!
      for ( uint8 bus = 0; bus < 10; bus++ ) {

        // 5-bit for devices => 32 devices possible !
        for ( uint8 device = 0; device < 32; device++ ) {

          // 3-bit for funcs => 8 possible funcs
          for ( uint8 func = 0; func < 8; func++ ) {
            if ( ConfigBlock::isValid( bus, device, func ) ) {
              d = new Device( bus, device, func );

              if ( print ) {
                //debug_msgs->color( kernel::Video::LightGreen );
                system->video.hex( d->config.classcode );
                system->video << ".";
                system->video.hex( d->config.subclass );
                system->video << ".";
                system->video.hex( d->config.progif );
                system->video << "[";
                system->video.hex( d->config.vendorID );
                system->video << ":";
                system->video.hex( d->config.deviceID );
                system->video << "]";
                //debug_msgs->color( kernel::Video::LightGrey );
                system->video << "\n";
              }

              uint32 classid = ( uint32 ) d->config.classcode << 16 | ( uint32 ) d->config.subclass << 8
                  | d->config.progif;
              devices.put( classid, d );

              deviceDetected.emit( d );

            }
          }
        }
      }
    }

    PCI::~PCI() {
    }

  }

}
