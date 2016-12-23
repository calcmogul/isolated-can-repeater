# MSP430 Software

## Dependencies

* msp430-elf-gcc
* mspdebug

## Max operation speed of circuit

The falling edge of the output needs to track the input within one clock cycle of the CAN bus. Otherwise, one side of the bus will temporarily give up dominant status on the bus to the other side unintentionally.

measured delay between a falling edge on the input and the output tracking it:
> 1.88us (compiled with -O3 and using 16MHz main clock)

> 1/(1.88*1E-6)*1E-3 = ~531.9149kHz max switching frequency

So we could use a max bus speed of 500kHz with this hardware implementation.

The delay between the output tracking a rising edge on the input stops the local side of the bus from asserting dominance for a period of time so the other side of the bus may do so. This delay should be at least long enough for a falling edge (dominant state) to propagate across the bus.
