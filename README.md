# Image-Stegnography
This project demonstrates the implementation of Image Steganography using the C programming language. Steganography is the practice of hiding information within another medium. In this project, we hide secret messages within an image file, ensuring that the human eye cannot detect any visible change in the image, while the hidden data can still be extracted programmatically.

Features:
>Encoding: Hide a secret message within an image using least significant bit (LSB) manipulation.
>Decoding: Retrieve the hidden message from the steganographic image.
>Supports BMP image format for simplicity and easy manipulation.
>Command-line interface for user interaction.

Working:
  >The least significant bit (LSB) of each pixel is modified to store the binary data of the secret message.
  >The changes are subtle and nearly imperceptible to the naked eye.
  >The message is extracted by reading the LSB of each pixel in the image and reconstructing the original text.

Applications:
  *Data security and privacy.
  *Watermarking.
  *Hidden communication.
