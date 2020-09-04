/**
 * PCI.hpp
 *
 * @since 29.03.2010
 * @author Arne Simon => email::[arne_simon@gmx.de]
 */

#ifndef PCI_HPP_
#define PCI_HPP_

#include <cpp.hpp>
#include <lib/collection/Map.hpp>
#include <lib/sync/Mutex.hpp>
#include <lib/Signal.hpp>
#include <lib/File.hpp>

namespace kernel {

  namespace driver {

    /**
     * Peripheral Component Interconnect
     *
     * @section layout The layout of an configuration entrie.
     * @code
     * +------------+-----------+------------+---------------+-----------------+-----------------+---------+
     * |   31       |  30 - 24  |   23 - 16  |    15 - 11    |     10 - 8      |     7 - 2       |  1 - 0  |
     * | Enable Bit | Reserved  | Bus Number | Device Number | Function Number | Register Number |   00    |
     * +------------+-----------+------------+---------------+-----------------+-----------------+---------+
     @endcode
     *
     * @note http://lowlevel.brainsware.org/wiki/index.php/PCI
     * @note http://wiki.osdev.org/PCI
     * @note http://www.pcidatabase.com/
     * @note http://www.pcidatabase.com/pci_c_header.php
     *
     */
    class PCI: public lib::File {
      public:
        static const uint32 config_address = 0xCF8;
        static const uint32 config_data = 0xCFC;

        static const uint16 CMD_IO_Space = 0x01;
        static const uint16 CMD_Memory_Space = ( 1 << 1 );
        static const uint16 CMD_Bus_Master = ( 1 << 2 );
        static const uint16 CMD_Special_Cycles = ( 1 << 3 );
        static const uint16 CMD_Memory_Write = ( 1 << 4 );
        static const uint16 CMD_VGA_Palette_Snoop = ( 1 << 5 );
        static const uint16 CMD_Parity_Error = ( 1 << 6 );
        static const uint16 CMD_IDSEL = ( 1 << 7 );
        static const uint16 CMD_SERR = ( 1 << 8 );
        static const uint16 CMD_Back_to_Back = ( 1 << 9 );
        static const uint16 CMD_Interrupt_Disable = ( 1 << 10 );

        /**
         * A configuration block for a PCI device.
         *
         * @note If you wonder why are most of this values twisted compared with the documentation?
         *    This is because of litte Endian issues.
         *    Remeber :
         *
         *    union U {
         *        struct {
         *            uint8 a;
         *            uint8 b;
         *            uint8 c;
         *            uint8 d;
         *        };
         *        uint16 data[2];
         *   } myU;
         *
         *   myU.data[0] = 0xAAFF;
         *
         *   This will set myU.a = 0xFF and myU.b = 0xAA, because of little Endian !!!
         */
        union ConfigBlock {
            struct {
                uint16 vendorID;
                uint16 deviceID;
                uint16 command;
                uint16 status;
                uint8 revisionID;
                uint8 progif;
                uint8 subclass;
                uint8 classcode;
                uint8 cache_line_size;
                uint8 latency_timer;
                uint8 header_type;
                uint8 BIST;

                /**
                 * BAR-Layout:
                 * +---------------+----+-----+---+
                 * |    31-4       | 3  | 2-1 | 0 |
                 * | Base Address  | P  | Typ | 0 |
                 * +---------------+----+-----+---+
                 */
                union {
                    struct {
                        uint32 bar0_addr;
                        uint32 bar1_addr;
                        uint32 bar2_addr;
                        uint32 bar3_addr;
                        uint32 bar4_addr;
                        uint32 bar5_addr;
                        uint32 cardbus_cis;
                        uint16 subsystem_vendor;
                        uint16 subsystem_id;
                        uint32 expansion_rom_addr;
                        uint8 capabilities_Pointer;
                        uint8 reserved1;
                        uint16 reserved2;
                        uint32 reserved3;
                        uint8 interrupt_line;
                        uint8 interrupt_pin;
                        uint8 min_grant;
                        uint8 max_latency;
                    }__attribute__((__packed__)) standart; ///< header type 0x00.

                    struct {
                        uint32 bar0_addr;
                        uint32 bar1_addr;
                    } bridge; ///< pic-to-pci bridge, type 0x01.
                };
            }__attribute__((__packed__));

            uint32 data[ 16 ];

            void read( uint32 bus, uint32 device, uint32 func );

            void write( uint32 bus, uint32 device, uint32 func );

            static bool isValid( uint32 bus, uint32 device, uint32 func );

        }__attribute__((__packed__));

        /**
         * @section sr Status-Register
         * <table>
         * <tr>
         * <th> Bit </th>
         * <th> Name </th>
         * <th> Typ </th>
         * <th> Bedeutung </th>
         * </tr>
         * <tr>
         * <td> 3 </td>
         * <td> Interrupt Status </td>
         * <td> RO&#160;/&#160;dynamic </td>
         * <td> Wenn dieses Bit auf 1 gesetzt ist möchte das betreffende PCI-Gerät einen Interrupt über sein #INT?-Signal melden. Nur wenn das Bit 10 im Command-Register gelöscht ist wird auch tatsächlich ein Interrupt (an den PIC o.ä.) signalisiert. Falls das PCI-Gerät keine Interrupts benutzt oder diese ausschließlich per <a href="/wiki/MSI" title="MSI" class="mw-redirect">MSI</a>(-X) signalisiert kann dieses Bit fest auf 0 geklemmt sein. Auf Bridges meldet dieses Bit nur eigene Interrupts, durch geleitete Interrupts, von sekundären Bussen zum übergeordnetem Bus, beeinflussen dieses Bit nicht.</td>
         * </tr>
         * <tr>
         * <td> 4 </td>
         * <td> Capabilities List </td>
         * <td> RO&#160;/&#160;fix </td>
         * <td> Wenn dieses Bit auf 1 gesetzt ist ist der Capabilities-Pointer (Offset 0x034) gültig.<br />Bei PCI-Express, Hypertransport und PCI-X (und wahrscheinlich auch AGP) muss dieses Bit immer gesetzt sein da dort bus-spezifische Capabilities Pflicht sind.</td>
         * </tr>
         * <tr>
         * <td> 5 </td>
         * <td> 66 MHz Capable </td>
         * <td> RO&#160;/&#160;fix </td>
         * <td> Dieses Bit gibt an ob eine PCI-Karte nicht nur mit maximal 33 MHz-Bus-Takt arbeiten kann sondern bis zu 66 MHz verträgt. Nur wenn alle PCI-Geräte auf dem betreffenden physischen PCI-Bus-Segment mit mehr als 33 MHz umgehen können darf dieses Bus-Segment auch mit mehr als 33 MHz, bis maximal 66 MHz, betrieben werden.<br />Dieses Bit ist nur beim klassischen PCI gültig und bei allen PCI-Nachfolgern immer fest auf 0 geklemmt, außer bei AGP dort ist es immer fest auf 1 geklemmt. Für die Software hat dieses Bit keine Bedeutung, auf dem PCI-Bus wird die Geschwindigkeit in Hardware ausgehandelt, wenn die Software auf den Konfigurations-Adressraum der angeschlossenen Geräte zugreifen kann ist der Bus bereits funktionsfähig.</td>
         * </tr>
         * <tr>
         * <td> 7 </td>
         * <td> Fast Back-to-Back Transactions Capable </td>
         * <td> RO&#160;/&#160;fix </td>
         * <td> Wenn diese Bit gesetzt ist bedeutet dasss dass das betreffende PCI-Gerät die minimal schnelleren Back-to-Back Transfers unterstützt. Als Target werden diese Transfers immer unterstützt, aber als Master dürfen diese Transfers nur benutzt werden falls das Bit 9 im Command-Register gesetzt ist.</td>
         * </tr>
         * <tr>
         * <td> 8 </td>
         * <td> Master Data Parity Error </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i>
         * </td></tr>
         * <tr>
         * <td> 10-9 </td>
         * <td> DEVSEL Timing </td>
         * <td> RO&#160;/&#160;fix </td>
         * <td> Diese 2 Bits, als unsigned Integer betrachtet, geben an nach wie vielen PCI-Bus-Takten das PCI-Gerät erkannt hat ob es das Target eines Zugriffs ist. Gültige Werte sind 0, 1 und 2. Der Wert 3 darf nicht verwendet werden, jedes PCI-Gerät muss nach spätestens 2 Takten dekodiert haben ob es das gewünschte Target ist oder nicht. Im 3ten Takt wird dann eine eventuell vorhandene Subtractive-Bridge aktiv und nimmt den Transfer entgegen oder der Zugriff geht ins Leere und der Initiator meldet das als Fehler.</td>
         * </tr>
         * <tr>
         * <td> 11 </td>
         * <td> Signalled Target-Abort </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i>
         * </td></tr>
         * <tr>
         * <td> 12 </td>
         * <td> Received Target-Abort </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i></td>
         * </tr>
         * <tr>
         * <td> 13 </td>
         * <td> Received Master-Abort </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i>
         * </td>
         * </tr>
         * <tr>
         * <td> 14 </td>
         * <td> Signalled System Error </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i>
         * </td>
         * </tr>
         * <tr>
         * <td> 15 </td>
         * <td> Detected Parity Error </td>
         * <td> RW1C&#160;/&#160;dynamic </td>
         * <td> <i>TODO</i>
         * </td></tr></table>
         * <p>Nicht beschriebene Bits sind reserviert und liefern beim lesen eine 0. Die Bits im Status-Register sind entweder Read-Only (RO) oder lassen sich durch beschreiben mit einer 1 rücksetzen (RW1C), das beschreiben mit einer 0 hat keinen Effekt. Einige Bits ändern ihren Zustand nie (fix) und andere können ihren Zustand durch bestimmte Ereignisse ändern (dynamic).</p>
         *
         * @section cr Command-Register
         * <table class="prettytable">
         * <tr>
         * <th> Bit </th>
         * <th> Name </th>
         * <th> Bedeutung
         * </th></tr>
         * <tr>
         * <td> 0 </td>
         * <td> IO Space </td>
         * <td> Nur wenn dieses Bit gesetzt ist, kann der I/O-Raum des Geräts verwendet werden, sonst ist er deaktiviert.<br />Falls das Gerät keine IO-Ports anbietet, ist dieses Bit fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 1 </td>
         * <td> Memory Space </td>
         * <td> Nur wenn dieses Bit gesetzt ist, können MMIO-Register verwendet werden, sonst sind diese deaktiviert.<br />Falls das Gerät kein MMIO (oder andere Speicherbereiche) anbietet, ist dieses Bit fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 2 </td>
         * <td> Bus Master </td>
         * <td> Ist dieses Bit gesetzt, so darf das Gerät als Busmaster auf dem PCI-Bus arbeiten. Das ist z.&#160;B. für PCI-DMA notwendig (wird u.&#160;a. von Netzwerkkarten und (S)ATA-Host-Controllern verwendet).<br />Falls das Gerät nicht busmasterfähig ist, ist dieses Bit fest auf 0 geklemmt.<br />Wenn dieses Bit auf 0 steht, wird damit auch <a href="/wiki/MSI" title="MSI" class="mw-redirect">MSI</a>(-X) unterbunden.
         * </td></tr>
         * <tr>
         * <td> 3 </td>
         * <td> Special Cycles </td>
         * <td> Wenn dieses Bit gelöscht ist, dann muss das Gerät alle Special Cycles auf dem PCI-Bus ignorieren. Special Cycles werden zum Broadcasten von speziellen Nachrichten an alle PCI-Geräte benutzt.<br />Es gibt nur wenige PCI-Geräte, bei denen dieses Bit überhaupt beschreibbar ist, bei den meisten ist es fest auf 0 geklemmt. Bei PCI-Express ist es immer fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 4 </td>
         * <td> Memory Write and Invalidate Enable </td>
         * <td> Falls gesetzt, kann das Gerät diesen Befehl (Memory Write and Invalidate) ausführen, sonst muss es sich mit einem einfachen Memory Write begnügen.<br />Bei PCI-Express ist dieses Bit immer fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 5 </td>
         * <td> VGA Palette Snoop </td>
         * <td> Dieses Bit ist nur für VGA-kompatible Geräte interessant (ist es gesetzt, so werden Zugriffe auf die VGA-Palettenregister gesondert behandelt), auf allen anderen Geräten ist es immer fest auf 0 geklemmt.<br />Bei PCI-Express und Hypertransport ist dieses Bit immer fest auf 0 geklemmt, dort wird sowas in den Bridges konfiguriert.
         * </td></tr>
         * <tr>
         * <td> 6 </td>
         * <td> Parity Error Response </td>
         * <td> Ist dieses Bit gesetzt, so wird bei Parityfehlern auf dem PCI-Bus das Detected-Parity-Error-Statusbit im Statusregister gesetzt. Nicht jede PCI-Hardware unterstützt die Generierung und Prüfung des Paritybits auf dem PCI-Bus, sodass dieses Bit dann fest auf 0 geklemmt ist. Daher sollte dieses Bit nur dann auf 1 gesetzt werden, wenn alle PCI-Geräte auf dem betreffenden physischen PCI-Bus-Segment die Parity-Bit-Generierung unterstützen.<br />Bei PCI-Express und Hypertransport gibt es leistungsfähigere Fehlererkennungsmechanismen, deren Benutzung zu bevorzugen ist.
         * </td></tr>
         * <tr>
         * <td> 7 </td>
         * <td> IDSEL Stepping /<br />Wait Cycle Control </td>
         * <td> Dieses Bit war nur in den ersten PCI-Spezifikationen gültig (vor 2.0) und muss immer auf 0 gesetzt werden. Auf allen PCI-Geräten seit der PCI-Spezifikation 2.0 (ab 1993) ist dieses Bit immer fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 8 </td>
         * <td> SERR# Enable </td>
         * <td> Wenn dieses Bit gesetzt ist darf/soll das Gerät erkannte schwerwiegende Fehler (z.&#160;B. Parityfehler während der Adressierungsphase oder ungültige Kommandos) über die extra #SERR-Leitung, oder bei PCI-Express über spezielle Messages, melden. Solche Fehler lösen für gewöhnlich einen NMI oder eine Machine-Check-Exception aus.<br />Bei PCI-Express und Hypertransport gibt es leistungsfähigere Fehlererkennungsmechanismen, deren Benutzung zu bevorzugen ist.
         * </td></tr>
         * <tr>
         * <td> 9 </td>
         * <td> Fast Back-to-Back Enable </td>
         * <td> Wenn dieses Bit auf 1 gesetzt ist soll das PCI-Gerät Back-to-Back-Zugriffe (als Target) unterstützen und darf auch solche Zugriffe (als Master) ausführen. Dieses Bit lässt sich nur auf 1 setzen, wenn das zugehörige Bit 7 im Status-Register diese Fähigkeit signalisiert, andernfalls ist es fest auf 0 geklemmt. Dieses Bit darf nur dann auf 1 gesetzt werden wenn alle PCI-Geräte auf dem betreffenden physischen PCI-Bus-Segment Back-to-Back-Zugriffe unterstützen. Wenn alle Geräte diesen Modus unterstützen, kann die Idle-Phase zwischen zwei PCI-Zyklen entfallen. Das erhöht etwas den Datendurchsatz auf dem PCI-Bus.<br />Bei PCI-Express, Hypertransport und PCI-X ist dieses Bit immer fest auf 0 geklemmt.
         * </td></tr>
         * <tr>
         * <td> 10 </td>
         * <td> Interrupt Disable </td>
         * <td> Ist dieses Flag gesetzt, dann kann das entsprechende Gerät keine IRQs auslösen. Nach dem Hardware-Reset muss dieses Bit dummerweise immer gelöscht sein, sodass IRQs anfangs nicht verboten sind. MSI(-X) ist davon unabhängig, dafür gibt es spezielle Interrupt-Enable-Bits in den entsprechenden Capabilities.
         * </td></tr></table>
         * <p>Bit 7 ist auf 0 zu setzen, Bits 11 bis 15 sind reserviert (und undefiniert) und müssen ebenfalls immer auf 0 gesetzt werden. Nach dem Reset des Geräts sind alle hier aufgeführten Bits auf 0 gesetzt (wobei das BIOS wohl einige davon setzen wird). Die Software sollte immer prüfen, ob sich die gewünschten Bits auch wirklich auf 1 setzen lassen, einige Bits sind eben fest auf 0 geklemmt. Es sind zwar nicht alle Bits beschreibbar aber kein Bit im Command-Register wird seinen Zustand von alleine ändern.
         * </p>
         */
        class Device {
          public:
            ConfigBlock config;
            uint32 classCodeIdx;
            uint32 deviceCodeIdx;
            uint8 bus;
            uint8 device;
            uint8 func;

            /**
             * Creates a new PCI.Device by bus, device, function code.
             * @param B
             * @param D
             * @param F
             */
            Device( uint8 B, uint8 D, uint8 F );
        };

        PCI();

        typedef lib::collection::Map< uint32, Device* > DeviceMap;
        DeviceMap devices;
        lib::Signal< Device* > deviceDetected;

        /**
         * @attention Scans only the first 10 busses, instead of all 256, because it is extreme unlikely to see more than 10 busses!
         */
        void scan( bool print );

        virtual ~PCI();

    };

  }
}

#endif /* PCI_HPP_ */
