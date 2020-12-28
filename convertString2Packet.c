// Code by Ivan Martinez
#include <stdio.h>
#define SIZE 30
char convertChar2HEX(int letter);
int convertHEX2DEC(char letter);
void printPacket(char packer[],int length);
void checkSum(char packer[],int length);

void main ()
{
	char input[SIZE+1];
	char packet[(SIZE*2)+7];
	int length = 3,pos1 = 0,temp;
	printf("Input: ");
	scanf("%[^\n]",input);
	for(int x = 0;input[x] != '\0';x++) // grab string length
	{
		length++;
	}
	packet[0] = 'A';
	packet[1] = 'A';
	//printf("%d\n",length%16);
	if((length%16) == 0)
	{
		packet[3] = 0;
		temp = (length)/16;
		packet[2] = convertChar2HEX(temp);
	}
	else
	{
		temp = (length)/16;
		packet[2] = convertChar2HEX(temp);
		temp = (length%16);
		packet[3] = convertChar2HEX(temp);
	}
	for(int x = 4;x < (length*2)-2;x=x+2)
	{
		if((input[pos1]%16) == 0)
		{
			packet[x+1] = '0';
			temp = input[pos1]/16;
			packet[x] = convertChar2HEX(temp);
		}
		else
		{
			temp = (input[pos1]/16);
			packet[x] = convertChar2HEX(temp);
			temp = (input[pos1]%16);
			packet[x+1] = convertChar2HEX(temp);
		}
		pos1++;
	}
	checkSum(packet,length*2);
	printPacket(packet,length*2);
	printf("\n");
}

void printPacket(char packet[],int length)
{
	for(int x = 0; x < length;x++)
	{
		if(x%2 == 0)
		{
			printf("0x");
		}
		if(packet[x] > 10)
		{
			printf("%c",packet[x]);
		}
		else
		{
			printf("%d",packet[x]);
		}
		if(x%2 == 1)
		{
			printf(" ");
		}
	}
}

void checkSum(char packet[],int length)
{
	int dec[SIZE+4];
	int binary[SIZE+3][8]={{0,0,0,0,0,0,0,0}};
	int pos2 =0,num;
	for(int x =0; x < length;x=x+2) //convert hex into dec
	{
		dec[pos2] = (convertHEX2DEC(packet[x])*16) + convertHEX2DEC(packet[x+1]);
		pos2++;
	}

	for(int y = 1; y < (length/2);y++) //convert decimal into binary
	{
		num = dec[y-1];
		for(int x = 0;x < 8;x++)
		{
			
			if(num%2 == 1)
			{
				binary[y][x] = 1;
			}
			else
			{
				binary[y][x] = 0;
			}
			num = num/2;
		}
	}
	
	for(int y = 1; y < (length/2);y++) // compare all bytes with xorg, save result onto binary[0]
	{
		for(int x = 0;x < 8;x++)
		{
			if(!(binary[0][x] == binary[y][x]))
			{
				binary[0][x] = 1;
			}
			else
			{
				binary[0][x] = 0;
			}
		}
	}

	int result = 0,pow = 1;
	for(int x = 0;x < 8;x++) // convert binary[0] into a decimal
	{
		if(binary[0][x] == 1)
		{
			if(x == 0)
			{
				result++;
			}
			else
			{
				for(int i = 0; i < x;i++)
				{
					pow = pow*2;	
				}
				result = result+pow;
				pow = 1;
			}
		}	

	}
	int temp;
	if(result%16 == 0) //convert decimal result into hex and place onto packet
	{
		packet[length-1] = '0';
		temp = result/16;
		packet[length-2] = convertChar2HEX(temp);
	}
	else
	{
		temp = (result/16);
		packet[length-2] = convertChar2HEX(temp);
		temp = (result%16);
		packet[length-1] = convertChar2HEX(temp);
	}
}

char convertChar2HEX(int letter)
{
	switch(letter)	
	{
		case 10:
			return 'A';
		case 11:
			return 'B';
		case 12:
			return 'C';
		case 13:
			return 'D';
		case 14:
			return 'E';
		case 15:
			return 'F';
		case 16:
			return 10;
		default:
			return letter;
	}
}
int convertHEX2DEC(char letter)
{
	switch(letter)	
	{
		case 'A':
			return 10;
		case 'B':
			return 11;
		case 'C':
			return 12;
		case 'D':
			return 13;
		case 'E':
			return 14;
		case 'F':
			return 15;
		case '0':
			return 0;
		default:
			return letter;
	}
}
