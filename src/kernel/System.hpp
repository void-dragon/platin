/**
 * System.hpp
 *
 * @date 05.02.2011
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <MultiBoot.hpp>
#include <kernel/PhysicalMemory.hpp>
#include <kernel/CMOS.hpp>
#include <kernel/CPU.hpp>
#include <kernel/PIT.hpp>
#include <kernel/Video.hpp>
#include <kernel/Thread.hpp>
#include <kernel/InterruptHandler.hpp>
#include <kernel/ISR.hpp>
#include <kernel/TSS.hpp>
#include <kernel/User.hpp>
#include <lib/collection/RingBuffer.hpp>
#include <lib/collection/Ring.hpp>
#include <lib/collection/List.hpp>
#include <lib/collection/Set.hpp>

/**
 * @attention Before this method, this commands are called
 *            in the following order:
 *            <ol>
 *              <li>push ebp</li>
 *              <li>push edi</li>
 *              <li>push esi</li>
 *              <li>push edx</li>
 *              <li>push ecx</li>
 *              <li>push ebx</li>
 *              <li>push eax</li>
 *              <li>push ss</li>
 *              <li>push gs</li>
 *              <li>push fs</li>
 *              <li>push es</li>
 *              <li>push ds</li>
 *              <li>push cr0</li>
 *              <li>push cr3</li>
 *            </ol>
 *
 * @attention After this method, this commands are called
 *            in the following order:
 *            <ol>
 *              <li>cr3</li>
 *              <li>cr0</li>
 *              <li>pop ds</li>
 *              <li>pop es</li>
 *              <li>pop fs</li>
 *              <li>pop gs</li>
 *              <li>pop ss</li>
 *              <li>pop eax</li>
 *              <li>pop ebx</li>
 *              <li>pop ecx</li>
 *              <li>pop edx</li>
 *              <li>pop esi</li>
 *              <li>pop edi</li>
 *              <li>pop ebp</li>
 *            </ol>
 */
extern "C" kernel::Thread::State* isrcallback( kernel::Thread::State* state );

namespace kernel {

  /**
   * System will setup the following components of the CPU:
   * <ol>
   *    <li>GDT - Global Description Table</li>
   *    <li>IDT - Interrupt Description Table</li>
   *    <li>PIC - Programm Interrupt Controller</li>
   *    <li>PIT - Programmable Interrupt Timer</li>
   * </ol>
   *
   * In addition to, the Programmable Interval Timer(PIT) will be turned of!
   *
   * @attention At the current state the core system is only
   *            designed and tested to run on a x86 CPUs.
   *
   * @note Kernel would run on multi core systems but uses only one core!!!
   *
   * @todo APIC usage for multi-core systems.
   */
  class System: public Process {
    public:
      typedef lib::collection::RingBuffer< uint8, 20 > Interrupts;
      typedef lib::collection::Ring< Thread* > SchedulingPlan;
      typedef lib::collection::List< ISR* > ISRList;
      typedef lib::collection::List< User* > Users;
      typedef lib::collection::List< Process* > Processes;

      static const uint8 EntryCount = 4;
      static const uint16 IRQCount = 256;

      /**
       * The address for the IDT and GDT.
       */
      struct Address {
          uint16 size; ///< Size of the table subtracted by 1.
          uint32 offset; ///< Virtual Address offset of the table itself.
      }__attribute__((__packed__));

      static Address gdt_address;
      static Address idt_address;

      //--- GDT Flags ---

      static const uint16 GDT_DATASEG = 0x02;
      static const uint16 GDT_CODESEG = 0x0a;
      static const uint16 GDT_TSS = 0x09;

      static const uint16 GDT_SEGMENT = 0x10;
      static const uint16 GDT_RING0 = 0x00;
      static const uint16 GDT_RING3 = 0x60;
      static const uint16 GDT_PRESENT = 0x80;

      static const uint16 GDT_GRAN_4K = 0x800;
      static const uint16 GDT_BIT32 = 0x400;

      //--- PIC Flags ---

      static const uint8 PIC_MasterCmd = 0x20;
      static const uint8 PIC_MasterData = 0x21;
      static const uint8 PIC_Master_Offset = 0x20;
      static const uint8 PIC_SlaveCmd = 0xa0;
      static const uint8 PIC_SlaveData = 0xa1;
      static const uint8 PIC_Slave_Offset = 0x28;

      static const uint8 PIC_EOI = 0x20;

      static const uint8 PIC_ICW1_ICW4 = 0x01; ///< ICW4 (not) needed
      static const uint8 PIC_ICW1_SINGLE = 0x02; ///< Single (cascade) mode
      static const uint8 PIC_ICW1_INTERVAL4 = 0x04; ///< Call address interval 4 (8)
      static const uint8 PIC_ICW1_LEVEL = 0x08; ///< Level triggered (edge) mode
      static const uint8 PIC_ICW1_INIT = 0x10; ///< Initialization - required!

      static const uint8 PIC_ICW4_8086 = 0x01; ///< 8086/88 (MCS-80/85) mode
      static const uint8 PIC_ICW4_AUTO = 0x02; ///< Auto (normal) EOI
      static const uint8 PIC_ICW4_BUF_SLAVE = 0x08; ///< Buffered mode/slave
      static const uint8 PIC_ICW4_BUF_MASTER = 0x0C; ///< Buffered mode/master
      static const uint8 PIC_ICW4_SFNM = 0x10; ///< Special fully nested (not)

      /**
       * GDT Descriptor.
       *
       * @note http://wiki.osdev.org/Global_Descriptor_Table
       */
      static uint64 gdt_table[ EntryCount ];

      /**
       * IDT Descriptor.
       *
       * @note From http://wiki.osdev.org/IDT
       * @note From http://www.logix.cz/michal/doc/i386/chp09-05.htm
       */
      static uint64 idt_table[ IRQCount ];
      static TSS tss;

      static void set_gdt( uint32 idx, uint32 base, uint32 limit, uint32 flags );

      /**
       * Sets up the global description table.
       *
       * The whole memory is marked as data and memory segment,
       * because we handle security issues with paging.
       */
      static void setup_gdt();

      /**
       * Sets a method into the interrupt description table.
       *
       * @param IRQ The interrupt on which the method will be called.
       *
       * @param F The pointer to a function which will be executed.
       */
      static void set_isr( uint8 IRQ, uint32 F );

      /**
       * Sets up the interrupt description table.
       *
       * The IDT maps, in protected mode, the IRQs to the
       * interrupt service routines.
       *
       * We fill this table with ASM interrupt-routines, which call
       * our general interrupt handler, isrcallback.
       */
      static void setup_idt();

      /**
       * Setup the PIC.
       * @note http://wiki.osdev.org/PIC#Programming_the_PIC_chips
       */
      static void setup_pic();

      static void eoi( int IRQ );

      /**
       * Switches to the given page directory.
       * The page directory is used for the virtual memory management.
       *
       * @attention The address has to be 4Kb aligned.
       * @attention The physical address is required.
       *
       * @param PD The pointer to the page directory.
       */
      static void switchToPageDirectory( uint32* PD );

      /**
       * @return Is paging activated, then true, else false.
       */
      static bool isPaged();

      friend class PhysicalMemory;
    public:
      /**
       * Enables the paging of the system.
       */
      static void enablePaging();

      /**
       * Disables the paging of the system.
       */
      static void disablePaging();

    public:
      //Memory memory;
      static PhysicalMemory physical_memory; ///< The physical memory handler.
      Video video;
      PIT timer;
      InterruptHandler* interrupthandler;
      uint32 ProcessIDPool; ///< Holds the next usable id for a process.
      uint32 ThreadIDPool; ///< Holds the next usable id for a thread.
      Interrupts interrupts; ///< Fired interrupts which should be handled by the kernel.
      Processes processes; ///< A list of processes.
      SchedulingPlan* plan;
      SchedulingPlan::Iterator* next;
      lib::collection::Set<User*> users;

      /**
       * An array of interrupt service routine lists.
       *
       * Interrupt list:
       * @code
       * +-----------+-------------------------------+
       * | Interrupt |           Function            |
       * +-----------+-------------------------------+
       * | 0x00      | Divide Error                  |
       * | 0x01      | Debug Exception               |
       * | 0x02      | RESERVED                      |
       * | 0x03      | Break Point Exception         |
       * | 0x04      | Overflow Error                |
       * | 0x05      | Bound Error                   |
       * | 0x06      | Invalid Opcode                |
       * | 0x07      | Device Not Aviable            |
       * | 0x08      | Double Fault                  |
       * | 0x09      | Coprocessor Segment Overrun   |
       * | 0x0A      | Invalid TSS                   |
       * | 0x0B      | Segment Not Present           |
       * | 0x0C      | Stack Fault                   |
       * | 0x0D      | General Protection Exception  |
       * | 0x0E      | Page Fault                    |
       * | 0x0F      | RESERVED                      |
       * | 0x10      | Floating Point Exception      |
       * | 0x11      | Align Check Exception         |
       * | 0x12      | Machine Check Exception       |
       * | 0x13      | Smid Floating Point Exception |
       * +-----------+-------------------------------+
       * | 0x20      | Timer, used for dispatch      |
       * | 0x21      | Keyboard                      |
       * +-----------+-------------------------------+
       * | 0x30      | System Exception              |
       * | 0x31      | Allocate Physical Page        |
       * | 0x32      | Free Physical Page            |
       * | 0x33      | Map Physical To Virtual Addr. |
       * | 0x34      | Allocate Virtual Memory       |
       * | 0x35      | Reallocate Virtual Memory     |
       * | 0x36      | Delete Virtual Memory         |
       * +-----------+-------------------------------+
       * @endcode
       *
       * @attention CPU Exception from 0x00 -> 0x13!
       * @attention The hardware interrupts are remapped to begin at 0x20!
       * @attention Interrupt 0x20 is the Timer/Dispatcher interrupt!
       * @attention The system interrupts begin at 0x30!
       * @attention Interrupt 0x30 is the standard exception!
       *
       */
      ISRList isr[ 100 ];

      /**
       * Setting up the absolute basic system.
       *
       * Here is all the mombo jumbo for setting up the GDT, IDT, Timer
       * and so on, for getting an runnable system in protected mode.
       *
       * @param size The size of the System class.
       * @param multiboot The MultiBoot structure created by GRUB.
       */
      static void* operator new( uint32 size, MultiBoot* multiboot );

      void initVirtualMemory();

      System();

      /**
       * Creates the scheduling plan.
       */
      void schedule();

      /**
       * Switches to the next threads.
       */
      Thread* dispatch();

      /**
       *
       * @param fromP
       * @param fromAddr
       * @param toP
       * @param toAddr
       * @param size
       */
      void copy( Process* fromP, uint32 fromAddr, Process* toP, uint32 toAddr, uint32 size );

      virtual ~System();

      //--- File functions ---

      uint64 size();

      void read( void** data, uint32* length );

      uint32 read( void* data, uint32 length );

      uint32 write( void* data, uint32 length );

      void* range( uint32 start, uint32 size );

      void range( uint32 start, uint32 size, void* data );
  };

}

extern kernel::System *system;

#endif /* SYSTEM_HPP_ */
