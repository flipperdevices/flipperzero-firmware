/*
 * sntp.c
 *
 *  Created on: 2014. 12. 15.
 *      Author: Administrator
 */


#include <string.h>

#include "sntp.h"
#include "socket.h"

ntpformat NTPformat;
datetime Nowdatetime;
uint8_t ntpmessage[48];
uint8_t *data_buf;
uint8_t NTP_SOCKET;
uint8_t time_zone;
uint16_t ntp_retry_cnt=0; //counting the ntp retry number

/*
00)UTC-12:00 Baker Island, Howland Island (both uninhabited)
01) UTC-11:00 American Samoa, Samoa
02) UTC-10:00 (Summer)French Polynesia (most), United States (Aleutian Islands, Hawaii)
03) UTC-09:30 Marquesas Islands
04) UTC-09:00 Gambier Islands;(Summer)United States (most of Alaska)
05) UTC-08:00 (Summer)Canada (most of British Columbia), Mexico (Baja California)
06) UTC-08:00 United States (California, most of Nevada, most of Oregon, Washington (state))
07) UTC-07:00 Mexico (Sonora), United States (Arizona); (Summer)Canada (Alberta)
08) UTC-07:00 Mexico (Chihuahua), United States (Colorado)
09) UTC-06:00 Costa Rica, El Salvador, Ecuador (Galapagos Islands), Guatemala, Honduras
10) UTC-06:00 Mexico (most), Nicaragua;(Summer)Canada (Manitoba, Saskatchewan), United States (Illinois, most of Texas)
11) UTC-05:00 Colombia, Cuba, Ecuador (continental), Haiti, Jamaica, Panama, Peru
12) UTC-05:00 (Summer)Canada (most of Ontario, most of Quebec)
13) UTC-05:00 United States (most of Florida, Georgia, Massachusetts, most of Michigan, New York, North Carolina, Ohio, Washington D.C.)
14) UTC-04:30 Venezuela
15) UTC-04:00 Bolivia, Brazil (Amazonas), Chile (continental), Dominican Republic, Canada (Nova Scotia), Paraguay,
16) UTC-04:00 Puerto Rico, Trinidad and Tobago
17) UTC-03:30 Canada (Newfoundland)
18) UTC-03:00 Argentina; (Summer) Brazil (Brasilia, Rio de Janeiro, Sao Paulo), most of Greenland, Uruguay
19) UTC-02:00 Brazil (Fernando de Noronha), South Georgia and the South Sandwich Islands
20) UTC-01:00 Portugal (Azores), Cape Verde
21) UTC&#177;00:00 Cote d'Ivoire, Faroe Islands, Ghana, Iceland, Senegal; (Summer) Ireland, Portugal (continental and Madeira)
22) UTC&#177;00:00 Spain (Canary Islands), Morocco, United Kingdom
23) UTC+01:00 Angola, Cameroon, Nigeria, Tunisia; (Summer)Albania, Algeria, Austria, Belgium, Bosnia and Herzegovina,
24) UTC+01:00 Spain (continental), Croatia, Czech Republic, Denmark, Germany, Hungary, Italy, Kinshasa, Kosovo,
25) UTC+01:00 Macedonia, France (metropolitan), the Netherlands, Norway, Poland, Serbia, Slovakia, Slovenia, Sweden, Switzerland
26) UTC+02:00 Libya, Egypt, Malawi, Mozambique, South Africa, Zambia, Zimbabwe, (Summer)Bulgaria, Cyprus, Estonia,
27) UTC+02:00 Finland, Greece, Israel, Jordan, Latvia, Lebanon, Lithuania, Moldova, Palestine, Romania, Syria, Turkey, Ukraine
28) UTC+03:00 Belarus, Djibouti, Eritrea, Ethiopia, Iraq, Kenya, Madagascar, Russia (Kaliningrad Oblast), Saudi Arabia,
29) UTC+03:00 South Sudan, Sudan, Somalia, South Sudan, Tanzania, Uganda, Yemen
30) UTC+03:30 (Summer)Iran
31) UTC+04:00 Armenia, Azerbaijan, Georgia, Mauritius, Oman, Russia (European), Seychelles, United Arab Emirates
32) UTC+04:30 Afghanistan
33) UTC+05:00 Kazakhstan (West), Maldives, Pakistan, Uzbekistan
34) UTC+05:30 India, Sri Lanka
35) UTC+05:45 Nepal
36) UTC+06:00 Kazakhstan (most), Bangladesh, Russia (Ural: Sverdlovsk Oblast, Chelyabinsk Oblast)
37) UTC+06:30 Cocos Islands, Myanmar
38) UTC+07:00 Jakarta, Russia (Novosibirsk Oblast), Thailand, Vietnam
39) UTC+08:00 China, Hong Kong, Russia (Krasnoyarsk Krai), Malaysia, Philippines, Singapore, Taiwan, most of Mongolia, Western Australia
40) UTC+09:00 Korea, East Timor, Russia (Irkutsk Oblast), Japan
41) UTC+09:30 Australia (Northern Territory);(Summer)Australia (South Australia))
42) UTC+10:00 Russia (Zabaykalsky Krai); (Summer)Australia (New South Wales, Queensland, Tasmania, Victoria)
43) UTC+10:30 Lord Howe Island
44) UTC+11:00 New Caledonia, Russia (Primorsky Krai), Solomon Islands
45) UTC+11:30 Norfolk Island
46) UTC+12:00 Fiji, Russia (Kamchatka Krai);(Summer)New Zealand
47) UTC+12:45 (Summer)New Zealand
48) UTC+13:00 Tonga
49) UTC+14:00 Kiribati (Line Islands)
*/
void get_seconds_from_ntp_server(uint8_t *buf, uint16_t idx)
{
	tstamp seconds = 0;
	uint8_t i=0;
	for (i = 0; i < 4; i++)
	{
		seconds = (seconds << 8) | buf[idx + i];
	}
	switch (time_zone)
	{
	case 0:
		seconds -=  12*3600;
		break;
	case 1:
		seconds -=  11*3600;
		break;
	case 2:
		seconds -=  10*3600;
		break;
	case 3:
		seconds -=  (9*3600+30*60);
		break;
	case 4:
		seconds -=  9*3600;
		break;
	case 5:
	case 6:
		seconds -=  8*3600;
		break;
	case 7:
	case 8:
		seconds -=  7*3600;
		break;
	case 9:
	case 10:
		seconds -=  6*3600;
		break;
	case 11:
	case 12:
	case 13:
		seconds -= 5*3600;
		break;
	case 14:
		seconds -=  (4*3600+30*60);
		break;
	case 15:
	case 16:
		seconds -=  4*3600;
		break;
	case 17:
		seconds -=  (3*3600+30*60);
		break;
	case 18:
		seconds -=  3*3600;
		break;
	case 19:
		seconds -=  2*3600;
		break;
	case 20:
		seconds -=  1*3600;
		break;
	case 21:                            //ï¼?
	case 22:
		break;
	case 23:
	case 24:
	case 25:
		seconds +=  1*3600;
		break;
	case 26:
	case 27:
		seconds +=  2*3600;
		break;
	case 28:
	case 29:
		seconds +=  3*3600;
		break;
	case 30:
		seconds +=  (3*3600+30*60);
		break;
	case 31:
		seconds +=  4*3600;
		break;
	case 32:
		seconds +=  (4*3600+30*60);
		break;
	case 33:
		seconds +=  5*3600;
		break;
	case 34:
		seconds +=  (5*3600+30*60);
		break;
	case 35:
		seconds +=  (5*3600+45*60);
		break;
	case 36:
		seconds +=  6*3600;
		break;
	case 37:
		seconds +=  (6*3600+30*60);
		break;
	case 38:
		seconds +=  7*3600;
		break;
	case 39:
		seconds +=  8*3600;
		break;
	case 40:
		seconds +=  9*3600;
		break;
	case 41:
		seconds +=  (9*3600+30*60);
		break;
	case 42:
		seconds +=  10*3600;
		break;
	case 43:
		seconds +=  (10*3600+30*60);
		break;
	case 44:
		seconds +=  11*3600;
		break;
	case 45:
		seconds +=  (11*3600+30*60);
		break;
	case 46:
		seconds +=  12*3600;
		break;
	case 47:
		seconds +=  (12*3600+45*60);
		break;
	case 48:
		seconds +=  13*3600;
		break;
	case 49:
		seconds +=  14*3600;
		break;

	}

	//calculation for date
	calcdatetime(seconds);
}

void SNTP_init(uint8_t s, uint8_t *ntp_server, uint8_t tz, uint8_t *buf)
{
	NTP_SOCKET = s;

	NTPformat.dstaddr[0] = ntp_server[0];
	NTPformat.dstaddr[1] = ntp_server[1];
	NTPformat.dstaddr[2] = ntp_server[2];
	NTPformat.dstaddr[3] = ntp_server[3];

	time_zone = tz;

	data_buf = buf;

	uint8_t Flag;
	NTPformat.leap = 0;           /* leap indicator */
	NTPformat.version = 4;        /* version number */
	NTPformat.mode = 3;           /* mode */
	NTPformat.stratum = 0;        /* stratum */
	NTPformat.poll = 0;           /* poll interval */
	NTPformat.precision = 0;      /* precision */
	NTPformat.rootdelay = 0;      /* root delay */
	NTPformat.rootdisp = 0;       /* root dispersion */
	NTPformat.refid = 0;          /* reference ID */
	NTPformat.reftime = 0;        /* reference time */
	NTPformat.org = 0;            /* origin timestamp */
	NTPformat.rec = 0;            /* receive timestamp */
	NTPformat.xmt = 1;            /* transmit timestamp */

	Flag = (NTPformat.leap<<6)+(NTPformat.version<<3)+NTPformat.mode; //one byte Flag
	memcpy(ntpmessage,(void const*)(&Flag),1);
}

int8_t SNTP_run(datetime *time)
{
	uint16_t RSR_len;
	uint32_t destip = 0;
	uint16_t destport;
	uint16_t startindex = 40; //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40

	switch(getSn_SR(NTP_SOCKET))
	{
	case SOCK_UDP:
		if ((RSR_len = getSn_RX_RSR(NTP_SOCKET)) > 0)
		{
			if (RSR_len > MAX_SNTP_BUF_SIZE) RSR_len = MAX_SNTP_BUF_SIZE;	// if Rx data size is lager than TX_RX_MAX_BUF_SIZE
			recvfrom(NTP_SOCKET, data_buf, RSR_len, (uint8_t *)&destip, &destport);

			get_seconds_from_ntp_server(data_buf,startindex);
			time->yy = Nowdatetime.yy;
			time->mo = Nowdatetime.mo;
			time->dd = Nowdatetime.dd;
			time->hh = Nowdatetime.hh;
			time->mm = Nowdatetime.mm;
			time->ss = Nowdatetime.ss;

			ntp_retry_cnt=0;
			close(NTP_SOCKET);

			return 1;
		}

		if(ntp_retry_cnt<0xFFFF)
		{
			if(ntp_retry_cnt==0)//first send request, no need to wait
			{
				sendto(NTP_SOCKET,ntpmessage,sizeof(ntpmessage),NTPformat.dstaddr,ntp_port);
				ntp_retry_cnt++;
			}
			else // send request again? it should wait for a while
			{
				if((ntp_retry_cnt % 0xFFF) == 0) //wait time
				{
					sendto(NTP_SOCKET,ntpmessage,sizeof(ntpmessage),NTPformat.dstaddr,ntp_port);
#ifdef _SNTP_DEBUG_
					printf("ntp retry: %d\r\n", ntp_retry_cnt);
#endif
					ntp_retry_cnt++;
				}
			}
		}
		else //ntp retry fail
		{
			ntp_retry_cnt=0;
#ifdef _SNTP_DEBUG_
			printf("ntp retry failed!\r\n");
#endif
			close(NTP_SOCKET);
		}
		break;
	case SOCK_CLOSED:
		socket(NTP_SOCKET,Sn_MR_UDP,ntp_port,0);
		break;
	}
	// Return value
	// 0 - failed / 1 - success
	return 0;
}

void calcdatetime(tstamp seconds)
{
	uint8_t yf=0;
	tstamp n=0,d=0,total_d=0,rz=0;
	uint16_t y=0,r=0,yr=0;
	signed long long yd=0;

	n = seconds;
	total_d = seconds/(SECS_PERDAY);
	d=0;
	uint32_t p_year_total_sec=SECS_PERDAY*365;
	uint32_t r_year_total_sec=SECS_PERDAY*366;
	while(n>=p_year_total_sec)
	{
		if((EPOCH+r)%400==0 || ((EPOCH+r)%100!=0 && (EPOCH+r)%4==0))
		{
			n = n -(r_year_total_sec);
			d = d + 366;
		}
		else
		{
			n = n - (p_year_total_sec);
			d = d + 365;
		}
		r+=1;
		y+=1;

	}

	y += EPOCH;

	Nowdatetime.yy = y;

	yd=0;
	yd = total_d - d;

	yf=1;
	while(yd>=28)
	{

		if(yf==1 || yf==3 || yf==5 || yf==7 || yf==8 || yf==10 || yf==12)
		{
			yd -= 31;
			if(yd<0)break;
			rz += 31;
		}

		if (yf==2)
		{
			if (y%400==0 || (y%100!=0 && y%4==0))
			{
				yd -= 29;
				if(yd<0)break;
				rz += 29;
			}
			else
			{
				yd -= 28;
				if(yd<0)break;
				rz += 28;
			}
		}
		if(yf==4 || yf==6 || yf==9 || yf==11 )
		{
			yd -= 30;
			if(yd<0)break;
			rz += 30;
		}
		yf += 1;

	}
	Nowdatetime.mo=yf;
	yr = total_d-d-rz;

	yr += 1;

	Nowdatetime.dd=yr;

	//calculation for time
	seconds = seconds%SECS_PERDAY;
	Nowdatetime.hh = seconds/3600;
	Nowdatetime.mm = (seconds%3600)/60;
	Nowdatetime.ss = (seconds%3600)%60;

}

tstamp changedatetime_to_seconds(void)
{
	tstamp seconds=0;
	uint32_t total_day=0;
	uint16_t i=0,run_year_cnt=0,l=0;

	l = Nowdatetime.yy;//low


	for(i=EPOCH;i<l;i++)
	{
		if((i%400==0) || ((i%100!=0) && (i%4==0)))
		{
			run_year_cnt += 1;
		}
	}

	total_day=(l-EPOCH-run_year_cnt)*365+run_year_cnt*366;

	for(i=1;i<=Nowdatetime.mo;i++)
	{
		if(i==5 || i==7 || i==10 || i==12)
		{
			total_day += 30;
		}
		if (i==3)
		{
			if (l%400==0 && l%100!=0 && l%4==0)
			{
				total_day += 29;
			}
			else
			{
				total_day += 28;
			}
		}
		if(i==2 || i==4 || i==6 || i==8 || i==9 || i==11)
		{
			total_day += 31;
		}
	}

	seconds = (total_day+Nowdatetime.dd-1)*24*3600;
	seconds += Nowdatetime.ss;//seconds
	seconds += Nowdatetime.mm*60;//minute
	seconds += Nowdatetime.hh*3600;//hour

	return seconds;
}
