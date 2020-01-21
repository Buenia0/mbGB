// This file is part of libmbGB.
// Copyright (C) 2019 Buenia.
//
// libmbGB is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbGB is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbGB.  If not, see <https://www.gnu.org/licenses/>.

#include "../../include/libmbGB/addons.h"
using namespace gb;

namespace gb
{

	MobileAdapterGB::MobileAdapterGB()
	{
		
	}
	
	MobileAdapterGB::~MobileAdapterGB()
	{
		
	}
	
	void MobileAdapterGB::update()
	{
		processbyte();
		transfer();
	}
	
	void MobileAdapterGB::processbyte()
	{
		switch (adapterstate)
		{
			case State::AwaitingPacket:
			{
				switch (adapterbyte)
				{
					case 0x99:
					{
						packetdata.push_back(0x99);
						packetsize += 1;
						statesteps = 1;
						linkbyte = 0x4B;
					}
					break;
					case 0x66:
					{
						if (statesteps == 1)
						{
							packetdata.push_back(0x66);
							packetsize += 1;
							statesteps = 0;
							adapterstate = State::PacketHeader;
						}
						
						linkbyte = 0x4B;
					}
					break;
					default: linkbyte = 0x4B; break;
				}
			}
			break;
			case State::PacketHeader:
			{
				packetdata.push_back(adapterbyte);
				packetsize += 1;

				calculatedchecksum += adapterbyte;
				
				if (statesteps == 0)
				{
					commandid = adapterbyte;
				}
				else if (statesteps == 3)
				{
					packetdatalength = adapterbyte;
				}
				
				statesteps += 1;
				
				if (statesteps == 4)
				{
					statesteps = 0;
					adapterstate = (packetdatalength == 0) ? State::PacketChecksum : State::PacketData;
				}
				
				linkbyte = 0x4B;
			}
			break;
			case State::PacketData:
			{
				calculatedchecksum += adapterbyte;
				
				packetdata.push_back(adapterbyte);
				packetsize += 1;
				statesteps += 1;
				
				if (statesteps == packetdatalength)
				{
					statesteps = 0;
					adapterstate = State::PacketChecksum;
				}
				
				linkbyte = 0x4B;
			}
			break;
			case State::PacketChecksum:
			{
				packetdata.push_back(adapterbyte);
				packetsize += 1;

				if (statesteps == 0)
				{
				    comparechecksum = (adapterbyte << 8);
				    statesteps = 1;
				    linkbyte = 0x4B;
				}
				else if (statesteps == 1)
				{
				    comparechecksum |= (adapterbyte);
				    statesteps = 0;
				    packetsize = 0;

				    if (calculatedchecksum == comparechecksum)
				    {
					calculatedchecksum = 0;
					comparechecksum = 0;
					linkbyte = 0x4B;
					adapterstate = State::AcknowledgingPacket;
				    }
				    else
				    {
					linkbyte = 0xF1;
					adapterstate = State::AwaitingPacket;
				    }
				}
			}
			break;
			case State::AcknowledgingPacket:
			{
			    packetdata.push_back(adapterbyte);
			    packetsize += 1;

			    if (statesteps == 0)
			    {
				if (adapterbyte == 0x80)
				{
				    statesteps = 1;
				    linkbyte = 0x88;
				}
				else
				{
				    linkbyte = 0x4B;
				    statesteps = 0;
				}
			    }
			    else if (statesteps == 1)
			    {
				linkbyte = (0x80 ^ commandid);
				statesteps = 0;
				processcommand();
			    }
			}
			break;
			case State::EchoPacket:
			{
			    if (statesteps < (int)(packetdata.size()))
			    {
				linkbyte = packetdata[statesteps];
				statesteps += 1;

				if (statesteps == (int)(packetdata.size()))
				{
				    packetdata.clear();
				    statesteps = 0;
				    adapterstate = State::AwaitingPacket;
				}
			    }
			}
			break;
		}
	}

	void MobileAdapterGB::processcommand()
	{
	    switch (commandid)
	    {
		case 0x10:
		{
		    packetdata[(packetdata.size() - 2)] = 0x88;
		    packetdata[(packetdata.size() - 1)] = 0x00;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x11:
		{
		    packetdata[(packetdata.size() - 2)] = 0x88;
		    packetdata[(packetdata.size() - 1)] = 0x00;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x12:
		{
		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x12);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0x12);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;

		    linebusy = true;
		}
		break;
		case 0x13:
		{
		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x12);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0x12);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;

		    linebusy = false;
		}
		break;
		case 0x15:
		{
		    if (port == 110)
		    {
			cout << "Processing POP server data" << endl;
			processpop();
		    }
		    else if (port == 80)
		    {
			cout << "Processing HTTP data" << endl;
			processhttp();
		    }
		    else
		    {
			cout << "Port: " << dec << (int)(port) << endl;
			exit(1);
		    }
		}
		break;
		case 0x17:
		{
		    packetdata.clear();
		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x17);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x01);

		    if (linebusy)
		    {
			packetdata.push_back(0x04);
		    }
		    else
		    {
			packetdata.push_back(0x00);
		    }

		    packetdata.push_back(0x00);

		    if (linebusy)
		    {
			packetdata.push_back(0x1C);
		    }
		    else
		    {
			packetdata.push_back(0x18);
		    }

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x19:
		{
		    uint8_t configoffset = packetdata[6];
		    uint8_t configlength = packetdata[7];

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x19);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back((configlength + 1));
		    packetdata.push_back(configlength);

		    for (int x = configoffset; x < (configoffset + configlength); x++)
		    {
			if (x < 192)
			{
			    packetdata.push_back(adapterdata[x]);
			}
			else
			{
			    cout << "Error - Mobile adapter trying to read out-of-bounds memory" << endl;
			    return;
			}
		    }

		    uint16_t checksum = 0;

		    for (int x = 2; x < (int)(packetdata.size()); x++)
		    {
			checksum += packetdata[x];
		    }

		    packetdata.push_back((checksum >> 8) & 0xFF);
		    packetdata.push_back((checksum & 0xFF));

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x1A:
		{
		    cout << "Write configuration data" << endl;
		    uint8_t configoffset = packetdata[6];
		    
		    for (int x = 7; x < (packetdatalength + 6); x++)
		    {
			if (configoffset < 192)
			{
			    adapterdata[configoffset] = packetdata[x];
			    configoffset += 1;
			}
			else
			{
			    cout << "Error - Mobile adapter trying to write out-of-bounds memory" << endl;
			    return;
			}
		    }

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x1A);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);

		    uint16_t checksum = 0;

		    for (int x = 2; x < (int)(packetdata.size()); x++)
		    {
			checksum += packetdata[x];
		    }

		    packetdata.push_back((checksum >> 8) & 0xFF);
		    packetdata.push_back((checksum & 0xFF));

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x21:
		{
		    cout << "ISP Login" << endl;

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x21);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x04);

		    packetdata.push_back(0x7F);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x01);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0xA5);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x22:
		{
		    cout << "ISP Logout" << endl;

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x22);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0x22);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x23:
		{
		    cout << "Open TCP Connection" << endl;

		    ipaddr = getipaddr();
		    port = getport();

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0xA3);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x01);

		    packetdata.push_back(0x77);

		    packetdata.push_back(0x01);
		    packetdata.push_back(0x1B);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		case 0x24:
		{
		    cout << "Close TCP Connection" << endl;

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x24);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x01);

		    packetdata.push_back(0x77);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0x9C);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;

		    popsessionstarted = false;
		}
		break;
		case 0x28:
		{
		    cout << "DNS Query" << endl;

		    packetdata.clear();

		    packetdata.push_back(0x99);
		    packetdata.push_back(0x66);

		    packetdata.push_back(0x28);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x00);
		    packetdata.push_back(0x04);

		    packetdata.push_back(0x08);
		    packetdata.push_back(0x08);
		    packetdata.push_back(0x08);
		    packetdata.push_back(0x08);

		    packetdata.push_back(0x00);
		    packetdata.push_back(0x4C);

		    packetdata.push_back(0x88);
		    packetdata.push_back(0x00);

		    packetsize = 0;
		    adapterstate = State::EchoPacket;
		}
		break;
		default: cout << "Unrecongized Mobile Adapter command of " << hex << (int)(commandid) << endl; exit(1); break;
	    }
	}

    void MobileAdapterGB::processpop()
    {
	string popresponse = "";
	string popdata = datatostr(packetdata.data(), packetdata.size());
	uint8_t popcommand = 0xFF;
	uint8_t responseid = 0;

	size_t usermatch = popdata.find("USER");
	size_t passmatch = popdata.find("PASS");
	size_t quitmatch = popdata.find("QUIT");
	size_t statmatch = popdata.find("STAT");
	size_t topmatch = popdata.find("TOP");
	size_t delematch = popdata.find("DELE");
	size_t retrmatch = popdata.find("RETR");

	if (usermatch != string::npos)
	{
	    popcommand = 1;
	}
	else if (passmatch != string::npos)
	{
	    popcommand = 2;
	}
	else if (quitmatch != string::npos)
	{
	    popcommand = 3;
	}
	else if (statmatch != string::npos)
	{
	    popcommand = 4;
	}
	else if (topmatch != string::npos)
	{
	    popcommand = 5;
	}
	else if (delematch != string::npos)
	{
	    popcommand = 6;
	}
	else if (retrmatch != string::npos)
	{
	    popcommand = 7;
	}
	else if ((packetdatalength == 1) && (!popsessionstarted))
	{
	    popsessionstarted = true;
	    cout << "Initialzing POP server..." << endl;
	    popcommand = 0;
	}
	else if ((packetdatalength == 1) && (popsessionstarted))
	{
	    popsessionstarted = false;
	    cout << "Shutting down POP server..." << endl;
	    popcommand = 9;
	}
	else
	{
	    cout << "Unrecognized pop command" << endl;
	    exit(1);
	}

	switch (popcommand)
	{
	    case 0x00:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x01:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x02:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x03:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x04:
	    {
		popresponse = "+OK 1\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x05:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
		poptransferstate = 0x1;
	    }
	    break;
	    case 0x06:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
	    }
	    break;
	    case 0x07:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x95;
		poptransferstate = 0x11;
	    }
	    break;
	    case 0x09:
	    {
		popresponse = "+OK\r\n";
		responseid = 0x9F;
	    }
	    break;
	    default:
	    {
		exit(1);
		popresponse = "-ERR\r\n";
		responseid = 0x95;
	    }
	    break;
	}

	switch (poptransferstate)
	{
	    case 0x01:
	    {
		poptransferstate = 2;
	    }
	    break;
	    case 0x02:
	    {
		popresponse = "Date: Sun, Dec 24 2019 08:55:00 - 0600\r\n";
		poptransferstate = 3;
		responseid = 0x95;
	    }
	    break;
	    case 0x03:
	    {
		popresponse = isxmas() ? "Subject: Merry Christmas!\r\n" : "Subject: Buenia says hi!\r\n";
		poptransferstate = 4;
		responseid = 0x95;
	    }
	    break;
	    case 0x04:
	    {
		popresponse = "From: buenia@test.com\r\n";
		poptransferstate = 5;
		responseid = 0x95;
	    }
	    break;
	    case 0x05:
	    {
		popresponse = "To: user@test.com\r\n";
		poptransferstate = 6;
		responseid = 0x95;
	    }
	    break;
	    case 0x06:
	    {
		popresponse = "Content-Type: text/plain\r\n\r\n";
		poptransferstate = 7;
		responseid = 0x95;
	    }
	    break;
	    case 0x07:
	    {
		popresponse = ".\r\n";
		poptransferstate = 0;
		responseid = 0x95;
	    }
	    break;
	    case 0x11:
	    {
		poptransferstate = 0x12;
	    }
	    break;
	    case 0x12:
	    {
		popresponse = "Date: Sun, Dec 24 2019 08:55:00 - 0600\r\n";
		poptransferstate = 0x13;
		responseid = 0x95;
	    }
	    break;
	    case 0x13:
	    {
		popresponse = isxmas() ? "Subject: Merry Christmas!\r\n" : "Subject: Buenia says hi!\r\n";
		poptransferstate = 0x14;
		responseid = 0x95;
	    }
	    break;
	    case 0x14:
	    {
		popresponse = "From: buenia@test.com\r\n";
		poptransferstate = 0x15;
		responseid = 0x95;
	    }
	    break;
	    case 0x15:
	    {
		popresponse = "To: user@test.com\r\n";
		poptransferstate = 0x16;
		responseid = 0x95;
	    }
	    break;
	    case 0x16:
	    {
		popresponse = "Content-Type: text/plain\r\n\r\n";
		poptransferstate = 0x17;
		responseid = 0x95;
	    }
	    break;
	    case 0x17:
	    {
		popresponse = isxmas() ? "Happy holidays from libmbGB!\r\n\r\n" : "Hello from libmbGB!\r\n\r\n";
		poptransferstate = 0x18;
		responseid = 0x95;
	    }
	    break;
	    case 0x18:
	    {
		popresponse = ".\r\n";
		poptransferstate = 0;
		responseid = 0x95;
	    }
	    break;
	}

	packetdata.clear();
	packetdata.resize(7 + popresponse.size(), 0);

	packetdata[0] = 0x99;
	packetdata[1] = 0x66;

	packetdata[2] = responseid;
	packetdata[3] = 0x00;
	packetdata[4] = 0x00;
	packetdata[5] = (popresponse.size() + 1);

	strtodata((packetdata.data() + 7), popresponse);

	uint16_t checksum = 0;

	for (int x = 2; x < (int)(packetdata.size()); x++)
	{
	    checksum += packetdata[x];
	}

	packetdata.push_back((checksum >> 8) & 0xFF);
	packetdata.push_back((checksum & 0xFF));

	packetdata.push_back(0x88);
	packetdata.push_back(0x00);

	packetsize = 0;
	adapterstate = State::EchoPacket;
    }

    void MobileAdapterGB::processhttp()
    {
	string httpresponse = "";
	string httpheader = "";
	uint8_t responseid = 0;
	bool notfound = true;
	bool ishtml = (httpdata.find(".html") != string::npos);

	httpdata += datatostr((packetdata.data() + 7), (packetdata.size() - 7));
	cout << httpdata << endl;

	if (httpdata.find("\r\n\r\n") == string::npos)
	{
	    httpresponse = "";
	    responseid = 0x95;
	}
	else if (httptransferstate == 0)
	{
	    httptransferstate = 1;
	    size_t getmatch = httpdata.find("GET");

	    if (getmatch != string::npos)
	    {
	        if (httpdata.find(serverin[0]) != string::npos)
		{
		    notfound = false;
		}
	    }
	}

	switch (httptransferstate)
	{
	    case 0x1:
	    {
		if (notfound)
		{
		    httpresponse = "HTTP/1.0 404 Not Found\r\n";
		    httptransferstate = 2;
		}
		else
		{
		    httpresponse = "HTTP/1.0 200 OK\r\n";
		    httptransferstate = 2;
		}
	    }
	    break;
	    case 0x2:
	    {
		if (!notfound)
		{
		    httpresponse = "";
		    responseid = 0x9F;
		    httptransferstate = 0;
		    httpdata = "";
		}
		else if (ishtml)
		{
		    httpresponse = "Content-Type: text/html\r\n\r\n";
		    responseid = 0x95;
		    httptransferstate = 3;
		    httpdata = "";
		}
	    }
	    break;
	    case 3:
	    {
		for (int i = 0; i < 254; i++)
		{
		    if (dataindex < (int)(htmltext[isxmas()].size()))
		    {
			httpresponse += htmltext[isxmas()][dataindex++];
		    }
		    else
		    {
			i = 255;
		    }
		}

		responseid = 0x95;
		httptransferstate = (dataindex < (int)(htmltext[isxmas()].size())) ? 0x3 : 0x4;
	    }
	    break;
	    case 4:
	    {
		httpresponse = "";
		responseid = 0x9F;
		httptransferstate = 0;
		httpdata = "";
	    }
	    break;
	}

	packetdata.clear();
	packetdata.resize(7 + httpresponse.size(), 0);

	packetdata[0] = 0x99;
	packetdata[1] = 0x66;
	packetdata[2] = responseid;
	packetdata[3] = 0x00;
	packetdata[4] = 0x00;
	packetdata[5] = (httpresponse.size() + 1);

	strtodata((packetdata.data() + 7), httpresponse);

	uint16_t checksum = 0;

	for (int i = 2; i < (int)(packetdata.size()); i++)
	{
	    checksum += packetdata[i];
	}

	packetdata.push_back((checksum >> 8));
	packetdata.push_back((checksum & 0xFF));

	packetdata.push_back(0x88);
	packetdata.push_back(0x00);

	packetsize = 0;
	adapterstate = State::EchoPacket;
    }

    GBPrinter::GBPrinter()
    {

    }

    GBPrinter::~GBPrinter()
    {

    }

    void GBPrinter::update()
    {
	processbyte();
	transfer();
    }

    void GBPrinter::processbyte()
    {
	switch (currentstate)
	{
	    case State::MagicBytes:
	    {
		switch (printerbyte)
		{
		case 0x88:
		{
		    statesteps = 1;
		    linkbyte = 0;
		}
		break;
		case 0x33:
		{
		    if (statesteps == 1)
		    {
			statesteps = 0;
			currentstate = State::Command;
			linkbyte = 0;
		    }
		}
		break;
		}
	    }
	    break;
	    case State::Command:
	    {
		currentchecksum += printerbyte;
		currentcommand = printerbyte;

		linkbyte = 0;

		currentstate = State::CompressionFlag;
	    }
	    break;
	    case State::CompressionFlag:
	    {
		currentchecksum += printerbyte;
		linkbyte = 0;
		currentstate = State::DataLength;
	    }
	    break;
	    case State::DataLength:
	    {
		currentchecksum += printerbyte;

		if (statesteps == 0)
		{
		    commanddatalength = printerbyte;
		    statesteps = 1;
		    linkbyte = 0;
		}
		else
		{
		    commanddatalength |= (printerbyte << 8);

		    if (commanddatalength > 0)
		    {
			currentstate = State::CommandData;
		    }
		    else
		    {
			currentstate = State::Checksum;
		    }

		    statesteps = 0;
		    linkbyte = 0;

		    if (currentcommand == 1)
		    {
			printerram.fill(0);
			ramfillamount = 0;
			printerrequest = false;
		    }
		}
	    }
	    break;
	    case State::CommandData:
	    {
		currentchecksum += printerbyte;

		if (currentcommand == 2)
		{
		    if (statesteps == 2)
		    {
			printpalette = printerbyte;
		    }
		}
		else if (currentcommand == 4)
		{
		    printerram.at(ramfillamount) = printerbyte;
		    ramfillamount = ((ramfillamount + 1) & 0x1FFF);
		}

		statesteps += 1;

		if (statesteps == commanddatalength)
		{
		    currentstate = State::Checksum;
		    statesteps = 0;

		    if (currentcommand == 2)
		    {
			printerrequest = true;
			printpicture();
			ramfillamount = 0;
		    }
		}

		linkbyte = 0;
	    }
	    break;
	    case State::Checksum:
	    {
		if (statesteps == 0)
		{
		    comparechecksum = printerbyte;
		    statesteps = 1;
		    linkbyte = 0;
		}
		else if (statesteps == 1)
		{
		    comparechecksum |= (printerbyte << 8);
		    checksumpass = (comparechecksum == currentchecksum);
		    currentchecksum = 0;
		    currentstate = State::AliveIndicator;
		    statesteps = 0;
		    linkbyte = 0x81;
		}
	    }
	    break;	
	    case State::AliveIndicator:
	    {
		if (printerbyte == 0)
		{
		    currentstate = State::Status;
		}
		else
		{
		    currentstate = State::MagicBytes;
		}

		linkbyte = 0x81;
	    }
	    break;
	    case State::Status:
	    {
		if (currentcommand != 1)
		{
		    uint8_t tempbyte = 0x0;
		    tempbyte |= (!checksumpass) ? 1 : 0;
		    tempbyte |= ((printerrequest ? 1 : 0) << 2);
		    tempbyte |= (((ramfillamount >= 0x280) ? 1 : 0) << 3);
		    linkbyte = tempbyte;
		}
		else
		{
		    linkbyte = 0;
		}

		currentstate = State::MagicBytes;
	    }
	    break;
	}
    }

    void GBPrinter::printpicture()
    {
	int canvasheight = (int)(ceil(ceil(ramfillamount / 16.0) / 20.0) * 8);

	RGB white;
	white.red = 0xFF;
	white.green = 0xFF;
	white.blue = 0xFF;

	printoutbuffer.resize((160 * canvasheight), white);

	for (int i = 0; i < (int)(printoutbuffer.size()); i++)
	{
	    uint8_t tempbyte = 0;
	    int bufferx = (i % 160);
	    int buffery = (i / 160);
	    int tilenum = (bufferx / 8) + ((buffery / 8) * 20);
	    int tilestartloc = (tilenum * 16);
	    int tilex = (bufferx % 8);
	    int tiley = (buffery % 8);

	    uint8_t byte0 = printerram.at((tilestartloc + (tiley * 2)) & 0x1FFF);
	    uint8_t byte1 = printerram.at((tilestartloc + (tiley * 2) + 1) & 0x1FFF);
	    tempbyte = ((byte0 >> (7 - tilex)) & 0x1) | (((byte1 >> (7 - tilex)) & 0x1) << 1);
	    uint8_t palette = ((printpalette >> ((3 - tempbyte)) * 2) & 0x3);

	    int color = 0;

	    switch (palette)
	    {
		case 0: color = 0; break;
		case 1: color = 0x40; break;
		case 2: color = 0x80; break;
		case 3: color = 0xFF; break;
	    }

	    RGB temp;
	    temp.red = color;
	    temp.green = color;
	    temp.blue = color;

	    printoutbuffer.at(i) = temp;
	}

	printpixels(printoutbuffer);
    }

    LinkCable::LinkCable()
    {
		
    }
	
    LinkCable::~LinkCable()
    {
		
    }

    void LinkCable::update()
    {
	if (link1.ready && link2.ready)
	{
	    if (link1.mode != link2.mode)
	    {
		transfer();
	    }
	}
	else
	{
	    if (link1.ready)
	    {
		if (link1.mode)
		{
		    link2.byte = 0xFF;
		    transfer();
		}
	    }

	    if (link2.ready)
	    {
		if (link2.mode)
		{
		    link1.byte = 0xFF;
		    transfer();
		}
	    }
	}
    }
}
