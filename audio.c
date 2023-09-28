// cl audio.c OpenAL32.lib user32.lib gdi32.lib
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "al.h"
#include "alc.h"

void LoadAudioListener()
{
	float orientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, orientation);
}

unsigned int LoadAudioClip(char *filename, int format, int frequency)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	unsigned char* data = (unsigned char*) malloc(size);
	fread(data, sizeof(unsigned char), size, file);
	fclose(file);
	unsigned int buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, data, size, frequency);
	free(data);	
	return buffer;
}

unsigned int LoadAudioSource(float px, float py, float pz, float vx, float vy, float vz, float pitch, float gain, int loop)
{
	unsigned int source;
	alGenSources(1, &source);
	alSource3f(source, AL_POSITION, px, py, pz);
	alSource3f(source, AL_VELOCITY, vx, vy, vz);
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, gain);
	alSourcei(source, AL_LOOPING, loop);
	return source;
}

int main(int argc, char** argv)
{
	const char* deviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	ALCdevice* device = alcOpenDevice(deviceName);
	printf("Audio device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));
	printf("Press z or x to play sound, press q to exit.");
	ALCcontext* context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	LoadAudioListener();
	unsigned int sources[2];
	unsigned int buffers[2];
	for (int i = 0; i < 2; i++) 
	{
		char filename[20];
		snprintf(filename, sizeof(filename), "audio%d.bin", i);	
		sources[i] = LoadAudioSource(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		buffers[i] = LoadAudioClip(filename, AL_FORMAT_MONO16, 44100);
		alSourcei(sources[i], AL_BUFFER, buffers[i]);
	}
	int exit = 0;
	while (!exit) 
	{
		char ch = getch(); 
		switch(ch)
		{
			case 'z':
				alSourcePlay(sources[0]);
				break;
			case 'x':
				alSourcePlay(sources[1]);
				break;
			case 'q' :
				exit = 1;
				break;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		alDeleteSources(1, &sources[i]);
		alDeleteBuffers(1, &buffers[i]);
	}
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	return 0;
}

/*
To generate byte array (*.bin) from audio files (WAV, MP3 etc.), you can use for example free Unity engine and script:

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class GenerateAudio : MonoBehaviour
{
	[SerializeField] AudioClip _AudioClip;

	void Start()
	{
		float[] samples = new float[_AudioClip.samples];
		_AudioClip.GetData(samples, 0);
		short[] shorts = new short[samples.Length];
		byte[] bytes = new byte[samples.Length * 2];
		for (int i = 0; i < samples.Length; i++) 
		{
			shorts[i] = (short)(samples[i] * 32767);
			byte[] data = System.BitConverter.GetBytes(shorts[i]);
			bytes[i * 2] = data[0];
			bytes[i * 2 + 1] = data[1];
		}
		string filePath = Path.Combine(Application.dataPath, "audio.bin");
		File.WriteAllBytes(filePath, bytes);
	}
}
*/