#include "mbed.h"
#include "arm_book_lib.h"

DigitalIn gasDetector(D2);//switch 1
DigitalIn gasAlarmState(D3);//switch2
DigitalIn OTAlarmState(D4);//switch3
DigitalIn OTAlarm(D5);//switch4
DigitalIn Reset(D6);//switch5
DigitalIn MonitoringMode(D7);//switch6

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

void inputsInit(){
    gasDetector.mode(PullDown);
    gasAlarmState.mode(PullDown);
    OTAlarm.mode(PullDown);
    OTAlarmState.mode(PullDown);
    Reset.mode(PullDown);
    MonitoringMode.mode(PullDown);
}   
//(i) send the current simulated gas alarm state
bool gasAlarmActive = OFF;
void switch2(){
   if (gasAlarmState == ON){
        if (gasAlarmActive){
            uartUsb.write("GAS ALARM ACTIVE\r\n", 18);
        } else{
            uartUsb.write("GAS ALARM CLEAR\r\n", 17);
        }
        ThisThread::sleep_for(200ms);
   }
}
//(ii) send the current overtemperature alarm state
bool tempAlarmActive = OFF;
void switch3(){
   if (OTAlarmState == ON){
        if (tempAlarmActive){
            uartUsb.write("TEMP ALARM ACTIVE\r\n", 19);
        } else{
            uartUsb.write("TEMP ALARM CLEAR\r\n", 18);
        }
        ThisThread::sleep_for(200ms);
   }
}
//(iii) enable monitoring mode
bool monitoringEnabled = OFF;//check if mode is active
int interval = 0;//for 2-second interval
void switch6(){
    if (MonitoringMode == ON){
        monitoringEnabled = !monitoringEnabled;//filp the value so that it can be turned ON/OFF
        interval = 0;//reset the timer

        if (monitoringEnabled){// send an message so that user can know the mode is ON/OFF
            uartUsb.write("MONITORING MODE IS ON\r\n", 23);
        } else{
            uartUsb.write("MONITORING MODE IS OFF\r\n", 24);
        }
        ThisThread::sleep_for(200ms); 
    }

    if (monitoringEnabled == ON){
        interval++;
        if (interval >= 200){//200 loops x 10ms = 2s (sleep for 10ms is inside main)
            if (gasAlarmActive){
                uartUsb.write("GAS ALARM ACTIVE\r\n", 18);
            } else{
                uartUsb.write("GAS ALARM CLEAR\r\n", 17);
            }
            if (tempAlarmActive){
                uartUsb.write("TEMP ALARM ACTIVE\r\n", 19);
            } else{
                uartUsb.write("TEMP ALARM CLEAR\r\n", 18);
            }
            interval = 0;
        }
    }
}
//(iv) trigger the gas alarm
void switch1(){
   if (gasDetector == ON){
        gasAlarmActive = !gasAlarmActive;
        if (gasAlarmActive == ON){
        uartUsb.write( "WARNING: GAS DETECTED\r\n", 23 );
        }
        ThisThread::sleep_for(200ms);
    }
}
//trigger the temperature alarm
void switch4(){
    if (OTAlarm == ON){
        tempAlarmActive = !tempAlarmActive;
        if (tempAlarmActive == ON){
        uartUsb.write( "WARNING: TEMPERATURE TOO HIGH\r\n", 31 );
        }
        ThisThread::sleep_for(200ms);
    }
}
//(v) reset the alarms
void switch5(){
    if (Reset == ON){
        gasAlarmActive = OFF;//turn off the gas alarm
        tempAlarmActive = OFF; //turn off the temperature alarm
        uartUsb.write( "ALARMS RESET\r\n", 14 );
        ThisThread::sleep_for(200ms);
    }
}

int main()
{
    inputsInit();
    while (true) {
        switch1();
        switch2();
        switch3();
        switch4();
        switch5();
        switch6();
        ThisThread::sleep_for(10ms);// interval for monitoring mode
    }
}