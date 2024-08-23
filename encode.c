#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/*
NAME : T KUSHAL
DATE : 12-07-2024
ID NO : 24004_037
DESCRIPTION : IMAGE STEGNOGRAPGHY
*/

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO : width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("INFO : height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Validates command line arguements for encoding */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
   if(strcmp(strchr( argv[2], '.') , ".bmp") == 0)
   {
        encInfo -> src_image_fname=argv[2];
   }
   else
       return e_failure;
 
   if(strcmp(strchr( argv[3], '.') , ".txt") == 0)
   {
          encInfo -> secret_fname = argv[3];
   }
     else
         return e_failure;

   if ( argv[4] != NULL)
   {
       encInfo -> stego_image_fname = argv[4];

   }
   else
       encInfo -> stego_image_fname = "stego.bmp";
   return e_success;
}

/*Verifys selected image is capable to hold secret data */

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret );
    if( encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (encInfo -> size_secret_file * 8)))
        return e_success;
    else
        return e_failure;

}

/*Provides file size*/

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0 ,SEEK_END);
    return ftell( fptr);

}

/*Copies header file into encoded file as it is */

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char str[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

/*Encodes magic string to make sure while decoding to check whether image is encoded or not */

Status encode_magic_string( char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string), encInfo);
    return e_success;

}

/*Encodes data into image data*/

Status encode_data_to_image( char *data, int size, EncodeInfo *encInfo)
{
    for( int i=0; i<size; i++)
    {
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image); //read 8 bytes of data from beautiful.bmp
        encode_byte_to_lsb(data[i], encInfo -> image_data); //hide 1 byte in 8bytes of data
        fwrite(encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image);   //write the encoded 8 bytes to stego.bmp
    }
}

/*Encodes a byte of data into 8byte of image data*/

Status encode_byte_to_lsb( char data, char *image_buffer)
{
    for( int i = 0; i < 8; i++)
    {
        image_buffer[i]= (image_buffer[i]& 0xFE) | ((data >> (7-i))& 1);

    }
    return e_success;
}

/*Secret file extension size is encoded*/

Status encode_secret_file_extn_size( int size, EncodeInfo *encInfo)
{
    //encode a integer->32bytes from beautiful.bmp is needed
    char str[32];
    fread(str, 32, 1, encInfo -> fptr_src_image);
    encode_size_to_lsb(size, str);
    fwrite(str, 32, 1, encInfo -> fptr_stego_image);
    return e_success;
}

/*Encodes size into image data*/

Status encode_size_to_lsb( int size, char *image_buffer)
{
    for( int i = 0; i < 32; i++)
    {   
        image_buffer[i]= (image_buffer[i]& 0xFE) | ((size >> (31-i))&1);

    }   
}

/*Encodes secret file extension  into image data*/

Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn) , encInfo);
    return e_success;
            
}

/*Encodes secret file size into image data */

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 32, 1, encInfo -> fptr_src_image);
    encode_size_to_lsb(file_size, str);
    fwrite(str, 32, 1, encInfo -> fptr_stego_image);
    return e_success;

}

/*Encodes secret file data into image data*/

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);
    char str[encInfo -> size_secret_file];
    fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
    encode_data_to_image(str, encInfo -> size_secret_file, encInfo);
    return e_success;
}

/*copy remaining data of image into encoded image file*/

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    while(fread(&ch, 1, 1, encInfo -> fptr_src_image) > 0)
    {
        fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

/*Process of encoding*/

Status do_encoding( EncodeInfo *encInfo)
{
    if( open_files(encInfo) == e_success )
    {
        printf("INFO : Successfully opened all the files\n \tDone\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("INFO : Check capacity is success\n \tDone\n");
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success )
            {
                 printf("INFO : Copy bmp header is successful\n \tDone\n");
                 if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                 {
                    printf("INFO : Magic string encoded successfully\n \tDone\n");
                    strcpy( encInfo -> extn_secret_file , strstr(encInfo -> secret_fname , "."));
                   if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
                    {
                        printf("INFO : Encoding secret file extension size is successful\n \tDone\n");
                        if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("INFO : Encoding secret file extn is successful\n \tDone\n");
                            if(encode_secret_file_size(encInfo -> size_secret_file, encInfo ) == e_success)
                            {
                                printf("INFO : Encoding secret file size is successful\n \tDone\n");
                                if(encode_secret_file_data( encInfo ) == e_success)
                                {
                                    printf("INFO : Encoding secret file data is successful\n \tDone\n");
                                    if(copy_remaining_img_data(encInfo) == e_success)
                                    {
                                        printf("INFO : Copied remaining bytes successfully\n \tDone\n");

                                    }
                                    else
                                    {
                                        printf("INFO : Failed to copy remaining bytes\n");
                                        return e_failure;
                                    }

                                }
                                else
                                {
                                    printf("INFO : Encoding secret file data is failure\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("INFO : Encoding secret file size is failure\n");
                                return e_failure;
                            }

                        }
                        else
                        {
                            printf("INFO : Encoding secret file extn is failure\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("INFO : Encoding secret file extension size is failed\n");
                        return e_failure;
                    }
                 }   
                 else
                 {   
                     printf("INFO : Magic string was not encoded\n");
                     return e_failure;
                 }
            }
            else
            {
                printf("INFO : Copy bmp header is a failure\n");
                return e_failure;
            }

        }
        else
        {
            printf("INFO : Check capacity is failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO : Failed to open the required files\n");
        return e_failure;
    }

    return e_success;
}
