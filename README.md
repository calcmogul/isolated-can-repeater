# Galvanically Isolated CAN Repeater

This PCB is designed to function as a CAN repeater that galvanically isolates the two sides of the bus. It uses the repeater logic in figure 3 of AND8358/D (the application note for the AMIS-42700's feedback suppression).

## Hardware

* [Microcontroller](http://www.digikey.com/product-detail/en/texas-instruments/MSP430G2553IN20/296-28429-5-ND/2638885) (MSP430G2553) x1
* [Socket](http://www.digikey.com/product-detail/en/assmann-wsw-components/AR-20-HZL-TT/AE10015-ND/821769) (2x10 0.3" row spacing) x1
* [NOT Gates](http://www.digikey.com/product-detail/en/texas-instruments/CD4069UBE/296-3518-5-ND/376616) (CD4069UBE) x1
* [NAND Gates](http://www.digikey.com/product-detail/en/texas-instruments/CD4011BE/296-2031-5-ND/67241) (CD4011UBE) x1
* [CAN Transceiver](http://www.digikey.com/product-detail/en/texas-instruments/ISO1050DUBR/296-24818-2-ND/2094633) (ISO1050SUBR) x2
* [Voltage Regulator 3.3V](https://www.sparkfun.com/products/526) (LD117AV33) x1
* [0.1μF Capacitor](http://www.digikey.com/product-detail/en/vishay-bc-components/K104Z15Y5VF5TL2/BC1160CT-ND/286782) x1
* [10μF Capacitor](http://www.digikey.com/product-detail/en/tdk-corporation/FK24X5R1C106K/445-8497-ND/2815427) x1

### Thruhole

* 1x12 pin header x1

### SMT

* 2x6 pin header x1

## MSP430 Software

See [MSP430 README](msp430/README.md).
