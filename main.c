#include "driverlib.h"
#include <stdint.h>
#include <stdio.h>
#include "HAL_UART_4746.h"
#include "HAL_SDCard.h"
#include "SDCard_API.h"

// Global Variables
uint16_t receiver_1, receiver_2, receiver_3, receiver_4, receiver_5, line;

SDCardLib_Status SDCard_status = SDCARDLIB_STATUS_NOT_PRESENT;

//Function Headers
void ADC_init(void);
void microSDinit(void);

char buffer[128];

uint16_t yHigh, yLow;

void main(void)

{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    UART_initGPIO();
    UART_init();
    line = 0;
    receiver_1 = 0;
    receiver_2 = 0;
    receiver_3 = 0;
    receiver_4 = 0;
    receiver_5 = 0;
// Activate Configuration
    PMM_unlockLPM5();

    // Initialize ADC
    ADC_init();

    // Start Conversion
    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0,
    ADC12_B_REPEATED_SEQOFCHANNELS);

    //Initialize the MicroSD card
    microSDinit();
    while (line <= 64)
    {

        ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0,
        ADC12_B_SEQOFCHANNELS);
        while (ADC12_B_getInterruptStatus(ADC12_B_BASE, 0, ADC12_B_IFG1)
                != ADC12_B_IFG1)
            ;   // Wait for conversion

        receiver_1 = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
        receiver_2 = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);
        receiver_3 = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);
//        receiver_4 = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);
//        receiver_5 = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_4);

        sprintf(buffer,
                "Line: %d\tReceiver 1: %d\tReceiver 2: %d\tReceiver 3: %d\t\r\n",
                line, receiver_1, receiver_2, receiver_3);

        write_SDCard(buffer);

        UART_transmitString(buffer);
        line++;

        // Clear IFG1 explicitly
//        ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG0);
//        ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG1);
//
    }
    sprintf(buffer, "Done\n");

}

void microSDinit()
{
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);   //CS
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN2);   //CD
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);   //MISO
    //MUST BE 8 OR CHAR LONG
    char fileName[20] = "test.txt";
    char directoryName[20] = "debug";
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    SDCard_status = SDCard_detectCard();
    if (SDCard_status == SDCARDLIB_STATUS_PRESENT)
        SDCard_status = createFile_SDCard(directoryName, fileName);
    write_SDCard(buffer);
}

// ADC_init
// Configures ADC to use joystick inputs
// Inputs: none
// Returns: none

void ADC_init()
{

//Initialize the ADC12B Module
    /*
     * Base address of ADC12B Module
     * Use internal ADC12B bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider/pre-divider of 1
     * Not use internal channel
     */
    ADC12_B_initParam initParam = { 0 };
    initParam.sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC;
    initParam.clockSourceSelect = ADC12_B_CLOCKSOURCE_ADC12OSC;
    initParam.clockSourceDivider = ADC12_B_CLOCKDIVIDER_1;
    initParam.clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1;
    initParam.internalChannelMap = ADC12_B_NOINTCH;
    ADC12_B_init(ADC12_B_BASE, &initParam);

//Enable the ADC12B module
    ADC12_B_enable(ADC12_B_BASE);

    /*
     * Base address of ADC12B Module
     * For memory buffers 0-7 sample/hold for 64 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Enable Multiple Sampling
     */
    ADC12_B_setupSamplingTimer(ADC12_B_BASE,
    ADC12_B_CYCLEHOLD_4_CYCLES,
                               ADC12_B_CYCLEHOLD_4_CYCLES,
                               ADC12_B_MULTIPLESAMPLESENABLE);

//Configure Memory Buffer
    /*
     * Base address of the ADC12B Module
     * Configure memory buffer 0
     * Map input A1 to memory buffer 0
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
//  Receiver 1
    ADC12_B_configureMemoryParam receiver1Param = { 0 };
    receiver1Param.memoryBufferControlIndex = ADC12_B_MEMORY_0;
    receiver1Param.inputSourceSelect = ADC12_B_INPUT_A2;
    receiver1Param.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    receiver1Param.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    receiver1Param.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    receiver1Param.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &receiver1Param);

//  Receiver 2
    ADC12_B_configureMemoryParam receiver2Param = { 0 };
    receiver2Param.memoryBufferControlIndex = ADC12_B_MEMORY_1;
    receiver2Param.inputSourceSelect = ADC12_B_INPUT_A3;
    receiver2Param.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    receiver2Param.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    receiver2Param.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    receiver2Param.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &receiver2Param);

//  Receiver 3
    ADC12_B_configureMemoryParam receiver3Param = { 0 };
    receiver3Param.memoryBufferControlIndex = ADC12_B_MEMORY_2;
    receiver3Param.inputSourceSelect = ADC12_B_INPUT_A4;
    receiver3Param.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    receiver3Param.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    receiver3Param.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    receiver3Param.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &receiver3Param);

//  Receiver 4
    ADC12_B_configureMemoryParam receiver4Param = { 0 };
    receiver4Param.memoryBufferControlIndex = ADC12_B_MEMORY_3;
    receiver4Param.inputSourceSelect = ADC12_B_INPUT_A5;
    receiver4Param.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    receiver4Param.endOfSequence = ADC12_B_NOTENDOFSEQUENCE;
    receiver4Param.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    receiver4Param.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &receiver4Param);

//  Receiver 5
    ADC12_B_configureMemoryParam receiver5Param = { 0 };
    receiver5Param.memoryBufferControlIndex = ADC12_B_MEMORY_4;
    receiver5Param.inputSourceSelect = ADC12_B_INPUT_A12;
    receiver5Param.refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS;
    receiver5Param.endOfSequence = ADC12_B_ENDOFSEQUENCE;
    receiver5Param.windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE;
    receiver5Param.differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE;
    ADC12_B_configureMemory(ADC12_B_BASE, &receiver5Param);

// Clear Interrupt
//    ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG0);
//    ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG1);
//    ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG2);

//Enable memory buffer 1 interrupt
//    ADC12_B_enableInterrupt(ADC12_B_BASE,ADC12_B_IE1,0,0);

}
