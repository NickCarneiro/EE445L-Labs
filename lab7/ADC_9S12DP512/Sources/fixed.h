//******FIXED.H******
//contains functions that output fixed point numbers
//with different precisions and resolutions

//---------------------Fixed_uDecOut2--------------------- 
// create a fixed point number string with precision=65535
// and resolution=1/100, ranging from 0 to 65534
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_uDecOut2(unsigned short number);


//---------------------Fixed_sDecOut3--------------------- 
// create a fixed point number string with precision=19999
// and resolution=1/1000, ranging from -9999 to 9999
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_sDecOut3(signed short number);


//---------------------Fixed_uBinOut8--------------------- 
// create a fixed point number string with precision=65535
// and resolution=1/256, ranging from 0 to 255.99
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_uBinOut8(unsigned short number);
