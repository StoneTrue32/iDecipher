//  iDecipher!  a set of cryptographic tools by R. Austin
//
//
//  Naming convention:  Function_Call, LocalVariable, GlobalVariableGlob, structuretype.object, LocalVariablePtr
//  Note that n, m, Pick and similar items are counters or inputs and can be reused in the same function.
//
//  TO DO:  frequency checker & plain checker
//  TO DO:  utilities such as make all cipher CAPS, remove spaces, etc.
//  TO DO:  option in manual key entry to preserve plain chars if already there, work on "blanks"
//  TO DO:  crib-checker
//  TO DO:  some menu flow issues, e.g. move to analysis directly from main and cipher entry (in case the user backs up by accident)
//  TO DO:  break up into multiple .c, .h, with a makefile
//  TO DO:  reorder manual entry to asks most frequent first?  May reorder on key entry in the first place?
//  TO DO:  add data file "signing" and check so only opens data files generated by program


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CharSetSizeGlob 20		// ASCII character set size assumed

struct cipherdata {			// Cipher data structure:  size and pointers to ciphertext / plaintext on heap; move to .h file
	int ciphersize;
	int keysize;
	char cipherchar[CharSetSizeGlob];
	int frequency[CharSetSizeGlob];
	char plainchar[CharSetSizeGlob];
	char * ciphertextptr;
	char * plaintextptr;
};

//  Function prototypes

void Enter_New_Cipher(struct cipherdata *Cipher);
void Load_Cipher_From_File(struct cipherdata *Cipher);
void Enter_Cipher_Text(struct cipherdata *Cipher);
void Open_Ex_Analysis(struct cipherdata *Cipher);
void Open_Data_File(struct cipherdata *Cipher);
void Choose_Analysis(struct cipherdata *Cipher);
void Basic_Analysis(struct cipherdata *Cipher);
void Manual_Key_Entry(struct cipherdata *Cipher);
void Freq_Checker (struct cipherdata *Cipher);
void Display_Plaintext(struct cipherdata *Cipher);
int Error_Trap(int, int, int);
void Save_Data_File (struct cipherdata *Cipher);
void Clear_Data_File (struct cipherdata *Cipher);
void Exit_Program(struct cipherdata *Cipher);

// Command line display menus; move to separate *.c file
void Display_Main_Menu();
void Display_Enter_New_Cipher_Menu();
void Display_Open_Ex_Analysis_Menu();
void Display_Analysis_Menu();


//  Main starts here

int main()

{
	char Option[10] = { 0 };
	int n = 0;

	//  Define & initialize Cipher1; used throughout

	struct cipherdata Cipher1;
	(Cipher1).ciphersize = 0;
	Clear_Data_File(&Cipher1);

	printf("\nWelcome to iDecipher!\n");
	Display_Main_Menu();

	while (1)
	{
		if ( Error_Trap(atoi(fgets(Option,10,stdin)), 1, 3) == 1)
		{
			switch (Option[0])
			{
				case '1':
					Enter_New_Cipher(&Cipher1);
					break;
				case '2':
					Open_Ex_Analysis(&Cipher1);
					break;
				case '3':
					Exit_Program(&Cipher1);
				default:
					printf("\nDEBUG - switch default\n");
					break;
			}

			Display_Main_Menu();
		}
		else
		{
			printf("Try entering a number 1 - 3\n");
			Display_Main_Menu();
		}
	}
}

void Enter_New_Cipher(struct cipherdata *Cipher)

{
	Display_Enter_New_Cipher_Menu();

	char Option[10] = { 0 };

	while (1)
	{
		if ( Error_Trap(atoi(fgets(Option,10,stdin)), 1, 4) == 1)
		{
			switch (Option[0])
			{
				case '1':
					Load_Cipher_From_File(Cipher);
					Choose_Analysis(Cipher);
					break;
				case '2':
					Enter_Cipher_Text(Cipher);
					Choose_Analysis(Cipher);
					break;
				case '3':
					return;
				case '4':
					Exit_Program(Cipher);
				default:
					printf("\nDEBUG - switch default\n");
					break;
			}

			Display_Enter_New_Cipher_Menu();
		}
		else
		{
			printf("Try entering a number 1 - 4\n");
			Display_Enter_New_Cipher_Menu();
		}
	}
}

void Load_Cipher_From_File(struct cipherdata *Cipher)

//  Enter cipher from .txt file

{
	if ( (*Cipher).ciphertextptr != NULL )
	{	Save_Data_File(Cipher);			// Offer to save data
		Clear_Data_File(Cipher);		// Clear all data
	}	

	char CipherBuffer[10000] = { 0 };
	char c = 0;

	char FileName[32] = { 0 };
	int namelen, n = 0;
	FILE *fp;
	printf("Please enter the file name to retrieve (.txt will be appended automatically): ");
	fgets (FileName, 32, stdin);
	namelen = strlen (FileName);
	FileName[namelen - 1] = 0;
	strcat(FileName, ".txt");
	printf("DEBUG - File Name is %s \n", FileName);

	fp = fopen (FileName, "r+");
	if (fp == 0)
	{
		printf("Did you get the filename right?  Try again...\n");
		return;
	}

	int Test = feof (fp);
	while (Test != 1)
	{
		c = getc(fp);
		CipherBuffer[n] = c;
		n++;
		Test = feof (fp);
	}

	CipherBuffer[n- 2] = '\0';				// Overwrite \n with \0 null.

	(*Cipher).ciphersize = n - 2;			// The null at the end is not part of the cipher; deal with count from zero

	printf("DEBUG - ciphersize is %d", (*Cipher).ciphersize);

	(*Cipher).ciphertextptr = (char *) malloc((*Cipher).ciphersize);
	(*Cipher).plaintextptr = (char *) malloc((*Cipher).ciphersize);

	if ( ( (*Cipher).ciphertextptr == NULL) || ( (*Cipher).plaintextptr == NULL) )
	{
		printf ("Whoa!  Something happened.  Let's try again.\n");
		return;
	}

	n = 0;

	printf ("\nDEBUG - cipher text is: ");

	while (n <= (*Cipher).ciphersize)
	{
		(*Cipher).ciphertextptr[n] = CipherBuffer[n];
		putc((*Cipher).ciphertextptr[n],stdout);
		n++;
	}

	printf ("\n");

	return;
}

void Enter_Cipher_Text(struct cipherdata *Cipher)

//  Enter cipher from stdin, return a pointer

{
	if ( (*Cipher).ciphertextptr != NULL )
	{	Save_Data_File(Cipher);			// Offer to save data
		Clear_Data_File(Cipher);		// Clear all data
	}	

	char CipherBuffer[10000] = { 0 };
	char c = 0;
	int n = 0;

	printf ("Enter your cipher text here: ");

	while (c != '\n')
	{
		c = getc(stdin);
		CipherBuffer[n] = c;
		n++;
	}

	CipherBuffer[n] = '\0';				// A null on the end.

	(*Cipher).ciphersize = n - 1;			// The null at the end is not part of the cipher

	(*Cipher).ciphertextptr = (char *) malloc((*Cipher).ciphersize);
	(*Cipher).plaintextptr = (char *) malloc((*Cipher).ciphersize);

	if ( ( (*Cipher).ciphertextptr == NULL) || ( (*Cipher).plaintextptr == NULL) )
	{
		printf ("Whoa!  Something happened.  Let's try again.\n");
		return;
	}

	n = 0;

	printf ("\nYou entered: ");

	while (n <= (*Cipher).ciphersize)
	{
		(*Cipher).ciphertextptr[n] = CipherBuffer[n];
		putc((*Cipher).ciphertextptr[n],stdout);
		n++;
	}

	printf ("\n");

	return;
}

void Open_Ex_Analysis(struct cipherdata *Cipher)

{
	Display_Open_Ex_Analysis_Menu();

	char Option[10] = { 0 };

	while (1)
	{
		if ( Error_Trap(atoi(fgets(Option,10,stdin)), 1, 3) == 1)
		{
			switch (Option[0])
			{
				case '1':
					Open_Data_File(Cipher);
					Choose_Analysis(Cipher);
					break;
				case '2':
					return;
				case '3':
					Exit_Program(Cipher);
				default:
					printf("\nDEBUG - switch default\n");
					break;
			}

			Display_Open_Ex_Analysis_Menu();
		}
		else
		{
			printf("Try entering a number 1 - 3\n");
			Display_Open_Ex_Analysis_Menu();
		}
	}
}

void Open_Data_File(struct cipherdata *Cipher)

//  Opens an existing cipher and key file for continued analysis

{
	if ( (*Cipher).ciphertextptr != NULL )
	{	Save_Data_File(Cipher);			// Offer to save data
		Clear_Data_File(Cipher);		// Clear all data
	}	

	char FileName[32] = { 0 };
	char Buffer[256] = { 0 };			// A buffer to read strings into
	int namelen, n, m = 0;
	FILE *fp;
	printf("Please enter the data file name to retrieve (.dat will be appended automatically): ");
	fgets (FileName, 32, stdin);
	namelen = strlen (FileName);
	FileName[namelen - 1] = 0;
	strcat(FileName, ".dat");
	printf("\nDEBUG - File Name is %s \n", FileName);

	fp = fopen (FileName, "r+");
	if (fp == 0)
	{
		printf("Did you get the filename right?  Try again...\n");
		return;
	}

	// Read the cipher size from file
	fscanf(fp, "%d", &( (*Cipher).ciphersize) );
	printf("DEBUG - ciphersize is %d\n", (*Cipher).ciphersize );

	// Allocate heap for cipher and plain text
	(*Cipher).ciphertextptr = (char *) malloc((*Cipher).ciphersize);
	(*Cipher).plaintextptr = (char *) malloc((*Cipher).ciphersize);

	if ( ( (*Cipher).ciphertextptr == NULL) || ( (*Cipher).plaintextptr == NULL) )
	{
		printf ("Whoa!  Something happened.  Let's try again.\n");
		return;
	}

	// Read the cipher text from the file & put it on the heap
	fseek (fp, 1, SEEK_CUR);
	n = 0;
	printf ("DEBUG - Cipher text is: ");
	while ( n < (*Cipher).ciphersize )
	{
		(*Cipher).ciphertextptr[n] = fgetc(fp);
		putc((*Cipher).ciphertextptr[n],stdout);
		n++;
	}
	printf ("\n");

	// Read the plain text from the file & put it on the heap
	fseek (fp, 1, SEEK_CUR);
	n = 0;
	printf ("DEBUG - Plain text is: ");
	while (n < (*Cipher).ciphersize)
	{
		(*Cipher).plaintextptr[n] = fgetc(fp);
		putc((*Cipher).plaintextptr[n],stdout);
		n++;
	}
	printf ("\n");

	// Read the key size from file
	fseek (fp, 1, SEEK_CUR);
	fscanf(fp, "%d", &( (*Cipher).keysize) );
	printf("DEBUG - keysize is %d \n", (*Cipher).keysize );

	// Read the cipher key characters from the file
	fseek (fp, 1, SEEK_CUR);
	n = 0;
	m = 0;
	fgets (Buffer, sizeof(Buffer), fp);
	printf ("DEBUG - Cipher key characters are: ");
	while (Buffer[n] != '\n')
	{
		if (Buffer[n] != ' ')
		{
			(*Cipher).cipherchar[m] = Buffer[n];
			printf( "%c ", (*Cipher).cipherchar[m] );
			m++;
		}
		n++;
	}
	printf ("\n");

	// Read the frequency of cipher characters from the file
	//fseek (fp, 1, SEEK_CUR);
	n = 0;
	m = 0;
	memset( Buffer, 0, sizeof(Buffer) );
	fgets (Buffer, sizeof(Buffer), fp);
	int c = 0;
	char LocalBuff[256] = { 0 };
	printf ("DEBUG - Frequency of cipher characters are: ");
	while (Buffer[n] != '\n')
	{
		memset( LocalBuff, 0, sizeof(LocalBuff) );
		c = 0;
		while (Buffer[n] != ' ')
		{
			LocalBuff[c] = Buffer[n];
			c++;
			n++;
		}
		(*Cipher).frequency[m] = atoi(LocalBuff);
		printf( "%d ", (*Cipher).frequency[m] );
		m++;
		n++;
	}
	printf ("\n");

	// Read the plain key characters from the file
	//fseek (fp, 1, SEEK_CUR);
	n = 0;
	m = 0;
	memset( Buffer, 0, sizeof(Buffer) );
	fgets (Buffer, sizeof(Buffer), fp);
	printf ("DEBUG - Plain key characters are: ");
	while (Buffer[n] != '\n')
	{
		if (Buffer[n] != ' ')
		{
			(*Cipher).plainchar[m] = Buffer[n];
			printf( "%c ", (*Cipher).plainchar[m] );
			m++;
		}
		n++;
	}
	printf ("\n");

	fclose (fp);
	return;
}

void Choose_Analysis(struct cipherdata *Cipher)

{
	Display_Analysis_Menu();

	char Option[10] = { 0 };

	while (1)
	{
		if ( Error_Trap(atoi(fgets(Option,10,stdin)), 1, 9) == 1)
		{
			switch (Option[0])
			{
				case '1':
					Basic_Analysis(Cipher);
					break;
				case '2':
					Manual_Key_Entry(Cipher);
					break;
				case '3':
					Freq_Checker(Cipher);
					break;
				case '8':
					return;
				case '9':
					Exit_Program(Cipher);
				default:
					printf("\nDEBUG - switch default\n");
					break;
			}

			Display_Analysis_Menu();
		}
		else
		{
			printf("Try entering a number 1 - 9\n");
			Display_Analysis_Menu();
		}
	}
}

void Basic_Analysis(struct cipherdata *Cipher)

// Develops key from user defined characteristics and does simple frquency analysis
// Loops through text ciphertext points at, notes all unique values and counts occurences
// TO DO - add ability to choose number of characters per cipher character, e.g. AXDE, is it A, X, D, E or AX, DE?

{
	if ( (*Cipher).keysize != 0 )
	{	Save_Data_File(Cipher);			// Offer to save data
		Clear_Data_File(Cipher);		// Clear all data
	}

	int n = 0;
	int m = 0;

	printf ("Let's analyze!\n");

	for (n = 0; n < (*Cipher).ciphersize; n++)		// Don't count the null at the end of ciphertext!
	{
		for (m = 0; m <= (*Cipher).keysize; m++)
		{
			if ((*Cipher).ciphertextptr[n] == (*Cipher).cipherchar[m])
			{
				(*Cipher).frequency[m]++;
				break;
			}
			else
			{
				if (m == (*Cipher).keysize)
				{
					(*Cipher).cipherchar[m] = (*Cipher).ciphertextptr[n];
					(*Cipher).keysize++;
					(*Cipher).frequency[m]++;
					break;
				}
			}
		}
	}

	printf ("\nDEBUG - Keysize is %d \n", (*Cipher).keysize);

	for (n = 0; n < (*Cipher).keysize; n++)
	{
		printf ("DEBUG - Cipher Character No. %d\t Is \t%c & Occurs \t%d times.\n",( n+1 ), (*Cipher).cipherchar[n], (*Cipher).frequency[n] );
	}

	return;
}

void Manual_Key_Entry(struct cipherdata *Cipher)

// User manually enters a plain for each cipher character and displays plaintext
// Displays whole list and then asks one at a time

{
	// Option to break out and have all remaining entered as cipher characters.

	if ( (*Cipher).plainchar[0] != 0 )		// COULD be zero; bad test
	{	Save_Data_File(Cipher);			// Offer to save data
		Clear_Data_File(Cipher);		// Clear all data
	}

	char Pick[10] = { 0 };
	int n = 0;

	for (n = 0; n < CharSetSizeGlob; n ++)
	{
		(*Cipher).plainchar[n] = 0;
	}

	Pick[10] = 0;

	printf ("OK, this is the hard way.\n");
	printf ("Let's start by printing all we know:\n");

	for (n = 0; n < (*Cipher).keysize; n++)
	{
		printf ("Cipher Character No. %d\t Is \t%c & Occurs \t%d times.\n",( n+1 ), (*Cipher).cipherchar[n],( (*Cipher).frequency[n] ));
	}

	printf ("\nNow let's go over this line by line...\n");

	for (n = 0; n < (*Cipher).keysize; n++)
	{
		printf ("Cipher Character No. %d\t Is \t%c & Occurs \t%d times.\n",( n+1 ), (*Cipher).cipherchar[n],( (*Cipher).frequency[n] ));
		printf ("What do you think the plain character is?  ");

		fgets(Pick, 10, stdin);

		if ( ( Error_Trap(Pick[0], 32, 126) == 1 )  || ( (char)Pick[0] == 10 ) )
		{
			if ( (char)Pick[0] == 10 )
			{
				printf ("DEBUG - nothing entered.  Cipher character will be used.\n");
				(*Cipher).plainchar[n] = (*Cipher).cipherchar[n];
			}
			else
			{
				(*Cipher).plainchar[n] = Pick[0];
				printf ("DEBUG - you entered %c\n", (*Cipher).plainchar[n]);
			}
		}
		else
		{
			printf ("Only one printable ASCII character please.\n");
			n--;					// Do over!
		}
	}

	Display_Plaintext(Cipher);

	return;
}

void Freq_Checker (struct cipherdata *Cipher)

//  Compares frequency of cipher characters to expected frequency (e.g. in English "e" occurs 12%)
//  Makes assignment if frequency matches.  Also option to stop after only certain number identified (e.g. quit when 10 found)
//  Upon completion, checks for common words (e.g. "the", "in", from English) from imported wordlist like "rockyou.txt"
//  May hard code the input for now to test algorithms?
//
//  Offer to save if existing data
//  Clear any entered key, plaintext - BUT NOT THE CIPHER! (add a flag to Clear_Data_File, 1: clear all, 2: clear all but ciphertext)
//  Convert cipher to all caps; this will adjust ciphersize
//  Run Basic_Analysis
//
//  Load frequencies to check, e.g. f(e) = 12%, language dependent
//  Set initial tolerance as multiple of standard deviation (e.g. 0.5*Sigma)
//
//  Need a way to not check same key twice?  Wheels?
//
//  For each character in frequency key until minimum assigned or reach end
//  	For each character in cipher key, skipping ones assigned (assumes monophonic simiple substitution)
//		Calculate Sigma from frequency key
//		Within tolerance?
//			If so make assignment to Possible_Char aray, move to next character in cipher key (assumes monophonic simiple substitution)
//			If not, move to next character in cipher key
//  
//  If minimum assigned, for each key generated by Wheels
//	Generate plain text
//	Load words to check as a different file - language dependent; open file and read as needed or put on heap?
//	Set amount of plaintext to check for (e.g. 20%)
//	Check resulting plain text for words
//		For each word in wordlist (might have to open file, load series of words to heap, and then repeat)
//			Look in the plain text for it
//				If there, count towards total & continue
//				If not, move to next word in wordlist
//	Check total plaintext
//		If enough words, display and check user feedback
//			Offer to save file for later
//		If not, decrease tolerance and repeat
//  If at end and minimum not assigned, decrease tolerance and repeat

//  Provide some counters on number of tests, plain checks, etc.
//  If high frequency match, but no plain words, is it transposition?  Is it another language?

{
	Basic_Analysis(Cipher);

	char Plain_Char [7] = {'e', 't', 'a', 'o', 'i', 'n', 'h'};		// TO DO:  Will be file read eventually for characters & plain frequencies
	int Plain_Freq [7] = {1200, 910, 810, 770, 730, 700, 590};		// Multiply by 10000 so integers, no float required
	int Tolerance = 150;							// TO DO:  Make this an input
	char Temp_Key [CharSetSizeGlob] = { 0 };
	int Freq, Exp_Freq_Lower, Exp_Freq_Upper = 0;
	int Possible_Key_Index [ (*Cipher).keysize + 1 ];				//  Index array to track which column a row is on
	int n, m, c = 0;

	// Array for possible keys - row for each cipher characters, column 0 is cipher, columns 1+ are possible plain; '0' for blanks
	char Possible_Key [(*Cipher).keysize] [CharSetSizeGlob + 2];

	for (n = 0; n < (*Cipher).keysize; n++)
	{
		for (m = 0; m <= (CharSetSizeGlob ); m++)
		{
			Possible_Key [n] [m] = '0';
		}
	}		

	//  Assign possible plain characters to a cipher character

	//  Cycle through cipher characters
	for (n = 0; n < (*Cipher).keysize; n++)
	{
		Possible_Key [n] [0] = (*Cipher).cipherchar[n];
		Freq = ( 10000 * (*Cipher).frequency[n] ) / (*Cipher).ciphersize;	
		//  Cycle through possible plain characters
		c = 1;
		for (m = 0; m < 7; m++)
		{
			Exp_Freq_Lower = Plain_Freq[m] - Tolerance;
			Exp_Freq_Upper = Plain_Freq[m] + Tolerance;
			if ( Error_Trap(Freq, Exp_Freq_Lower, Exp_Freq_Upper) == 1 ) 
			{
				Possible_Key[n] [c] = Plain_Char[m];
				c++;
				printf("DEBUG - %c\t has ELF of %d, EUF of %d; %c has Freq of %d\n", Plain_Char[m], Exp_Freq_Lower, Exp_Freq_Upper, (*Cipher).cipherchar[n], Freq);
			}
		}
	}

	// DEBUG print to stdout
	for (n = 0; n < (*Cipher).keysize; n++)
	{
		for (m = 0; m < 7; m++)
		{
			if ( Possible_Key[n] [m + 1] != '0')
				printf("DEBUG - %c\t could be %c\t\n", Possible_Key[n] [0], Possible_Key[n] [m + 1]);
		}
	}


	//  Wheels:  build key from possibles, and display plaintext (later will check for plain words first) - WORK IN PROGRESS

	for (n = 0; n <= (*Cipher).keysize; n++)
		Possible_Key_Index [n] = 1;					// Initialize index

	int Row_Resets = 0;
	while ( Row_Resets < (*Cipher).keysize)
	{
		for (n = 0; n < (*Cipher).keysize; n++)				// For each cipher character
		{
			if ( Possible_Key [n] [Possible_Key_Index[n]] != '0')		// If not at the end of a "row"
			{
				Temp_Key [n] = Possible_Key[n] [Possible_Key_Index[n]];
				Possible_Key_Index[n]++;	
			}
			else 
			{
				if ( Possible_Key [n] [Possible_Key_Index[CharSetSizeGlob + 1]] == 0)				
				{
					Possible_Key [n] [Possible_Key_Index[CharSetSizeGlob + 1]] = 1;				
					Row_Resets++;			
				}				
				Possible_Key_Index[n] = 1;
				Possible_Key_Index[n+1]++;
			}
		}
		printf("DEBUG - Temp Key: ");
		for (n = 0; n< (*Cipher).keysize; n++)
		{
			printf("%c", Temp_Key[n]);
		}
		printf("\n");
	}	
	printf("... and then a miracle happens.\n");	
}

void Display_Plaintext(struct cipherdata *Cipher)

// Function to display plaintext for a given cipher text & key
// Note from VBA work:  this is a heavily used function!
// Also will need to output plaintext to various tools like frequency comparison & plain word check

{
	//  TO DO:  check if Cipher.plainchar populated; otherwise break

	int n = 0;
	int m = 0;

	for (n = 0; n < (*Cipher).ciphersize; n++)
	{
		for (m = 0; m < (*Cipher).keysize; m++)
		{
			if ((*Cipher).ciphertextptr[n] == (*Cipher).cipherchar[m])
			{
				(*Cipher).plaintextptr[n] = (*Cipher).plainchar[m];
				break;
			}
		}
		//  No match, go with cipher text.
		if ( (*Cipher).plaintextptr[n] == 0 )
			(*Cipher).plaintextptr[n] = (*Cipher).ciphertextptr[n];
	}

	n = 0;

	printf ("\nThe plaintext is: ");

	while (n < (*Cipher).ciphersize)
	{
		putc((*Cipher).plaintextptr[n],stdout);
		n++;
	}

	printf ("\n");

	return;
}

int Error_Trap(Test, LowerBound, UpperBound)

//  Tests user input against upper and lower bounds; returns 1 if good & 0 if bad

{
	int Error = 0;

	if (Test >= LowerBound && Test <= UpperBound)
	{
		Error = 1;
	}
	else
	{
		Error = 0;
	}

	return Error;
}

void Save_Data_File (struct cipherdata *Cipher)

//  Checks for entered data, asks, and saves the current cipher text & key details as user defined data file
//  TO DO:  Save only key, or only cipher, at user request.

{
	char Pick[10] = { 0 };

	printf("\nThere is already some data in memory.\nSave it (Y / N)?  ");
	fgets (Pick, 10, stdin);
	switch (Pick[0])
	{
		case 'Y':	
			break;
		case 'y':
			break;
		case 'N':
			return;
		case 'n':
			return;
		default:
			printf("Not sure what you said...\n");
			return;
	}

	char FileName[32] = { 0 };
	int namelen, n = 0;
	FILE *fp;
	printf("Please enter a data file name (.dat will be appended automatically): ");
	fgets (FileName, 32, stdin);
	namelen = strlen (FileName);
	FileName[namelen - 1] = 0;
	strcat(FileName, ".dat");
	printf("DEBUG - File Name is %s \n", FileName);

	// Save the ciphersize to file
	fp = fopen (FileName, "w");
	fprintf (fp, "%d\n", (*Cipher).ciphersize);

	// Save the ciphertext to the file
	n = 0;
	while (n < (*Cipher).ciphersize)
	{
		fprintf (fp, "%c", (*Cipher).ciphertextptr[n]);
		n++;
	}
	fprintf (fp, "\n");

	// Save the plaintext to the file
	n = 0;
	while (n < (*Cipher).ciphersize)
	{
		fprintf(fp, "%c", (*Cipher).plaintextptr[n]);
		n++;
	}
	fprintf (fp, "\n");

	// Save the key size to file
	fprintf (fp, "%d", (*Cipher).keysize);
	fprintf (fp, "\n");

	// Save the cipher key character to the file
	n = 0;
	while (n < (*Cipher).keysize)
	{
		fprintf (fp, "%c ", (*Cipher).cipherchar[n]);
		n++;
	}
	fprintf (fp, "\n");

	// Save the cipher character frequency to the file
	n = 0;
	while (n < (*Cipher).keysize)
	{
		fprintf(fp, "%d ", (*Cipher).frequency[n]);
		n++;
	}
	fprintf (fp, "\n");

	// Save the plain key character to the file
	n = 0;
	while (n < (*Cipher).keysize)
	{
		fprintf(fp, "%c ", (*Cipher).plainchar[n]);
		n++;
	}
	fprintf (fp, "\n");

	fclose (fp);
	return;
}

void Clear_Data_File(struct cipherdata *Cipher)

//  Clears everything for new analysis

{
	int n = 0;

	// Clear allocated memory

	if ( (*Cipher).ciphersize != 0 )
	{
		for (n = 0; n < (*Cipher).ciphersize; n++)
		{
			(*Cipher).ciphertextptr[n] = 0;
			(*Cipher).plaintextptr[n] = 0;
		}
		free ( (*Cipher).ciphertextptr );
		free ( (*Cipher).plaintextptr );
	}

	(*Cipher).ciphersize = 0;
	(*Cipher).keysize = 0;
	for (n = 0; n < CharSetSizeGlob; n ++)
	{
		(*Cipher).cipherchar[n] = 0;
		(*Cipher).frequency[n] = 0;
		(*Cipher).plainchar[n] = 0;
	}
	(*Cipher).ciphertextptr = NULL;
	(*Cipher).plaintextptr = NULL;

}

void Exit_Program(struct cipherdata *Cipher)

//  Saves, clears & exits
{
	if ( (*Cipher).ciphertextptr != NULL)
	{
		Save_Data_File(Cipher);
	}
	Clear_Data_File(Cipher);
	printf("\nLater, Dude!\n\n");
	exit(0);
}

//  Menu displays follow

void Display_Main_Menu()

//  Displays the main menu

{
	printf("\n1 - Enter new cipher\n");
	printf("2 - Open existing analysis *.dat file\n");
	printf("3 - Exit\n");
	printf("\nPlease enter a selection, 1 - 3:  ");
	return;
}

void Display_Enter_New_Cipher_Menu()

//  Displays the new cipher entry menu

{
	printf("\n1 - Load new cipher from file (.txt) & go to analysis\n");
	printf("2 - Enter new cipher manually & go to analysis\n");
	printf("3 - Return to main menu\n");
	printf("4 - Exit\n");
	printf("\nPlease enter a selection, 1 - 4:  ");
	return;
}

void Display_Open_Ex_Analysis_Menu()

//  Displays the open existing analysis menu

{
	printf("\n1 - Open existing analysis .dat file & go to analysis\n");
	printf("2 - Return to main menu\n");
	printf("3 - Exit\n");
	printf("\nPlease enter a selection, 1 - 3:  ");
	return;
}

void Display_Analysis_Menu()

//  Displays the analysis menu

{
	printf("\n1 - Develop key from user defined features & perform frequency analysis\n");
	printf("2 - Enter a key manually\n");
	printf("3 - Calculate possible keys from frequency analysis\n");
	printf("4 - Caeser shift!\n");
	printf("5 - Check for defined plain words or cribs\n");
	printf("8 - Return to cipher entry menu\n");
	printf("9 - Exit\n");
	printf("\nPlease enter a selection, 1 - 9:  ");
	return;
}

