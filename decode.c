#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>
/*
NAME : T KUSHAL
DATE : 12-07-2024
ID NO : 24004_037
DESCRIPTION : IMAGE STEGNOGRAPGHY
*/

/*Open files for access/modify data*/

Status open_files_decrypt(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_e_src_image = fopen(decInfo->e_src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_e_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->e_src_image_fname);

        return e_failure;
    }

    // Secret file
    decInfo->fptr_secret_file = fopen(decInfo->secret_file_name, "w");
    // Do Error handling
    if (decInfo->fptr_secret_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_file_name);

        return e_failure;
    }

    return e_success;
}


/* Verify magic string is available to confirm encoded image */

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    
    char str[8];
    fseek(decInfo -> fptr_e_src_image, 54,SEEK_SET);    //skip header
   
  
    unsigned char m_str[2];
    for(int i=0; i<strlen(magic_string); i++)
    {
        fread(str, 8, 1, decInfo -> fptr_e_src_image);
       
       decode_byte_from_8byte(str,m_str+i);             
        
    }
    m_str[strlen(magic_string)]='\0';

   if( strcmp(m_str,magic_string) == 0)
   {
       return e_success;
   }
   else
       return e_failure;
}

/* Converts 8 bytes encoded data to a character */

Status decode_byte_from_8byte(char str[],char *data)
{
    char result=0;
    for(int i=0; i<8;i++)
    {
      result|=(str[i] & 0x01)<<(7-i);       //extracts LSB from 1 byte appends it to character bit and shifts
      
    }
    *data=result;
    
    return e_success;
}

/* Decodes size of secret file extension */

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    uint size=0;
    char str[32];
    fread(str, 32, 1, decInfo -> fptr_e_src_image);
    for(int i=0; i<32; i++)
    {
        size|=(str[i] & 0x01)<<(31-i);  
    }
    decInfo -> extn_size=size;
    
    return e_success;

}

/* Decodes secret file extension */

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
   char extn_str[decInfo -> extn_size];
   char str[8];

   for( int i=0; i<decInfo -> extn_size; i++ )
   {
       fread(str, 8, 1,decInfo -> fptr_e_src_image);
       decode_byte_from_8byte(str, extn_str+i);
   }

    extn_str[decInfo -> extn_size] = '\0';

    strcpy(decInfo -> extn_secret_file , extn_str);
    
    /* Verifys decoded extension is matches with output file */

    if( ( strcmp( strchr( decInfo -> secret_file_name, '.') , decInfo -> extn_secret_file)) != 0 )
    {
        printf("INFO :Conflicting extension extracted from image,extension will be renamed to decoded extension\n");
        fclose(decInfo -> fptr_secret_file);
        remove(decInfo -> secret_file_name);


        char *ptr;
        ptr=strchr( decInfo -> secret_file_name, '.' );
        strcpy( ptr ,  decInfo -> extn_secret_file);    //changes extension to decoded extension
        

        // Secret file open after renaming
        decInfo->fptr_secret_file = fopen(decInfo->secret_file_name, "w");

        if (decInfo->fptr_secret_file == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_file_name);
    
            return e_failure;
        }   
    }

    return e_success;
}



/*Decodes size of secret file */

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    uint size=0;
    char str[32];
    fread(str, 32, 1, decInfo -> fptr_e_src_image);
    for(int i=0; i<32; i++)
    {   
        size|=(str[i] & 0x01)<<(31-i);
    }   
    decInfo -> secret_file_size=size;
 
    return e_success;

}

/* Decodes secret file data and saves it to output file */

Status decode_secret_data(DecodeInfo *decInfo) 
{
    char secret_str[decInfo -> secret_file_size];
    char str[8];
    fseek(decInfo -> fptr_secret_file, 0 ,SEEK_SET);
    for(int i=0; i<decInfo -> secret_file_size; i++)
    {
        fread(str, 8, 1, decInfo -> fptr_e_src_image);
        decode_byte_from_8byte(str, secret_str+i);
        
    }

    fwrite(secret_str, decInfo -> secret_file_size, 1, decInfo -> fptr_secret_file);

    return e_success;
}

/* Decoding process */

Status do_decoding( DecodeInfo *decInfo)
{
    if( open_files_decrypt(decInfo) == e_success )
    {
         printf("INFO : Successfully opened all the files\n \tDone\n");
         if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
         {
               printf("INFO : Magic string decoded successfully\n \tDone\n");
               if(decode_secret_file_extn_size(decInfo) == e_success)
               {
                   printf("INFO : Secret file extension size is found\n \tDone\n");
                   if(decode_secret_file_extn(decInfo) == e_success)
                   {
                       printf("INFO : Secret file extension is fetched successfully\n \tDone\n");
                       if(decode_secret_file_size(decInfo) == e_success)
                       {
                           printf("INFO : Secret file size decoded successfully\n \tDone\n");
                           if(decode_secret_data(decInfo) == e_success)
                           {
                               printf("INFO : Secret file data decoding successful\n \tDone\n");
                           }
                           else
                           {
                               printf("INFO : Secret file data decoding is failed\n");
                               return e_failure;
                           }

                       }
                       else
                       {
                           printf("INFO : Secret file size decoding is failed\n");
                           return e_failure;
                       }
                   }
                   else
                   {
                       printf("INFO : Secret file extension fetching failed\n");
                       return e_failure;
                   } 
               }
               else
               {
                   printf("INFO : Secret file extn size is not found\n");
                   return e_failure;
               }

         }
         else
         { 
               printf("INFO : Magic string was not found\n");
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


/*Validates command line arguements in the case of decoding */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
   if(strcmp(strchr( argv[2], '.') , ".bmp") == 0)
   {
        decInfo -> e_src_image_fname=argv[2];
   }   
   else
       return e_failure;


   if ( argv[3] != NULL)
   {
       decInfo -> secret_file_name = argv[3];

   }
   else
   {
       decInfo -> secret_file_name = "output.txt";
   }
  
   return e_success;
}
     
