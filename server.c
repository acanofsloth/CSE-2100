//modified from https://www.educative.io/edpresso/how-to-implement-tcp-sockets-in-c
//further modified by Ivan Martinez
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //needed for socket related functions
#include <arpa/inet.h> //needed for internet related functions

#define SIZE 2000
int checkSum(char packer[],int length);
char convertChar2HEX(int letter);
int convertHEX2DEC(char letter);
void printPacket(char packet[],int length);

int main(void)
{
    int socket_desc; //status of socket creation
    int client_sock; //status of client connection
    int client_size;//
    int inputPort;
    int message_length = 0;
    char choice = 'y';
    struct sockaddr_in server_addr; //struct containing information about our server address
    struct sockaddr_in client_addr; //struct containing information about our client address
    char server_message[SIZE]; //array containing the server message
    char client_message[SIZE]; //array containing the clinet message
    char inputIP[16];
    
    //Set message arrays to null
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create socket. will return a negative if unsucessful
    socket_desc = socket(AF_INET, SOCK_STREAM, 0); 
    
    //Check to see if socket sucessfully created
    if(socket_desc < 0){
    	//Display error and exit if not
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    if(socket_desc >= 0) 
    {
	printf("\nPort value: ");
	scanf("%d",&inputPort);
	printf("\nIP address: ");
	scanf("%s",&inputIP);
    }

    // Set port and IP:
    server_addr.sin_family = AF_INET; //set socket domain
    server_addr.sin_port = htons(inputPort); //set socket port
    server_addr.sin_addr.s_addr = inet_addr(inputIP); //set socket IP address
    
    // Bind to the set port and IP
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    	//Display error and exit if unsucessful
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    //while(choice == 'y') //Start infinite loop​
    //{
    	// Listen for clients
    	if(listen(socket_desc, 1) < 0){
        	printf("Error while listening\n");
        	return -1;
    	}
    	printf("\nListening for incoming connections.....\n"); // Wait for messages​ 
    
    	// Accept an incoming connection and get information about our client
    	client_size = sizeof(client_addr);
    	client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    	if (client_sock < 0){
    		//Display error and don't connect if unable to connect
        	printf("Can't accept\n");
        	return -1;
    	}
    	printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    	// Receive client's message:
    	if (recv(client_sock, client_message, SIZE, 0) < 0){
        	printf("Couldn't receive\n");
        	return -1;
    	}
	message_length = convertHEX2DEC(client_message[2])*16 + convertHEX2DEC(client_message[3]);

	if(checkSum(client_message,message_length*2) == 1)  //Check to see if it is valid, including the checksum calculation​ 
	{
    		printf("Msg from client: ", client_message); //  Print out message to terminal​ 
		printPacket(client_message,message_length*2);
		printf("\n");
    		strcpy(server_message, "Message Received."); // Send message back to client saying message received​ 
    	}
	else
	{
		strcpy(server_message, "Message Rejected."); // Send message back to client saying message rejected​
	}
    	if (send(client_sock, server_message, strlen(server_message), 0) < 0){
        	printf("Can't send\n");
        	return -1;
    	}
	//printf("\nDo you want to recieve another message? (y/n) ");
	//scanf("%s",&choice);
	//if(choice != 'y')
	//{
	//	choice = 'n'; // End "infinite loop"
	//}
    //}
    // Closing the socket:
    close(client_sock);
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
int checkSum(char packet[],int length)
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
		temp = result/16;
		//printf("1 temp = %d , packet[l-2] = %d",convertChar2HEX(temp),packet[length-2]);
		if((packet[length-1] == '0') && (packet[length-2] == convertChar2HEX(temp)))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		temp = (result/16);
		//printf("2 temp = %d , packet[l-2] = %d",convertChar2HEX(temp),packet[length-2]);
		if(packet[length-2] == convertChar2HEX(temp))
		{
			temp = (result%16);
			if(packet[length-1] == convertChar2HEX(temp))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
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
