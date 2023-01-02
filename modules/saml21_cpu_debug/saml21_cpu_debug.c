#include <stdio.h>
#include <inttypes.h>

#include "periph/pm.h"

#include "board.h"

void saml21_cpu_debug(void)
{
    size_t count, num, num1;

    puts("Oscillators:");
    if (OSCCTRL->XOSCCTRL.bit.ENABLE) {
        printf(" OSCCTRL->XOSCMCTRL");
        if(OSCCTRL->XOSCCTRL.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSCCTRL->XOSCCTRL.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        if(OSCCTRL->XOSCCTRL.bit.XTALEN) { printf(" XTALEN"); }
        puts("");
    }
    if (OSCCTRL->OSC16MCTRL.bit.ENABLE) {
        char *osc16m_freqs[] = { "4MHz", "8MHz", "12MHz", "16MHz" };
        printf(" OSCCTRL->OSC16MCTRL %s", osc16m_freqs[OSCCTRL->OSC16MCTRL.bit.FSEL]);
        if(OSCCTRL->OSC16MCTRL.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSCCTRL->OSC16MCTRL.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        puts("");
    }
    if (OSCCTRL->DFLLCTRL.bit.ENABLE) {
        printf(" OSCCTRL->DFLLCTRL");
        if (OSCCTRL->DFLLCTRL.bit.MODE) { printf(" CLOSED-LOOP MUL:%d", OSCCTRL->DFLLMUL.bit.MUL); }
        if(OSCCTRL->DFLLCTRL.bit.WAITLOCK) { printf(" WAITLOCK"); }
        if(OSCCTRL->DFLLCTRL.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSCCTRL->DFLLCTRL.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        if(OSCCTRL->DFLLCTRL.bit.USBCRM) { printf(" USBCRM"); }
        if(OSCCTRL->DFLLCTRL.bit.LLAW) { printf(" LLAW"); }
        puts("");
    }
    if (OSCCTRL->DPLLCTRLA.bit.ENABLE ) {
        printf(" OSCCTRL->DPLLCTRLA");
        if(OSCCTRL->DPLLCTRLA.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSCCTRL->DPLLCTRLA.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        puts("");
    }
    if (OSC32KCTRL->XOSC32K.bit.ENABLE) {
        printf(" OSC32KCTRL->XOSC32K");
        if(OSC32KCTRL->XOSC32K.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSC32KCTRL->XOSC32K.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        if(OSC32KCTRL->XOSC32K.bit.EN1K) { printf(" EN1K"); }
        if(OSC32KCTRL->XOSC32K.bit.EN32K) { printf(" EN32K"); }
        if(OSC32KCTRL->XOSC32K.bit.XTALEN) { printf(" XTALEN"); }
        if(OSC32KCTRL->XOSC32K.bit.WRTLOCK) { printf(" WRTLOCK"); }
        puts("");
    }
    if (OSC32KCTRL->OSC32K.bit.ENABLE) {
        printf(" OSC32KCTRL->OSC32K");
        if(OSC32KCTRL->OSC32K.bit.ONDEMAND) { printf(" ONDEMAND"); }
        if(OSC32KCTRL->OSC32K.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
        if(OSC32KCTRL->OSC32K.bit.EN1K) { printf(" EN1K"); }
        if(OSC32KCTRL->OSC32K.bit.EN32K) { printf(" EN32K"); }
        if(OSC32KCTRL->OSC32K.bit.WRTLOCK) { printf(" WRTLOCK"); }
        puts("");
    }
    if (OSC32KCTRL->OSCULP32K.bit.WRTLOCK) {
        puts(" OSC32KCTRL->OSCULP32K WRTLOCK");
    }
    char *rtc_sources[] = { "ULP1K", "ULP32K", "OSC1K", "OSC32K", "XOSC1K", "XOSC32K" };
    printf(" OSC32KCTRL->RTCCTRL.RTCSEL = %s\n", rtc_sources[OSC32KCTRL->RTCCTRL.bit.RTCSEL]);

    puts("Clock generators:");
    char *clock_sources[] = {
        "XOSC", "GCLK_IN", "GCLK_GEN1", "OSCULP32K", "OSC32K",
        "XOSC32K", "OSC16M", "DFLL48M", "DPLL96M"
    };
    num = sizeof(GCLK->GENCTRL) / sizeof(GCLK_GENCTRL_Type);
    for(size_t i=0; i<num; i++) {
        if (GCLK->GENCTRL[i].bit.GENEN) {
            printf(" GCLK->GENCTRL[%02d].SRC = %s", i, clock_sources[GCLK->GENCTRL[i].bit.SRC]);
            if(GCLK->GENCTRL[i].bit.DIV) {
                uint16_t div = GCLK->GENCTRL[i].bit.DIV;
                if(GCLK->GENCTRL[i].bit.DIVSEL) { div = 1 << (div + 1); }
                printf("/%d", div);
            }
            if(GCLK->GENCTRL[i].bit.RUNSTDBY) { printf(" RUNSTDBY"); }
            if(GCLK->GENCTRL[i].bit.OE) { printf(" OE"); }
            if(GCLK->GENCTRL[i].bit.OOV) { printf(" OOV"); }
            if(GCLK->GENCTRL[i].bit.IDC) { printf(" IDC"); }
            switch(i) {
                case 0:
                    printf(" [GCLK_MAIN]");
                    break;
                case 1:
                    printf(" [GCLK_GEN1]");
                    break;
                default:
                    break;
            }
            puts("");
        }
    }
    char *gclk_ids[] = {
        "OSCCTRL_GCLK_ID_DFLL48", "OSCCTRL_GCLK_ID_FDPLL", "OSCCTRL_GCLK_ID_FDPLL32K",
        "EIC_GCLK_ID", "USB_GCLK_ID",
        "EVSYS_GCLK_ID_0", "EVSYS_GCLK_ID_1", "EVSYS_GCLK_ID_2", "EVSYS_GCLK_ID_3",
        "EVSYS_GCLK_ID_4", "EVSYS_GCLK_ID_5", "EVSYS_GCLK_ID_6", "EVSYS_GCLK_ID_7",
        "EVSYS_GCLK_ID_8", "EVSYS_GCLK_ID_9", "EVSYS_GCLK_ID_10", "EVSYS_GCLK_ID_11",
        "SERCOMx_GCLK_ID_SLOW", "SERCOM0_GCLK_ID_CORE", "SERCOM1_GCLK_ID_CORE",
        "SERCOM2_GCLK_ID_CORE", "SERCOM3_GCLK_ID_CORE", "SERCOM4_GCLK_ID_CORE",
        "SERCOM5_GCLK_ID_SLOW", "SERCOM5_GCLK_ID_CORE",
        "TCC0_GCLK_ID, TCC1_GCLK_ID", "TCC2_GCLK_ID", "TC0_GCLK_ID, TC1_GCLK_ID",
        "TC2_GCLK_ID, TC3_GCLK_ID", "TC4_GCLK_ID", "ADC_GCLK_ID", "AC_GCLK_ID",
        "DAC_GCLK_ID", "PTC_GCLK_ID", "CCL_GCLK_ID", "NVMCTRL_GCLK_ID",
    };
    num = sizeof(GCLK->PCHCTRL)/sizeof(GCLK_PCHCTRL_Type);
    for(size_t i=0; i<num; i++) {
        if (GCLK->PCHCTRL[i].bit.CHEN) {
            printf(" GCLK->PCHCTRL[%02d].SRC = %d [%s]", i, GCLK->PCHCTRL[i].bit.GEN, gclk_ids[i]);
            if(GCLK->PCHCTRL[i].bit.WRTLOCK) { printf(" WRTLOCK"); }
            puts("");
        }
    }

    puts("Main clock:");
    printf(" MCLK->CPUDIV   = GCLK_MAIN/%d\n", MCLK->CPUDIV.reg);
    printf(" MCLK->LPDIV    = GCLK_MAIN/%d\n", MCLK->LPDIV.reg);
    printf(" MCLK->BUPDIV   = GCLK_MAIN/%d\n", MCLK->BUPDIV.reg);
    if (MCLK->AHBMASK.reg != MCLK_AHBMASK_RESETVALUE) {
        printf(" MCLK->AHBMASK  = 0x%08lx\n", MCLK->AHBMASK.reg);
    }
    if (MCLK->APBAMASK.reg != MCLK_APBAMASK_RESETVALUE) {
        printf(" MCLK->APBAMASK = 0x%08lx\n", MCLK->APBAMASK.reg);
    }
    if (MCLK->APBBMASK.reg != MCLK_APBBMASK_RESETVALUE) {
        printf(" MCLK->APBBMASK = 0x%08lx\n", MCLK->APBBMASK.reg);
    }
    if (MCLK->APBCMASK.reg != MCLK_APBCMASK_RESETVALUE) {
        printf(" MCLK->APBCMASK = 0x%08lx\n", MCLK->APBCMASK.reg);
    }
    if (MCLK->APBDMASK.reg != MCLK_APBDMASK_RESETVALUE) {
        printf(" MCLK->APBDMASK = 0x%08lx\n", MCLK->APBDMASK.reg);
    }
    if (MCLK->APBEMASK.reg != MCLK_APBEMASK_RESETVALUE) {
        printf(" MCLK->APBEMASK = 0x%08lx\n", MCLK->APBEMASK.reg);
    }

    puts("Reset controller:");
    if (RSTC->RCAUSE.reg) {
        printf(" RSTC->RCAUSE = ");
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_BACKUP) {
            printf("BACKUP");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_SYST) {
            printf("SYST");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_WDT) {
            printf("WDT");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_EXT) {
            printf("EXT");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_BOD33) {
            printf("BOD33");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_BOD12) {
            printf("BOD12");
        }
        if (RSTC->RCAUSE.reg & RSTC_RCAUSE_POR) {
            printf("POR");
        }
        puts("");
    }
    if (RSTC->BKUPEXIT.reg) {
        printf(" RSTC->BKUPEXIT = ");
        if (RSTC->BKUPEXIT.reg & RSTC_BKUPEXIT_BBPS) {
            printf(" BBPS");
        }
        if (RSTC->BKUPEXIT.reg & RSTC_BKUPEXIT_RTC) {
            printf(" RTC");
        }
        if (RSTC->BKUPEXIT.reg & RSTC_BKUPEXIT_EXTWAKE) {
            printf(" EXTWAKE");
        }
        puts("");
    }
    if (RSTC->WKDBCONF.reg) {
        char *debounce_counts[] = { "OFF", "2CK32", "3CK32", "32CK32", "512CK32", "4096CK32", "32768CK32", "-" };
        printf(" RSTC->WKDBCONF = %s\n", debounce_counts[RSTC->WKDBCONF.bit.WKDBCNT]);
    }
    if (RSTC->WKPOL.reg) {
        printf(" RSTC->WKPOL.reg = 0x%02x\n", RSTC->WKPOL.reg & RSTC_WKPOL_MASK);
    }
    if (RSTC->WKEN.reg) {
        printf(" RSTC->WKEN.reg = 0x%02x\n", RSTC->WKEN.reg & RSTC_WKEN_MASK);
    }
    if (RSTC->WKCAUSE.reg) {
        printf(" RSTC->WKCAUSE.reg = 0x%02x\n", RSTC->WKCAUSE.reg & RSTC_WKCAUSE_MASK);
    }

    puts("Power manager:");
    printf(" PM->CTRLA.IORET        = %d\n", (PM->CTRLA.bit.IORET) ? 1 : 0);
    char *sleep_modes[] = { "-", "-", "IDLE", "-", "STANDBY", "BACKUP", "OFF", "-" };
    printf(" PM->SLEEPCFG.SLEEPMODE = %s\n", sleep_modes[PM->SLEEPCFG.bit.SLEEPMODE]);
    printf(" PM->PLCFG.PLSEL        = PL%d%s\n", PM->PLCFG.bit.PLSEL, PM->PLCFG.bit.PLDIS ? " PLDIS" : "");

    puts("Supply controller:");
    if (SUPC->BOD33.bit.ENABLE) {
        char *bod33_actions[] = { "NONE", "RESET", "INT", "BKUP" };
        printf(" SUPC->BOD33 = BKUPLEVEL:%d LEVEL:%d PSEL:%d %s %s %s",
            SUPC->BOD33.bit.BKUPLEVEL,
            (2 << SUPC->BOD33.bit.LEVEL),
            SUPC->BOD33.bit.PSEL,
            SUPC->BOD33.bit.VMON ? "VDD" : "VBAT",
            SUPC->BOD33.bit.ACTCFG ? "CONTINUOUS" : "SAMPLING",
            bod33_actions[SUPC->BOD33.bit.ACTION]
        );
        if (SUPC->BOD33.bit.RUNBKUP) { printf(" RUNBKUP"); }
        if (SUPC->BOD33.bit.RUNSTDBY) { printf(" RUNSTBY"); }
        if (SUPC->BOD33.bit.HYST) { printf(" HYST"); }
        puts("");
    }
    if (SUPC->VREG.bit.ENABLE) {
        printf(" SUPC->VREG = VSPER:%d VSVSTEP:%d LPEFF:%s",
            SUPC->VREG.bit.VSPER,
            SUPC->VREG.bit.VSVSTEP,
            SUPC->VREG.bit.LPEFF ? "full" : "limited"
        );
        if (SUPC->VREG.bit.RUNSTDBY) { printf(" RUNSTBY"); }
        if (SUPC->VREG.bit.STDBYPL0) { printf(" STBYPL0"); }
        puts(SUPC->VREG.bit.SEL == SAM0_VREG_LDO? " LDO" : " BUCK");
    }
    char *vref_selections[] = { "1.024V", "", "2.048V", "4.096V", "", "", "", "" };
    printf(" SUPC->VREF = %s", vref_selections[SUPC->VREF.bit.SEL]);
    if (SUPC->VREF.bit.ONDEMAND) { printf(" ONDEMAND"); }
    if (SUPC->VREF.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
    if (SUPC->VREF.bit.VREFOE) { printf(" VREFOE"); }
    if (SUPC->VREF.bit.TSEN) { printf(" TSEN"); }
    puts("");
    printf(" SUPC->BBPS =");
    if (SUPC->BBPS.bit.PSOKEN) { printf(" PSOKEN"); }
    if (SUPC->BBPS.bit.WAKEEN) { printf(" WAKEEN"); }
    char *bbps_configs[] = { "NONE", "AWPS", "FORCED", "BOD33" };
    printf(" CONF:%s\n", bbps_configs[SUPC->BBPS.bit.CONF]);
    if (SUPC->BKOUT.bit.EN) {
        printf(" SUPC->BKOUT");
        if (SUPC->BKOUT.bit.RTCTGL) { printf(" RTCTGL"); }
        if (SUPC->BKOUT.bit.EN & 0x01) { printf(" OUT[0]"); }
        if (SUPC->BKOUT.bit.EN & 0x02) { printf(" OUT[1]"); }
        puts("");
    }

    puts("GPIO:");
    count = 0;
    num = sizeof(PORT->Group)/sizeof(PortGroup);
    num1 = sizeof(PORT->Group[0].PINCFG)/sizeof(PORT_PINCFG_Type);
    for (size_t i=0; i<num; i++) {
        for (size_t j=0; j<num1; j++) {
//            if (PORT->Group[i].PINCFG[j].reg || (PORT->Group[i].DIR.reg & (1 << j))) {
                printf(" P%c.%02d:", 'A' + i, j);
                if (PORT->Group[i].PINCFG[j].bit.PMUXEN) {
                    printf(" MUX%c", 'A' + (((PORT->Group[i].PMUX[j/2].reg >> 4*(j % 2))) & 0x0f));
                } else {
                    if (PORT->Group[i].DIR.reg & (1 << j)){
                        printf(" OUT value=%d", (PORT->Group[i].OUT.reg & (1 << j)) ? 1 : 0);
                    }else{
                        printf(" IN value=%d", (PORT->Group[i].IN.reg & (1 << j)) ? 1 : 0);
                    }
                }
                if (PORT->Group[i].PINCFG[j].bit.PULLEN) {
                    printf(" PULL%s", (PORT->Group[i].OUT.reg & (1 << j)) ? "UP" : "DOWN");
                }
                if ((PORT->Group[i].PINCFG[j].bit.INEN)) {
                    printf(" INEN");
                }
                if (PORT->Group[i].CTRL.bit.SAMPLING & (1 << j)) {
                    printf(" SAMPLING");
                }
                if (PORT->Group[i].PINCFG[j].bit.DRVSTR) {
                    printf(" DRVSTR");
                }
                puts("");
                count++;
//            }
        }
        char *event_actions[] = { "OUT", "SET", "CLR", "TGL" };
        if (PORT->Group[i].EVCTRL.bit.PORTEI0) {
            printf("P%c: event=0, action=%s, pins=0x%02x", i, event_actions[PORT->Group[i].EVCTRL.bit.EVACT0], PORT->Group[i].EVCTRL.bit.PID0);
        }
        if (PORT->Group[i].EVCTRL.bit.PORTEI1) {
            printf("P%c: event=1, action=%s, pins=0x%02x", i, event_actions[PORT->Group[i].EVCTRL.bit.EVACT1], PORT->Group[i].EVCTRL.bit.PID1);
        }
        if (PORT->Group[i].EVCTRL.bit.PORTEI2) {
            printf("P%c: event=2, action=%s, pins=0x%02x", i, event_actions[PORT->Group[i].EVCTRL.bit.EVACT2], PORT->Group[i].EVCTRL.bit.PID2);
        }
        if (PORT->Group[i].EVCTRL.bit.PORTEI3) {
            printf("P%c: event=3, action=%s, pins=0x%02x", i, event_actions[PORT->Group[i].EVCTRL.bit.EVACT3], PORT->Group[i].EVCTRL.bit.PID3);
        }
    }
    if (count == 0) {
        puts(" NONE");
    }

    puts("SERCOMs:");
    Sercom *sercoms[] = SERCOM_INSTS;
    char *sercom_modes[] = { "USART:ext-clock", "USART:int-clock", "SPI:slave", "SPI:master", "I2C:slave", "I2C:master", "-", "-" };
    char *usart_frame_formats[] = { "USART", "USART:parity", "AUTOBAUD", "AUTOBAUD:parity" };
    char *usart_sample_rates[] = { "16x:arithmetic", "16x:fractional", "8x:arithmetic", "8x:fractional", "3x:arithmetic" };
    char *i2c_speeds[] = { "100kHz", "1MHz", "3.4MHz", "-" };
    count = 0;
    for (size_t i=0; i<SERCOM_INST_NUM; i++) {
        if (sercoms[i]->USART.CTRLA.reg) {
            printf(" SERCOM%d->CTRLA = %s", i, sercom_modes[sercoms[i]->USART.CTRLA.bit.MODE]);
            if (sercoms[i]->USART.CTRLA.bit.ENABLE) { printf(" ENABLE"); }
            if (sercoms[i]->USART.CTRLA.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
            switch (sercoms[i]->USART.CTRLA.bit.MODE) {
                case 1:  // USART, internal clock
                    if (sercoms[i]->USART.CTRLA.bit.FORM < sizeof(usart_frame_formats)) {
                         printf(" %s", usart_frame_formats[sercoms[i]->USART.CTRLA.bit.FORM]);
                    }
                    printf(" %sSYNC", sercoms[i]->USART.CTRLA.bit.CMODE ? "": "A");
                    if (sercoms[i]->USART.CTRLA.bit.SAMPR < sizeof(usart_sample_rates)) {
                         printf(" %s", usart_sample_rates[sercoms[i]->USART.CTRLA.bit.SAMPR]);
                    }
                    printf(" BAUD:0x%04x", sercoms[i]->USART.BAUD.reg);
                    break;
                case 3:  // SPI, master
                    printf(" BAUD:0x%02x", sercoms[i]->SPI.BAUD.reg);
                    break;
                case 5:  // I2C, master
                    printf(" %s", i2c_speeds[sercoms[i]->I2CM.CTRLA.bit.SPEED]);
                    printf(" BAUD:0x%04lx", sercoms[i]->I2CM.BAUD.reg);
                    break;
                default:
                    break;
            }
            puts("");
            count++;
        }
    }
    if (count == 0) {
        puts(" NONE");
    }

    puts("Timers:");
    Tc *timers[] = TC_INSTS;
    char *timer_modes[] = { "COUNT16", "COUNT8", "COUNT32" };
    char *timer_divs[] = { "", "/2", "/4", "/8", "/16", "/64", "/256", "/1024" };
    count = 0;
    for (size_t i=0; i<TC_INST_NUM; i++) {
        if (timers[i]->COUNT8.CTRLA.reg) {
            printf(" TC%d->CTRLA = %s GCLK_TC%s", i, timer_modes[timers[i]->COUNT8.CTRLA.bit.MODE], timer_divs[timers[i]->COUNT8.CTRLA.bit.PRESCALER]);
            if (timers[i]->COUNT8.CTRLA.bit.ENABLE) { printf(" ENABLE"); }
            if (timers[i]->COUNT8.CTRLA.bit.RUNSTDBY) { printf(" RUNSTDBY"); }
            puts("");
            count++;
        }
    }
    if (count == 0) {
        puts(" NONE");
    }

    puts("External Interrupt Controller:");
    char *eic_clocks[] = { "GCLK_EIC", "CLK_ULP32K" };
    printf("EIC.CTRLA.CKSEL = %s\n", eic_clocks[EIC->CTRLA.bit.CKSEL]);
    printf("EIC.CTRLA.ENABLE = %d\n", EIC->CTRLA.bit.ENABLE);
}
