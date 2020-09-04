/**
 * @mainpage Platin Code Documentation
 *
 * This is the documentation for source code of the
 * Platin kernel.
 * 
 * @section Features
 * <ul>
 *  <li>C++ - Implemented</li>
 *  <li>Paging</li>
 *  <li>Kernel-Paging</li>
 *  <li>Multi-Threaded Kernel</li>
 *  <li>Easy module creation</li>
 *  <li>Well documented</li>
 * </ul>
 *
 * @todo Use VirtualMemory manager for physical memory, no kernel level paging!!
 * @todo ELF executables
 */
#include <MultiBoot.hpp>

#include <kernel/System.hpp>
#include <kernel/Thread.hpp>
#include <kernel/Process.hpp>
#include <kernel/driver/Keyboard.hpp>
#include <kernel/driver/PCI.hpp>
#include <kernel/driver/ATA.hpp>
#include <kernel/driver/filesystem/Ext2.hpp>
#include <kernel/driver/filesystem/MyFS.hpp>
#include <kernel/driver/fileformat/Elf32.hpp>
#include <kernel/driver/graphic/Vesa.hpp>

#include <lib/std.hpp>
#include <lib/Time.hpp>

/**
 * The kernel main and init function.
 *
 * What this function does:
 * <ol>
 *  <li>Booting/Setup the system</li>
 *  <li>showing some system information</li>
 * </ol>
 *
 * @param multi_boot The multiboot structure from GRUB.
 */
void main( MultiBoot* multi_boot ) {

  new ( multi_boot ) kernel::System();

  // some stuff, so we see that the system is alive
  system->video.color( kernel::Video::Brown );
  system->video << "   Platin - " << __DATE__ << "-" << __TIME__;
  system->video.color( kernel::Video::LightGrey );
  system->video << "\n\n";

  uchar cpuName[ 48 ];
  lib::cpustring( cpuName );

  system->video.color( kernel::Video::Green );
  system->video << cpuName << "\nFlags: ";
  kernel::CPU::Info::print( system->video );
  system->video.color( kernel::Video::LightGrey );
  system->video << "\n";

  system->video.color( kernel::Video::Magenta );
  system->video << "Memory Size: " << system->physical_memory.memsize << " Byte \n";
  system->video.color( kernel::Video::LightGrey );

  kernel::driver::Keyboard key; // create keyboard driver

  system->isr[ 0x21 ].pushBack( &key ); // adding the keyboard to the interrupt service routines

  kernel::driver::PCI pci;

  pci.scan( true );

  kernel::driver::PCI::DeviceMap::Node* n = pci.devices.find( 0x010180 );

  if ( not n )
    n = pci.devices.find( 0x01018A );

  if ( n ) {
    kernel::driver::PCI::Device* device = n->value;

    kernel::driver::ATA* ide = new kernel::driver::ATA( device );

//      kernel::driver::MyFS* myfs = new kernel::driver::MyFS( &ide->drives[ 0 ], 3 );
//
//      kernel::driver::MyFS::File* r = myfs->root;
//
//      r->write( ( void* ) "hallo", 5 );
//
//      char* str;
//      uint32 len;
//
//      r->read( ( void** ) &str, &len );
//
//      system->video << "--" << len << "--" << "\n";
//      system->video.write( str, len );
//      system->video << "\n-----";

    kernel::driver::Ext2* ext2 = new kernel::driver::Ext2( &ide->drives[ 0 ], 0 );

    kernel::driver::Ext2::File* r = ext2->root;
    kernel::driver::fileformat::Elf32 *elf;

    if ( r->hasFiles() ) {
      lib::File::Files* files = r->files();

      lib::File::Files::KeyIterator* iter = files->keyIterator();
      lib::String filename( "PlatinPython" );

      while ( *iter ) {
        if ( ( **iter )->compare( &filename ) == 0 ) {
          elf = new kernel::driver::fileformat::Elf32( iter->value() );
          elf->execute();
        }
        ++( *iter );
      }

      delete iter;
      delete files;
    }
  }

  system->video << " -- ";

  system->timer.init( kernel::PIT::Mod_RateGenerator );
  system->timer.load( 6000 );
  lib::sti();

  // at this point our kernel thread is an idle thread!
  while ( true ) {
    asm volatile ("hlt;");
  }
}
