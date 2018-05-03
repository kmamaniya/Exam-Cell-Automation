#include "Includes.h"
#include "DHT11.h"
#include <reg51.h>
#define debounceTime 5000

long currentTime;
int alarmMode;
unsigned char dispArr1[7];
unsigned char dispArr[10];
sbit modepin = P0^0;
sbit changetens = P0^1;
sbit changeunits= P0^2;
sbit setpin = P0^3;
sbit AMLed = P3^2;
sbit PMLed = P3^3;
sbit buzzer = P3^4;
sbit latch = P3^0;
sbit finalspin = P0^5;
sbit unitspin = P0^4;
int flag=0;
unsigned char AlarmHH=1;
unsigned char AlarmMM=0;
unsigned char AlarmAP=0;
unsigned char YY1=20,YY2,MM,DD,HH,MIN,SS,AP;
int bitflag=0;
int timeset=0;
int alarmCount=0;
int units[]={585,600,660,705,720,780,795,810,870,885,900,960};		// 945,1000,1100,1145,1200,1300,1330,1355,1430,1445,1500,1600
int finals[]={645,660,720,780,830,840,885,900,960,1020,1070,1080}; //1045,1100,1200,1300,1350,1400,1445,1500,1600,1700,1750,1800

void CalculateTempTo7seg(unsigned char temp){
	//Temperature
	dispArr[8]=temp/10;
	dispArr[9]=temp%10;
}

void CalculateDateTo7seg(char * pDateArray){   // Displays date in DD:MM:YYYY format
	//Date
	
	DD=pDateArray[1];
	dispArr[0]=(char)(DD/10);
	dispArr[1]=(char)(DD%10);
	//Months
	MM=pDateArray[2];
	dispArr[2]=(char)(MM/10);
	dispArr[3]=(char)(MM%10);
	//Year
	dispArr[4]=(char)YY1/10;
	dispArr[5]=(char)YY1%10;
	
	YY2=pDateArray[3];
	dispArr[6]=(char)(YY2/10);
	dispArr[7]=(char)(YY2%10);
	YY2=pDateArray[3];
}
void CalculateTimeTo7seg(char *pTimeArray) {  // Displays date in DD:MM:YYYY format
	//Hours
	HH=pTimeArray[2];
	dispArr1[0]=(HH/10);
	dispArr1[1]=(HH%10);
	//Minutes
	MIN=pTimeArray[1];
	dispArr1[2]=(MIN/10);
	dispArr1[3]=(MIN%10);
	//Seconds
	SS=pTimeArray[0];
	dispArr1[4]=(SS/10);
	dispArr1[5]=(SS%10);
	//AM PM
	AP=(unsigned char)pTimeArray[3];
	dispArr1[6]=AP;
	currentTime = 60*HH+MIN+AP*720;
	if(HH==12)
		currentTime-=720;
}



void display(){
	char i;
	
	if(dispArr1[6]==1){
		PMLed=1;delay(100);
		AMLed=0;
	}
	else if(dispArr1[6]==0){
		PMLed=0;delay(100);
		AMLed=1;
	}
	else{
		PMLed=0;delay(100);
		AMLed=0;
	}
	
	for(i=0;i<10;i++){
		P1 = dispArr[i];
		delay(10);
		P2 = (unsigned char)i;
		delay(10);
		latch=0;
		delay(10);
		latch=1;
		delay(10);
	}
	for(i=0;i<6;i++){
		P1 = dispArr1[i];
		delay(10);
		P2 = (unsigned char)(i+10);
		delay(10);
		latch=0;
		delay(10);
		latch=1;
		delay(10);
	}
}

void setTime(){
	int i;
	timeset=0;
	bitflag=0;
		while(1){
				if(modepin==0){
					delay(debounceTime);
					if(modepin==0){
						while(modepin!=1);
						flag = 2;
						if(timeset==1)
							Set_DS1307_RTC_Time(AP,HH,MIN,SS);
						Set_DS1307_RTC_Date(DD,MM,YY2,4);
						break;
					}
				}				
				for(i=0;i<10;i++)
					dispArr[i]=0;
				
				dispArr1[0]=HH/10;
				dispArr1[1]=HH%10;
				dispArr1[2]=MIN/10;
				dispArr1[3]=MIN%10;
				dispArr1[4]=SS/10;
				dispArr1[5]=SS%10;
				dispArr1[6]=AP;
				display();
				
				if(changetens==0){
					delay(debounceTime);
					if(changetens==0){
						timeset=1;
						while(changetens!=1);
						switch(bitflag){
							case 0: HH+=10;
											if(HH>12)
												HH%=10;
											break;
							case 1: MIN+=10;
											if(MIN>=60)
												MIN%=10;
											break;
							case 2: SS+=10;
											if(SS>=60)
												SS%=10;
											break;
							case 3: AP++;
											AP%=2;
							default: break;
						}
					}
				}
				if(changeunits==0){
					delay(debounceTime);
					if(changeunits==0){
						timeset=1;
						while(changeunits!=1);
						switch(bitflag){
							case 0: HH++;
											if(HH>12)
												HH=1;
											break;
							case 1: MIN++;
											MIN=MIN%60;
											break;
							case 2: SS++;
											SS=SS%60;
											break;
							case 3: AP++;
											AP%=2;
							default: break;
						}
					}
				}
			if(setpin==0){
				delay(debounceTime);
				if(setpin==0){
					while(setpin!=1);
					bitflag++;
					bitflag%=4;
				}
			}
			delay(100);
		}
}

void setDate(){
		int i;
		bitflag=0;
		while(flag<2){
		for(i=0;i<6;i++)
			dispArr1[i]=0;
		dispArr1[6]=2;	
		dispArr[8]=0;
		dispArr[9]=0;
		dispArr[0]=DD/10;
		dispArr[1]=DD%10;
		dispArr[2]=MM/10;
		dispArr[3]=MM%10;
		dispArr[4]=YY1/10;
		dispArr[5]=YY1%10;
		dispArr[6]=YY2/10;
		dispArr[7]=YY2%10;
		display();
			
		if(changetens==0){
			delay(debounceTime);
			if(changetens==0){
				while(changetens!=1);
				switch(bitflag){
					case 0: DD+=10;
									if(DD>31)
										DD%=10;
									break;
					case 1: MM+=10;
									if(MM>12)
										MM%=10;
									break;
					case 2:	YY1+=10;
									if(YY1>99)
										YY1%=10;
									break;
					case 3:	YY2+=10;
									if(YY2>99)
										YY2%=10;
					default: break;
				}
			}
		}
		if(changeunits==0){
			delay(debounceTime);
			if(changeunits==0){
				while(changeunits!=1);
				switch(bitflag){
					case 0: DD++;
									if(DD>31)
										DD=1;
									break;
					case 1: MM++;
									if(MM>12)
										MM=1;
									break;
					case 2:	YY1++;
									YY1%=100;
									break;
					case 3:	YY2++;
									YY2%=100;
					default: break;
				}
			}
		}
		if(setpin==0){
			delay(debounceTime);
			if(setpin==0){
				while(setpin!=1);
				bitflag++;
				bitflag%=4;
			}
		}
		if(modepin==0){
			delay(debounceTime);
			if(modepin==0){
				while(modepin!=1);
				setTime();
			}
		}
		delay(100);
	}	
}


/*void setAlarm(){
		int i=0;
		bitflag=0;
		while(flag<2){
			for(i=0;i<10;i++)
				dispArr[i]=0;
			
			dispArr1[0]=AlarmHH/10;
			dispArr1[1]=AlarmHH%10;
			dispArr1[2]=AlarmMM/10;
			dispArr1[3]=AlarmMM%10;
			dispArr1[4]=0;
			dispArr1[5]=0;
			dispArr1[6]=AlarmAP;
			display();
			if(changetens==0){
					while(changetens!=1 );
					if(bitflag==0){
						AlarmHH+=10;
						if(AlarmHH>12)
							AlarmHH%=10;
					}
					else if(bitflag==1){
						AlarmMM+=10;
						if(AlarmMM>=60)
							AlarmMM%=10;
					}
					
					else {
							AlarmAP++;
							AlarmAP%=2;
					}
			}
			if(changeunits==0){
					while(changeunits!=1);
					if(bitflag==0){
						AlarmHH++;
						if(AlarmHH>12)
							AlarmHH=1;
					}
					else if(bitflag==1){
						AlarmMM++;
						AlarmMM%=60;
					}
					else {
							AlarmAP++;
							AlarmAP%=2;
					}
				}
			if(setpin==0){
				while(setpin!=1);
				bitflag++;
				bitflag%=3;
			}
			if(modepin==0){
					while(modepin!=1);
					setDate();
			}
			delay(100);
		}
		AlarmTime=60*AlarmHH+AlarmMM + AlarmAP*720;
		flag=0;
}*/


int getAlarmCount(int mode){
	int i=0;
	if(mode==1){
		for(i=0;i<12;i++)
			if(currentTime<=units[i])
				break;
		if(i==12)
			return 0;
		else return i;
		}
	else if(mode==2){
		for(i=0;i<12;i++)
			if(currentTime<=finals[i])
				break;
		if(i==12)
			return 0;
		else return i;
		}
	else return -1;
}

int setAlarmMode(){
	if(unitspin==0 && finalspin==1)
		return 1;
	else if(unitspin==1 && finalspin==0)
		return 2;
	else 
		return 0;
}

void setAlarm(){
	if(alarmMode==1 && units[alarmCount]== currentTime && SS<15)			
			buzzer=0;
	else if(alarmMode==2 && finals[alarmCount]== currentTime && SS<15)			
			buzzer=0;
	else buzzer=1;
}

// Main function
void main()
{	
	alarmCount=0;
	alarmMode=1;
	modepin=1;
	changeunits=1;
	changetens=1;
	setpin=1;
	unitspin=1;
	finalspin=1;
	InitI2C();	// Initialize i2c pins	
	flag=0;
	//set initial time
	//Set_DS1307_RTC_Time(PM_Time, 12, 59, 58);	// Set time 
	//Set initial date
	//Set_DS1307_RTC_Date(24, 8, 17, 0);	// Set Set 24-08-2017
	while(1){
		flag=0;
		CalculateDateTo7seg(Get_DS1307_RTC_Date());
		CalculateTimeTo7seg(Get_DS1307_RTC_Time());
		CalculateTempTo7seg(12);
		display();
		alarmMode=setAlarmMode();
		alarmCount=getAlarmCount(alarmMode);
		setAlarm();
		if(modepin==0){
			delay(debounceTime);
			if(modepin==0){
				while(modepin!=1);
				setDate();
			}
		}
		delay(65000);
	}
}