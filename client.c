//modified from https://www.educative.io/edpresso/how-to-implement-tcp-sockets-in-c
//further modified by Ivan Martinez
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //needed for socket related functions
#include <arpa/inet.h> //needed for internet related functions

#define SIZE 2000
char convertChar2HEX(int letter);
int convertHEX2DEC(char letter);
void printPacket(char packer[],int length);
void checkSum(char packer[],int length);

int main(void)
{
    int socket_desc, inputPort;
    char inputIP[16];
    char choice = 'y';
    struct sockaddr_in server_addr;
    char server_message[SIZE];
    char client_message[SIZE];
    char packet[SIZE];
    int length,pos1,temp;
    // Clean buffers:
    memset(server_message,'\0',sizeof(server_message));
    memset(client_message,'\0',sizeof(client_message));
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Unable to create socket\n");
        return -1;
    }
    
    printf("Socket created successfully\n");
    
    if(socket_desc >= 0){ // Take in IP address and Port as command line arguments​ 
	printf("\nPort value: ");
	scanf("%d",&inputPort);
	printf("\nIP address: ");
	scanf("%s",&inputIP);
    }
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(inputPort); 
    server_addr.sin_addr.s_addr = inet_addr(inputIP);
    
    //while(choice == 'y') // Start "infinite loop"​
    //{
    	// Send connection request to server:
    	if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        	printf("Unable to connect\n");
        	return -1;
    	}
    	printf("Connected with server successfully\n");
   
	length = 3;
	pos1 = 0;

    	// Take in message from command line
    	printf("Enter message: "); 
    	scanf("%s",client_message);
	// Convert it to the packet form from last lab
	for(int x = 0;client_message[x] != '\0';x++) 
	{
		length++;
	}
	printf("\n");
	packet[0] = 'A';
	packet[1] = 'A';
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
		if((client_message[pos1]%16) == 0)
		{
			packet[x+1] = '0';
			temp = client_message[pos1]/16;
			packet[x] = convertChar2HEX(temp);
		}
		else
		{
			temp = (client_message[pos1]/16);
			packet[x] = convertChar2HEX(temp);
			temp = (client_message[pos1]%16);
			packet[x+1] = convertChar2HEX(temp);
		}
		pos1++;
	}
	checkSum(packet,length*2);
	printPacket(packet,length*2); // Print out packet to terminal​ 
	printf("\n");	
	
    	//  Send packet to server​
    	if(send(socket_desc, packet, length*2, 0) < 0){ //
    	    printf("Unable to send message\n");
    	    return -1;
    	}
    	
    	// Receive the server's response:
    	if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
    	    printf("Error while receiving server's msg\n");
    	    return -1;
    	}
    
    	printf("Server's response: %s\n",server_message); //Print out reply from server
    
	//printf("\nDo you want to send another message? (y/n) ");
	//scanf("%s",&choice);
	//if(choice != 'y')
	//{
	//	choice = 'n'; // End "infinite loop"
	//}
    //}
    // Close the socket:
    close(socket_desc);
    
    return 0;
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
