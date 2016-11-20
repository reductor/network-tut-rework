#include "Server.h"

bool Server::recvall(Connection & connection, char * data, int totalbytes)
{
	int bytesreceived = 0; //Holds the total bytes received
	while (bytesreceived < totalbytes) //While we still have more bytes to recv
	{
		int RetnCheck = recv(connection.socket, data, totalbytes - bytesreceived, NULL); //Try to recv remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to recv bytes
			return false; //Return false - failed to recvall
		bytesreceived += RetnCheck; //Add to total bytes received
	}
	return true; //Success!
}

bool Server::sendall(Connection & connection, char * data, int totalbytes)
{
	int bytessent = 0; //Holds the total bytes sent
	while (bytessent < totalbytes) //While we still have more bytes to send
	{
		int RetnCheck = send(connection.socket, data + bytessent, totalbytes - bytessent, NULL); //Try to send remaining bytes
		if (RetnCheck == SOCKET_ERROR) //If there is a socket error while trying to send bytes
			return false; //Return false - failed to sendall
		bytessent += RetnCheck; //Add to total bytes sent
	}
	return true; //Success!
}

bool Server::Sendint32_t(Connection & connection, int32_t _int32_t)
{
	_int32_t = htonl(_int32_t); //Convert long from Host Byte Order to Network Byte Order
	if (!sendall(connection, (char*)&_int32_t, sizeof(int32_t))) //Try to send long (4 byte int)... If int fails to send
		return false; //Return false: int not successfully sent
	return true; //Return true: int successfully sent
}

bool Server::Getint32_t(Connection & connection, int32_t & _int32_t)
{
	if (!recvall(connection, (char*)&_int32_t, sizeof(int32_t))) //Try to receive long (4 byte int)... If int fails to be recv'd
		return false; //Return false: Int not successfully received
	_int32_t = ntohl(_int32_t); //Convert long from Network Byte Order to Host Byte Order
	return true;//Return true if we were successful in retrieving the int
}

bool Server::SendPacketType(Connection & connection, PacketType _packettype)
{
	if (!Sendint32_t(connection, (int32_t)_packettype)) //Try to send packet type... If packet type fails to send
		return false; //Return false: packet type not successfully sent
	return true; //Return true: packet type successfully sent
}

bool Server::GetPacketType(Connection & connection, PacketType & _packettype)
{
	int packettype;
	if (!Getint32_t(connection, packettype)) //Try to receive packet type... If packet type fails to be recv'd
		return false; //Return false: packet type not successfully received
	_packettype = (PacketType)packettype;
	return true;//Return true if we were successful in retrieving the packet type
}

void Server::SendString(Connection & connection, std::string & _string)
{
	PS::ChatMessage message(_string);
	connection.pm.Append(message.toPacket());
}

bool Server::GetString(Connection & connection, std::string & _string)
{
	int32_t bufferlength; //Holds length of the message
	if (!Getint32_t(connection, bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char * buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	if (!recvall(connection, buffer, bufferlength)) //receive message and store the message in buffer array. If buffer fails to be received...
	{
		delete[] buffer; //delete buffer to prevent memory leak
		return false; //return false: Fails to receive string buffer
	}
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	return true;//Return true if we were successful in retrieving the string
}
