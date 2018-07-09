#include <iostream>
#include <bitset>
#include <cmath>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <time.h>

using namespace std;
char GSM_output[228];

class GSM_Cipher{
	
	public:	
	unsigned int k1[16], k2[16], k3[16], k4[16], k5[16], k6[16], k7[16], k8[16], j, Left_32[32], Right_32[32], 
		FL_Out_32, Left_Input_32, Right_Input_32, FI_Input_16, FO_Out_32, FI_Out_16, KGCORE_CO[8];
		
	unsigned short Keyprime[8], Key[8], KL_One[8], KL_Two[8], KO_One[8], 
		KO_Two[8], KO_Three[8], KI_One[8], KI_Two[8], KI_Three[8];

	int temp_Key_128[128], A_register[2], Length;
		
	unsigned int kasumi_out[2], kasumi_input[2];
	char input_text[64], Count_22bit[22], Key_input[128];
	

		
		int BinToDec(unsigned int *k, int Size);
		void KeySchedule(int *Key_128);
		unsigned short Left_Rotate(unsigned short num, int No_Rotation);
		void Kasumi_Encryption( unsigned int *kasumi_input, int *Key_128);
		void Convert64to32();
		int Function_FL(int FL_input_32, int Rounds);
		int Function_FO(int FO_input_32, int Rounds);
		unsigned short Function_FI( unsigned short FI_Input_16, int Key);
		void Display_32bit(int decimal_value);
		void KGCORE(int *Key_128, char *Count_22bit, int Length);
		void Build_Blocks(unsigned int *KGCORE_CO);
		void Print_in_Hex(char * Block);
};

void GSM_Cipher::Print_in_Hex(char * Block){

	int temp , var, j, index = 0;
	int Result[29];
	for(int i = 0; i < 29; i++){
		var = i * 4; temp = 0, j = 0;
		while(j < 4){
			temp += (int) (Block[var + j]  - 48)* (8/(pow(2, j)));
			j ++;
		}
		//cout<<temp;
		Result[index++] = temp;

	}
	for(int i = 0; i < 29; i++)
		Display_32bit(Result[i]);
}

int GSM_Cipher::BinToDec(unsigned int *k, int Size){
	int temp = 0;
	
	for(int i = 0; i < Size; i++){
		temp += k[i]* pow(2, (Size - (i+1)));
	}

	return temp;
}

void GSM_Cipher::Build_Blocks(unsigned int  Data[]){
	char  Block1[116], Block2[116];
	unsigned int temp, value;
	int index = 0;
	for(int k = 0; k < 8; k++){
		
		int j = 0;
		temp = Data[k]; 
		while(j < 32){
			
			value = pow(2, 31);
			if(temp >= value )
				GSM_output[index++] = '1';
			else
				GSM_output[index++] = '0';
			temp = temp << 1;
			j++;

		}
		 
	} 
	for(int i = 0; i < 114; i++){
		Block1[i] = GSM_output[i];
		Block2[i] = GSM_output[114 + i]; 

	}
	Block1[114] = '0';
	Block2[114] = '0';
	Block1[115] = '0';
	Block2[115] = '0';
	
	cout<<"\nBlock1\n";
	Print_in_Hex(Block1);
	cout<<"\nBlock2\n";
	Print_in_Hex(Block2);
}

unsigned short GSM_Cipher::Left_Rotate(unsigned short num, int No_Rotation){
	return (num << No_Rotation) | (num >> (16 - No_Rotation));
}


void GSM_Cipher::KeySchedule(int *Key_128){
	
	int No_Rotation = 1;
	int i = 0, Size = 16;
	int Const_array[8] = { 291, 17767, 35243, 52719, 65244, 47768, 30292, 12816};
	for(j = 0; j < 16; j++){
		
		k1[j] = Key_128[j+(i * 16)];
		k2[j] = Key_128[j+((i+1)*16)];
		k3[j] = Key_128[j+((i+2)*16)];
		k4[j] = Key_128[j+((i+3)*16)];
		k5[j] = Key_128[j+((i+4)*16)];
		k6[j] = Key_128[j+((i+5)*16)];
		k7[j] = Key_128[j+((i+6)*16)];
		k8[j] = Key_128[j+((i+7)*16)];	
		
	}
	
	Key[0] = BinToDec(k1, Size);
	Key[1] = BinToDec(k2, Size);
	Key[2] = BinToDec(k3, Size);
	Key[3] = BinToDec(k4, Size);
	Key[4] = BinToDec(k5, Size);
	Key[5] = BinToDec(k6, Size);
	Key[6] = BinToDec(k7, Size);
	Key[7] = BinToDec(k8, Size);
	
	for(j = 0; j < 8; j++)
	 	Keyprime[j] = Key[j] ^ Const_array[j];	
		
	for(int Rounds = 0; Rounds < 8; Rounds++){
		KL_One[Rounds] = Left_Rotate(Key[Rounds], No_Rotation);	
		KL_Two[Rounds] = Keyprime[((Rounds + 2) % 8)];
		KI_One[Rounds] = Keyprime[((Rounds + 4) % 8)];
		KI_Two[Rounds] = Keyprime[((Rounds + 3) % 8)];
		KI_Three[Rounds] = Keyprime[((Rounds + 7) % 8)];
		KO_One[Rounds] = Left_Rotate(Key[(Rounds + 1) % 8], No_Rotation + 4);
		KO_Two[Rounds] = Left_Rotate(Key[(Rounds + 5) % 8], No_Rotation + 7);
		KO_Three[Rounds] = Left_Rotate(Key[(Rounds + 6) % 8], No_Rotation + 12);
		
	}
	
}

void GSM_Cipher::Convert64to32(){
 	
 	for(int i = 0; i < 32; i++){
 		Left_32[i] = (int)input_text[i] - 48;
 		Right_32[i] = (int)input_text[i + 32] - 48;

 	}
 
 }

void GSM_Cipher::Display_32bit(int decimal_value){
	
	std::stringstream ss;
	ss<< std::hex << decimal_value; // int decimal_value
	std::string res ( ss.str() );
	std::cout << res;

}

int GSM_Cipher::Function_FL(int FL_Input_32, int Rounds){
	
	int Lin_16[16], Rin_16[16], No_Rotation = 1; 
	unsigned short Left_Dec, Right_Dec, R1out_16, FL_Leftout_16, FL_Rightout_16, temp, LRot_result;
	
	Left_Dec = FL_Input_32 >> 16;
	Right_Dec = FL_Input_32 & 65535;
		
	temp = Left_Dec & KL_One[Rounds];
	LRot_result = Left_Rotate(temp, No_Rotation);
	FL_Rightout_16 = Right_Dec ^ LRot_result;
	
	temp = FL_Rightout_16 | KL_Two[Rounds];
	LRot_result = Left_Rotate(temp, No_Rotation);
	FL_Leftout_16 = LRot_result ^ Left_Dec;
	
	FL_Out_32 = (((int)FL_Leftout_16)<<16) + FL_Rightout_16;
	
	return 1;
}

int GSM_Cipher::Function_FO(int FO_input_32, int Rounds){

	unsigned short Left_Dec, Right_Dec, FI_Out;
	Left_Dec = FO_input_32 >> 16;
	Right_Dec = FO_input_32 & 65535;
	
	FI_Input_16 = Left_Dec ^ KO_One[Rounds];
	FI_Out = Function_FI(FI_Input_16, KI_One[Rounds]);
	Left_Dec = Right_Dec;
	Right_Dec = FI_Out ^ Right_Dec;

	FI_Input_16 = Left_Dec ^ KO_Two[Rounds];
	FI_Out = Function_FI(FI_Input_16, KI_Two[Rounds]);
	Left_Dec = Right_Dec;
	Right_Dec = FI_Out ^ Right_Dec;

	FI_Input_16 = Left_Dec ^ KO_Three[Rounds];
	FI_Out = Function_FI(FI_Input_16, KI_Three[Rounds]);
	Left_Dec = Right_Dec;
	Right_Dec = FI_Out ^ Right_Dec;

	FO_Out_32 = (((int)Left_Dec)<<16) + Right_Dec;
	
}

unsigned short GSM_Cipher::Function_FI(unsigned short FI_Input_16, int Key){
	
unsigned short S7[128] = {
   54, 50, 62, 56, 22, 34, 94, 96, 38,  6, 63, 93,  2, 18,123, 33,
   55,113, 39,114, 21, 67, 65, 12, 47, 73, 46, 27, 25,111,124, 81,
   53,  9,121, 79, 52, 60, 58, 48,101,127, 40,120,104, 70, 71, 43,
   20,122, 72, 61, 23,109, 13,100, 77,  1, 16,  7, 82, 10,105, 98,
   117,116, 76, 11, 89,106,  0,125,118, 99, 86, 69, 30, 57,126, 87,
   112, 51, 17,  5, 95, 14, 90, 84, 91,  8, 35,103, 32, 97, 28, 66,
   102, 31, 26, 45, 75,  4, 85, 92, 37, 74, 80, 49, 68, 29,115, 44,
   64,107,108, 24,110, 83, 36, 78, 42, 19, 15, 41, 88,119, 59,  3
};

unsigned short S9[512] = {
  167,239,161,379,391,334,  9,338, 38,226, 48,358,452,385, 90,397,
  183,253,147,331,415,340, 51,362,306,500,262, 82,216,159,356,177,
  175,241,489, 37,206, 17,  0,333, 44,254,378, 58,143,220, 81,400,
   95,  3,315,245, 54,235,218,405,472,264,172,494,371,290,399, 76,
  165,197,395,121,257,480,423,212,240, 28,462,176,406,507,288,223,
  501,407,249,265, 89,186,221,428,164, 74,440,196,458,421,350,163,
  232,158,134,354, 13,250,491,142,191, 69,193,425,152,227,366,135,
  344,300,276,242,437,320,113,278, 11,243, 87,317, 36, 93,496, 27,
  
  487,446,482, 41, 68,156,457,131,326,403,339, 20, 39,115,442,124,
  475,384,508, 53,112,170,479,151,126,169, 73,268,279,321,168,364,
  363,292, 46,499,393,327,324, 24,456,267,157,460,488,426,309,229,
  439,506,208,271,349,401,434,236, 16,209,359, 52, 56,120,199,277,
  465,416,252,287,246,  6, 83,305,420,345,153,502, 65, 61,244,282,
  173,222,418, 67,386,368,261,101,476,291,195,430, 49, 79,166,330,
  280,383,373,128,382,408,155,495,367,388,274,107,459,417, 62,454,
  132,225,203,316,234, 14,301, 91,503,286,424,211,347,307,140,374,
  
   35,103,125,427, 19,214,453,146,498,314,444,230,256,329,198,285,
   50,116, 78,410, 10,205,510,171,231, 45,139,467, 29, 86,505, 32,
   72, 26,342,150,313,490,431,238,411,325,149,473, 40,119,174,355,
  185,233,389, 71,448,273,372, 55,110,178,322, 12,469,392,369,190,
    1,109,375,137,181, 88, 75,308,260,484, 98,272,370,275,412,111,
  336,318,  4,504,492,259,304, 77,337,435, 21,357,303,332,483, 18,
   47, 85, 25,497,474,289,100,269,296,478,270,106, 31,104,433, 84,
  414,486,394, 96, 99,154,511,148,413,361,409,255,162,215,302,201,
  
  266,351,343,144,441,365,108,298,251, 34,182,509,138,210,335,133,
  311,352,328,141,396,346,123,319,450,281,429,228,443,481, 92,404,
  485,422,248,297, 23,213,130,466, 22,217,283, 70,294,360,419,127,
  312,377,  7,468,194,  2,117,295,463,258,224,447,247,187, 80,398,
  284,353,105,390,299,471,470,184, 57,200,348, 63,204,188, 33,451,
   97, 30,310,219, 94,160,129,493, 64,179,263,102,189,207,114,402,
  438,477,387,122,192, 42,381,  5,145,118,180,449,293,323,136,380,
   43, 66, 60,455,341,445,202,432,  8,237, 15,376,436,464, 59,461
};
 
unsigned short Left, Right, Temp_Value;

Left = FI_Input_16 >> 7;
Right = FI_Input_16 & 127;

Temp_Value = S9[Left];
Left = Right;
Right = Temp_Value ^ Right;

Temp_Value = S7[Left] ^ (Right & 127);
Temp_Value = Temp_Value ^ (Key >> 9);
Right = Right ^ (Key & 511);
Left = Right;
Right = Temp_Value;


Temp_Value = S9[Left];
Left = Right;
Right = Temp_Value ^ Right;

Temp_Value = S7[Left] ^ (Right & 127);
Left = Temp_Value;

FI_Out_16 = (unsigned short)((Left<<9) + Right);

return FI_Out_16;
}

void GSM_Cipher::Kasumi_Encryption(unsigned int *Kasumi_input, int *Key_128){
	KeySchedule(Key_128);
	
	int temp;
	Left_Input_32 = kasumi_input[0];
	Right_Input_32 = kasumi_input[1];
	
	for(int Rounds = 0; Rounds < 8; Rounds++){
		if(Rounds % 2 == 0){
			Function_FL(Left_Input_32, Rounds);
			Function_FO(FL_Out_32, Rounds);
			Right_Input_32 = FO_Out_32 ^ Right_Input_32;
			temp = Left_Input_32;
			Left_Input_32 = Right_Input_32;
			Right_Input_32 = temp;
			//cout<<Left_Input_32<<"==========\n"<<Right_Input_32;
		}
		
		else{
			Function_FO(Left_Input_32, Rounds);
			Function_FL(FO_Out_32, Rounds);
			Right_Input_32 = FL_Out_32 ^ Right_Input_32;
			temp = Left_Input_32;
			Left_Input_32 = Right_Input_32;
			Right_Input_32 = temp;
		}
		//Display_32bit(Left_Input_32);
		//Display_32bit(Right_Input_32);
	}
	kasumi_out[0] = Left_Input_32;
	kasumi_out[1] = Right_Input_32;
}

void GSM_Cipher::KGCORE(int *Key_128, char *Count_22bit, int Length){
	
	unsigned int temp1[32], temp2[32];
	int kasumi_input_OFB[2], index = 0;	
	
	int KGCORE_input_64bit [64] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			    	          		0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	
	for(int i = 10; i < 32; i++)
		KGCORE_input_64bit[i] = (int)Count_22bit[i - 10] - 48 ;
	for(int i = 0; i < 32; i++){
		temp1[i] = KGCORE_input_64bit[i];
		temp2[i] = KGCORE_input_64bit[i + 32];
	}
	kasumi_input[0] = BinToDec(temp1, 32);
	kasumi_input[1] = BinToDec(temp2, 32);
	
	int Key_mask[128] = { 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
						  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
						  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
						  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1
						};	
	int modified_Key[128];
	
	for(int i = 0; i < 128; i++)
		modified_Key[i] = Key_128[i] ^ Key_mask[i];
	Kasumi_Encryption(kasumi_input, modified_Key);
	//KASUMI OFB Mode
	
	int Blkcnt = 0;
	A_register[0] = kasumi_out[0];
	A_register[1] = kasumi_out[1];
	kasumi_input[0] = A_register[0];
	kasumi_input[1] = A_register[1] ^ Blkcnt;

	while(Length > 0){
		Kasumi_Encryption(kasumi_input, Key_128);
		KGCORE_CO[index++] = kasumi_out[0];
		KGCORE_CO[index++] = kasumi_out[1];
		Blkcnt++;
		kasumi_input[0] = A_register[0] ^ kasumi_out[0];
		kasumi_input[1] = A_register[1] ^ kasumi_out[1] ^ Blkcnt;
		Length = Length - 64;
	}
	for(int i = 0; i < 8; i++){
		Display_32bit(KGCORE_CO[i]);
		cout<<"\n";
	}
		

	Build_Blocks(KGCORE_CO);
}


int  main(){
	
	int No_testcase ;
	cin>>No_testcase;
	clock_t tStart = clock();
	while (No_testcase > 0){
		
		GSM_Cipher obj;
		int keylength;
		//cout<<"Enter 22- bit Count value ";
		cin>>obj.Count_22bit;
		//cout<<"Enter key ";
		cin>>obj.Key_input;
		//cout<<"Enter length of Plaintext in bits";
		cin>>obj.Length;
		keylength = strlen(obj.Key_input);

		if(keylength < 128){
			for(int i = keylength; i < 128  ; i++){
				obj.Key_input[i] = obj.Key_input[i - keylength];
			}
		}
		obj.Key_input[128] = '\0';
		for(int i = 0; i < 128; i++)
			obj.temp_Key_128[i] = (int)obj.Key_input[i] - 48;

		obj.KGCORE(obj.temp_Key_128, obj.Count_22bit, obj.Length);
		No_testcase --;
		printf("\nTime taken: %.2fmilli seconds\n", (double)(clock() - tStart) / ((CLOCKS_PER_SEC) / 1000));
	}
	printf("\nTime taken: %.2fmilli seconds\n", (double)(clock() - tStart) / ((CLOCKS_PER_SEC) / 1000));
	return 0;
}
