// poc_kmt.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include <windows.h>
#include <iostream>
#include <ktmw32.h>
#include <handleapi.h>

//from https://stackoverflow.com/a/17387176, i'm lazy//
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
/////////////

int main()
{
	HANDLE trans = CreateTransaction(NULL,
		0,
		1,
		0,
		0,
		INFINITE,
		0);

	if (trans != INVALID_HANDLE_VALUE)
	{
		HANDLE fileHandle = CreateFileTransactedA("poc_ktm.txt",
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_HIDDEN,
			NULL,
			trans,
			0,
			NULL);

		char str[] = "POC KTM";
		DWORD bytesWritten;

		WriteFile(fileHandle, str, strlen(str), &bytesWritten, NULL);

		CloseHandle(fileHandle);

		BOOL res = DeleteFileTransactedA("poc_ktm.txt", trans);

		if (res != FALSE)
		{
			std::cout << "Everything is alright." << std::endl;

			CommitTransaction(trans);
		}
		else
		{
			std::cout << "Oh no..." << std::endl;
			std::cout << GetLastErrorAsString().c_str() << std::endl;

			RollbackTransaction(trans);
		}

	}
	else
	{
		std::cout << "Oh no... could not even create a transaction" << std::endl;
		std::cout << GetLastErrorAsString().c_str() << std::endl;
	}

}