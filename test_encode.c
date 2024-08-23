#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

/*
NAME : T KUSHAL
DATE : 12-07-2024
ID NO : 24004_037
DESCRIPTION : IMAGE STEGNOGRAPGHY
*/
int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("INFO : Invalid arguement\nFor encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor Decoding : ./a.out -d stego.bmp [decode.txt]\n");
        return 0;
    }



    if(check_operation_type(argv)== e_encode)
    {
        printf("INFO : You Selected Encoding\n");
        EncodeInfo structure_for_encoding;
        if( read_and_validate_encode_args(argv, &structure_for_encoding) == e_success)
        {
	    printf("INFO : Read and validate encode arguements is a success\n");
            if( do_encoding( &structure_for_encoding ) == e_success )
                printf("INFO : ## Encoding Done successfully ##\n");
            
            else
            {  

                printf("INFO : Encoding is a Failure\n");
            }
        }
        else
            printf("INFO : Read and validate encode arguements is a failure\n");

    }
    else if(check_operation_type(argv)== e_decode)
    {   
        printf("INFO : You Selected Decoding\n");
        DecodeInfo structure_for_decoding;
        if(read_and_validate_decode_args(argv,&structure_for_decoding) == e_success)
        {
            printf("INFO : Read and validate decode arguements is success\n");
            if( do_decoding(&structure_for_decoding) == e_success)
            {
                printf("INFO : ## Decoding Done successfully ##\n");

            }
            else
            {
                printf("INFO : Decoding is a Failure\n");
            }
        }
        else
            printf("INFO : Read and validate decode arguements is a failure\n");
    }
    else
        printf("INFO : Invalid arguement\nFor encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor Decoding : ./a.out -d stego.bmp [decode.txt]\n");
    


    return 0;
}



OperationType check_operation_type(char *argv[])
{
    if(strcmp( argv[1], "-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d")==0)
    {
        return e_decode;
    }
    else
        return e_unsupported;
}
