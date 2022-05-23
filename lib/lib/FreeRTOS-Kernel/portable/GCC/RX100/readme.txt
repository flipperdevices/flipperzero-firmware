The following table shows which port is recommended to be used.


RX MCU Group    CPU     FPU         FPU         Port Layer
                Core    (Single     (Double     CC-RX                   GNURX               ICCRX (*6)
                Type    Precision)  Precision)

RX110           RXv1    No          ---         Renesas/RX100 (*1,*2)   GCC/RX100 (*1,*2)   IAR/RX100 (*1,*2)
RX111           RXv1    No          ---         Renesas/RX100 (*1,*2)   GCC/RX100 (*1,*2)   IAR/RX100 (*1,*2)
RX113           RXv1    No          ---         Renesas/RX100 (*1,*2)   GCC/RX100 (*1,*2)   IAR/RX100 (*1,*2)
RX130           RXv1    No          ---         Renesas/RX100 (*1,*2)   GCC/RX100 (*1,*2)   IAR/RX100 (*1,*2)
RX13T           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600

RX210           RXv1    No          ---         Renesas/RX200 (*3)      N/A (*3)            N/A (*3)
RX21A           RXv1    No          ---         Renesas/RX200 (*3)      N/A (*3)            N/A (*3)
RX220           RXv1    No          ---         Renesas/RX200 (*3)      N/A (*3)            N/A (*3)
RX230,RX231     RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX23E-A         RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX23W           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX23T           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX24T           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX24U           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2

RX610           RXv1    Yes         ---         N/A (*4)                N/A (*4)            N/A (*4)
RX62N,RX621     RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX630           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX634           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX63N,RX631     RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX64M           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX65N,RX651     RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX66N           RXv3    Yes         Yes         Renesas/RX700v3_DPFPU   GCC/RX700v3_DPFPU   IAR/RX700v3_DPFPU
RX62T           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX62G           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX63T           RXv1    Yes         ---         Renesas/RX600           GCC/RX600           IAR/RX600
RX66T           RXv3    Yes         No          Renesas/RX600v2 (*5)    GCC/RX600v2 (*5)    IAR/RXv2 (*5)

RX71M           RXv2    Yes         ---         Renesas/RX600v2         GCC/RX600v2         IAR/RXv2
RX72M           RXv3    Yes         Yes         Renesas/RX700v3_DPFPU   GCC/RX700v3_DPFPU   IAR/RX700v3_DPFPU
RX72N           RXv3    Yes         Yes         Renesas/RX700v3_DPFPU   GCC/RX700v3_DPFPU   IAR/RX700v3_DPFPU
RX72T           RXv3    Yes         No          Renesas/RX600v2 (*5)    GCC/RX600v2 (*5)    IAR/RXv2 (*5)

Notes:

*1: If the application writer wants to use their own tick interrupt configuration when tickless idle
functionality is not used, please define configSETUP_TICK_INTERRUPT() (in FreeRTOSConfig.h) and provide
the configuration function.  Please be aware that port.c is hard coded to use CMT0 though it seems to be
configured to use any CMTn according to the definition of configTICK_VECTOR (in FreeRTOSConfig.h).

*2: If the application writer wants to use their own tick interrupt configuration when tickless idle
functionality is used, please modify port.c for the configuration.  Please be aware that port.c is
hard coded to use CMT0 though it seems to be configured to use any CMTn according to the definition of
configTICK_VECTOR (in FreeRTOSConfig.h).

*3: RX100 ports are also available.

*4: RX600 ports use MVTIPL instruction but RX610 MCUs don't support this instruction.

*5: RX700v3_DPFPU ports are also available with the following definition in FreeRTOSConfig.h.

#define configUSE_TASK_DPFPU_SUPPORT            0

*6: PriorityDefinitions.h has to be provided for port_asm.s in case of other than RX700v3_DPFPU port.
It contains two definitions of interrupt priority like the following.

#define configKERNEL_INTERRUPT_PRIORITY         1
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    4


For more information about Renesas RX MCUs, please visit the following URL:

https://www.renesas.com/products/microcontrollers-microprocessors/rx.html

