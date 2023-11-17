#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdint.h>
#include <stdio.h>

#include "dirent.h"
#include "BMPLoad.h"

using namespace std;

#define BUNDLE_SIGNATURE "BND!VID"
#define VERSION 1

// USER SETTINGS:

#define FOLDER_PATH "C:/Users/Georg/Desktop/bad_apple_flipper/laputa"
#define SAMPLE_RATE 44100
#define FPS 24
#define HEIGHT 64
#define WIDTH 128 /*width of your video, determined by ffmpeg. Here it is for 4:3 aspect ratio*/

uint32_t count_files(const char* path)
{
    uint32_t file_count = 0;
    DIR* dirp;
    struct dirent* entry;

    dirp = opendir(path); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        //cout << "ping\n";
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }
    closedir(dirp);

    free(entry);

    return file_count;
}

int main()
{
    std::cout << "Starting the converter...\n";

    uint8_t* Data;
    int32_t width, height, bits;

    char frame_filename[1000];
    char audio_filename[1000];
    char bundle_filename[1000];

    char frames_folder[1000];

    strcpy(audio_filename, FOLDER_PATH);
    strcat(audio_filename, "/audio.wav");

    strcpy(bundle_filename, FOLDER_PATH);
    strcat(bundle_filename, "/bundle.bnd");

    strcpy(frames_folder, FOLDER_PATH);
    strcat(frames_folder, "/frames");

    uint32_t num_frames = count_files(frames_folder);

    FILE* audio = fopen(audio_filename, "rb");
    FILE * bundle = fopen(bundle_filename, "wb");

    //the header, the FPS is determined by sample rate and audio chunk settings. There sample rate is 44100 Hz and FPS is 30 so audio chunk is 1470
    uint16_t audio_chunk_size = SAMPLE_RATE / FPS;

    uint8_t* audio_chunk = (uint8_t*)malloc(audio_chunk_size);
    uint8_t version = VERSION;

    fwrite(BUNDLE_SIGNATURE, sizeof(BUNDLE_SIGNATURE) - 1, 1, bundle);
    fwrite(&version, sizeof(version), 1, bundle);
    fwrite(&num_frames, sizeof(num_frames), 1, bundle);
    fwrite(&audio_chunk_size, sizeof(audio_chunk_size), 1, bundle);
    uint16_t sample_rate = SAMPLE_RATE;
    fwrite(&sample_rate, sizeof(sample_rate), 1, bundle);

    uint8_t frame_height = HEIGHT;
    uint8_t frame_width = WIDTH;

    fwrite(&frame_height, sizeof(frame_height), 1, bundle);
    fwrite(&frame_width, sizeof(frame_width), 1, bundle);

    //seek for raw audio data start point
    //fseek(audio, 44, 0);

    char data_sig[5];
    data_sig[4] = '\0';

    uint16_t pos = 0;

    while (strcmp(data_sig, "data") != 0) //because not all wav files' headers are 44 bytes long! ffmpeg no exception
    {
        fseek(audio, pos, 0);
        fread(&data_sig, 4, 1, audio);
        pos++;
    }

    fseek(audio, pos + 4 + 4, 0); //after "data" there are 4 more bytes of data size

    for (uint32_t i = 0; i < num_frames; i++)
    //for (uint32_t i = 0; i < 300; i++)
    {
        snprintf(frame_filename, sizeof(frame_filename), "%s/frames/frame%07d.bmp", FOLDER_PATH, i + 1);

        cout << "Frame " << i + 1 << endl;

        Data = BmpLoad(frame_filename, &width, &height, &bits);

        if (Data == NULL)
        {
            printf("PROGRAM TERMINATED with ERROR\n");
            exit(1);
        }

        //write blocks of image-audio data
        //we imply that bmps are 24 bits per pixel; since only black and white, we will look at some bit in this 3-byte "number" for every pixel and chain them 8 pixels in 1 byte
        uint8_t pixel_frame[WIDTH * HEIGHT / 8] = { 0 };

        for (uint32_t j = 0; j < WIDTH * HEIGHT / 8; j++)
        {
            for (uint32_t k = 0; k < 8; k++)
            {
                pixel_frame[j] |= (Data[(j * 8 + k) * 3]) ? (0 << (k)) : (1 << (k));
            }
        }

        fwrite(pixel_frame, sizeof(pixel_frame), 1, bundle);

        fread(audio_chunk, audio_chunk_size, 1, audio);
        fwrite(audio_chunk, audio_chunk_size, 1, bundle);

        free(Data);
    }

    fclose(audio);
    fclose(bundle);

    free(audio_chunk);

    cout << "Conversion finished\n";
}
