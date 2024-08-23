#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <string.h>

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    /* Encoded Source Image info */
    char *e_src_image_fname;
    FILE *fptr_e_src_image;
   
   

    /* Secret File Info */
    char *secret_file_name;
    FILE *fptr_secret_file;
    char extn_secret_file[MAX_FILE_SUFFIX];
    
    uint extn_size;
    long secret_file_size;

    

}DecodeInfo;




/*Do decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Read and validate the command line arguements for decoding */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Extract 1bit LSB from each byte and translate into char */ 
Status decode_byte_from_8byte(char str[], char *data);

/*Extract encoded magic string verify the image contains secret data */
Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

/*Decode secret file extension size from image */
Status decode_secret_file_extn_size(DecodeInfo *decInfo); 

/*Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*Find secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/*Decode the secret file data from the image write it into new file with secret file extension */
Status decode_secret_data(DecodeInfo *decInfo);



#endif
