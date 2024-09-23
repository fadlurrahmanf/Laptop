#include "neo6mGPS.h"




void neo6mGPS::begin(HardwareSerial &port)
{
	_port = &port;
	_port->begin(9600);

	setupGPS(115200, 10);
}




void neo6mGPS::begin(usb_serial_class &port)
{
	usingUSB = true;
	usb_port = &port;
	usb_port->begin(9600);

	setupGPS(115200, 10);
}




void neo6mGPS::begin(HardwareSerial &port, uint32_t baud, uint16_t hertz)
{
	_port = &port;
	_port->begin(9600);

	setupGPS(baud, hertz);
}




void neo6mGPS::begin(usb_serial_class &port, uint32_t baud, uint16_t hertz)
{
	usingUSB = true;
	usb_port = &port;
	usb_port->begin(9600);

	setupGPS(baud, hertz);
}




void neo6mGPS::setupGPS(uint32_t baud, uint16_t hertz)
{
	disableAllNmea();
	enableSelectedNmea();
	changeBaud(baud);
	changeFreq(hertz);
}




void neo6mGPS::disableAllNmea()
{
	setSentence(GPGGA, false);
	setSentence(GPGLL, false);
	setSentence(GPGLV, false);
	setSentence(GPGSA, false);
	setSentence(GPRMC, false);
	setSentence(GPVTG, false);
}




void neo6mGPS::enableAllNmea()
{
	setSentence(GPGGA, true);
	setSentence(GPGLL, true);
	setSentence(GPGSA, true);
	setSentence(GPGLV, true);
	setSentence(GPRMC, true);
	setSentence(GPVTG, true);
}




void neo6mGPS::enableSelectedNmea()
{
	//comment or uncomment based on what sentences desired

	//setSentence(GPGGA, true);
	//setSentence(GPGLL, true);
	//setSentence(GPGSA, true);
	//setSentence(GPGLV, true);
	setSentence(GPRMC, true);
	//setSentence(GPVTG, true);
}




void neo6mGPS::changeBaud(uint32_t baud)
{
	char configPacket[BAUD_LEN];
	memcpy(configPacket, CFG_PRT, BAUD_LEN);

	configPacket[BAUD_0] = (char)(baud & 0xFF);
	configPacket[BAUD_1] = (char)((baud >> 8) & 0xFF);
	configPacket[BAUD_2] = (char)((baud >> 16) & 0xFF);
	configPacket[BAUD_3] = (char)((baud >> 24) & 0xFF);

	insertChecksum(configPacket, BAUD_LEN);
	sendPacket(configPacket, BAUD_LEN);

	delay(100);

	if (usingUSB)
	{
		usb_port->flush();
		usb_port->begin(baud);
	}
	else
	{
		_port->flush();
		_port->begin(baud);
	}
}




void neo6mGPS::changeFreq(uint16_t hertz)
{
	uint16_t normHerz = hertz / (1000 / ((CFG_RATE[MEAS_RATE_2] << 8) | CFG_RATE[MEAS_RATE_1]));
	char configPacket[FREQ_LEN];
	memcpy(configPacket, CFG_RATE, FREQ_LEN);

	configPacket[NAV_RATE_1] = (char)(normHerz & 0xFF);
	configPacket[NAV_RATE_2] = (char)((normHerz >> 8) & 0xFF);

	insertChecksum(configPacket, FREQ_LEN);
	sendPacket(configPacket, FREQ_LEN);
}




bool neo6mGPS::available()
{
	char recChar;
	bool endProcessing;

	if (usingUSB)
	{
		while (usb_port->available())
		{
			recChar = usb_port->read();
			endProcessing = parseData(recChar);

			if (endProcessing)
				return true;
		}
	}
	else
	{
		while (_port->available())
		{
			recChar = _port->read();
			endProcessing = parseData(recChar);

			if (endProcessing)
				return true;
		}
	}

	return false;
}




bool neo6mGPS::parseData(char recChar)
{
	if (recChar == '\n')
	{
		startByteFound = false;
		fieldNum = 0;
		fieldIndex = 0;
		updateValues();
			
		return true;
	}
	else if (((recChar == ',') && startByteFound) || ((recChar == '*') && startByteFound))
	{
		fieldNum++;
		fieldIndex = 0;
	}
	else if (!startByteFound)
	{
		if (recChar == '$')
		{
			startByteFound = true;
			data[fieldNum][fieldIndex] = recChar;
			fieldIndex++;
		}
	}
	else
	{
		if ((fieldNum < NUM_FIELDS) && (fieldIndex < FIELD_LEN))
		{
			data[fieldNum][fieldIndex] = recChar;
			fieldIndex++;
		}
	}

	return false;
}




// hhmmss.sss
void neo6mGPS::calc_utc_time(float time)
{
	utc_hour = (int)(time / 10000);
	utc_min = (int)((time - (utc_hour * 10000)) / 100);
	utc_sec = time - (utc_hour * 10000) - (utc_min * 100);
}




// ddmmyy
void neo6mGPS::calc_utc_date(int date)
{
	utc_day = date / 10000;
	utc_month = (date - (utc_day * 10000)) / 100;
	utc_year = 2000 + date - (utc_day * 10000) - (utc_month * 100);
}




// (d)ddmm.mmmm, D
float neo6mGPS::dm_dd(float loc, char dir)
{
	float result = (int)(loc / 100);

	result += (loc - (result * 100)) / 60.0;

	if (dir == 'S' || dir == 'W')
		result = -result;

	return result;
}




void neo6mGPS::updateValues()
{
	if (findSentence(GPGGA_header))
	{
		calc_utc_time(atof(data[1]));

		l{"alt_text":"Lunar New Year 2023","animated_url":"","can_show_after_expiration":false,"cta_log_url":"","dark_animated_url":"","dark_background_color":"#481414","dark_cta_log_url":"","dark_height_px":200,"dark_log_url":"https://www.google.com/async/ddllog?async=doodle:207425729,slot:54,type:1,cta:0","dark_mime_type":"image/png","dark_num_bytes":42284,"dark_share_button_bg":"#ffffff","dark_share_button_icon":"iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAAAAXNSR0IArs4c6QAAASlJREFUSEvNlTFKA0EUhr83W5jF9QaCNnbpcgHBwgOIgh7AJK2mtLVTsEvUA1joBcTGA0QrOxv1CllNBJ0nKy4ksNmdGQlk2mE+3rz/ezNCwBp2k3URdkVlMzuuoreqXMXt9D7HiS83gxrDCUpj4qzwYC2dHO4NHvWSc4H9ooIULmqttJnteYM/u0sviK4U3lTldaE9WJ0f8LC3uGGQM6Du3IqypD8u4+XoOzoFdv6Az8BaZXhTk4ZHhb7AHpAAKSrHGtk+VrYrdStLeqyqGytfh3Fz9Oai6K8VpUnDQNGtWuv9zgU4MSCuCnmDK1rxpOhBUMUl4Y2n79/j7IpFumH0WqxpgB4FWVHVu2CPq8D5ftDkucJdDZqf121m7/HMfpBpav77z3MN+QfLddUXZUGCLQAAAABJRU5ErkJggg==","dark_share_button_opacity":0.0,"dark_share_button_x":474,"dark_share_button_y":174,"dark_width_px":500,"expiration_time":"13318880399743645","fingerprint":"c1246630","full_page_url":"https://www.google.com/?hl=en-GB&gl=ID&ntp=2","height_px":200,"iframe_height_px":0,"iframe_width_px":0,"log_url":"https://www.google.com/async/ddllog?async=doodle:207425729,slot:22,type:1,cta:0","mime_type":"image/png","num_bytes":42284,"on_click_url":"https://www.google.com/search?q=Lunar+New+Year&oi=ddle&ct=207425729&hl=en-GB&si=AEcPFx4IkMgAgMJkMTLSH7WR6YtBj6EFtw5Le3AcsuhiKcMgwsFgvACog3grhm702oglHmpIZ8gZD2dgQ3PG9qQztNe4ltMvQT7eN5EN_yACadHi0MAfGF0r4d9XmJQr9EYlWrGJ76jVY5HW5sM2TVS9Cfj9CShjVs_BNCXO0hdQOk2_mp2J_No%3D&source=doodle-ntp","share_button_bg":"#ffffff","share_button_icon":"iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAAAAXNSR0IArs4c6QAAASFJREFUSEvNlbFKw1AUhr9TVyHiZizo4ubmCwgOPoBU0Aeoe02hg6uDmIKrvkAHfYHi4gPUTm5d2lLaUYMdLUciiVgxuTdXCs16uR8n5//OuYLDN7le21fRE+Awud4WldZG/e0pxUlRbgINgb1fd59FJUjhhcHj0LsFqhkF3flBdBafuYD7wFYGeOAH0fbygEdN76Ck3AC71q3IS3p0tV6WlVlT4DgB9oAdY3iZSQtdlA5wCqwCU+BSlY4IFaNuhqTTwh74KJ37jdehjaJfVoxDLy/pd4WjzSB6tAHODYgB/K2QCzhP+heFmlPFOWP6M/3iPY5/8S/dVLmPd4IIF05WmHrn7LEJnJ47TZ4t3Nagpdpui9nHC3tBstT895tnG/Inv4uwFx0+LywAAAAASUVORK5CYII=","share_button_opacity":0.0,"share_button_x":474,"share_button_y":174,"short_link":"https://g.co/doodle/hpmjub2","type":"SIMPLE","url":"https:/