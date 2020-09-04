/**
 * Elf32.cpp
 *
 * @since 08.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#include "Elf32.hpp"

#include <kernel/System.hpp>

namespace kernel {

  namespace driver {

    namespace fileformat {

      Elf32::Elf32( lib::File* file )
          : _file( file ) {
        file->read( ( void** ) &data, &length );

        header = ( Header* ) data;

        // is file an ELF file?
        if ( header->ident[ 0 ] == 0x7f && header->ident[ 1 ] == 'E' && header->ident[ 2 ] == 'L'
            && header->ident[ 3 ] == 'F' ) {
          progammheaders = new ProgrammHeader*[ header->phnum ];

          for ( uint32 i = 0; i < header->phnum; ++i )
            progammheaders[ i ] = ( ProgrammHeader* ) ( data + header->phoff + i * header->phentsize );
        }
      }

      void Elf32::execute() {
        if ( header->type == Header::TYPE_EXEC ) {
          new Elf32Process( this );
        }
      }

      Elf32::~Elf32() {
        delete data;
      }

      Elf32Process::Elf32Process( Elf32* Elf ) {
        for ( uint32 i = 0; i < Elf->header->phnum; ++i ) {
          if ( Elf->progammheaders[ i ]->type == Elf32::ProgrammHeader::LOAD ) {
            system->video << Elf->progammheaders[ i ]->filesz << " : " << Elf->progammheaders[ i ]->memsz << "\n";
            system->video.hex( Elf->progammheaders[ i ]->vaddr );
            system->video << " : " << Elf->progammheaders[ i ]->offset << "\n";
            system->video.hex( Elf->header->entry );
            system->video << "\n";

            virtual_memory.map( ( uint32 ) ( Elf->data + Elf->progammheaders[ i ]->offset ),
                Elf->progammheaders[ i ]->vaddr );
            virtual_memory.map( 0xb8000, 0xb8000 );
          }
        }

        new Thread( this, Elf->header->entry, 4000 );
      }

    }

  }

}
