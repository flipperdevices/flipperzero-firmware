#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dirent.h"
#include "BMPLoad.h"

#define BUNDLE_SIGNATURE "BND!VID"
#define VERSION 1

uint32_t count_files(const char* path)
{
	uint32_t file_count = 0;
	DIR* dirp;
	struct dirent* entry;

	dirp = opendir(path); /* There should be error handling after this */
	
	while ((entry = readdir(dirp)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{ /* If the entry is a regular file */
			file_count++;
		}
	}
	
	closedir(dirp);

	free(entry);

	return file_count;
}

int main(int argc, char **argv)
{
	uint32_t sample_rate = 0;
	uint32_t fps = 0;
	uint32_t source_height = 0;
	uint32_t source_width = 0;
	uint32_t height = 0;
	uint32_t width = 0;
	
	bool usage = false;
	
	char folder_path[10000] = { 0 };
	
	// Scan arguments
	for (int c = 1; c < argc - 1; c++)
	{
		if (argv[c][0] == '-')
		{
			switch(tolower(argv[c][1]))
			{
				case '?':
				{
					usage = 1;
					break;
				}
				
				case 'w':
				{
					sscanf(&argv[c + 1][0], "%u", &width);
					break;
				}
				
				case 'h':
				{
					sscanf(&argv[c + 1][0], "%u", &height);
					break;
				}
				
				case 'f':
				{
					sscanf(&argv[c + 1][0], "%u", &fps);
					break;
				}
				
				case 's':
				{
					switch(tolower(argv[c][2]))
					{
						case 'r': // `-sr`
						{
							sscanf(&argv[c + 1][0], "%u", &sample_rate);
							break;
						}
						
						case 'w': // `-sw`
						{
							sscanf(&argv[c + 1][0], "%u", &source_width);
							break;
						}
						
						case 'h': // `-sh`
						{
							sscanf(&argv[c + 1][0], "%u", &source_height);
							break;
						}
						
						default: break;
					}
					
					break;
				}
				
				case 'p':
				{
					sscanf(&argv[c + 1][0], "%s", folder_path);
					break;
				}
				
				default: break;
			}
		}
	}

	// Usage display
	if ((argc < 2) || (usage))
	{
		printf("Usage: conv [options] (\"./conv.exe [options]\" on Windows)\n"
		"?              Show this help\n"
		"-p <value>     Path to your folder with/forward/slashes.\n"
		"Folder must contain \"source.mp4\" file which will be converted.\n"
		"-w <value>     width of resulting video, must not exceed 128\n"
		"and must be a multiple of 8.\n"
		"-h <value>     height of resulting video, must not exceed 64\n"
		"-f <value>     frames per second of resulting video, not recommended\n"
		"to be higher than 30.\n"
		"-sr <value>    audio sample rate. You must specify it so\n"
		"(sample_rate / fps) gives an integer, or audio will be out of sync.\n"
		"-sw <value>    Width of your source video. ffmpeg most likely would\n"
		"not work if you give wrong number.\n"
		"-sh <value>    Height of your source video. ffmpeg most likely would\n"
		"not work if you give wrong number.\n\n"
		"Example command:\n"
		"\"./conv.exe -p C:/Users/amogus/whatever/69/420 -w 96 -h 64 -f 30 -sr 44100 -sw 990 -sh 720\"");

		return 1;
	}
	
	printf("Starting the converter...\n");

	uint8_t* Data;
	int32_t bmp_width, bmp_height, bmp_bits;

	char frame_filename[10000];
	char audio_filename[10000];
	char bundle_filename[10000];

	char frames_folder[10000];

	strcpy(audio_filename, folder_path);
	strcat(audio_filename, "/audio.wav");

	strcpy(bundle_filename, folder_path);
	strcat(bundle_filename, "/bundle.bnd");

	strcpy(frames_folder, folder_path);
	strcat(frames_folder, "/frames");
	
	mkdir(frames_folder);
	
	FILE* ffmpeg;
	
	char ffmpeg_command[30000] = {0};
	
	snprintf(ffmpeg_command, sizeof(ffmpeg_command), "ffmpeg -i %s/source.mp4 -f lavfi -i color=gray:s=%dx%d -f lavfi -i color=black:s=%dx%d -f lavfi -i color=white:s=%dx%d -filter_complex \"[0:v]scale=%dx%d,threshold\" -r %d -s %dx%d -pix_fmt rgb24 %s/frames/frame%c07d.bmp", 
	folder_path, source_width, source_height, source_width, source_height, source_width, source_height, source_width, source_height, fps, width, height, folder_path, '%');
	
	//printf("\n\n\n%s\n\n\n", ffmpeg_command);
	
	ffmpeg = popen(ffmpeg_command, "w");
	
	if (ffmpeg == NULL)
	{
		printf("Error opening pipe to ffmpeg. Maybe you don't have ffmpeg installed on your computer.\n");
		exit(1);
	}
	
	pclose(ffmpeg);
	
	snprintf(ffmpeg_command, sizeof(ffmpeg_command), "ffmpeg -i %s/source.mp4 -ac 1 -acodec pcm_u8 -ar %d %s/audio.wav", 
	folder_path, sample_rate, folder_path);
	
	ffmpeg = popen(ffmpeg_command, "w");
	
	if (ffmpeg == NULL)
	{
		printf("Error opening pipe to ffmpeg.\n");
		exit(1);
	}
	
	pclose(ffmpeg);

	uint32_t num_frames = count_files(frames_folder);

	FILE* audio = fopen(audio_filename, "rb");
	
	if(audio == NULL)
	{
		printf("Error: temporary audio file not found.\n");
		exit(1);
	}
	
	FILE* bundle = fopen(bundle_filename, "wb");
	
	if(bundle == NULL)
	{
		printf("Error: failed to create .bnd file.\n");
		exit(1);
	}

	//the header, the FPS is determined by sample rate and audio chunk settings. There sample rate is 44100 Hz and FPS is 30 so audio chunk is 1470
	uint16_t audio_chunk_size = sample_rate / fps;

	uint8_t* audio_chunk = (uint8_t*)malloc(audio_chunk_size);
	uint8_t version = VERSION;

	fwrite(BUNDLE_SIGNATURE, sizeof(BUNDLE_SIGNATURE) - 1, 1, bundle);
	fwrite(&version, sizeof(version), 1, bundle);
	fwrite(&num_frames, sizeof(num_frames), 1, bundle);
	fwrite(&audio_chunk_size, sizeof(audio_chunk_size), 1, bundle);
	uint16_t sample_rate_16 = sample_rate;
	fwrite(&sample_rate_16, sizeof(sample_rate_16), 1, bundle);

	uint8_t frame_height = height;
	uint8_t frame_width = width;

	fwrite(&frame_height, sizeof(frame_height), 1, bundle);
	fwrite(&frame_width, sizeof(frame_width), 1, bundle);
	
	//printf("%d %d %d %d %d %d\n", version, num_frames, audio_chunk_size, sample_rate_16, frame_height, frame_width);

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
	
	uint8_t* pixel_frame = (uint8_t*)malloc(width * height / 8);

	for (uint32_t i = 0; i < num_frames; i++)
	//for (uint32_t i = 0; i < 300; i++)
	{
		snprintf(frame_filename, sizeof(frame_filename), "%s/frames/frame%07d.bmp", folder_path, i + 1);

		if(i % 100 == 0)
		{
			printf("Frame %d\n", i + 1);
		}

		Data = BmpLoad(frame_filename, &bmp_width, &bmp_height, &bmp_bits);

		if (Data == NULL)
		{
			printf("Error: could not open frame file \"%s\"\n", frame_filename);
			exit(1);
		}

		//write blocks of image-audio data
		//we imply that bmps are 24 bits per pixel; since only black and white, we will look at some bit in this 3-byte "number" for every pixel and chain them 8 pixels in 1 byte
		
		memset(pixel_frame, 0, width * height / 8);

		for (uint32_t j = 0; j < width * height / 8; j++)
		{
			for (uint32_t k = 0; k < 8; k++)
			{
				pixel_frame[j] |= (Data[(j * 8 + k) * 3]) ? (0 << (k)) : (1 << (k));
			}
		}

		fwrite(pixel_frame, width * height / 8 * sizeof(pixel_frame[0]), 1, bundle);

		fread(audio_chunk, audio_chunk_size, 1, audio);
		fwrite(audio_chunk, audio_chunk_size, 1, bundle);
		
		free(Data);
		
		remove(frame_filename);
	}
	
	free(pixel_frame);

	fclose(audio);
	fclose(bundle);

	free(audio_chunk);
	
	rmdir(frames_folder);
	remove(audio_filename);

	printf("Conversion finished.\n");
	
	return 0;
}
